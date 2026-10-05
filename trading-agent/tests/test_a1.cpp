#include "harness.hpp"
#include "ta/a1.hpp"
#include "ta/indicators.hpp"
#include "ta/risk.hpp"
#include "ta/screener.hpp"

using th::date_of;

namespace {

ta::Bar bar_at(int k, double close, double up = 1.03, double down = 0.985, double vol = 1e7) {
    return {date_of(k), close, close * up, close * down, close, vol};
}

// 230 slow days, a 40-day run-up, then a tight 10-day base: bar 279 is a valid A1 setup.
ta::Series setup_series() {
    ta::Series s;
    double c = 100;
    int k = 0;
    for (; k < 230; ++k, c *= 1.0005) s.push_back(bar_at(k, c));
    for (; k < 270; ++k, c *= 1.012) s.push_back(bar_at(k, c));
    for (; k < 280; ++k, c *= 1.001) s.push_back(bar_at(k, c));
    return s;
}

ta::BacktestResult run_from(const ta::Series& s, const std::string& start,
                            const ta::ExclusionList& excl = {}) {
    const ta::MarketData md({{"AAA", s}});
    return ta::run_a1_backtest(md, ta::A1Config{}, excl, 1e6, start);
}

}  // namespace

// ---------------------------------------------------------------- indicators

TEST(indicators_basic) {
    ta::Series s;
    for (int k = 0; k < 5; ++k) s.push_back({date_of(k), 0, 11.0 + k, 10.0 + k, 10.0 + k, 100});
    CHECK_NEAR(ta::sma_close(s, 4, 5), 12.0, 1e-12);
    CHECK(std::isnan(ta::sma_close(s, 3, 5)));
    CHECK_NEAR(ta::ret(s, 4, 4), 14.0 / 10.0 - 1.0, 1e-12);
    CHECK_NEAR(ta::highest_high(s, 4, 2), 15.0, 1e-12);
    CHECK_NEAR(ta::lowest_low(s, 4, 2), 13.0, 1e-12);
    CHECK_NEAR(ta::adr_frac(s, 1, 1), 12.0 / 11.0 - 1.0, 1e-12);
    CHECK_NEAR(ta::avg_traded_value(s, 1, 2), (10.0 + 11.0) * 100 / 2, 1e-9);
}

TEST(indicators_never_look_ahead) {
    ta::Series s = setup_series();
    const double before = ta::sma_close(s, 100, 20);
    s[101].close = 1e9;  // changing the future must not change the past
    CHECK_NEAR(ta::sma_close(s, 100, 20), before, 1e-12);
}

// ---------------------------------------------------------------- risk

