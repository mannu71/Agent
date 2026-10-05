#include <cmath>

#include "harness.hpp"
#include "ta/a2.hpp"

namespace {

constexpr int kGapDay = 330;

// Intraday 5-minute bars for one day from 09:15 to 15:25 following `path` closes.
ta::Series day_bars(const std::string& date, const std::vector<ta::Bar>& head, double tail_close) {
    ta::Series out = head;
    int minutes = 9 * 60 + 15 + 5 * static_cast<int>(head.size());
    double c = head.empty() ? tail_close : head.back().close;
    while (minutes <= 15 * 60 + 25) {
        char ts[32];
        std::snprintf(ts, sizeof ts, "%s %02d:%02d", date.c_str(), minutes / 60, minutes % 60);
        const double next = c + (tail_close - c) * 0.2;
        out.push_back({ts, c, std::max(c, next) * 1.001, std::min(c, next) * 0.999, next, 1000});
        c = next;
        minutes += 5;
    }
    return out;
}

struct Scenario {
    ta::Universe daily;
    ta::IntradayData intraday;
    ta::A2Inputs in;
    double pc = 0;
};

// One stock with 330 days of history, then an 8% gap on day 330 after an evening filing.
Scenario gap_scenario(double close_1520_mult, bool with_catalyst = true, const std::string& filing_time = "18:00") {
    Scenario sc;
    ta::Series s = th::random_walk(77, kGapDay, 500, 0.03, 1e6, true);
    sc.pc = s.back().close;
    const double pc = sc.pc;
    const std::string gd = th::iso_day(kGapDay);
    s.push_back({gd, pc * 1.08, pc * 1.15, pc * 1.06, pc * 1.12, 5e6});
    double c = pc * 1.12;
    for (int k = kGapDay + 1; k < kGapDay + 40; ++k) {
        c *= 1.01;
        s.push_back({th::iso_day(k), c, c * 1.01, c * 0.995, c, 2e6});
    }
    sc.daily["GAPCO"] = s;

    const std::vector<ta::Bar> head = {
        {gd + " 09:15", pc * 1.08, pc * 1.09, pc * 1.065, pc * 1.088, 50000},  // green first bar
        {gd + " 09:20", pc * 1.088, pc * 1.10, pc * 1.085, pc * 1.095, 30000},  // breaks its high
    };
    ta::Series bars = day_bars(gd, head, pc * 1.11);
    for (auto& b : bars) {
        if (ta::time_of(b.date) >= "15:20") b.open = b.close = b.high = b.low = pc * close_1520_mult;
    }
    sc.intraday.bars["GAPCO"] = bars;
    sc.intraday.finalize();

    sc.in.intraday = &sc.intraday;
    sc.in.cash_band["GAPCO"] = 0.20;
    if (with_catalyst) {
        const std::string day = filing_time < "09:15" || filing_time > "15:30" ? th::iso_day(kGapDay - 1) : gd;
        sc.in.catalysts["GAPCO"] = {(filing_time < "09:15" ? gd : day) + " " + filing_time};
    }
    return sc;
}

ta::A2Config test_config() {
    ta::A2Config cfg;
    cfg.min_history_events = 0;  // a single event can be approved in a unit test
    cfg.min_rv = 0;              // the scenario has no prior-day intraday bars
    return cfg;
}

}  // namespace

