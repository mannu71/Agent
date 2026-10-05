#pragma once

#include <set>
#include <string>
#include <utility>
#include <vector>

#include "ta/bar.hpp"
#include "ta/risk.hpp"
#include "ta/screener.hpp"

namespace ta {

// Explicit costs as a fraction of traded value, per side. The rulebook's base case
// is ~0.5% round trip for NSE delivery (STT, stamp duty, exchange, GST, DP,
// slippage); stress-test at 1.0%.
struct CostModel {
    double buy_frac = 0.0025;
    double sell_frac = 0.0025;
    double round_trip() const { return buy_frac + sell_frac; }
};

struct BacktestConfig {
    double initial_equity = 1'000'000;
    std::string start_date;  // inclusive, empty = from first bar
    std::string end_date;    // inclusive, empty = to last bar

    double tick = 0.05;              // NSE tick size
    double entry_limit_frac = 0.005; // buy-stop-limit: limit 0.5% above the trigger
    double stop_adr_mult = 1.0;      // initial stop = fill x (1 - mult x ADR20), see README
    int partial_day = 3;             // sell partial at this day's close if in profit
    double partial_frac = 1.0 / 3.0;
    int time_stop_day = 20;          // exit at this day's close if open profit < 1R
    int max_hold_days = 120;
    double fast_trail_min_adr = 0.05; // ADR20 >= 5% trails on SMA10, else SMA20

    ScreenConfig screen;
    RiskConfig risk;
    CostModel cost;
};

struct Trade {
    std::string symbol;
    std::string entry_date;
    std::string exit_date;
    double entry_price = 0;
    long qty = 0;                 // initial shares
    double risk_per_share = 0;    // entry - initial stop
    double pnl = 0;               // net of all costs, including partial exits
    double r_multiple = 0;        // pnl / (qty * risk_per_share)
    std::string exit_reason;
};

struct Metrics {
    int trades = 0;
    double win_rate = 0;
    double avg_r = 0;
    double profit_factor = 0;
    double total_return = 0;
    double cagr = 0;
    double max_drawdown = 0;
    double sharpe = 0;  // daily returns, annualised, risk-free rate 0
};

struct BacktestResult {
    std::vector<Trade> trades;
    std::vector<std::pair<std::string, double>> equity_curve;  // date, equity at close
    Metrics metrics;
    RiskState final_risk_state = RiskState::Normal;
};

BacktestResult run_backtest(const Universe& universe, const BacktestConfig& cfg,
                            const std::set<std::string>& excluded = {});

Metrics compute_metrics(const std::vector<Trade>& trades,
                        const std::vector<std::pair<std::string, double>>& equity_curve);

}  // namespace ta
