#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "ta/casebook.hpp"

namespace ta {

// Forward (live) paper record of the concept engine. Each run recomputes every case from
// history with the same code as the backtest, then:
//  - appends to DIR/journal.log (hash-chained) each event not journalled before: a setup
//    appearing, a fill, an exit, and the agent taking or closing a trade. Events are keyed by
//    pattern, symbol, timeframe, side, time and order type, so rerunning on the same data adds
//    nothing; if a recomputed exit differs from the journalled one, a `revision` is appended
//    instead of editing history;
//  - rewrites the derived views setups.csv, open.csv, trades.csv, memory.csv and summary.txt.
struct LiveReport {
    int new_events = 0;
    int revisions = 0;
    int setups = 0, open_setups = 0, closed_setups = 0;
    int agent_open = 0, agent_closed = 0;
    double agent_pnl = 0;        // closed trades, money
    double agent_open_r = 0;     // open positions, R at the last price after exit costs
    std::string digest;          // human-readable summary of this run
};

// `last_t`: last 1-minute bar time per symbol (decides pending vs expired orders).
LiveReport write_live(const std::string& dir, const std::vector<Case>& cases, const PortfolioResult& agent,
                      std::int64_t live_from, const std::map<std::string, std::int64_t>& last_t);

}  // namespace ta
