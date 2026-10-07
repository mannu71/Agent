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
    config_set(c, "a1.entry_mode", a1.entry_mode);
    if (a1.entry_mode != "stop" && a1.entry_mode != "close") throw std::runtime_error("a1.entry_mode must be stop or close");
    config_set(c, "a1.confirm_close_pos", a1.confirm_close_pos);
    config_set(c, "a1.confirm_vol_mult", a1.confirm_vol_mult);
    config_set(c, "a1.partial_day", a1.exits.partial_day);
    config_set(c, "a1.time_stop_day", a1.exits.time_stop_day);
    config_set(c, "a1.max_hold_days", a1.exits.max_hold_days);
    config_set(c, "a1.partial_frac", a1.exits.partial_frac);
    std::string runner = a1.exits.runner_atr ? "atr" : "sma";
    config_set(c, "a1.runner_trail", runner);
    if (runner != "atr" && runner != "sma") throw std::runtime_error("a1.runner_trail must be sma or atr");
    a1.exits.runner_atr = runner == "atr";
    config_set(c, "a1.runner_atr_n", a1.exits.runner_atr_n);
    config_set(c, "a1.runner_atr_k", a1.exits.runner_atr_k);
    config_set(c, "a1.mom_skip_days", a1.screen.mom_skip_days);
    config_set(c, "a1.vol_scale", a1.vol_scale);
    config_set(c, "a1.vol_target", a1.vol_target);
    config_set(c, "a1.vol_n", a1.vol_n);
    config_set(c, "a1.vol_scale_cap", a1.vol_scale_cap);
    apply_settings(c, a1.regime);
}

void apply_settings(const Config& c, RegimeConfig& r) {
    config_set(c, "regime.trend_sma", r.trend_sma);
    config_set(c, "regime.vol_lookback", r.vol_lookback);
    config_set(c, "regime.vol_red_pct", r.vol_red_pct);
    config_set(c, "regime.realized_vol_n", r.realized_vol_n);
    config_set(c, "regime.crash_fall", r.crash_fall);
    config_set(c, "regime.crash_rebound", r.crash_rebound);
    config_set(c, "regime.crash_bear_lookback", r.crash_bear_lookback);
    config_set(c, "regime.crash_vol_n", r.crash_vol_n);
    config_set(c, "regime.crash_vol_pct", r.crash_vol_pct);
    config_set(c, "regime.funding_red_annual", r.funding_red_annual);
    config_set(c, "regime.event_days_before", r.event_days_before);
    config_set(c, "regime.event_days_after", r.event_days_after);
    config_set(c, "regime.event_trading_days", r.event_trading_days);
    // Per-event-type windows: regime.event_days_before.<tag> / regime.event_days_after.<tag>
    const std::string before = "regime.event_days_before.", after = "regime.event_days_after.";
    for (const auto& [k, v] : c) {
        if (k.rfind(before, 0) == 0) r.event_before_by_tag[k.substr(before.size())] = std::stoi(v);
        if (k.rfind(after, 0) == 0) r.event_after_by_tag[k.substr(after.size())] = std::stoi(v);
    }
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
    config_set(c, "a2.weakness_time", a2.weakness_time);
    config_set(c, "a2.require_catalyst", a2.require_catalyst);
    config_set(c, "a2.skip_after_gap_day", a2.skip_after_gap_day);
    config_set(c, "a2.min_rv", a2.min_rv);
    config_set(c, "a2.partial_mode", a2.partial_mode);
    a2.apply_partial_mode();  // validates the mode
}

void apply_settings(const Config& c, CryptoTrendConfig& b) {
    risk_keys(c, "b.", b.risk);
    config_set(c, "b.vol_target", b.vol_target);
    config_set(c, "b.asset_cap", b.asset_cap);
    config_set(c, "b.rebalance_band", b.rebalance_band);
    config_set(c, "b.cost_bps", b.cost_bps);
    config_set(c, "b.tax_rate", b.tax_rate);
    config_set(c, "b.funding_annual_default", b.funding_annual_default);
    apply_settings(c, b.regime);
}