TEST(risk_sizes_by_risk_and_caps) {
    ta::RiskConfig cfg;
    cfg.max_position_frac = 1.0;  // isolate the risk-based size
    ta::RiskManager rm(cfg);
    rm.start_day(1e6);
    // 0.4% of 1e6 = 4000 at risk; 4 per share -> 1000 shares.
    auto d = rm.size_long(1e6, 1e6, 0, 100, 96, 0.05, 0.005);
    CHECK(d.qty == 1000);

    ta::RiskManager capped{ta::RiskConfig{}};
    capped.start_day(1e6);
    // 5% position cap: 50,000 / 100 = 500 shares.
    CHECK(capped.size_long(1e6, 1e6, 0, 100, 96, 0.05, 0.005).qty == 500);
    // Cash limits size too.
    CHECK(capped.size_long(1e6, 10'000, 0, 100, 96, 0.05, 0.005).qty == 99);
}

TEST(risk_rejects_bad_trades) {
    ta::RiskManager rm{ta::RiskConfig{}};
    rm.start_day(1e6);
    CHECK(rm.size_long(1e6, 1e6, 0, 100, 90, 0.05, 0.005).reject_reason == "stop_too_wide");
    // 0.5% round trip on a 1% stop is 0.5R > 0.25R.
    CHECK(rm.size_long(1e6, 1e6, 0, 100, 99, 0.05, 0.005).reject_reason == "cost_exceeds_r_limit");
    CHECK(rm.size_long(1e6, 1e6, 0, 100, 101, 0.05, 0.005).reject_reason == "invalid_stop");
    CHECK(rm.size_long(1e6, 1e6, 1e6, 100, 96, 0.05, 0.005).reject_reason == "size_zero");
}

TEST(risk_drawdown_halves_then_latches_off) {
    ta::RiskManager rm{ta::RiskConfig{}};
    rm.start_day(100);
    rm.end_day(100);
    rm.end_day(92);  // -8%
    CHECK(rm.state() == ta::RiskState::Halved);
    rm.end_day(95);  // recovered to -5%
    CHECK(rm.state() == ta::RiskState::Normal);
    rm.end_day(84);  // -16%
    CHECK(rm.state() == ta::RiskState::Off);
    rm.end_day(100);  // stays off even after recovery
    CHECK(rm.state() == ta::RiskState::Off);
    CHECK(rm.size_long(100, 100, 0, 10, 9.6, 0.05, 0.005).reject_reason == "risk_off");
    rm.manual_reset();
    CHECK(rm.state() == ta::RiskState::Normal);
}

TEST(risk_halved_state_halves_size) {
    ta::RiskConfig cfg;
    cfg.max_position_frac = 1.0;
    ta::RiskManager rm(cfg);
    rm.start_day(1e6);
    rm.end_day(1e6);
    rm.end_day(0.9e6);  // -10%: halved
    CHECK(rm.size_long(0.9e6, 0.9e6, 0, 100, 96, 0.05, 0.005).qty == 450);
}

TEST(risk_daily_loss_blocks_entries) {
    ta::RiskManager rm{ta::RiskConfig{}};
    rm.start_day(1000);
    CHECK(rm.allows_new_entries(995));
    CHECK(!rm.allows_new_entries(989));
}

// ---------------------------------------------------------------- screener

TEST(zscore_clips_and_handles_flat) {
    auto z = ta::zscore_clipped({1, 1, 1});
    CHECK(z[0] == 0 && z[2] == 0);
    std::vector<double> x(50, 0.0);
    x[0] = 1000;
    z = ta::zscore_clipped(x);
    CHECK_NEAR(z[0], 3.0, 1e-12);
}

TEST(setup_detected_on_tight_base) {
    const ta::Series s = setup_series();
    const ta::ScreenConfig cfg;
    CHECK(ta::passes_universe(s, s.size() - 1, cfg));
    const double pivot = ta::setup_pivot(s, s.size() - 1, cfg);
    CHECK(!std::isnan(pivot));
    CHECK_NEAR(pivot, s.back().high, 1e-9);
}

TEST(setup_rejected_without_runup) {
    ta::Series s;
    double c = 150;
    for (int k = 0; k < 280; ++k, c *= 1.0005) s.push_back(bar_at(k, c));
    CHECK(std::isnan(ta::setup_pivot(s, s.size() - 1, ta::ScreenConfig{})));
}

TEST(universe_rejects_illiquid_and_cheap) {
    ta::Series s = setup_series();
    for (auto& b : s) b.volume = 10;  // tiny turnover
    CHECK(!ta::passes_universe(s, s.size() - 1, ta::ScreenConfig{}));
    ta::ScreenConfig cfg;
    cfg.min_price = 1e6;
    CHECK(!ta::passes_universe(setup_series(), 279, cfg));
}

// ---------------------------------------------------------------- backtest

TEST(backtest_breakout_then_gap_stop) {
    ta::Series s = setup_series();
    const double pivot = s.back().high;
    const double trigger = pivot + 0.05;
    s.push_back({date_of(280), trigger, trigger * 1.04, trigger * 0.99, trigger * 1.03, 1e7});
    const double gap_open = trigger * 0.90;
    s.push_back({date_of(281), gap_open, gap_open * 1.01, gap_open * 0.99, gap_open, 1e7});

    const auto r = run_from(s, date_of(279));  // trade only the intended signal
    CHECK(r.trades.size() == 1);
    if (r.trades.size() != 1) return;
    const auto& t = r.trades[0];
    CHECK_NEAR(t.entry_price, trigger, 1e-9);
    CHECK(t.exit_reason == "stop_gap");
    CHECK(t.exit_date == date_of(281));
    CHECK(t.r_multiple < -1.0);  // a gap through the stop costs more than 1R

    // Accounting: final equity == initial equity + sum of trade P&L.
    CHECK_NEAR(r.equity_curve.back().second, 1e6 + t.pnl, 1e-6);
    // 5% position cap binds: floor(1e6 * 0.05 / fill).
    CHECK(t.qty == static_cast<long>(std::floor(1e6 * 0.05 / trigger)));
}

TEST(backtest_partial_then_trail_exit) {
    ta::Series s = setup_series();
    const double trigger = s.back().high + 0.05;
    double c = trigger * 1.03;
    s.push_back({date_of(280), trigger, trigger * 1.04, trigger * 0.99, c, 1e7});
    for (int k = 281; k < 290; ++k) {
        c *= 1.03;
        s.push_back(bar_at(k, c, 1.02, 0.99));
    }
    // Sharp reversal closes below the trailing SMA (but above the raised stop).
    const double drop = trigger * 1.02;  // below SMA20, above the stop (raised to entry)
    s.push_back({date_of(290), s.back().close, s.back().close, drop * 0.995, drop, 1e7});
    s.push_back(bar_at(291, drop, 1.01, 0.99));

    const auto r = run_from(s, date_of(279));
    CHECK(r.trades.size() == 1);
    if (r.trades.size() != 1) return;
    const auto& t = r.trades[0];
    CHECK(t.exit_reason == "trail");
    CHECK(t.exit_date == date_of(291));
    CHECK(t.pnl > 0);
    CHECK_NEAR(r.equity_curve.back().second, 1e6 + t.pnl, 1e-6);
}

TEST(backtest_skips_open_gapped_past_limit) {
    ta::Series s = setup_series();
    const double open = (s.back().high + 0.05) * 1.02;  // 2% above trigger > 0.5% limit
    s.push_back({date_of(280), open, open * 1.02, open * 0.99, open, 1e7});
    const auto r = run_from(s, date_of(279));
    CHECK(r.trades.empty());
}

TEST(backtest_excluded_symbol_never_traded) {
    ta::Series s = setup_series();
    const double trigger = s.back().high + 0.05;
    s.push_back({date_of(280), trigger, trigger * 1.04, trigger * 0.99, trigger * 1.03, 1e7});
    const auto r = run_from(s, "", ta::ExclusionList({"AAA"}));
    CHECK(r.trades.empty());
}

TEST(metrics_from_known_curve) {
    std::vector<std::pair<std::string, double>> curve = {{"a", 100}, {"b", 110}, {"c", 99}, {"d", 121}};
    std::vector<ta::Trade> trades(2);
    trades[0].pnl = 30;
    trades[0].r_multiple = 3;
    trades[1].pnl = -10;
    trades[1].r_multiple = -1;
    const auto m = ta::compute_metrics(trades, curve);  // 252 periods a year
    CHECK(m.trades == 2);
    CHECK_NEAR(m.win_rate, 0.5, 1e-12);
    CHECK_NEAR(m.avg_r, 1.0, 1e-12);
    CHECK_NEAR(m.profit_factor, 3.0, 1e-12);
    CHECK_NEAR(m.total_return, 0.21, 1e-12);
    CHECK_NEAR(m.max_drawdown, 0.1, 1e-12);
}


namespace {
ta::Universe walk_universe(int symbols, int days) {
    ta::Universe u;
    for (int k = 0; k < symbols; ++k) {
        u["S" + std::to_string(k)] = th::random_walk(100 + k, days, 300 + 10 * k, 0.03, 1e6);
    }
    return u;
}
}  // namespace

TEST(a1_save_load_matches_continuous_run) {
    const ta::MarketData md(walk_universe(40, 700));
    const auto& dates = md.dates();
    const ta::A1Config cfg;

    ta::A1Engine full(md, cfg, {}, {}, 1e6);
    std::vector<ta::Trade> full_trades;
    for (const auto& d : dates) {
        full.step(d);
        for (auto& t : full.drain_closed()) full_trades.push_back(t);
    }

    // Same run, but stop, serialise through text, and resume in a fresh engine every 50 days.
    std::vector<ta::Trade> split_trades;
    std::vector<ta::KvRecord> saved;
    double equity = 0;
    for (std::size_t k = 0; k < dates.size(); k += 50) {
        ta::A1Engine eng(md, cfg, {}, {}, 1e6);
        std::vector<ta::KvRecord> reparsed;
        for (const auto& r : saved) reparsed.push_back(ta::KvRecord::parse(r.encode()));
        if (!reparsed.empty()) eng.load(reparsed);
        for (std::size_t j = k; j < std::min(dates.size(), k + 50); ++j) {
            eng.step(dates[j]);
            for (auto& t : eng.drain_closed()) split_trades.push_back(t);
        }
        saved = eng.save();
        equity = eng.equity();
    }
    CHECK(!full_trades.empty());
    CHECK(full_trades.size() == split_trades.size());
    CHECK_NEAR(equity, full.equity(), 1e-6);
    for (std::size_t k = 0; k < std::min(full_trades.size(), split_trades.size()); ++k) {
        CHECK(full_trades[k].symbol == split_trades[k].symbol);
        CHECK_NEAR(full_trades[k].pnl, split_trades[k].pnl, 1e-6);
    }
}

TEST(a1_shared_book_limits_positions_and_heat) {
    const ta::MarketData md(walk_universe(40, 700));
    ta::SharedBook full_book;
    full_book.external_positions = 12;  // the other sleeve already uses every slot
    ta::A1Engine eng(md, ta::A1Config{}, {}, {}, 1e6);
    ta::StepContext ctx;
    ctx.shared = &full_book;
    for (const auto& d : md.dates()) eng.step(d, ctx);
    CHECK(eng.drain_closed().empty() && eng.positions() == 0);

    ta::SharedBook heat;
    heat.heat_cap = 1.0;  // one rupee of open risk allowed in total
    ta::A1Engine eng2(md, ta::A1Config{}, {}, {}, 1e6);
    ctx.shared = &heat;
    for (const auto& d : md.dates()) eng2.step(d, ctx);
    CHECK(eng2.drain_closed().empty());
}

TEST(a1_random_walk_has_no_edge_after_costs) {
    // Leakage smoke test: on prices with no predictability the rules must not find an edge.
    const ta::MarketData md(walk_universe(60, 900));
    const auto r = ta::run_a1_backtest(md, ta::A1Config{}, {}, 1e6);
    CHECK(r.metrics.trades > 30);
    CHECK(r.metrics.avg_r < 0.1);
}

TEST(a1_regime_red_blocks_entries) {
    const ta::MarketData md(walk_universe(40, 700));
    ta::Series index;
    double c = 1000;
    for (const auto& d : md.dates()) {
        index.push_back({d, c, c, c, c, 1});
        c *= 0.999;  // permanent downtrend: trend gate red after 200 days
    }
    ta::EquityRegimeInputs reg;
    reg.index = &index;
    const auto with_gate = ta::run_a1_backtest(md, ta::A1Config{}, {}, 1e6, md.dates()[260], "", reg);
    CHECK(with_gate.trades.empty());
    const auto without = ta::run_a1_backtest(md, ta::A1Config{}, {}, 1e6, md.dates()[260]);
    CHECK(!without.trades.empty());
}

TEST(a1_partial_off_switch) {
    // Same winning trade as the partial/trail test, with the partial disabled: no partial
    // sale and the stop is not raised to entry.
    ta::Series s = setup_series();
    const double trigger = s.back().high + 0.05;
    double c = trigger * 1.03;
    s.push_back({date_of(280), trigger, trigger * 1.04, trigger * 0.99, c, 1e7});
    for (int k = 281; k < 290; ++k) {
        c *= 1.03;
        s.push_back(bar_at(k, c, 1.02, 0.99));
    }
    const ta::MarketData md({{"AAA", s}});
    ta::A1Config cfg;
    cfg.exits.partial_frac = 0;
    ta::A1Engine eng(md, cfg, {}, {}, 1e6);
    std::vector<ta::KvRecord> events;
    ta::StepContext ctx;
    ctx.events = &events;
    for (const auto& d : md.dates()) {
        if (d >= date_of(279)) eng.step(d, ctx);
    }
    bool partial = false;
    for (const auto& e : events) partial |= e.str("reason") == "partial";
    CHECK(!partial);
    CHECK(eng.positions() == 1);
    if (eng.positions() == 1) {
        const auto& pos = eng.book().positions().begin()->second;
        CHECK(pos.qty == pos.trade.qty);
        CHECK(pos.stop < pos.trade.entry_price);
    }
}
