#include "ta/settings.hpp"

#include <set>
#include <stdexcept>

namespace ta {

namespace {

void risk_keys(const Config& c, const std::string& p, RiskConfig& r) {
    config_set(c, p + "risk_per_trade", r.risk_per_trade);
    config_set(c, p + "max_position_frac", r.max_position_frac);
    config_set(c, p + "max_positions", r.max_positions);
    config_set(c, p + "max_stop_adr_mult", r.max_stop_adr_mult);
    config_set(c, p + "max_cost_to_r", r.max_cost_to_r);
    config_set(c, p + "daily_loss_limit", r.daily_loss_limit);
    config_set(c, p + "drawdown_halve", r.drawdown_halve);
    config_set(c, p + "drawdown_off", r.drawdown_off);
    config_set(c, p + "max_gross_leverage", r.max_gross_leverage);
}

void cost_keys(const Config& c, const std::string& p, CostModel& m) {
    double rt = -1;
    config_set(c, p + "cost_round_trip", rt);
    if (rt >= 0) m.buy_frac = m.sell_frac = rt / 2;
}

}  // namespace

void apply_settings(const Config& c, Allocation& a) {
    config_set(c, "alloc.core", a.core);
    config_set(c, "alloc.buffer", a.buffer);
    config_set(c, "alloc.a1", a.a1);
    config_set(c, "alloc.a2", a.a2);
    config_set(c, "alloc.b", a.b);
    config_set(c, "alloc.c", a.c);
    config_set(c, "alloc.d", a.d);
    config_set(c, "alloc.reserve", a.reserve);
}

void apply_settings(const Config& c, A1Config& a1) {
    risk_keys(c, "a1.", a1.risk);
    cost_keys(c, "a1.", a1.cost);
    config_set(c, "a1.min_price", a1.screen.min_price);
    config_set(c, "a1.min_avg_traded_value", a1.screen.min_avg_traded_value);
    config_set(c, "a1.top_fraction", a1.screen.top_fraction);
    config_set(c, "a1.min_runup", a1.screen.min_runup);
    config_set(c, "a1.min_adr", a1.screen.min_adr);
    config_set(c, "a1.base_len", a1.screen.base_len);
    config_set(c, "a1.max_tightness", a1.screen.max_tightness);
    config_set(c, "a1.max_below_pivot", a1.screen.max_below_pivot);
    config_set(c, "a1.entry_limit_frac", a1.entry_limit_frac);
    config_set(c, "a1.stop_adr_mult", a1.stop_adr_mult);
    config_set(c, "a1.partial_day", a1.exits.partial_day);
    config_set(c, "a1.time_stop_day", a1.exits.time_stop_day);
    config_set(c, "a1.max_hold_days", a1.exits.max_hold_days);
    config_set(c, "regime.trend_sma", a1.regime.trend_sma);
    config_set(c, "regime.vol_red_pct", a1.regime.vol_red_pct);
    config_set(c, "regime.event_days_before", a1.regime.event_days_before);
    config_set(c, "regime.event_days_after", a1.regime.event_days_after);
}

void apply_settings(const Config& c, A2Config& a2) {
    risk_keys(c, "a2.", a2.risk);
    cost_keys(c, "a2.", a2.cost);
    config_set(c, "a2.min_gap", a2.min_gap);
    config_set(c, "a2.window_end", a2.window_end);
    config_set(c, "a2.approve_pct", a2.approve_pct);
    config_set(c, "a2.min_history_events", a2.min_history_events);
    config_set(c, "a2.max_entries_per_day", a2.max_entries_per_day);
    config_set(c, "a2.max_hold_days", a2.exits.max_hold_days);
}

void apply_settings(const Config& c, CryptoTrendConfig& b) {
    risk_keys(c, "b.", b.risk);
    config_set(c, "b.vol_target", b.vol_target);
    config_set(c, "b.asset_cap", b.asset_cap);
    config_set(c, "b.rebalance_band", b.rebalance_band);
    config_set(c, "b.cost_bps", b.cost_bps);
    config_set(c, "b.tax_rate", b.tax_rate);
    config_set(c, "b.funding_annual_default", b.funding_annual_default);
    config_set(c, "regime.funding_red_annual", b.regime.funding_red_annual);
}

void apply_settings(const Config& c, D1Config& d1) {
    risk_keys(c, "d1.", d1.risk);
    config_set(c, "d1.pct_threshold", d1.pct_threshold);
    config_set(c, "d1.stop_frac", d1.stop_frac);
    config_set(c, "d1.lot_size", d1.lot_size);
    config_set(c, "d1.cost_points", d1.cost_points);
    config_set(c, "d1.min_sleeve_equity", d1.min_sleeve_equity);
}

void apply_settings(const Config& c, OptionsConfig& o) {
    config_set(c, "c.budget_frac", o.budget_frac);
    config_set(c, "c.cost_per_leg_lot", o.cost_per_leg_lot);
}

std::vector<std::pair<std::string, std::string>> default_settings() {
    const Allocation a;
    const A1Config a1;
    const A2Config a2;
    const CryptoTrendConfig b;
    const D1Config d1;
    const OptionsConfig o;
    auto f = [](double v) { return fmt_double(v); };
    std::vector<std::pair<std::string, std::string>> out = {
        {"alloc.core", f(a.core)}, {"alloc.buffer", f(a.buffer)}, {"alloc.a1", f(a.a1)}, {"alloc.a2", f(a.a2)},
        {"alloc.b", f(a.b)}, {"alloc.c", f(a.c)}, {"alloc.d", f(a.d)}, {"alloc.reserve", f(a.reserve)},
    };
    auto risk = [&](const std::string& p, const RiskConfig& r) {
        out.insert(out.end(), {{p + "risk_per_trade", f(r.risk_per_trade)},
                               {p + "max_position_frac", f(r.max_position_frac)},
                               {p + "max_positions", std::to_string(r.max_positions)},
                               {p + "max_stop_adr_mult", f(r.max_stop_adr_mult)},
                               {p + "max_cost_to_r", f(r.max_cost_to_r)},
                               {p + "daily_loss_limit", f(r.daily_loss_limit)},
                               {p + "drawdown_halve", f(r.drawdown_halve)},
                               {p + "drawdown_off", f(r.drawdown_off)},
                               {p + "max_gross_leverage", f(r.max_gross_leverage)}});
    };
    risk("a1.", a1.risk);
    out.insert(out.end(), {{"a1.cost_round_trip", f(a1.cost.round_trip())},
                           {"a1.min_price", f(a1.screen.min_price)},
                           {"a1.min_avg_traded_value", f(a1.screen.min_avg_traded_value)},
                           {"a1.top_fraction", f(a1.screen.top_fraction)},
                           {"a1.min_runup", f(a1.screen.min_runup)},
                           {"a1.min_adr", f(a1.screen.min_adr)},
                           {"a1.base_len", std::to_string(a1.screen.base_len)},
                           {"a1.max_tightness", f(a1.screen.max_tightness)},
                           {"a1.max_below_pivot", f(a1.screen.max_below_pivot)},
                           {"a1.entry_limit_frac", f(a1.entry_limit_frac)},
                           {"a1.stop_adr_mult", f(a1.stop_adr_mult)},
                           {"a1.partial_day", std::to_string(a1.exits.partial_day)},
                           {"a1.time_stop_day", std::to_string(a1.exits.time_stop_day)},
                           {"a1.max_hold_days", std::to_string(a1.exits.max_hold_days)},
                           {"regime.trend_sma", std::to_string(a1.regime.trend_sma)},
                           {"regime.vol_red_pct", f(a1.regime.vol_red_pct)},
                           {"regime.event_days_before", std::to_string(a1.regime.event_days_before)},
                           {"regime.event_days_after", std::to_string(a1.regime.event_days_after)},
                           {"regime.funding_red_annual", f(b.regime.funding_red_annual)}});
    risk("a2.", a2.risk);
    out.insert(out.end(), {{"a2.cost_round_trip", f(a2.cost.round_trip())},
                           {"a2.min_gap", f(a2.min_gap)},
                           {"a2.window_end", a2.window_end},
                           {"a2.approve_pct", f(a2.approve_pct)},
                           {"a2.min_history_events", std::to_string(a2.min_history_events)},
                           {"a2.max_entries_per_day", std::to_string(a2.max_entries_per_day)},
                           {"a2.max_hold_days", std::to_string(a2.exits.max_hold_days)}});
    risk("b.", b.risk);
    out.insert(out.end(), {{"b.vol_target", f(b.vol_target)},
                           {"b.asset_cap", f(b.asset_cap)},
                           {"b.rebalance_band", f(b.rebalance_band)},
                           {"b.cost_bps", f(b.cost_bps)},
                           {"b.tax_rate", f(b.tax_rate)},
                           {"b.funding_annual_default", f(b.funding_annual_default)}});
    risk("d1.", d1.risk);
    out.insert(out.end(), {{"d1.pct_threshold", f(d1.pct_threshold)},
                           {"d1.stop_frac", f(d1.stop_frac)},
                           {"d1.lot_size", f(d1.lot_size)},
                           {"d1.cost_points", f(d1.cost_points)},
                           {"d1.min_sleeve_equity", f(d1.min_sleeve_equity)},
                           {"c.budget_frac", f(o.budget_frac)},
                           {"c.cost_per_leg_lot", f(o.cost_per_leg_lot)}});
    return out;
}

void check_settings_keys(const Config& c) {
    std::set<std::string> known;
    for (const auto& [k, v] : default_settings()) known.insert(k);
    for (const auto& [k, v] : c) {
        // Account and data keys are handled by the paper runner itself.
        if (k.rfind("data.", 0) == 0 || k.rfind("account.", 0) == 0 || k.rfind("paper.", 0) == 0 ||
            k.rfind("kill.", 0) == 0) {
            continue;
        }
        if (!known.count(k)) throw std::runtime_error("unknown config key: " + k);
    }
}

}  // namespace ta
