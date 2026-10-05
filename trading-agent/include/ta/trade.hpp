#pragma once

#include <string>
#include <utility>
#include <vector>

#include "ta/kv.hpp"

namespace ta {

// Explicit costs as a fraction of traded value, per side. The rulebook's base case
// is ~0.5% round trip for NSE delivery (STT, stamp duty, exchange, GST, DP,
// slippage); stress-test at 1.0%.
struct CostModel {
    double buy_frac = 0.0025;
    double sell_frac = 0.0025;
    double round_trip() const { return buy_frac + sell_frac; }
};

struct Trade {
    std::string sleeve;
    std::string symbol;
    std::string entry_date;
    std::string exit_date;
    int side = 1;                 // +1 long, -1 short
    double entry_price = 0;
    long qty = 0;                 // initial units
    double risk_per_share = 0;    // |entry - initial stop|
    double pnl = 0;               // net of all costs, including partial exits
    double r_multiple = 0;        // pnl / (qty * risk_per_share)
    std::string exit_reason;

    KvRecord to_kv() const;
    static Trade from_kv(const KvRecord& r);
};

std::string trades_csv_header();
std::string to_csv(const Trade& t);

struct Metrics {
    int trades = 0;
    double win_rate = 0;
    double avg_r = 0;
    double profit_factor = 0;
    double total_return = 0;
    double cagr = 0;
    double max_drawdown = 0;
    double sharpe = 0;  // periodic returns, annualised, risk-free rate 0
};

using EquityCurve = std::vector<std::pair<std::string, double>>;

// `periods_per_year`: 252 for Indian equities, 365 for crypto.
Metrics compute_metrics(const std::vector<Trade>& trades, const EquityCurve& equity_curve,
                        double periods_per_year = 252.0);

}  // namespace ta
