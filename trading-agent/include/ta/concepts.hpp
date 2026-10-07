#pragma once

#include <cstdint>
#include <set>
#include <string>
#include <vector>

#include "ta/flow.hpp"

namespace ta {

// Market context at the moment a setup appears. Every field uses only data available at
// that moment, so the memory can be keyed on it without look-ahead.
struct SetupContext {
    std::string trend;    // "up" / "down": prior UTC day's close vs its 50-day SMA
    std::string session;  // asia [0,7) / london [7,13) / ny [13,21) / late [21,24) UTC
    std::string vol;      // "low" / "mid" / "high": ATR14 / ATR100 of the signal timeframe
    std::string vwap;     // "above" / "below": signal close vs the UTC-day session VWAP
    std::string flow;     // "with" / "against": signal bar's delta sign vs the setup's side
};

// A trade idea emitted by a pattern detector at the close of a signal bar.
struct Setup {
    std::int64_t t = 0;          // decision time (minutes): close of the signal bar
    std::string pattern;
    std::string symbol;
    int tf = 0;                  // signal timeframe, minutes
    int side = 1;                // +1 long, -1 short
    char order = 'M';            // 'M' market at the next open, 'L' limit at entry, 'S' stop at entry
    double ref = 0;              // signal bar's close (for matched random baselines)
    double entry = 0;            // limit / stop price (ignored for 'M')
    double stop = 0;             // initial stop (absolute price)
    double target = 0;           // take-profit (absolute price); 0 = none (time exit only)
    double rr = 0;               // target distance in R for 'M' orders (target set at fill)
    std::int64_t expiry = 0;     // order valid until this time (minutes)
    std::int64_t max_hold = 0;   // minutes after the fill before a time exit
    SetupContext ctx;
};

// Parameters of every detector. Defaults are the sources' published values where one
// exists (see knowledge/concepts.md); nothing here is fitted.
struct ConceptConfig {
    int swing_n = 3;              // fractal swing point: N bars each side (confirmed N bars later)
    int atr_n = 14;
    double rr = 2.0;              // reward:risk for fixed targets
    double stop_buffer_atr = 0.1; // stop placed this many ATRs beyond the structure level
    double min_risk_frac = 0.0015;// skip setups whose stop is closer than this fraction of price
    int hold_bars = 48;           // time exit after this many signal-timeframe bars
    int order_expiry_bars = 20;   // limit orders expire after this many bars
    int sweep_lookback = 50;      // swing levels older than this many bars are ignored
    double fvg_min_atr = 0.25;    // minimum gap size in ATRs
    double displacement_atr = 1.0;// middle candle range >= this many ATRs ("displacement")
    int range_len = 40;           // Wyckoff trading-range length (bars)
    double range_max_atr = 8.0;   // range height <= this many ATRs counts as a trading range
    double climax_vol_mult = 1.5; // spring / upthrust bar volume vs the range's average
    double value_area = 0.70;     // volume-profile value area share
    int profile_bins = 50;
};

// Runs the requested detectors over one symbol's signal-timeframe bars. `daily` (UTC days)
// supplies the trend context. Known patterns: sweep, sweep_absorb, sweep_cont, fvg, ob,
// choch, hl_pullback, spring, va80.
std::vector<Setup> detect_setups(const FlowSeries& bars, const FlowSeries& daily, int tf_minutes,
                                 const std::string& symbol, const std::set<std::string>& concepts,
                                 const ConceptConfig& cfg);

// Confirmed fractal swing points: swing_high[i] is true when bar i's high is strictly above
// the N bars on each side. It is known only at bar i + N.
void swing_points(const FlowSeries& bars, int n, std::vector<bool>& swing_high, std::vector<bool>& swing_low);

// Wilder ATR over n bars ending at each bar (NaN before n bars exist).
std::vector<double> atr_series(const FlowSeries& bars, int n);

// Value area of a set of bars: (POC, VAL, VAH) from a volume profile with `bins` price
// buckets, each bar's volume spread evenly over its high-low range.
struct ValueArea {
    double poc = 0, val = 0, vah = 0;
};
ValueArea value_area(const FlowSeries& bars, std::size_t from, std::size_t to, int bins, double share);

}  // namespace ta
