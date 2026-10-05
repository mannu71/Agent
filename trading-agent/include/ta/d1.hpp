#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

#include "ta/data.hpp"
#include "ta/kv.hpp"
#include "ta/risk.hpp"
#include "ta/trade.hpp"

namespace ta {

// Sleeve D1: Nifty last-half-hour momentum (Baltussen form). The only intraday
// candidate in the rulebook; it gets zero capital until it clears 2x costs out of sample.
struct D1Config {
    std::string signal_time = "15:00";   // s = price at 15:00 / prior close - 1
    std::string exit_time = "15:28";
    double pct_threshold = 0.70;         // trade only if |s| >= 70th percentile of prior days
    std::size_t lookback_days = 250;
    std::size_t min_history = 60;
    double stop_frac = 0.0075;           // catastrophic stop, 0.75% (U)
    double lot_size = 65;                // Nifty lot from Jan 2026
    int lots = 1;                        // one lot only
    double cost_points = 19;             // round trip, base; 38 stress
    double min_sleeve_equity = 1.5e6;    // one lot needs ~Rs 15 lakh at 1x
    std::set<std::string> skip_days;     // e.g. expiry days, when excluded
    RiskConfig risk;
};

// Signal per day from intraday bars ("YYYY-MM-DD HH:MM" bar start times): price at
// signal_time (close of the last bar starting before it) over the prior day's last
// close, minus 1. Computed from a continuous unadjusted futures file this jumps on roll
// days (the prior close belongs to the expired contract), so pass the spot index as the
// signal series whenever possible.
std::map<std::string, double> d1_signals(const Series& intraday, const D1Config& cfg);

class D1Engine {
public:
    // `intraday` is what is traded (near-month futures). `signal_bars`, when given (e.g.
    // Nifty spot 1- or 5-minute bars), is used for the signal instead, so futures rolls
    // cannot leak carry into s.
    D1Engine(const Series& intraday, D1Config cfg, double initial_equity, const Series* signal_bars = nullptr);

    void step(const std::string& date, double risk_multiplier = 1.0, std::vector<KvRecord>* events = nullptr);

    double equity() const { return cash_; }
    const std::string& last_date() const { return last_date_; }
    const std::vector<std::string>& dates() const { return dates_; }
    RiskManager& risk() { return risk_; }
    const RiskManager& risk() const { return risk_; }
    std::vector<Trade> drain_closed();

    std::vector<KvRecord> save() const;
    void load(const std::vector<KvRecord>& records);

private:
    const Series& bars_;
    D1Config cfg_;
    DayIndex index_;
    std::map<std::string, double> signal_;
    std::vector<std::string> dates_;
    RiskManager risk_;
    double cash_;
    std::string last_date_;
    std::vector<Trade> closed_;
};

struct D1BacktestResult {
    std::vector<Trade> trades;
    EquityCurve equity_curve;
    Metrics metrics;
    double mean_move_points = 0;   // mean gross move in the signal direction, entry to exit
    double move_t = 0;             // t-statistic of that mean
    double cost_points = 0;
    // Rulebook switch-on: mean move >= 2x cost, t > 3 and at least 250 trades.
    bool passes_gate = false;
};

D1BacktestResult run_d1_backtest(const Series& intraday, const D1Config& cfg, double initial_equity,
                                 const std::string& start = "", const std::string& end = "",
                                 const Series* signal_bars = nullptr);

}  // namespace ta