TEST(a2_entry_rules) {
    ta::A2Config cfg;
    ta::GapEvent ev;
    ev.open = 108;
    ev.upper_band = 120;
    const std::vector<ta::Bar> green = {{"2024-01-01 09:15", 108, 109, 106.5, 108.8, 1},
                                        {"2024-01-01 09:20", 108.8, 110, 108.5, 109.5, 1}};
    auto e = ta::simulate_entry(ev, green.data(), green.size(), cfg);
    CHECK(e.filled);
    CHECK_NEAR(e.fill, 109.05, 1e-9);
    CHECK_NEAR(e.stop, 106.45, 1e-9);

    const std::vector<ta::Bar> red = {{"2024-01-01 09:15", 108, 109, 106.5, 107.5, 1},
                                      {"2024-01-01 09:20", 108.8, 110, 108.5, 109.5, 1}};
    CHECK(ta::simulate_entry(ev, red.data(), red.size(), cfg).reason == "first_bar_red");

    // Bar opens far above the limit and never trades back down: no fill.
    const std::vector<ta::Bar> runaway = {{"2024-01-01 09:15", 108, 109, 106.5, 108.8, 1},
                                          {"2024-01-01 09:20", 112, 113, 111, 112, 1}};
    CHECK(!ta::simulate_entry(ev, runaway.data(), runaway.size(), cfg).filled);

    // Trigger after the entry window: no fill.
    cfg.window_end = "10:15";
    const std::vector<ta::Bar> late = {{"2024-01-01 09:15", 108, 109, 106.5, 108.8, 1},
                                       {"2024-01-01 10:15", 108.8, 110, 108.5, 109.5, 1}};
    CHECK(ta::simulate_entry(ev, late.data(), late.size(), cfg).reason == "not_triggered");

    // Cash stock opening near its upper band is skipped.
    ta::GapEvent locked = ev;
    locked.open = 118;
    CHECK(ta::simulate_entry(locked, green.data(), green.size(), cfg).reason == "band_lock");

    // F&O stock whose trigger sits within 1% of the band is skipped.
    ta::GapEvent fno = ev;
    fno.fno = true;
    fno.upper_band = 109.5;
    CHECK(ta::simulate_entry(fno, green.data(), green.size(), cfg).reason == "near_upper_band");
}

TEST(a2_enters_gap_with_overnight_catalyst) {
    Scenario sc = gap_scenario(1.11);
    const ta::MarketData md(sc.daily);
    const auto r = ta::run_a2_backtest(md, test_config(), sc.in, 1e6, th::iso_day(kGapDay - 1));
    CHECK(r.events.size() == 1);
    if (r.events.size() != 1) return;
    CHECK(r.events[0].status == "entered");
    CHECK(r.portfolio.trades.size() == 1);
    if (r.portfolio.trades.empty()) return;
    CHECK_NEAR(r.portfolio.trades[0].entry_price, sc.pc * 1.09 + 0.05, 1e-6);
    CHECK(r.portfolio.trades[0].entry_date == th::iso_day(kGapDay));
    CHECK(r.portfolio.trades[0].pnl > 0);
    CHECK(!std::isnan(r.shadow[0]));
}

TEST(a2_requires_catalyst_before_open) {
    Scenario none = gap_scenario(1.11, false);
    const ta::MarketData md(none.daily);
    CHECK(ta::run_a2_backtest(md, test_config(), none.in, 1e6).events.empty());

    // A filing during the session is a mid-session event, not an A2 gap.
    Scenario mid = gap_scenario(1.11, true, "11:00");
    const ta::MarketData md2(mid.daily);
    CHECK(ta::run_a2_backtest(md2, test_config(), mid.in, 1e6).events.empty());
}

TEST(a2_weak_at_1520_exits_at_close) {
    Scenario sc = gap_scenario(1.085);  // below the 1.09 x pc + 0.05 entry, above the stop, at 15:20
    const ta::MarketData md(sc.daily);
    const auto r = ta::run_a2_backtest(md, test_config(), sc.in, 1e6, th::iso_day(kGapDay - 1));
    CHECK(r.portfolio.trades.size() == 1);
    if (r.portfolio.trades.empty()) return;
    CHECK(r.portfolio.trades[0].exit_reason == "weak_day0");
    CHECK(r.portfolio.trades[0].exit_date == th::iso_day(kGapDay));
}

TEST(a2_warmup_approves_nothing) {
    Scenario sc = gap_scenario(1.11);
    const ta::MarketData md(sc.daily);
    ta::A2Config cfg;
    cfg.min_rv = 0;
    const auto r = ta::run_a2_backtest(md, cfg, sc.in, 1e6);  // needs 30 prior events
    CHECK(r.events.size() == 1 && r.events[0].status == "warmup");
    CHECK(r.portfolio.trades.empty());
}