void apply_settings(const Config& c, D1Config& d1) {
    risk_keys(c, "d1.", d1.risk);
    config_set(c, "d1.pct_threshold", d1.pct_threshold);
    config_set(c, "d1.stop_frac", d1.stop_frac);
    config_set(c, "d1.lot_size", d1.lot_size);
    config_set(c, "d1.cost_frac", d1.cost_frac);
    config_set(c, "d1.slippage_points", d1.slippage_points);
    config_set(c, "d1.min_beta", d1.min_beta);
    config_set(c, "d1.min_sleeve_equity", d1.min_sleeve_equity);
    config_set(c, "d1.min_history", d1.min_history);
}

void apply_settings(const Config& c, OptionsConfig& o) {
    config_set(c, "c.budget_frac", o.budget_frac);
    config_set(c, "c.cost_per_leg_lot", o.cost_per_leg_lot);
}

void apply_settings(const Config& c, ConceptConfig& s) {
    config_set(c, "smc.swing_n", s.swing_n);
    config_set(c, "smc.atr_n", s.atr_n);
    config_set(c, "smc.rr", s.rr);
    config_set(c, "smc.stop_buffer_atr", s.stop_buffer_atr);
    config_set(c, "smc.min_risk_frac", s.min_risk_frac);
    config_set(c, "smc.hold_bars", s.hold_bars);
    config_set(c, "smc.order_expiry_bars", s.order_expiry_bars);
    config_set(c, "smc.sweep_lookback", s.sweep_lookback);
    config_set(c, "smc.fvg_min_atr", s.fvg_min_atr);
    config_set(c, "smc.displacement_atr", s.displacement_atr);
    config_set(c, "smc.range_len", s.range_len);
    config_set(c, "smc.range_max_atr", s.range_max_atr);
    config_set(c, "smc.climax_vol_mult", s.climax_vol_mult);
    config_set(c, "smc.value_area", s.value_area);
    config_set(c, "smc.profile_bins", s.profile_bins);
}

