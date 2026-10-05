#include <fstream>
#include <limits>

#include "harness.hpp"
#include "ta/portfolio.hpp"
#include "ta/regime.hpp"

namespace {
ta::Series trend_series(int n, double start, double step) {
    ta::Series s;
    double c = start;
    for (int k = 0; k < n; ++k, c *= step) s.push_back({th::iso_day(k), c, c * 1.01, c * 0.99, c, 1});
    return s;
}
}  // namespace

TEST(days_from_iso_matches_known_dates) {
    CHECK(ta::days_from_iso("1970-01-01") == 0);
    CHECK(ta::days_from_iso("2000-03-01") - ta::days_from_iso("2000-02-28") == 2);  // leap year
    CHECK(ta::days_from_iso("2024-01-01") == 19723);
}

TEST(trend_gate_follows_200dma) {
    const auto up = trend_series(260, 100, 1.001);
    CHECK(ta::trend_gate(up, 259, 200) == ta::Gate::Green);
    const auto down = trend_series(260, 100, 0.999);
    CHECK(ta::trend_gate(down, 259, 200) == ta::Gate::Red);
    CHECK(ta::trend_gate(up, 100, 200) == ta::Gate::Unknown);
}

TEST(vix_gate_uses_only_past_values) {
    std::map<std::string, double> vix;
    for (int k = 0; k < 300; ++k) vix[th::iso_day(k)] = 12 + (k % 5);
    vix[th::iso_day(299)] = 13;  // an ordinary day
    ta::RegimeConfig cfg;
    CHECK(ta::vix_gate(vix, th::iso_day(299), cfg) == ta::Gate::Green);
    vix[th::iso_day(300)] = 40;
    CHECK(ta::vix_gate(vix, th::iso_day(300), cfg) == ta::Gate::Red);
    CHECK(ta::vix_gate(vix, th::iso_day(299), cfg) == ta::Gate::Green);  // future spike ignored
}

TEST(crash_gate_flags_fall_then_rebound) {
    ta::Series s = trend_series(100, 100, 1.0);
    double c = 100;
    for (int k = 100; k < 140; ++k, c *= 0.99) s.push_back({th::iso_day(k), c, c, c, c, 1});
    for (int k = 140; k < 161; ++k, c *= 1.006) s.push_back({th::iso_day(k), c, c, c, c, 1});
    ta::RegimeConfig cfg;
    CHECK(ta::crash_gate(s, s.size() - 1, cfg) == ta::Gate::Red);
    CHECK(ta::crash_gate(s, 99, cfg) == ta::Gate::Green);
}

TEST(crowding_gate_rules) {
    ta::RegimeConfig cfg;
    const double nan = std::numeric_limits<double>::quiet_NaN();
    CHECK(ta::crowding_gate(0.10, 1, 0.5, cfg) == ta::Gate::Green);
    CHECK(ta::crowding_gate(0.35, 2, 1, cfg) == ta::Gate::Red);
    CHECK(ta::crowding_gate(0.35, 1, 2, cfg) == ta::Gate::Green);
    CHECK(ta::crowding_gate(0.35, nan, nan, cfg) == ta::Gate::Red);
}

TEST(event_calendar_calendar_day_window) {
    ta::EventCalendar cal;
    cal.add("2026-02-01", "budget");
    ta::RegimeConfig cfg;
    cfg.event_trading_days = false;
    CHECK(cal.blackout("2026-01-31", cfg));
    CHECK(cal.blackout("2026-02-02", cfg));
    CHECK(!cal.blackout("2026-02-03", cfg));
}

TEST(event_blackout_counts_trading_sessions) {
    // 2009 election result on Saturday 16 May; the +17.7% session was Monday 18 May.
    const std::vector<std::string> sessions = {"2009-05-11", "2009-05-12", "2009-05-13", "2009-05-14",
                                               "2009-05-15", "2009-05-18", "2009-05-19", "2009-05-20",
                                               "2009-05-21", "2009-05-22", "2009-05-25"};
    ta::EventCalendar cal;
    cal.add("2009-05-16", "General election result");
    ta::RegimeConfig cfg;  // election window: 5 sessions before, 3 after
    CHECK(cal.blackout("2009-05-18", cfg, &sessions));  // Monday is blocked
    CHECK(cal.blackout("2009-05-11", cfg, &sessions));  // 5 sessions before
    CHECK(cal.blackout("2009-05-21", cfg, &sessions));  // 3 sessions after
    CHECK(!cal.blackout("2009-05-22", cfg, &sessions));

    ta::EventCalendar rbi;
    rbi.add("2009-05-13", "RBI policy");  // 0 before, 1 after
    CHECK(!rbi.blackout("2009-05-12", cfg, &sessions));
    CHECK(rbi.blackout("2009-05-13", cfg, &sessions));
    CHECK(rbi.blackout("2009-05-14", cfg, &sessions));
    CHECK(!rbi.blackout("2009-05-15", cfg, &sessions));

    // Calendar-day counting (the old behaviour) would leave Monday open after a Saturday event.
    ta::RegimeConfig cal_days;
    cal_days.event_trading_days = false;
    cal_days.event_after_by_tag["election"] = 1;
    CHECK(!cal.blackout("2009-05-18", cal_days, &sessions));
}

TEST(equity_regime_actions) {
    const auto down = trend_series(260, 100, 0.999);
    ta::EquityRegimeInputs in;
    in.index = &down;
    const auto r = ta::evaluate_equity_regime(in, th::iso_day(259), ta::RegimeConfig{});
    CHECK(r.trend == ta::Gate::Red);
    CHECK(!r.allow_new_entries());
    const auto none = ta::evaluate_equity_regime({}, th::iso_day(259), ta::RegimeConfig{});
    CHECK(none.allow_new_entries() && none.risk_multiplier() == 1.0);
}

TEST(allocation_validation_and_scaling) {
    ta::Allocation a;
    CHECK(a.validate().empty());
    CHECK_NEAR(a.active_share(), 0.25, 1e-12);
    a.a2 = 0.05;
    a.core = 0.70;
    CHECK(!a.validate().empty());  // A1 + A2 = 15% > 13%
    // 0.40% of E_A for a sleeve holding 40% of E_A is 1% of the sleeve.
    CHECK_NEAR(ta::to_sleeve_fraction(0.004, 250000, 100000), 0.01, 1e-12);
}
