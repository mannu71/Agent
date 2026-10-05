#pragma once

#include <memory>
#include <string>
#include <vector>

#include "ta/a1.hpp"
#include "ta/a2.hpp"
#include "ta/crypto_trend.hpp"
#include "ta/d1.hpp"
#include "ta/kv.hpp"
#include "ta/market.hpp"
#include "ta/portfolio.hpp"
#include "ta/risk.hpp"

namespace ta {

// A forward paper-trading account. Fills are simulated against each day's bars with
// the same engines and rules as the backtests; nothing is sent to a broker. Gate 3 of
// the blueprint additionally requires running through the real broker API, which this
// container cannot reach.
//
// Directory layout:
//   account.cfg   capital, start date, data paths, limit overrides
//   state/*.state engine state (key=value records), one file per sleeve plus the breaker
//   journal.log   hash-chained log of every signal, fill, reject, trade and run
//   trades.csv    closed trades, all sleeves
//   equity.csv    end-of-day equity per sleeve and for the active book
//   scores.csv    the A1 screener's score for every eligible stock each day
class PaperAccount {
public:
    // Creates the directory and account.cfg. `settings` adds or overrides keys
    // (e.g. data.equity_dir=...).
    static void init(const std::string& dir, double capital, const std::string& start, const Config& settings);

    explicit PaperAccount(const std::string& dir);
    ~PaperAccount();

    // Processes every new trading day up to `until` (empty = latest data). Returns a
    // human-readable report.
    std::string run(const std::string& until = "");
    std::string status() const;
    // Kill switch: flatten every sleeve at its last processed close and latch everything off.
    std::string kill(const std::string& reason);
    // Clears the Off latch of one sleeve ("a1", "a2", "b", "d1") or of the breaker ("book").
    std::string reset(const std::string& sleeve);
    // Screener scorecard from scores.csv: rolling rank IC, precision@k and the disable rule.
    std::string scorecard(std::size_t horizon, std::size_t top_k) const;

private:
    struct Impl;
    std::unique_ptr<Impl> p_;
};

}  // namespace ta