std::vector<std::pair<std::string, std::string>> default_settings() {
    const Allocation a;
    const A1Config a1;
    const A2Config a2;
    const CryptoTrendConfig b;
    const D1Config d1;
    const OptionsConfig o;
    const ConceptConfig sc;
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
                           {"a1.entry_mode", a1.entry_mode},
                           {"a1.confirm_close_pos", f(a1.confirm_close_pos)},
                           {"a1.confirm_vol_mult", f(a1.confirm_vol_mult)},
                           {"a1.partial_day", std::to_string(a1.exits.partial_day)},
                           {"a1.time_stop_day", std::to_string(a1.exits.time_stop_day)},
                           {"a1.max_hold_days", std::to_string(a1.exits.max_hold_days)},
                           {"a1.partial_frac", f(a1.exits.partial_frac)},
                           {"a1.runner_trail", a1.exits.runner_atr ? "atr" : "sma"},
                           {"a1.runner_atr_n", std::to_string(a1.exits.runner_atr_n)},
                           {"a1.runner_atr_k", f(a1.exits.runner_atr_k)},
                           {"a1.mom_skip_days", std::to_string(a1.screen.mom_skip_days)},
                           {"a1.vol_scale", a1.vol_scale ? "1" : "0"},
                           {"a1.vol_target", f(a1.vol_target)},
                           {"a1.vol_n", std::to_string(a1.vol_n)},
                           {"a1.vol_scale_cap", f(a1.vol_scale_cap)}});
    const RegimeConfig& rg = a1.regime;
    out.insert(out.end(), {{"regime.trend_sma", std::to_string(rg.trend_sma)},
                           {"regime.vol_lookback", std::to_string(rg.vol_lookback)},
                           {"regime.vol_red_pct", f(rg.vol_red_pct)},
                           {"regime.realized_vol_n", std::to_string(rg.realized_vol_n)},
                           {"regime.crash_fall", f(rg.crash_fall)},
                           {"regime.crash_rebound", f(rg.crash_rebound)},
                           {"regime.crash_bear_lookback", std::to_string(rg.crash_bear_lookback)},
                           {"regime.crash_vol_n", std::to_string(rg.crash_vol_n)},
                           {"regime.crash_vol_pct", f(rg.crash_vol_pct)},
                           {"regime.funding_red_annual", f(rg.funding_red_annual)},
                           {"regime.event_days_before", std::to_string(rg.event_days_before)},
                           {"regime.event_days_after", std::to_string(rg.event_days_after)},
                           {"regime.event_trading_days", rg.event_trading_days ? "1" : "0"}});
    for (const auto& [tag, d] : rg.event_before_by_tag) out.emplace_back("regime.event_days_before." + tag, std::to_string(d));
    for (const auto& [tag, d] : rg.event_after_by_tag) out.emplace_back("regime.event_days_after." + tag, std::to_string(d));
    risk("a2.", a2.risk);
    out.insert(out.end(), {{"a2.cost_round_trip", f(a2.cost.round_trip())},
                           {"a2.min_gap", f(a2.min_gap)},
                           {"a2.window_end", a2.window_end},
                           {"a2.approve_pct", f(a2.approve_pct)},
                           {"a2.min_history_events", std::to_string(a2.min_history_events)},
                           {"a2.max_entries_per_day", std::to_string(a2.max_entries_per_day)},
                           {"a2.max_hold_days", std::to_string(a2.exits.max_hold_days)},
                           {"a2.weakness_time", a2.weakness_time},
                           {"a2.require_catalyst", a2.require_catalyst ? "1" : "0"},
                           {"a2.skip_after_gap_day", a2.skip_after_gap_day ? "1" : "0"},
                           {"a2.min_rv", f(a2.min_rv)},
                           {"a2.partial_mode", a2.partial_mode}});
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
                           {"d1.cost_frac", f(d1.cost_frac)},
                           {"d1.slippage_points", f(d1.slippage_points)},
                           {"d1.min_beta", f(d1.min_beta)},
                           {"d1.min_sleeve_equity", f(d1.min_sleeve_equity)},
                           {"d1.min_history", std::to_string(d1.min_history)},
                           {"smc.swing_n", std::to_string(sc.swing_n)},
                           {"smc.atr_n", std::to_string(sc.atr_n)},
                           {"smc.rr", f(sc.rr)},
                           {"smc.stop_buffer_atr", f(sc.stop_buffer_atr)},
                           {"smc.min_risk_frac", f(sc.min_risk_frac)},
                           {"smc.hold_bars", std::to_string(sc.hold_bars)},
                           {"smc.order_expiry_bars", std::to_string(sc.order_expiry_bars)},
                           {"smc.sweep_lookback", std::to_string(sc.sweep_lookback)},
                           {"smc.fvg_min_atr", f(sc.fvg_min_atr)},
                           {"smc.displacement_atr", f(sc.displacement_atr)},
                           {"smc.range_len", std::to_string(sc.range_len)},
                           {"smc.range_max_atr", f(sc.range_max_atr)},
                           {"smc.climax_vol_mult", f(sc.climax_vol_mult)},
                           {"smc.value_area", f(sc.value_area)},
                           {"smc.profile_bins", std::to_string(sc.profile_bins)},
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
            k.rfind("kill.", 0) == 0 || k.rfind("regime.event_days_before.", 0) == 0 ||
            k.rfind("regime.event_days_after.", 0) == 0) {
            continue;
        }
        if (!known.count(k)) throw std::runtime_error("unknown config key: " + k);
    }
}

}  // namespace ta
