#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "ta/bar.hpp"

namespace ta {

// Sleeve A1 (momentum-leader breakouts) screen. Parameters follow the rulebook;
// thresholds marked "U" there are unverified and must be calibrated in backtests.
struct ScreenConfig {
    double min_price = 100.0;          // rupees
    double min_avg_traded_value = 1e8; // Rs 10 crore per day over 20 days
    double top_fraction = 0.2;         // act on the top quintile of the composite score
    double min_runup = 0.30;           // max(21-day, 63-day) return >= +30%
    double min_adr = 0.04;             // ADR20 >= 4%
    std::size_t base_len = 20;         // base window (rulebook allows 10-40 bars)
    double max_tightness = 0.5;        // 5-day range <= 0.5 x base range
    double max_below_pivot = 0.15;     // close within 15% below the base high
    std::size_t mom_skip_days = 0;     // 21 = classic 12-1 momentum (skip the latest month)
};

struct Candidate {
    std::string symbol;
    double score = 0;   // composite momentum z-score
    double pivot = 0;   // base high: the breakout level
    double adr = 0;     // ADR20 on the signal day
};

// A symbol's series and the index of the signal day within it.
struct SymbolView {
    std::string symbol;
    const Series* series = nullptr;
    std::size_t idx = 0;
};

// z-score each value against the sample, clipped to [-clip, clip].
// A sample with zero spread maps to all zeros.
std::vector<double> zscore_clipped(const std::vector<double>& x, double clip = 3.0);

bool passes_universe(const Series& s, std::size_t i, const ScreenConfig& cfg);

// Returns the pivot (base high) when bar i completes a valid setup, else NaN.
double setup_pivot(const Series& s, std::size_t i, const ScreenConfig& cfg);

struct ScoredSymbol {
    std::string symbol;
    double score = 0;
    const SymbolView* view = nullptr;
};

// Composite momentum score for every eligible symbol (vol-adjusted 6m and 12m return,
// close / 52-week high, 63-day return), sorted best first. Logged daily so the
// screener's rank IC can be monitored.
std::vector<ScoredSymbol> score_universe(const std::vector<SymbolView>& views, const ScreenConfig& cfg);

// Ranks eligible symbols by composite momentum (vol-adjusted 6m and 12m return,
// close / 52-week high, 63-day return), keeps the top fraction, then applies the
// setup filters. Result is sorted by score, best first.
std::vector<Candidate> screen(const std::vector<SymbolView>& views, const ScreenConfig& cfg);

}  // namespace ta
