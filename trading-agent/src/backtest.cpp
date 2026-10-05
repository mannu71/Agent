#include "ta/backtest.hpp"

#include <algorithm>
#include <cmath>
#include <map>
#include <unordered_map>

#include "ta/indicators.hpp"

namespace ta {

namespace {

struct Position {
    Trade trade;           // filled in as the position lives
    long qty = 0;          // shares still held
    double stop = 0;
    double adr_at_entry = 0;
    double last_close = 0;
    int days_held = 0;
    bool partial_done = false;
    bool exit_next_open = false;
};

class Engine {
public:
    Engine(const Universe& u, const BacktestConfig& cfg, const std::set<std::string>& excluded)
        : u_(u), cfg_(cfg), excluded_(excluded), risk_(cfg.risk), cash_(cfg.initial_equity) {
        std::set<std::string> dates;
        for (const auto& [sym, s] : u_) {
            auto& idx = index_[sym];
            for (std::size_t k = 0; k < s.size(); ++k) {
                idx[s[k].date] = k;
                dates.insert(s[k].date);
            }
        }
        dates_.assign(dates.begin(), dates.end());
    }

    BacktestResult run() {
        for (const auto& date : dates_) {
            if (!cfg_.start_date.empty() && date < cfg_.start_date) continue;
            if (!cfg_.end_date.empty() && date > cfg_.end_date) break;
            step(date);
        }
        // Close anything still open at the last known close so every trade is counted.
        const std::string last = result_.equity_curve.empty() ? "" : result_.equity_curve.back().first;
        for (auto& [sym, pos] : positions_) sell(pos, pos.qty, pos.last_close, last, "end_of_data");
        positions_.clear();
        if (!result_.equity_curve.empty()) result_.equity_curve.back().second = cash_;  // after exit costs

        result_.metrics = compute_metrics(result_.trades, result_.equity_curve);
        result_.final_risk_state = risk_.state();
        return std::move(result_);
    }

private:
    const Bar* bar(const std::string& sym, const std::string& date, std::size_t* i = nullptr) const {
        const auto& idx = index_.at(sym);
        const auto it = idx.find(date);
        if (it == idx.end()) return nullptr;
        if (i) *i = it->second;
        return &u_.at(sym)[it->second];
    }

    double equity(const std::string& date, bool at_open) const {
        double e = cash_;
        for (const auto& [sym, pos] : positions_) {
            const Bar* b = bar(sym, date);
            const double px = b ? (at_open ? b->open : b->close) : pos.last_close;
            e += static_cast<double>(pos.qty) * px;
        }
        return e;
    }

    double gross_exposure(const std::string& date) const {
        return equity(date, true) - cash_;
    }

    void buy(Position& pos, long qty, double px) {
        const double notional = static_cast<double>(qty) * px;
        const double cost = notional * cfg_.cost.buy_frac;
        cash_ -= notional + cost;
        pos.trade.pnl -= cost;
    }

    void sell(Position& pos, long qty, double px, const std::string& date, const std::string& reason) {
        if (qty <= 0) return;
        const double notional = static_cast<double>(qty) * px;
        const double cost = notional * cfg_.cost.sell_frac;
        cash_ += notional - cost;
        pos.trade.pnl += static_cast<double>(qty) * (px - pos.trade.entry_price) - cost;
        pos.qty -= qty;
        if (pos.qty == 0) {
            pos.trade.exit_date = date;
            pos.trade.exit_reason = reason;
            const double risk = static_cast<double>(pos.trade.qty) * pos.trade.risk_per_share;
            pos.trade.r_multiple = risk > 0 ? pos.trade.pnl / risk : 0;
            result_.trades.push_back(pos.trade);
        }
    }

    void process_exits_at_open(const std::string& date) {
        for (auto it = positions_.begin(); it != positions_.end();) {
            Position& pos = it->second;
            const Bar* b = bar(it->first, date);
            if (b) {
                if (pos.exit_next_open) {
                    sell(pos, pos.qty, b->open, date, "trail");
                } else if (b->open <= pos.stop) {
                    sell(pos, pos.qty, b->open, date, "stop_gap");
                } else if (b->low <= pos.stop) {
                    sell(pos, pos.qty, pos.stop, date, "stop");
                }
            }
            it = pos.qty == 0 ? positions_.erase(it) : std::next(it);
        }
    }

    void process_entries(const std::string& date) {
        const double eq_open = equity(date, true);
        if (!risk_.allows_new_entries(eq_open)) {
            pending_.clear();
            return;
        }
        for (const Candidate& c : pending_) {
            if (positions_.count(c.symbol)) continue;
            if (static_cast<int>(positions_.size()) >= cfg_.risk.max_positions) break;
            const Bar* b = bar(c.symbol, date);
            if (!b) continue;

            const double trigger = c.pivot + cfg_.tick;
            if (b->high < trigger) continue;                                  // never triggered
            if (b->open > trigger * (1.0 + cfg_.entry_limit_frac)) continue;  // gapped past the limit
            const double fill = std::max(b->open, trigger);
            const double stop = fill * (1.0 - cfg_.stop_adr_mult * c.adr);

            const SizeDecision d = risk_.size_long(eq_open, cash_, gross_exposure(date), fill, stop,
                                                   c.adr, cfg_.cost.round_trip());
            if (d.qty <= 0) continue;

            Position pos;
            pos.trade.symbol = c.symbol;
            pos.trade.entry_date = date;
            pos.trade.entry_price = fill;
            pos.trade.qty = d.qty;
            pos.trade.risk_per_share = fill - stop;
            pos.qty = d.qty;
            pos.stop = stop;
            pos.adr_at_entry = c.adr;
            pos.last_close = b->close;
            buy(pos, d.qty, fill);

            // Daily bars cannot say whether the low came before or after the fill,
            // so assume the worst: a low through the stop on entry day is a stop-out.
            if (b->low <= stop) {
                sell(pos, pos.qty, stop, date, "stop_entry_day");
                continue;
            }
            positions_.emplace(c.symbol, std::move(pos));
        }
        pending_.clear();
    }

