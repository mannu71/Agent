#pragma once

#include <string>
#include <utility>
#include <vector>

#include "ta/data.hpp"
#include "ta/kv.hpp"
#include "ta/market.hpp"
#include "ta/portfolio.hpp"
#include "ta/regime.hpp"
#include "ta/risk.hpp"
#include "ta/screener.hpp"
#include "ta/swing.hpp"
#include "ta/trade.hpp"

namespace ta {

// Sleeve A1: momentum-leader breakouts on NSE stocks (rulebook "live at small size").
struct A1Config {
    ScreenConfig screen;
    RiskConfig risk;
    CostModel cost;
    ExitRules exits;                  // day-3 partial, day-20 time stop, 250-day cap, SMA10/20 trail
    RegimeConfig regime;
    double tick = 0.05;               // NSE tick size
    double entry_limit_frac = 0.005;  // buy-stop-limit: limit 0.5% above the trigger
    double stop_adr_mult = 1.0;       // initial stop = fill x (1 - mult x ADR20), see README
    // "stop": buy-stop above the pivot during the session (rulebook). "close": buy at the
    // close only if the breakout holds there: close above the trigger, in the top part of
    // the day's range, on above-average volume (O'Neil's confirmation rule; thresholds
    // fixed in advance, not fitted).
    std::string entry_mode = "stop";
    double confirm_close_pos = 0.5;   // (close - low) / (high - low) at least this
    double confirm_vol_mult = 1.5;    // day's volume at least this x the prior 20-day average
    // Volatility-scaled sizing (Barroso & Santa-Clara; Cederburg et al.: momentum was the
    // one family where it held up in real time): risk x min(cap, target / sigma_n of the
    // index). target 0 = expanding median of the index's own sigma_n (no look-ahead).
    // Replaces the binary volatility-gate halving when an index series is supplied.
    bool vol_scale = true;
    std::size_t vol_n = 126;
    double vol_target = 0;
    double vol_scale_cap = 1.0;
};

// Inputs from outside the sleeve for one step: shared limits, the active-book
// breaker's size multiplier and an optional event sink for the journal.
struct StepContext {
    const SharedBook* shared = nullptr;
    double risk_multiplier = 1.0;
    bool block_new_entries = false;
    std::vector<KvRecord>* events = nullptr;
};

// One trading day at a time, so a backtest and a paper-trading run execute exactly the
// same code. State round-trips through save()/load().
class A1Engine {
public:
    A1Engine(const MarketData& md, A1Config cfg, ExclusionList excluded, EquityRegimeInputs regime,
             double initial_equity);

    void step(const std::string& date, const StepContext& ctx = {});

    double equity() const { return last_equity_; }
    const std::string& last_date() const { return last_date_; }
    double open_risk() const { return book_.open_risk(); }
    int positions() const { return book_.count(); }
    const SwingBook& book() const { return book_; }
    const std::vector<Candidate>& pending() const { return pending_; }
    // Composite scores of every eligible stock at the last close, for the scorecard.
    const std::vector<std::pair<std::string, double>>& last_scores() const { return last_scores_; }
    RiskManager& risk() { return risk_; }
    const RiskManager& risk() const { return risk_; }

    std::vector<Trade> drain_closed() { return book_.drain_closed(); }
    void flatten(const std::string& date, const std::string& reason, std::vector<KvRecord>* events);

    std::vector<KvRecord> save() const;
    void load(const std::vector<KvRecord>& records);

private:
    void enter(const std::string& date, const StepContext& ctx);
    double vol_multiplier(const std::string& date) const;  // NaN when not computable
    void screen_for_tomorrow(const std::string& date, std::vector<KvRecord>* events);

    const MarketData& md_;
    A1Config cfg_;
    ExclusionList excluded_;
    EquityRegimeInputs regime_in_;
    RiskManager risk_;
    SwingBook book_;
    std::vector<Candidate> pending_;
    double pending_risk_mult_ = 1.0;   // volatility gate at the signal close
    bool pending_allowed_ = true;      // trend / crash gates at the signal close
    std::vector<std::pair<std::string, double>> last_scores_;
    std::vector<double> index_vol_;  // index sigma_n per index bar, precomputed
    double last_equity_;
    std::string last_date_;
};

struct BacktestResult {
    std::vector<Trade> trades;
    EquityCurve equity_curve;
    Metrics metrics;
    RiskState final_risk_state = RiskState::Normal;
};

// Runs A1 over [start, end] (empty = all data). Positions still open at the end are
// closed at the last close so every trade is counted.
BacktestResult run_a1_backtest(const MarketData& md, const A1Config& cfg, const ExclusionList& excluded,
                               double initial_equity, const std::string& start = "",
                               const std::string& end = "", const EquityRegimeInputs& regime = {});

}  // namespace ta