TEST(a2_save_load_round_trip) {
    Scenario sc = gap_scenario(1.11);
    const ta::MarketData md(sc.daily);
    const auto& dates = md.dates();
    ta::A2Engine full(md, test_config(), sc.in, 1e6);
    for (const auto& d : dates) full.step(d);

    std::vector<ta::KvRecord> saved;
    double equity = 0;
    std::size_t events = 0;
    for (std::size_t k = 0; k < dates.size(); k += 7) {
        ta::A2Engine eng(md, test_config(), sc.in, 1e6);
        std::vector<ta::KvRecord> reparsed;
        for (const auto& r : saved) reparsed.push_back(ta::KvRecord::parse(r.encode()));
        if (!reparsed.empty()) eng.load(reparsed);
        for (std::size_t j = k; j < std::min(dates.size(), k + 7); ++j) eng.step(dates[j]);
        saved = eng.save();
        equity = eng.equity();
        events = eng.events().size();
    }
    CHECK_NEAR(equity, full.equity(), 1e-6);
    CHECK(events == full.events().size());
}

TEST(a2_catalyst_switch_runs_any_gap_baseline) {
    Scenario sc = gap_scenario(1.11, false);  // no filing
    const ta::MarketData md(sc.daily);
    ta::A2Config cfg = test_config();
    CHECK(ta::run_a2_backtest(md, cfg, sc.in, 1e6).events.empty());
    cfg.require_catalyst = false;  // the paper's catalyst-free universe
    CHECK(ta::run_a2_backtest(md, cfg, sc.in, 1e6).events.size() == 1);
}

TEST(a2_skips_gap_after_gap_day) {
    Scenario sc = gap_scenario(1.11);
    auto& s = sc.daily["GAPCO"];
    s[kGapDay - 1].open = s[kGapDay - 2].close * 1.07;  // the day before also gapped 7%
    const ta::MarketData md(sc.daily);
    ta::A2Config cfg = test_config();
    CHECK(ta::run_a2_backtest(md, cfg, sc.in, 1e6).events.empty());
    cfg.skip_after_gap_day = false;
    CHECK(ta::run_a2_backtest(md, cfg, sc.in, 1e6).events.size() == 1);
}

TEST(a2_relative_volume_gate) {
    auto with_prior_volume = [](double first_bar_volume) {
        Scenario sc = gap_scenario(1.11);
        const std::string prev = th::iso_day(kGapDay - 1);
        ta::Series bars = day_bars(prev, {}, sc.pc);
        bars[0].volume = first_bar_volume;
        for (auto& b : bars) sc.intraday.bars["GAPCO"].push_back(b);
        sc.intraday.finalize();
        return sc;
    };
    ta::A2Config cfg = test_config();
    cfg.min_rv = 1.0;
    Scenario quiet = with_prior_volume(10000);   // gap-day first bar 50,000 -> RV 5
    const ta::MarketData md1(quiet.daily);
    const auto r1 = ta::run_a2_backtest(md1, cfg, quiet.in, 1e6);
    CHECK(r1.events.size() == 1);
    if (!r1.events.empty()) CHECK_NEAR(r1.events[0].rv, 5.0, 1e-9);
    Scenario busy = with_prior_volume(100000);   // RV 0.5 -> gated out
    const ta::MarketData md2(busy.daily);
    CHECK(ta::run_a2_backtest(md2, cfg, busy.in, 1e6).events.empty());
}

TEST(a2_paper_targets_mode) {
    Scenario sc = gap_scenario(1.11);
    const ta::MarketData md(sc.daily);
    ta::A2Config cfg = test_config();
    cfg.partial_mode = "targets";
    std::vector<ta::KvRecord> events;
    ta::A2Engine eng(md, cfg, sc.in, 1e6);
    ta::StepContext ctx;
    ctx.events = &events;
    for (const auto& d : md.dates()) {
        if (d >= th::iso_day(kGapDay - 1)) eng.step(d, ctx);
    }
    int targets = 0;
    for (const auto& e : events) targets += e.type == "fill" && e.str("reason") == "target";
    CHECK(targets == 4);  // 25% each at 2R, 4R, 8R and 10R
    const auto closed = eng.drain_closed();
    CHECK(closed.size() == 1 && closed[0].exit_reason == "target");
    bool threw = false;
    try {
        ta::A2Config bad;
        bad.partial_mode = "fifths";
        bad.apply_partial_mode();
    } catch (const std::exception&) {
        threw = true;
    }
    CHECK(threw);
}
