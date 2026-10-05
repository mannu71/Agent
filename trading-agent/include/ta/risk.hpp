#pragma once

#include <string>

#include "ta/kv.hpp"

namespace ta {

// Hard limits from the blueprint / rulebook. Defaults are judgment-based, not
// evidence-derived optima -- change them in one place, here.
struct RiskConfig {
    double risk_per_trade = 0.004;      // 0.40% of sleeve equity at risk to the stop
    double max_risk_per_trade = 0.01;   // absolute ceiling, whatever the config says
    double max_position_frac = 0.05;    // one stock <= 5% of sleeve equity
    int max_positions = 12;
    double max_stop_adr_mult = 1.0;     // skip if stop distance > 1.0 x ADR20
    double max_cost_to_r = 0.25;        // skip if round-trip cost > 0.25R
    double daily_loss_limit = 0.01;     // -1% on the day -> no new entries today
    double drawdown_halve = 0.075;      // -7.5% from high -> half risk
    double drawdown_off = 0.15;         // -15% from high -> off until a human resets
    double max_gross_leverage = 1.0;    // cash equity: no leverage
};

enum class RiskState { Normal, Halved, Off };

struct SizeDecision {
    long qty = 0;
    std::string reject_reason;  // empty when qty > 0
};

// Deterministic risk gate. No forecasting, no ML: every decision is a pure
// function of the config and the numbers passed in.
class RiskManager {
public:
    explicit RiskManager(RiskConfig cfg);

    void start_day(double equity);
    void end_day(double equity);

    RiskState state() const { return state_; }
    double drawdown() const;
    bool daily_loss_breached(double equity_now) const;
    bool allows_new_entries(double equity_now) const;

    // Units to buy for a long entry at `entry` with protective stop `stop` < entry.
    // `gross_exposure` is the current market value of open positions. `multiplier`
    // scales risk down further (active-book breaker, volatility gate); it never scales up.
    SizeDecision size_long(double equity, double cash, double gross_exposure, double entry,
                           double stop, double adr, double round_trip_cost_frac,
                           double multiplier = 1.0) const;
    // Same for a short entry, with stop > entry.
    SizeDecision size_short(double equity, double cash, double gross_exposure, double entry,
                            double stop, double adr, double round_trip_cost_frac,
                            double multiplier = 1.0) const;

    // Clears the Off latch. Only for a human after reviewing what went wrong.
    void manual_reset();

    const RiskConfig& config() const { return cfg_; }

    // Persistence for paper trading.
    KvRecord snapshot() const;
    void restore(const KvRecord& r);

private:
    SizeDecision size_units(double equity, double cash, double gross_exposure, double entry,
                            double risk_per_unit, double adr, double round_trip_cost_frac,
                            double multiplier) const;

    RiskConfig cfg_;
    RiskState state_ = RiskState::Normal;
    double high_water_ = 0;
    double last_equity_ = 0;
    double day_start_equity_ = 0;
};

const char* to_string(RiskState s);

}  // namespace ta
