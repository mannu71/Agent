#include <fstream>

#include "harness.hpp"
#include "ta/casebook.hpp"
#include "ta/concepts.hpp"
#include "ta/flow.hpp"
#include "ta/journal.hpp"
#include "ta/live.hpp"

namespace {

ta::FlowBar fb(std::int64_t t, double o, double h, double l, double c, double v = 100, double tb = 50) {
    return {t, o, h, l, c, v, tb};
}

// A deterministic wavy series with enough bars for the detectors' warm-up.
ta::FlowSeries wave(std::size_t n, int tf) {
    ta::FlowSeries s;
    double p = 100;
    for (std::size_t k = 0; k < n; ++k) {
        const double drift = ((k / 7) % 2 == 0 ? 0.4 : -0.35) + 0.05 * static_cast<double>((k * 37) % 11) / 10.0;
        const double o = p, c = p + drift;
        s.push_back(fb(static_cast<std::int64_t>(k) * tf, o, std::max(o, c) + 0.3, std::min(o, c) - 0.3, c, 100 + (k % 5) * 20,
                       40 + (k % 3) * 15));
        p = c;
    }
    return s;
}

}  // namespace

TEST(flow_time_round_trip_and_resample) {
    CHECK(ta::parse_minutes("1970-01-01 00:00") == 0);
    CHECK(ta::format_minutes(ta::parse_minutes("2024-02-29 23:59")) == "2024-02-29 23:59");
    CHECK(ta::day_of(ta::parse_minutes("2024-03-01 00:00")) == ta::day_of(ta::parse_minutes("2024-03-01 23:59")));
    ta::FlowSeries m = {fb(0, 1, 2, 0.5, 1.5, 10, 7), fb(1, 1.5, 3, 1, 2, 10, 2), fb(5, 2, 2, 2, 2, 1, 1)};
    const auto r = ta::resample(m, 5);
    CHECK(r.size() == 2);
    CHECK_NEAR(r[0].high, 3, 1e-12);
    CHECK_NEAR(r[0].low, 0.5, 1e-12);
    CHECK_NEAR(r[0].close, 2, 1e-12);
    CHECK_NEAR(r[0].delta(), 2 * 9 - 20, 1e-12);
}

TEST(flow_csv_loads_taker_buy) {
    const auto dir = th::temp_dir("flow");
    std::ofstream(dir + "/x.csv") << "date,open,high,low,close,volume,taker_buy\n"
                                     "2024-01-01 00:01,2,3,1,2.5,10,6\n2024-01-01 00:00,1,2,0.5,1.5,8,1\n";
    const auto s = ta::load_flow(dir + "/x.csv");
    CHECK(s.size() == 2 && s[0].t + 1 == s[1].t);
    CHECK_NEAR(s[1].delta(), 2.0, 1e-12);
}

TEST(swing_points_are_strict_fractals) {
    ta::FlowSeries s;
    const double highs[] = {1, 2, 5, 2, 1, 3, 3, 1};
    for (int k = 0; k < 8; ++k) s.push_back(fb(k, 1, highs[k], 0, 1));
    std::vector<bool> sh, sl;
    ta::swing_points(s, 2, sh, sl);
    CHECK(sh[2]);
    CHECK(!sh[5] && !sh[6]);  // equal highs are not strict swing points
}

TEST(value_area_covers_share_around_poc) {
    ta::FlowSeries s;
    for (int k = 0; k < 10; ++k) s.push_back(fb(k, 100, 101, 99, 100, k == 5 ? 1000 : 10));
    s.push_back(fb(10, 120, 121, 119, 120, 10));
    const auto va = ta::value_area(s, 0, s.size(), 22, 0.7);
    CHECK(va.poc > 99 && va.poc < 101);
    CHECK(va.val >= 98.9 && va.vah <= 102);
}

TEST(fvg_detected_with_displacement) {
    ta::FlowSeries s = wave(200, 15);
    const std::int64_t t0 = s.back().t + 15;
    const double p = s.back().close;
    // c1, a big displacement candle c2, then c3 whose low stays above c1's high.
    s.push_back(fb(t0, p, p + 0.2, p - 0.2, p + 0.1));
    s.push_back(fb(t0 + 15, p + 0.1, p + 6, p, p + 5.8));
    s.push_back(fb(t0 + 30, p + 5.8, p + 7, p + 5, p + 6.5));
    const auto setups = ta::detect_setups(s, {}, 15, "X", {"fvg"}, ta::ConceptConfig{});
    bool found = false;
    for (const auto& st : setups) {
        if (st.t == t0 + 45 && st.side == 1 && st.order == 'L') {
            found = true;
            CHECK_NEAR(st.entry, ((p + 5) + (p + 0.2)) / 2.0, 1e-9);
            CHECK(st.stop < p - 0.2);
        }
    }
    CHECK(found);
}

