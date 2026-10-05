#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

#include "ta/a1.hpp"  // StepContext, BacktestResult
#include "ta/data.hpp"
#include "ta/market.hpp"
#include "ta/risk.hpp"
#include "ta/swing.hpp"

namespace ta {

// Sleeve A2: episodic-pivot gaps, the mechanical skeleton of "The Power of Price Action
// Reading" adapted to NSE, with coded proxies for the trader's chart preferences.
// The rulebook allows paper trading only until every validation gate passes.
struct A2Config {
    double min_gap = 0.06;               // official open / prior close - 1
    double min_price = 50;
    double min_avg_traded_value = 1e8;   // Rs 10 crore over 20 days
    double cash_band_skip = 0.98;        // cash stocks: skip if open >= 0.98 x upper band
    double fno_band = 0.10;              // F&O dynamic band before flex
    double fno_trigger_margin = 0.01;    // F&O: skip if trigger within 1% of the upper band
    std::string or_start = "09:15";      // first 5-minute bar (after the pre-open auction)
    std::string window_end = "10:15";    // buy-stop valid for bars starting before this
    std::string weakness_time = "15:20"; // day 0: below entry at this bar -> exit at the close
    double tick = 0.05;
    double entry_limit_frac = 0.005;
    double approve_pct = 0.80;           // approve above the 80th percentile of prior events
    std::size_t approval_window = 250;   // trading days of event history
    std::size_t min_history_events = 30; // fewer prior events -> nothing approved yet
    int max_entries_per_day = 2;
    std::size_t rv_days = 14;            // first-5-minute relative volume baseline
    RiskConfig risk;
    CostModel cost;
    ExitRules exits;

    A2Config() {
        risk.risk_per_trade = 0.0025;
        exits.partial_day = 3;
        exits.time_stop_day = 0;        // no time stop; 30-day cap instead
        exits.max_hold_days = 30;
        exits.trail_fast = 10;
        exits.trail_slow = 10;          // always SMA10
        exits.fast_trail_min_adr = 0;
    }
};

struct IntradayData {
    std::map<std::string, Series> bars;    // symbol -> 5-minute bars ("YYYY-MM-DD HH:MM")
    std::map<std::string, DayIndex> index; // built by finalize()
    void finalize();
    // [first, last) bars for symbol on day, or nullptr when missing.
    const Bar* day(const std::string& sym, const std::string& date, std::size_t* n) const;
};

struct A2Inputs {
    const IntradayData* intraday = nullptr;
    std::map<std::string, std::vector<std::string>> catalysts;  // symbol -> sorted filing timestamps
    std::set<std::string> fno;                                  // F&O stocks
    std::map<std::string, double> cash_band;                    // cash stocks: band fraction (0.10, 0.20)
    ExclusionList excluded;
};
// "symbol,YYYY-MM-DD HH:MM" per line.
std::map<std::string, std::vector<std::string>> load_catalysts(const std::string& path);
// "symbol,band" per line; band "FO" marks an F&O stock, otherwise a percentage (e.g. 20).
void load_bands(const std::string& path, A2Inputs& in);

struct GapEvent {
    std::string date;
    std::string symbol;
    double gap = 0;
    double open = 0;
    double prior_close = 0;
    double adr = 0;
    bool fno = false;
    double upper_band = 0;
    std::vector<double> vars;   // 8 oriented proxy variables (higher = preferred)
    bool neglect = false, range_breakout = false, early_cycle = false;  // rulebook trait flags
    double score = 0;
    bool approved = false;
    std::string status;         // approved / below_threshold / warmup / skipped:<reason>
};

// Day-0 intraday entry simulation shared by the engine and the shadow study.
struct EntryResult {
    bool filled = false;
    double fill = 0, stop = 0;
    std::size_t bar = 0;  // offset of the fill bar within the day
    std::string reason;   // why there was no fill
};
EntryResult simulate_entry(const GapEvent& ev, const Bar* bars, std::size_t n, const A2Config& cfg);

class A2Engine {
public:
    A2Engine(const MarketData& md, A2Config cfg, A2Inputs in, double initial_equity);

    void step(const std::string& date, const StepContext& ctx = {});

    double equity() const { return last_equity_; }
    const std::string& last_date() const { return last_date_; }
    double open_risk() const { return book_.open_risk(); }
    int positions() const { return book_.count(); }
    const SwingBook& book() const { return book_; }
    RiskManager& risk() { return risk_; }
    const RiskManager& risk() const { return risk_; }
    // Every gap event seen so far, approved or not, in date order.
    const std::vector<GapEvent>& events() const { return history_; }

    std::vector<Trade> drain_closed() { return book_.drain_closed(); }
    void flatten(const std::string& date, const std::string& reason, std::vector<KvRecord>* events);

    std::vector<KvRecord> save() const;
    void load(const std::vector<KvRecord>& records);

private:
    std::vector<GapEvent> detect(const std::string& date) const;
    void score(GapEvent& ev);
    void manage_day0(const std::string& sym, const GapEvent& ev, const EntryResult& e, const Bar* bars,
                     std::size_t n, std::vector<KvRecord>* events);

    const MarketData& md_;
    A2Config cfg_;
    A2Inputs in_;
    RiskManager risk_;
    SwingBook book_;
    std::vector<GapEvent> history_;
    std::map<std::string, std::size_t> date_pos_;
    double last_equity_;
    std::string last_date_;
};

// Shadow outcome of one event with the full A2 exits and unit risk, used to compare
// approved with rejected events (the rulebook's uplift test). NaN when no fill.
double shadow_r(const GapEvent& ev, const MarketData& md, const IntradayData& intraday, const A2Config& cfg);

struct A2BacktestResult {
    BacktestResult portfolio;
    std::vector<GapEvent> events;
    std::vector<double> shadow;   // shadow R per event (NaN = no fill)
    double approved_mean_r = 0, rejected_mean_r = 0, uplift = 0, uplift_t = 0;
    int approved_n = 0, rejected_n = 0;
};

A2BacktestResult run_a2_backtest(const MarketData& md, const A2Config& cfg, const A2Inputs& in,
                                 double initial_equity, const std::string& start = "",
                                 const std::string& end = "");

}  // namespace ta