    void manage_at_close(const std::string& date) {
        for (auto it = positions_.begin(); it != positions_.end();) {
            Position& pos = it->second;
            std::size_t i = 0;
            const Bar* b = bar(it->first, date, &i);
            if (b) {
                ++pos.days_held;
                pos.last_close = b->close;
                const double entry = pos.trade.entry_price;

                if (!pos.partial_done && pos.days_held == cfg_.partial_day && b->close > entry) {
                    const auto part = static_cast<long>(
                        std::floor(static_cast<double>(pos.trade.qty) * cfg_.partial_frac));
                    sell(pos, std::min(part, pos.qty), b->close, date, "partial");
                    pos.stop = std::max(pos.stop, entry);
                    pos.partial_done = true;
                }

                const double one_r = entry + pos.trade.risk_per_share;
                const std::size_t trail_n = pos.adr_at_entry >= cfg_.fast_trail_min_adr ? 10 : 20;
                const double trail = sma_close(u_.at(it->first), i, trail_n);
                if (pos.qty > 0 && pos.days_held >= cfg_.max_hold_days) {
                    sell(pos, pos.qty, b->close, date, "max_hold");
                } else if (pos.qty > 0 && pos.days_held == cfg_.time_stop_day && b->close < one_r) {
                    sell(pos, pos.qty, b->close, date, "time_stop");
                } else if (!std::isnan(trail) && b->close < trail) {
                    pos.exit_next_open = true;
                }
            }
            it = pos.qty == 0 ? positions_.erase(it) : std::next(it);
        }
    }

    void screen_for_tomorrow(const std::string& date) {
        if (risk_.state() == RiskState::Off) return;
        std::vector<SymbolView> views;
        for (const auto& [sym, s] : u_) {
            if (excluded_.count(sym) || positions_.count(sym)) continue;
            std::size_t i = 0;
            if (bar(sym, date, &i)) views.push_back({sym, &s, i});
        }
        pending_ = screen(views, cfg_.screen);
    }

    void step(const std::string& date) {
        // Day-start equity for the daily loss limit is yesterday's close mark.
        risk_.start_day(result_.equity_curve.empty() ? cash_ : result_.equity_curve.back().second);

        process_exits_at_open(date);
        process_entries(date);
        manage_at_close(date);

        const double eq = equity(date, false);
        result_.equity_curve.emplace_back(date, eq);
        risk_.end_day(eq);
        screen_for_tomorrow(date);
    }

    const Universe& u_;
    const BacktestConfig& cfg_;
    const std::set<std::string>& excluded_;
    std::unordered_map<std::string, std::unordered_map<std::string, std::size_t>> index_;
    std::vector<std::string> dates_;

    RiskManager risk_;
    double cash_;
    std::map<std::string, Position> positions_;
    std::vector<Candidate> pending_;
    BacktestResult result_;
};

}  // namespace

BacktestResult run_backtest(const Universe& universe, const BacktestConfig& cfg,
                            const std::set<std::string>& excluded) {
    return Engine(universe, cfg, excluded).run();
}

Metrics compute_metrics(const std::vector<Trade>& trades,
                        const std::vector<std::pair<std::string, double>>& equity_curve) {
    Metrics m;
    m.trades = static_cast<int>(trades.size());
    double wins = 0, gross_win = 0, gross_loss = 0, sum_r = 0;
    for (const Trade& t : trades) {
        if (t.pnl > 0) {
            ++wins;
            gross_win += t.pnl;
        } else {
            gross_loss -= t.pnl;
        }
        sum_r += t.r_multiple;
    }
    if (m.trades > 0) {
        m.win_rate = wins / m.trades;
        m.avg_r = sum_r / m.trades;
    }
    m.profit_factor = gross_loss > 0 ? gross_win / gross_loss : 0;

    if (equity_curve.size() >= 2) {
        const double first = equity_curve.front().second;
        const double last = equity_curve.back().second;
        m.total_return = last / first - 1.0;
        const double years = static_cast<double>(equity_curve.size() - 1) / 252.0;
        m.cagr = last > 0 ? std::pow(last / first, 1.0 / years) - 1.0 : -1.0;

        double peak = first, sum = 0, sum_sq = 0;
        for (std::size_t k = 1; k < equity_curve.size(); ++k) {
            const double e = equity_curve[k].second;
            peak = std::max(peak, e);
            m.max_drawdown = std::max(m.max_drawdown, 1.0 - e / peak);
            const double r = e / equity_curve[k - 1].second - 1.0;
            sum += r;
            sum_sq += r * r;
        }
        const double n = static_cast<double>(equity_curve.size() - 1);
        if (n >= 2) {
            const double mean = sum / n;
            const double sd = std::sqrt(std::max(0.0, (sum_sq - sum * sum / n) / (n - 1.0)));
            m.sharpe = sd > 0 ? mean / sd * std::sqrt(252.0) : 0;
        }
    }
    return m;
}

}  // namespace ta