TEST(detectors_never_look_ahead) {
    const auto full = wave(900, 15);
    const std::set<std::string> all = {"sweep", "sweep_absorb", "fvg", "ob", "choch", "hl_pullback", "spring", "va80"};
    const auto a = ta::detect_setups(full, {}, 15, "X", all, ta::ConceptConfig{});
    ta::FlowSeries cut(full.begin(), full.begin() + 600);
    const auto b = ta::detect_setups(cut, {}, 15, "X", all, ta::ConceptConfig{});
    const std::int64_t end = cut.back().t + 15;
    std::size_t na = 0;
    for (const auto& s : a) na += s.t <= end ? 1 : 0;
    CHECK(!b.empty());
    CHECK(na == b.size());
    for (std::size_t i = 0, j = 0; i < a.size() && j < b.size(); ++i) {
        if (a[i].t > end) continue;
        CHECK(a[i].t == b[j].t && a[i].pattern == b[j].pattern && a[i].side == b[j].side);
        CHECK_NEAR(a[i].stop, b[j].stop, 1e-12);
        ++j;
    }
}

TEST(case_market_long_hits_target_net_of_costs) {
    ta::FlowSeries m;
    for (int k = 0; k < 10; ++k) m.push_back(fb(k, 100 + k, 100.5 + k, 99.5 + k, 100 + k));
    ta::Setup s;
    s.t = 0;
    s.side = 1;
    s.order = 'M';
    s.stop = 98;
    s.rr = 2;
    s.expiry = 1;
    s.max_hold = 1000;
    ta::ExecCost zero{0, 0, 0, 0};
    const auto c = ta::simulate_case(s, m, zero);
    CHECK(c.filled && c.exit_reason == "target");
    CHECK_NEAR(c.target_px, 104, 1e-12);
    CHECK_NEAR(c.r_gross, 2.0, 1e-12);
    const auto c2 = ta::simulate_case(s, m, ta::ExecCost{});
    CHECK(c2.r_net < c2.r_gross);
}

TEST(case_stop_wins_when_minute_touches_both) {
    ta::FlowSeries m = {fb(0, 100, 100.2, 99.8, 100), fb(1, 100, 105, 97, 100)};
    ta::Setup s;
    s.t = 0;
    s.side = 1;
    s.order = 'M';
    s.stop = 98;
    s.target = 102;
    s.expiry = 1;
    s.max_hold = 100;
    const auto c = ta::simulate_case(s, m, ta::ExecCost{0, 0, 0, 0});
    CHECK(c.exit_reason == "stop");
    CHECK_NEAR(c.r_gross, -1.0, 1e-12);
}

TEST(case_limit_order_fills_or_expires) {
    ta::FlowSeries m = {fb(0, 105, 106, 104, 105), fb(1, 105, 105, 101, 102), fb(2, 102, 110, 102, 109)};
    ta::Setup s;
    s.t = 0;
    s.side = 1;
    s.order = 'L';
    s.entry = 102;
    s.stop = 100;
    s.target = 106;
    s.expiry = 5;
    s.max_hold = 100;
    const auto c = ta::simulate_case(s, m, ta::ExecCost{0, 0, 0, 0});
    CHECK(c.filled && c.entry_t == 1 && c.exit_reason == "target");
    CHECK_NEAR(c.r_gross, 2.0, 1e-12);
    s.expiry = 1;  // gone before price came back
    CHECK(!ta::simulate_case(s, m, ta::ExecCost{0, 0, 0, 0}).filled);
}

TEST(memory_recall_ignores_unfinished_cases) {
    // 40 winning cases that all finish after the 41st setup appears: the memory must be
    // empty at that moment, so the 41st is not taken even though its peers won.
    std::vector<ta::Case> cases;
    for (int i = 0; i < 41; ++i) {
        ta::Case c;
        c.filled = true;
        c.setup.t = i;
        c.setup.pattern = "p";
        c.setup.symbol = "S" + std::to_string(i);
        c.setup.side = 1;
        c.entry_t = i;
        c.exit_t = 1000 + i;
        c.entry_px = 100;
        c.stop_px = 99;
        c.r_net = 1.0;
        cases.push_back(c);
    }
    ta::PortfolioConfig cfg;
    cfg.max_open = 100;
    auto r = ta::run_casebook(cases, cfg);
    CHECK(r.taken == 0);
    // Now let the 41st appear after the first 40 finished: memory says yes.
    cases[40].setup.t = cases[40].entry_t = 5000;
    cases[40].exit_t = 6000;
    r = ta::run_casebook(cases, cfg);
    CHECK(r.taken == 1);
    CHECK_NEAR(r.trades[0].r_multiple, 1.0, 1e-12);
    // Without the recall rule every case is a trade.
    cfg.recall.enabled = false;
    CHECK(ta::run_casebook(cases, cfg).taken == 41);
}

