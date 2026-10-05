#include "ta/risk.hpp"

#include <algorithm>
#include <cmath>

namespace ta {

RiskManager::RiskManager(RiskConfig cfg) : cfg_(cfg) {}

void RiskManager::start_day(double equity) {
    day_start_equity_ = equity;
    if (high_water_ == 0) high_water_ = equity;
    last_equity_ = equity;
}

void RiskManager::end_day(double equity) {
    last_equity_ = equity;
    high_water_ = std::max(high_water_, equity);
    if (state_ == RiskState::Off) return;  // latched until manual_reset()
    const double dd = drawdown();
    if (dd >= cfg_.drawdown_off) {
        state_ = RiskState::Off;
    } else if (dd >= cfg_.drawdown_halve) {
        state_ = RiskState::Halved;
    } else {
        state_ = RiskState::Normal;
    }
}

double RiskManager::drawdown() const {
    if (high_water_ <= 0) return 0;
    return 1.0 - last_equity_ / high_water_;
}

bool RiskManager::daily_loss_breached(double equity_now) const {
    return day_start_equity_ > 0 && equity_now < day_start_equity_ * (1.0 - cfg_.daily_loss_limit);
}

bool RiskManager::allows_new_entries(double equity_now) const {
    return state_ != RiskState::Off && !daily_loss_breached(equity_now);
}

SizeDecision RiskManager::size_long(double equity, double cash, double gross_exposure, double entry,
                                    double stop, double adr, double round_trip_cost_frac) const {
    if (state_ == RiskState::Off) return {0, "risk_off"};
    if (!(entry > 0) || !(stop > 0) || stop >= entry) return {0, "invalid_stop"};
    if (!(adr > 0)) return {0, "invalid_adr"};

    const double risk_per_share = entry - stop;
    if (risk_per_share / entry > cfg_.max_stop_adr_mult * adr + 1e-12) return {0, "stop_too_wide"};
    if (round_trip_cost_frac * entry > cfg_.max_cost_to_r * risk_per_share) {
        return {0, "cost_exceeds_r_limit"};
    }

    double risk_frac = std::min(cfg_.risk_per_trade, cfg_.max_risk_per_trade);
    if (state_ == RiskState::Halved) risk_frac *= 0.5;

    const double by_risk = std::floor(equity * risk_frac / risk_per_share);
    const double by_position = std::floor(equity * cfg_.max_position_frac / entry);
    const double by_leverage =
        std::floor(std::max(0.0, equity * cfg_.max_gross_leverage - gross_exposure) / entry);
    const double by_cash = std::floor(std::max(0.0, cash) / (entry * (1.0 + round_trip_cost_frac)));

    const long qty = static_cast<long>(std::min({by_risk, by_position, by_leverage, by_cash}));
    if (qty <= 0) return {0, "size_zero"};
    return {qty, ""};
}

void RiskManager::manual_reset() {
    state_ = RiskState::Normal;
    high_water_ = last_equity_;
}

const char* to_string(RiskState s) {
    switch (s) {
        case RiskState::Normal: return "normal";
        case RiskState::Halved: return "halved";
        case RiskState::Off: return "off";
    }
    return "?";
}

}  // namespace ta
