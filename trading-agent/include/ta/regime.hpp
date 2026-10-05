#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

#include "ta/bar.hpp"

namespace ta {

// Regime gates switch sleeves on and off; they do not forecast. Thresholds are set
// from each series' own rolling history where the blueprint says so. Every
// threshold here is unverified (U) and counts as a trial in validation.
enum class Gate { Green, Red, Unknown };
const char* to_string(Gate g);

struct RegimeConfig {
    std::size_t trend_sma = 200;     // index above its 200-day average -> green
    std::size_t vol_lookback = 250;  // percentile window for volatility
    double vol_red_pct = 0.80;       // red when today's vol is in the top 20% of its own year
    std::size_t realized_vol_n = 30; // realised-vol window when no VIX series is given
    double crash_fall = -0.15;       // momentum-crash state: 63-day return <= -15% ...
    double crash_rebound = 0.10;     // ... and 21-day return >= +10% (panic rebound)
    double funding_red_annual = 0.30;  // perp funding above 30%/yr with rising OI
    int event_days_before = 1;       // calendar days of blackout around scheduled events
    int event_days_after = 1;
};

// Days since 1970-01-01 for an ISO date "YYYY-MM-DD".
long days_from_iso(const std::string& iso);

// Fraction of `history` values <= x (0 when history is empty).
double percentile_rank(const std::vector<double>& history, double x);

Gate trend_gate(const Series& index, std::size_t i, std::size_t sma_n);
// Realised vol of `s` at i vs its own trailing distribution.
Gate realized_vol_gate(const Series& s, std::size_t i, const RegimeConfig& cfg);
// Dated volatility index (e.g. India VIX close) vs its own trailing distribution; uses
// only values dated on or before `date`.
Gate vix_gate(const std::map<std::string, double>& vix, const std::string& date, const RegimeConfig& cfg);
Gate crash_gate(const Series& index, std::size_t i, const RegimeConfig& cfg);
// Red when annualised funding > threshold and open interest is rising. If open interest
// is unknown (NaN), funding alone decides -- the conservative choice.
Gate crowding_gate(double funding_annualized, double oi_now, double oi_prev, const RegimeConfig& cfg);

// Scheduled binary events (elections, Union Budget, RBI policy, FOMC, CPI...).
class EventCalendar {
public:
    static EventCalendar load(const std::string& path);  // "date,tag" lines
    void add(const std::string& date, const std::string& tag) { events_[date].insert(tag); }
    bool blackout(const std::string& date, int days_before, int days_after) const;

private:
    std::map<std::string, std::set<std::string>> events_;
};

// Equity-market regime for one date.
struct EquityRegime {
    Gate trend = Gate::Unknown;
    Gate vol = Gate::Unknown;
    Gate crash = Gate::Unknown;
    bool event_blackout = false;

    // Sleeve A actions from the blueprint's gate table.
    bool allow_new_entries() const {
        return trend != Gate::Red && crash != Gate::Red && !event_blackout;
    }
    double risk_multiplier() const { return vol == Gate::Red ? 0.5 : 1.0; }
};

struct EquityRegimeInputs {
    const Series* index = nullptr;                   // e.g. Nifty 50 daily
    const std::map<std::string, double>* vix = nullptr;  // India VIX close
    const EventCalendar* events = nullptr;
};

// Missing inputs leave that gate Unknown, which does not block trading.
EquityRegime evaluate_equity_regime(const EquityRegimeInputs& in, const std::string& date,
                                    const RegimeConfig& cfg);

}  // namespace ta