TEST(memory_tax_hits_only_winners) {
    std::vector<ta::Case> cases(2);
    for (int i = 0; i < 2; ++i) {
        cases[i].filled = true;
        cases[i].setup.t = cases[i].entry_t = i * 10;
        cases[i].exit_t = i * 10 + 5;
        cases[i].setup.symbol = "S";
        cases[i].entry_px = 100;
        cases[i].stop_px = 99;
        cases[i].r_net = i == 0 ? 1.0 : -1.0;
    }
    ta::PortfolioConfig cfg;
    cfg.recall.enabled = false;
    cfg.tax_rate = 0.312;
    const auto r = ta::run_casebook(cases, cfg);
    CHECK(r.taken == 2);
    CHECK_NEAR(r.trades[0].pnl, 5000 * (1 - 0.312), 1e-6);
    CHECK(r.trades[1].pnl < -5000 * 0.9);  // the loss is not offset
}

TEST(random_baseline_keeps_order_type_and_distances) {
    ta::FlowSeries m;
    for (int k = 0; k < 500; ++k) m.push_back(fb(k, 100 + (k % 7), 101 + (k % 7), 99 + (k % 7), 100 + (k % 7)));
    ta::Case c;
    c.filled = true;
    c.setup.t = 10;
    c.setup.symbol = "X";
    c.setup.side = 1;
    c.setup.order = 'L';
    c.setup.ref = 100;
    c.setup.entry = 98;
    c.setup.stop = 96;
    c.setup.target = 102;
    c.setup.expiry = 30;
    c.setup.max_hold = 50;
    c.entry_px = 98;
    c.stop_px = 96;
    c.target_px = 102;
    const ta::FlowSeries* p = &m;
    const auto r = ta::random_cases({c}, {{"X", p}}, ta::ExecCost{0, 0, 0, 0}, 3);
    CHECK(r.size() == 1);
    const auto& s = r[0].setup;
    CHECK(s.order == 'L');
    CHECK_NEAR(s.entry / s.ref, 0.98, 1e-12);
    CHECK_NEAR(s.stop / s.ref, 0.96, 1e-12);
    CHECK(s.expiry - s.t == 20);
}

TEST(live_journal_is_append_only_and_idempotent) {
    const auto dir = th::temp_dir("live");
    auto mk = [](std::int64_t t, const std::string& sym, double r, const std::string& reason) {
        ta::Case c;
        c.setup.t = t;
        c.setup.pattern = "fvg";
        c.setup.symbol = sym;
        c.setup.tf = 60;
        c.setup.side = 1;
        c.setup.order = 'L';
        c.setup.expiry = t + 600;
        c.filled = true;
        c.entry_t = t + 5;
        c.exit_t = t + 50;
        c.entry_px = 100;
        c.stop_px = 99;
        c.exit_px = 100 + r;
        c.r_net = r;
        c.exit_reason = reason;
        return c;
    };
    std::vector<ta::Case> cases = {mk(10, "A", 1.5, "target"), mk(20, "B", 0.2, "end_of_data")};
    ta::PortfolioResult none;
    const std::map<std::string, std::int64_t> last = {{"A", 1000}, {"B", 1000}};
    const auto r1 = ta::write_live(dir, cases, none, 0, last);
    CHECK(r1.new_events == 5);  // 2 setups, 2 fills, 1 exit
    CHECK(r1.open_setups == 1 && r1.closed_setups == 1);
    const auto r2 = ta::write_live(dir, cases, none, 0, last);
    CHECK(r2.new_events == 0 && r2.revisions == 0);
    // B closes; A's exit is recomputed differently: B's exit is new, A's is a revision.
    cases[1].exit_reason = "stop";
    cases[1].r_net = -1.0;
    cases[0].r_net = 1.4;
    const auto r3 = ta::write_live(dir, cases, none, 0, last);
    CHECK(r3.new_events == 1 && r3.revisions == 1);
    const auto v = ta::Journal::verify(dir + "/journal.log");
    CHECK(v.ok && v.lines == 7);
}
