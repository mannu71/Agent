#include <cmath>
#include <limits>

#include "harness.hpp"
#include "ta/d1.hpp"
#include "ta/options.hpp"

namespace {

// 5-minute futures bars 09:15-15:25; the day drifts by `drift` before 15:00 and by
// `late` after 15:00.
void add_day(ta::Series& s, const std::string& date, double& px, double drift, double late,
             double low_after_1500 = 0) {
    for (int m = 9 * 60 + 15; m <= 15 * 60 + 25; m += 5) {
        char ts[32];
        std::snprintf(ts, sizeof ts, "%s %02d:%02d", date.c_str(), m / 60, m % 60);
        const bool after = m >= 15 * 60;
        const double step = after ? late / 6.0 : drift / 69.0;
        const double o = px;
        px *= 1.0 + step;
        double lo = std::min(o, px) * 0.9999;
        if (after && low_after_1500 > 0) lo = std::min(lo, low_after_1500);
        s.push_back({ts, o, std::max(o, px) * 1.0001, lo, px, 1000});
    }
}

}  // namespace

TEST(d1_trades_big_moves_in_signal_direction) {
    ta::Series s;
    double px = 23000;
    for (int k = 0; k < 120; ++k) add_day(s, th::iso_day(k), px, k % 2 ? 0.001 : -0.001, 0.0);
    add_day(s, th::iso_day(120), px, 0.02, 0.003);  // +2% by 15:00: top of the distribution
    ta::D1Config cfg;
    const auto r = ta::run_d1_backtest(s, cfg, 2e6, th::iso_day(120));
    CHECK(r.trades.size() == 1);
    if (r.trades.empty()) return;
    CHECK(r.trades[0].side == 1);
    CHECK(r.trades[0].exit_reason == "time_exit");
    CHECK(r.trades[0].pnl > 0);
}

TEST(d1_skips_small_moves_and_small_accounts) {
    ta::Series s;
    double px = 23000;
    for (int k = 0; k < 120; ++k) add_day(s, th::iso_day(k), px, k % 2 ? 0.01 : -0.01, 0.0);
    add_day(s, th::iso_day(120), px, 0.0005, 0.0);  // tiny move: below the 70th percentile
    ta::D1Config cfg;
    CHECK(ta::run_d1_backtest(s, cfg, 2e6, th::iso_day(120)).trades.empty());

    ta::Series big;
    px = 23000;
    for (int k = 0; k < 120; ++k) add_day(big, th::iso_day(k), px, k % 2 ? 0.001 : -0.001, 0.0);
    add_day(big, th::iso_day(120), px, 0.02, 0.0);
    CHECK(ta::run_d1_backtest(big, cfg, 1e6, th::iso_day(120)).trades.empty());  // below Rs 15 lakh
}

TEST(d1_stop_caps_the_loss) {
    ta::Series s;
    double px = 23000;
    for (int k = 0; k < 120; ++k) add_day(s, th::iso_day(k), px, k % 2 ? 0.001 : -0.001, 0.0);
    add_day(s, th::iso_day(120), px, 0.02, -0.03);  // reverses hard after 15:00
    const auto r = ta::run_d1_backtest(s, ta::D1Config{}, 2e6, th::iso_day(120));
    CHECK(r.trades.size() == 1);
    if (r.trades.empty()) return;
    CHECK(r.trades[0].exit_reason == "stop");
    CHECK(r.trades[0].r_multiple > -1.5);
}

TEST(options_iron_condor_payoff) {
    const auto s = ta::parse_structure("P:22000:-1:85,P:21800:1:40,C:24000:-1:70,C:24200:1:30", "2026-10-13", 65);
    CHECK_NEAR(ta::net_credit(s), (85 - 40 + 70 - 30) * 65.0, 1e-9);
    CHECK_NEAR(ta::max_profit(s), 85 * 65.0, 1e-9);
    CHECK_NEAR(ta::max_loss(s), (200 - 85) * 65.0, 1e-9);
    const auto be = ta::breakevens(s);
    CHECK(be.size() == 2);
    if (be.size() == 2) {
        CHECK_NEAR(be[0], 22000 - 85, 1e-9);
        CHECK_NEAR(be[1], 24000 + 85, 1e-9);
    }
}

TEST(options_rejects_naked_and_expiry_day) {
    const auto naked = ta::parse_structure("C:24000:-1:70", "2026-10-13", 65);
    CHECK(!std::isfinite(ta::max_loss(naked)));
    ta::OptionsContext ctx;
    ctx.today = "2026-10-06";
    ctx.active_book = 2.5e6;
    auto c = ta::check_structure(naked, ta::OptionsConfig{}, ctx);
    CHECK(!c.ok && c.multiplier == 0);

    const auto condor = ta::parse_structure("P:22000:-1:85,P:21800:1:40,C:24000:-1:70,C:24200:1:30", "2026-10-13", 65);
    c = ta::check_structure(condor, ta::OptionsConfig{}, ctx);
    CHECK(c.ok);
    // Budget 0.75% of 25 lakh = 18,750; max loss 7,475 + 250 costs -> 2 copies.
    CHECK(c.multiplier == 2);

    ctx.today = "2026-10-13";
    CHECK(!ta::check_structure(condor, ta::OptionsConfig{}, ctx).ok);
    ctx.today = "2026-10-06";
    ctx.event_blackout = true;
    CHECK(!ta::check_structure(condor, ta::OptionsConfig{}, ctx).ok);
}

TEST(options_credit_must_cover_costs) {
    const auto thin = ta::parse_structure("P:22000:-1:1.0,P:21950:1:0.5", "2026-10-13", 65);
    ta::OptionsContext ctx;
    ctx.today = "2026-10-06";
    ctx.active_book = 2.5e6;
    const auto c = ta::check_structure(thin, ta::OptionsConfig{}, ctx);
    CHECK(!c.ok);
}
