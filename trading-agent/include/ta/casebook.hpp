#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "ta/concepts.hpp"
#include "ta/flow.hpp"
#include "ta/trade.hpp"

namespace ta {

// Execution costs as fractions of notional. Defaults: Delta Exchange India perpetuals.
struct ExecCost {
    double taker = 0.0005;          // market and stop orders
    double maker = 0.0002;          // resting limit orders (entries and targets)
    double slippage = 0.0002;       // per market/stop fill, adverse
    double funding_per_8h = 0.0001; // charged to either side, pro rata (conservative)
    ExecCost scaled(double m) const { return {taker * m, maker * m, slippage * m, funding_per_8h * m}; }
};

// One setup played out on 1-minute bars in isolation: the agent's experience of it.
struct Case {
    Setup setup;
    bool filled = false;
    std::int64_t entry_t = 0, exit_t = 0;
    double entry_px = 0, exit_px = 0, stop_px = 0, target_px = 0;
    double r_gross = 0;   // price move in R, after slippage
    double r_net = 0;     // after fees and funding
    std::string exit_reason;
};

// Fills on the first 1-minute bar at or after the setup time. When one minute touches
// both the stop and the target, the stop is assumed (worst case).
Case simulate_case(const Setup& s, const FlowSeries& m1, const ExecCost& cost);

// The memory key: which past cases count as "the same situation".
std::string memory_key(const Setup& s, const std::vector<std::string>& fields);

// Recall rule: take a setup only when the memory holds at least `min_cases` finished
// cases with the same key, their mean net R is at least `min_mean_r` and the one-sided
// 95% lower bound of the mean is above zero.
struct RecallRule {
    bool enabled = true;
    std::size_t min_cases = 30;
    double min_mean_r = 0.05;
    double z = 1.645;
    std::vector<std::string> fields = {"concept", "tf", "side", "trend"};
};

struct PortfolioConfig {
    double equity = 1e6;
    double risk_frac = 0.005;  // equity risked per trade
    int max_open = 3;          // concurrent positions across symbols
    double tax_rate = 0;       // India VDA: tax on each winning trade, losses not offset
    std::int64_t trade_from = 0;  // earlier cases only build the memory (warm-up), never trade
    RecallRule recall;
};

struct PortfolioResult {
    std::vector<Trade> trades;
    std::vector<std::size_t> case_of_trade;  // index into the input cases, parallel to trades
    EquityCurve curve;     // realised equity at each UTC day with an exit
    int setups = 0, filled = 0, recalled = 0, taken = 0;
    // Memory at the end of the data: every key with >= min_cases finished cases, its case
    // count, mean net R and one-sided lower bound, and whether the recall rule approves it.
    struct KeyStats {
        std::size_t n = 0;
        double mean = 0, lower = 0;
        bool approved = false;
    };
    std::map<std::string, KeyStats> memory_now;
    double tax_paid = 0;
};

// Walks every filled case in time order. The memory consulted for a setup holds only
// cases that had exited before the setup's decision time.
PortfolioResult run_casebook(const std::vector<Case>& cases, const PortfolioConfig& cfg);

// Random-entry benchmark: for each filled case, the same symbol, side, order type, and
// entry / stop / target distances relative to the signal price, placed at a random minute
// with the same order lifetime and hold limit. Only the timing is random.
std::vector<Case> random_cases(const std::vector<Case>& cases, const std::map<std::string, const FlowSeries*>& m1,
                               const ExecCost& cost, unsigned seed);

}  // namespace ta
