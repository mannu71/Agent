#include "ta/crypto_trend.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

namespace ta {

namespace {

constexpr double kTiny = 1e-12;

double vol_annual(const Series& s, std::size_t i, std::size_t n, double periods) {
    if (i < n || n < 2) return std::numeric_limits<double>::quiet_NaN();
    double sum = 0, sum_sq = 0;
    for (std::size_t k = i + 1 - n; k <= i; ++k) {
        const double r = std::log(s[k].close / s[k - 1].close);
        sum += r;
        sum_sq += r * r;
    }
    const double nn = static_cast<double>(n);
    return std::sqrt(std::max(0.0, (sum_sq - sum * sum / nn) / (nn - 1.0))) * std::sqrt(periods);
}

double value_on(const std::map<std::string, double>& m, const std::string& date, double def) {
    const auto it = m.find(date);
    return it == m.end() ? def : it->second;
}

double value_before(const std::map<std::string, double>& m, const std::string& date) {
    auto it = m.lower_bound(date);
    if (it == m.begin()) return std::numeric_limits<double>::quiet_NaN();
    return std::prev(it)->second;
}

}  // namespace

void CryptoTrendConfig::replication_mode() {
    cost_bps = 10;
    funding_annual_default = 0;
    tax_rate = 0;
    risk.drawdown_halve = 2.0;  // unreachable: no overlay
    risk.drawdown_off = 2.0;
    regime.funding_red_annual = std::numeric_limits<double>::infinity();
}

CryptoTrendEngine::CryptoTrendEngine(const MarketData& md, CryptoTrendConfig cfg,
                                     std::map<std::string, CryptoAux> aux, double initial_equity)
    : md_(md), cfg_(std::move(cfg)), aux_(std::move(aux)), risk_(cfg_.risk), cash_(initial_equity),
      last_equity_(initial_equity) {}

bool CryptoTrendEngine::update_signals(const std::string& asset, std::size_t i) {
    const Series& s = md_.series(asset);
    bool changed = false;
    for (const std::size_t n : cfg_.lookbacks) {
        if (i + 1 < n) continue;
        double up = s[i].close, down = s[i].close;
        for (std::size_t k = i + 1 - n; k <= i; ++k) {
            up = std::max(up, s[k].close);
            down = std::min(down, s[k].close);
        }
        const double mid = (up + down) / 2.0;
        Signal& sig = signals_[asset][n];
        if (sig.long_) {
            // The paper tests today's close against the stop carried from yesterday
            // (TS_t = max(TS_t-1, Mid_t-1)), then ratchets with today's mid for tomorrow.
            if (s[i].close <= sig.stop) {
                sig.long_ = false;
                changed = true;
            } else {
                sig.stop = std::max(sig.stop, mid);
            }
        } else if (s[i].close >= up) {
            sig.long_ = true;
            sig.stop = mid;
            if (!priming_) ++entries_[n];
            changed = true;
        }
    }
    return changed;
}

double CryptoTrendEngine::target_weight(const std::string& asset, std::size_t i) const {
    const double sigma = vol_annual(md_.series(asset), i, cfg_.vol_n, cfg_.periods_per_year);
    if (std::isnan(sigma) || sigma <= 0) return 0;
    const double w = std::min(cfg_.vol_target / sigma, cfg_.max_lookback_weight);
    double sum = 0;
    const auto it = signals_.find(asset);
    for (const std::size_t n : cfg_.lookbacks) {
        if (it == signals_.end()) break;
        const auto s = it->second.find(n);
        if (s != it->second.end() && s->second.long_) sum += w;
    }
    return std::min(sum / static_cast<double>(cfg_.lookbacks.size()), cfg_.asset_cap);
}

void CryptoTrendEngine::prime(const std::string& date) {
    priming_ = true;
    for (const auto& d : md_.dates()) {
        if (d >= date) break;
        if (!primed_until_.empty() && d <= primed_until_) continue;
        for (const auto& [asset, s] : md_.universe()) {
            std::size_t i = 0;
            if (md_.bar(asset, d, &i)) update_signals(asset, i);
        }
        primed_until_ = d;
    }
    priming_ = false;
}

double CryptoTrendEngine::mark(const std::string& date) const {
    double e = cash_;
    for (const auto& [asset, h] : holdings_) {
        const Bar* b = md_.bar(asset, date);
        e += h.qty * (b ? b->close : h.last_close);
    }
    return e;
}

double CryptoTrendEngine::exposure(const std::string& asset) const {
    const auto it = holdings_.find(asset);
    return it == holdings_.end() ? 0 : it->second.qty;
}

void CryptoTrendEngine::trade_to(const std::string& asset, double target_qty, double px, const std::string& date,
                                 const std::string& reason, std::vector<KvRecord>* events) {
    Holding& h = holdings_[asset];
    const double dq = target_qty - h.qty;
    if (std::fabs(dq) * px < kTiny) return;
    const double cost = std::fabs(dq) * px * cfg_.cost_bps / 1e4;
    costs_paid_ += cost;
    double tax = 0;
    if (dq > 0) {
        if (h.qty <= kTiny) {
            h.trade = Trade{};
            h.trade.sleeve = "B";
            h.trade.symbol = asset;
            h.trade.entry_date = date;
            h.trade.entry_price = px;
        }
        h.avg_cost = (h.qty * h.avg_cost + dq * px) / (h.qty + dq);
        h.qty += dq;
        cash_ -= dq * px + cost;
        h.trade.pnl -= cost;
    } else {
        const double sold = -dq;
        // Indian VDA rules allow only the cost of acquisition as a deduction; fees are not
        // deductible and losses cannot offset gains, so each gain is taxed on its own.
        const double gain = sold * (px - h.avg_cost);
        tax = cfg_.tax_rate * std::max(0.0, gain);
        tax_paid_ += tax;
        cash_ += sold * px - cost - tax;
        h.qty -= sold;
        h.trade.pnl += gain - cost;
        if (h.qty <= kTiny) h.qty = 0;
    }
    h.last_close = px;

    if (events) {
        KvRecord e;
        e.type = "fill";
        e.set("sleeve", std::string("B")).set("date", date).set("symbol", asset);
        e.set("side", std::string(dq > 0 ? "buy" : "sell")).set("units", std::fabs(dq)).set("price", px);
        e.set("cost", cost).set("tax", tax).set("reason", reason);
        events->push_back(e);
    }
    if (h.qty == 0) {
        h.trade.exit_date = date;
        h.trade.exit_reason = reason;
        closed_.push_back(h.trade);
        if (events) events->push_back(h.trade.to_kv());
        holdings_.erase(asset);
    }
}

void CryptoTrendEngine::step(const std::string& date, double risk_multiplier, std::vector<KvRecord>* events) {
    risk_.start_day(last_equity_);

    // Funding accrues on yesterday's holdings at today's close (longs pay when positive).
    for (auto& [asset, h] : holdings_) {
        const Bar* b = md_.bar(asset, date);
        if (!b) continue;
        const auto a = aux_.find(asset);
        const double rate = a == aux_.end() ? cfg_.funding_annual_default / cfg_.periods_per_year
                                            : value_on(a->second.funding, date, cfg_.funding_annual_default / cfg_.periods_per_year);
        const double f = h.qty * b->close * rate;
        cash_ -= f;
        funding_paid_ += f;
        h.trade.pnl -= f;
    }

    std::map<std::string, bool> changed;
    if (primed_until_.empty() || date > primed_until_) {
        for (const auto& [asset, s] : md_.universe()) {
            std::size_t i = 0;
            if (md_.bar(asset, date, &i)) changed[asset] = update_signals(asset, i);
        }
        primed_until_ = date;
    }

    const double equity = mark(date);
    if (risk_.state() == RiskState::Off) {
        std::vector<std::string> held;
        for (const auto& [asset, h] : holdings_) held.push_back(asset);
        for (const auto& asset : held) {
            const Bar* b = md_.bar(asset, date);
            trade_to(asset, 0, b ? b->close : holdings_.at(asset).last_close, date, "risk_off", events);
        }
    } else {
        const double n_assets = static_cast<double>(md_.universe().size());
        double mult = std::clamp(risk_multiplier, 0.0, 1.0);
        if (risk_.state() == RiskState::Halved) mult *= 0.5;
        for (const auto& [asset, s] : md_.universe()) {
            std::size_t i = 0;
            const Bar* b = md_.bar(asset, date, &i);
            if (!b) continue;
            double asset_mult = mult;
            const auto a = aux_.find(asset);
            if (a != aux_.end() && a->second.funding.count(date)) {
                const double fa = a->second.funding.at(date) * cfg_.periods_per_year;
                const double oi_now = value_on(a->second.open_interest, date, std::numeric_limits<double>::quiet_NaN());
                const double oi_prev = value_before(a->second.open_interest, date);
                if (crowding_gate(fa, oi_now, oi_prev, cfg_.regime) == Gate::Red) asset_mult *= 0.5;
            }
            const double target_qty = target_weight(asset, i) * asset_mult * (equity / n_assets) / b->close;
            const double cur = exposure(asset);
            const double target_notional = target_qty * b->close;
            const double cur_notional = cur * b->close;
            const bool signal_change = changed.count(asset) && changed.at(asset);
            const bool drift = target_notional > 0 &&
                               std::fabs(cur_notional - target_notional) > cfg_.rebalance_band * target_notional;
            // The 1x cap is a hard limit: price drift between rebalances may not breach it.
            const bool over_cap = cur_notional > cfg_.asset_cap * (equity / n_assets) * (1.0 + 1e-9);
            if (signal_change || drift || over_cap || (target_qty <= 0 && cur > 0)) {
                trade_to(asset, target_qty, b->close, date, signal_change ? "signal" : "rebalance", events);
            }
        }
    }

    for (auto& [asset, h] : holdings_) {
        if (const Bar* b = md_.bar(asset, date)) h.last_close = b->close;
    }
    last_equity_ = mark(date);
    last_date_ = date;
    risk_.end_day(last_equity_);
}

std::vector<Trade> CryptoTrendEngine::drain_closed() {
    std::vector<Trade> out;
    out.swap(closed_);
    return out;
}

void CryptoTrendEngine::flatten(const std::string& date, const std::string& reason, std::vector<KvRecord>* events) {
    std::vector<std::string> held;
    for (const auto& [asset, h] : holdings_) held.push_back(asset);
    for (const auto& asset : held) {
        const Bar* b = md_.bar(asset, date);
        trade_to(asset, 0, b ? b->close : holdings_.at(asset).last_close, date, reason, events);
    }
    last_equity_ = cash_;
}

std::vector<KvRecord> CryptoTrendEngine::save() const {
    std::vector<KvRecord> out;
    KvRecord h;
    h.type = "b";
    h.set("cash", cash_).set("tax_paid", tax_paid_).set("funding_paid", funding_paid_).set("costs_paid", costs_paid_);
    h.set("last_date", last_date_).set("last_equity", last_equity_).set("primed_until", primed_until_);
    out.push_back(h);
    out.push_back(risk_.snapshot());
    for (const auto& [asset, hold] : holdings_) {
        KvRecord r = hold.trade.to_kv();
        r.type = "holding";
        r.set("asset", asset).set("units", hold.qty).set("avg_cost", hold.avg_cost).set("last_close", hold.last_close);
        out.push_back(r);
    }
    for (const auto& [asset, sigs] : signals_) {
        for (const auto& [n, s] : sigs) {
            KvRecord r;
            r.type = "signal_state";
            r.set("asset", asset).set("n", static_cast<long>(n)).set("long", s.long_).set("stop", s.stop);
            out.push_back(r);
        }
    }
    for (const auto& [n, c] : entries_) {
        KvRecord r;
        r.type = "entries";
        r.set("n", static_cast<long>(n)).set("count", c);
        out.push_back(r);
    }
    return out;
}

void CryptoTrendEngine::load(const std::vector<KvRecord>& records) {
    holdings_.clear();
    signals_.clear();
    entries_.clear();
    for (const auto& r : records) {
        if (r.type == "b") {
            cash_ = r.num("cash");
            tax_paid_ = r.num("tax_paid");
            funding_paid_ = r.num("funding_paid");
            costs_paid_ = r.num("costs_paid");
            last_date_ = r.str("last_date");
            last_equity_ = r.num("last_equity");
            primed_until_ = r.str("primed_until");
        } else if (r.type == "risk") {
            risk_.restore(r);
        } else if (r.type == "holding") {
            Holding h;
            h.trade = Trade::from_kv(r);
            h.qty = r.num("units");
            h.avg_cost = r.num("avg_cost");
            h.last_close = r.num("last_close");
            holdings_[r.str("asset")] = h;
        } else if (r.type == "signal_state") {
            Signal s;
            s.long_ = r.integer("long") != 0;
            s.stop = r.num("stop");
            signals_[r.str("asset")][static_cast<std::size_t>(r.integer("n"))] = s;
        } else if (r.type == "entries") {
            entries_[static_cast<std::size_t>(r.integer("n"))] = r.integer("count");
        }
    }
}

CryptoBacktestResult run_crypto_backtest(const MarketData& md, const CryptoTrendConfig& cfg,
                                         const std::map<std::string, CryptoAux>& aux, double initial_equity,
                                         const std::string& start, const std::string& end) {
    CryptoTrendEngine eng(md, cfg, aux, initial_equity);
    if (!start.empty()) eng.prime(start);
    CryptoBacktestResult r;
    for (const auto& d : md.dates()) {
        if (!start.empty() && d < start) continue;
        if (!end.empty() && d > end) break;
        eng.step(d);
        r.equity_curve.emplace_back(d, eng.equity());
    }
    if (!r.equity_curve.empty()) {
        eng.flatten(r.equity_curve.back().first, "end_of_data", nullptr);
        r.equity_curve.back().second = eng.equity();
    }
    r.trades = eng.drain_closed();
    r.metrics = compute_metrics(r.trades, r.equity_curve, cfg.periods_per_year);
    r.tax_paid = eng.tax_paid();
    r.funding_paid = eng.funding_paid();
    r.costs_paid = eng.costs_paid();
    r.entries_by_lookback = eng.entries_by_lookback();
    r.final_risk_state = eng.risk().state();
    return r;
}

}  // namespace ta
