#include "ta/d1.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

#include "ta/regime.hpp"

namespace ta {

std::map<std::string, double> d1_signals(const Series& intraday, const D1Config& cfg) {
    std::map<std::string, double> out;
    const DayIndex idx = index_by_day(intraday);
    double prior_close = 0;
    for (const auto& [day, range] : idx) {
        double at_signal = 0;
        for (std::size_t k = range.first; k < range.second; ++k) {
            if (time_of(intraday[k].date) < cfg.signal_time) at_signal = intraday[k].close;
        }
        if (prior_close > 0 && at_signal > 0) out[day] = at_signal / prior_close - 1.0;
        prior_close = intraday[range.second - 1].close;
    }
    return out;
}

D1Engine::D1Engine(const Series& intraday, D1Config cfg, double initial_equity, const Series* signal_bars)
    : bars_(intraday),
      cfg_(std::move(cfg)),
      index_(index_by_day(intraday)),
      signal_(d1_signals(signal_bars ? *signal_bars : intraday, cfg_)),
      risk_(cfg_.risk),
      cash_(initial_equity) {
    for (const auto& [day, range] : index_) dates_.push_back(day);
}

void D1Engine::step(const std::string& date, double risk_multiplier, std::vector<KvRecord>* events) {
    risk_.start_day(cash_);
    last_date_ = date;
    const auto sig = signal_.find(date);
    const auto day = index_.find(date);
    auto finish = [&] { risk_.end_day(cash_); };
    if (sig == signal_.end() || day == index_.end()) return finish();
    if (risk_.state() == RiskState::Off || risk_multiplier <= 0 || cfg_.skip_days.count(date)) return finish();
    if (cash_ < cfg_.min_sleeve_equity) return finish();

    // Threshold from the prior lookback days' |s| only.
    std::vector<double> hist;
    for (auto it = signal_.find(date); it != signal_.begin() && hist.size() < cfg_.lookback_days;) {
        --it;
        hist.push_back(std::fabs(it->second));
    }
    if (hist.size() < cfg_.min_history) return finish();
    if (percentile_rank(hist, std::fabs(sig->second)) < cfg_.pct_threshold) return finish();

    // Entry at the 15:00 price, i.e. the close of the last bar starting before 15:00.
    std::size_t k = day->second.first;
    double entry = 0;
    for (; k < day->second.second && time_of(bars_[k].date) < cfg_.signal_time; ++k) entry = bars_[k].close;
    if (entry <= 0 || k >= day->second.second) return finish();

    const int side = sig->second > 0 ? 1 : -1;
    const double stop = entry * (1.0 - side * cfg_.stop_frac);
    double exit = 0;
    std::string reason = "time_exit";
    for (; k < day->second.second; ++k) {
        const Bar& b = bars_[k];
        if (time_of(b.date) >= cfg_.exit_time) {
            exit = b.open;
            break;
        }
        const bool gap_hit = side > 0 ? b.open <= stop : b.open >= stop;
        const bool hit = side > 0 ? b.low <= stop : b.high >= stop;
        if (gap_hit || hit) {
            exit = gap_hit ? b.open : stop;
            reason = "stop";
            break;
        }
        exit = b.close;  // last bar before 15:28 if no bar starts at 15:28
    }

    const double units = cfg_.lot_size * cfg_.lots;
    const double cost_points = cfg_.cost_frac * entry + cfg_.slippage_points;
    const double points = side * (exit - entry) - cost_points;
    Trade t;
    t.sleeve = "D1";
    t.symbol = "NIFTY-FUT";
    t.entry_date = t.exit_date = date;
    t.side = side;
    t.entry_price = entry;
    t.qty = static_cast<long>(units);
    t.risk_per_share = entry * cfg_.stop_frac;
    t.pnl = points * units;
    t.r_multiple = t.pnl / (units * t.risk_per_share);
    t.exit_reason = reason;
    cash_ += t.pnl;
    closed_.push_back(t);
    if (events) {
        KvRecord r = t.to_kv();
        r.set("signal", sig->second).set("exit_price", exit).set("cost_points", cost_points);
        events->push_back(r);
    }
    finish();
}

std::vector<Trade> D1Engine::drain_closed() {
    std::vector<Trade> out;
    out.swap(closed_);
    return out;
}

std::vector<KvRecord> D1Engine::save() const {
    KvRecord h;
    h.type = "d1";
    h.set("cash", cash_).set("last_date", last_date_);
    return {h, risk_.snapshot()};
}

void D1Engine::load(const std::vector<KvRecord>& records) {
    for (const auto& r : records) {
        if (r.type == "d1") {
            cash_ = r.num("cash");
            last_date_ = r.str("last_date");
        } else if (r.type == "risk") {
            risk_.restore(r);
        }
    }
}

D1BacktestResult run_d1_backtest(const Series& intraday, const D1Config& cfg, double initial_equity,
                                 const std::string& start, const std::string& end, const Series* signal_bars) {
    D1Engine eng(intraday, cfg, initial_equity, signal_bars);
    D1BacktestResult r;
    for (const auto& d : eng.dates()) {
        if (!start.empty() && d < start) continue;
        if (!end.empty() && d > end) break;
        eng.step(d);
        r.equity_curve.emplace_back(d, eng.equity());
    }
    r.trades = eng.drain_closed();
    r.metrics = compute_metrics(r.trades, r.equity_curve);
    const double units = cfg.lot_size * cfg.lots;
    double sum = 0, sum_sq = 0, cost_sum = 0, net_late = 0;
    int n_late = 0;
    for (const auto& t : r.trades) {
        const double cost = cfg.cost_frac * t.entry_price + cfg.slippage_points;
        const double gross = t.pnl / units + cost;
        sum += gross;
        sum_sq += gross * gross;
        cost_sum += cost;
        if (t.entry_date >= "2022") {
            net_late += t.pnl / units;
            ++n_late;
        }
    }
    const double n = static_cast<double>(r.trades.size());
    if (n >= 1) r.cost_points = cost_sum / n;
    if (n >= 2) {
        r.mean_move_points = sum / n;
        const double sd = std::sqrt(std::max(0.0, (sum_sq - sum * sum / n) / (n - 1)));
        r.move_t = sd > 0 ? r.mean_move_points / (sd / std::sqrt(n)) : 0;
    }
    r.net_points_2022_on = n_late > 0 ? net_late / n_late : std::numeric_limits<double>::quiet_NaN();

    // Baltussen regression over every day in range: r(15:00 -> exit) on s.
    const std::map<std::string, double> sig = d1_signals(signal_bars ? *signal_bars : intraday, cfg);
    const DayIndex idx = index_by_day(intraday);
    std::map<std::string, std::vector<std::pair<double, double>>> by_year;
    std::vector<std::pair<double, double>> all;
    for (const auto& [day, range] : idx) {
        if ((!start.empty() && day < start) || (!end.empty() && day > end)) continue;
        const auto it = sig.find(day);
        if (it == sig.end()) continue;
        double entry = 0, exit = 0;
        for (std::size_t k = range.first; k < range.second; ++k) {
            const std::string tm = time_of(intraday[k].date);
            if (tm < cfg.signal_time) entry = intraday[k].close;
            else if (tm >= cfg.exit_time) {
                exit = intraday[k].open;
                break;
            } else exit = intraday[k].close;
        }
        if (entry <= 0 || exit <= 0) continue;
        all.emplace_back(it->second, exit / entry - 1.0);
        by_year[day.substr(0, 4)].emplace_back(it->second, exit / entry - 1.0);
    }
    auto ols = [](const std::vector<std::pair<double, double>>& xy, double& b, double& t) {
        b = t = 0;
        const double m = static_cast<double>(xy.size());
        if (m < 3) return;
        double mx = 0, my = 0;
        for (const auto& [x, y] : xy) {
            mx += x;
            my += y;
        }
        mx /= m;
        my /= m;
        double sxx = 0, sxy = 0;
        for (const auto& [x, y] : xy) {
            sxx += (x - mx) * (x - mx);
            sxy += (x - mx) * (y - my);
        }
        if (sxx <= 0) return;
        b = sxy / sxx;
        double rss = 0;
        for (const auto& [x, y] : xy) {
            const double e = y - my - b * (x - mx);
            rss += e * e;
        }
        const double se = std::sqrt(rss / (m - 2) / sxx);
        t = se > 0 ? b / se : 0;
    };
    ols(all, r.beta, r.beta_t);
    for (const auto& [year, xy] : by_year) {
        double b = 0, t = 0;
        ols(xy, b, t);
        r.beta_by_year[year] = {b, static_cast<int>(xy.size())};
    }
    const bool late_ok = std::isnan(r.net_points_2022_on) || r.net_points_2022_on > 0;
    r.passes_gate = r.mean_move_points >= 2 * r.cost_points && r.move_t > 3 && r.trades.size() >= 250 &&
                    r.beta >= cfg.min_beta && late_ok;
    return r;
}

}  // namespace ta
