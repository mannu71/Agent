#include <cmath>
#include <random>

#include "harness.hpp"
#include "ta/monitor.hpp"
#include "ta/validate.hpp"

namespace {
std::vector<ta::Trade> trades_with_r(const std::vector<double>& rs, const std::string& year = "2024") {
    std::vector<ta::Trade> out;
    for (double r : rs) {
        ta::Trade t;
        t.r_multiple = r;
        t.pnl = r * 1000;
        t.exit_date = year + "-06-01";
        out.push_back(t);
    }
    return out;
}
}  // namespace

TEST(normal_quantiles) {
    CHECK_NEAR(ta::norm_inv(0.975), 1.959964, 1e-5);
    CHECK_NEAR(ta::norm_inv(0.5), 0.0, 1e-9);
    CHECK_NEAR(ta::norm_cdf(1.959964), 0.975, 1e-6);
}

TEST(spearman_and_ties) {
    CHECK_NEAR(ta::spearman({1, 2, 3, 4}, {10, 20, 30, 40}), 1.0, 1e-12);
    CHECK_NEAR(ta::spearman({1, 2, 3, 4}, {4, 3, 2, 1}), -1.0, 1e-12);
    CHECK(std::isnan(ta::spearman({1, 1, 1}, {1, 2, 3})));
}

TEST(score_outcomes_rank_ic_on_perfect_scores) {
    // Score = next-period drift: a perfect screener has rank IC 1 and full precision.
    ta::Universe u;
    ta::ScoreLog log;
    for (int s = 0; s < 10; ++s) {
        ta::Series ser;
        double c = 100;
        const double drift = (s - 4.5) / 100.0;
        for (int k = 0; k < 30; ++k, c *= 1 + drift) ser.push_back({th::date_of(k), c, c, c, c, 1});
        u["S" + std::to_string(s)] = ser;
        log[th::date_of(10)].emplace_back("S" + std::to_string(s), drift);
    }
    const ta::MarketData md(u);
    const auto out = ta::score_outcomes(log, md, 5, 3);
    CHECK(out.size() == 1);
    if (out.empty()) return;
    CHECK_NEAR(out[0].rank_ic, 1.0, 1e-12);
    CHECK_NEAR(out[0].precision_at_k, 1.0, 1e-12);
    CHECK_NEAR(out[0].base_rate, 0.5, 1e-12);
}

TEST(forward_return_needs_future_data) {
    ta::Series s;
    for (int k = 0; k < 5; ++k) s.push_back({th::date_of(k), 100.0 + k, 0, 0, 101.0 + k, 1});
    CHECK_NEAR(ta::forward_return(s, 1, 2), 104.0 / 102.0 - 1.0, 1e-12);
    CHECK(std::isnan(ta::forward_return(s, 3, 2)));
}

TEST(cusum_fires_on_sustained_drop) {
    ta::Cusum c(0.03, 0.01, 0.2);
    for (int k = 0; k < 50; ++k) c.update(0.03);
    CHECK(!c.alarm());
    for (int k = 0; k < 20; ++k) c.update(-0.02);
    CHECK(c.alarm());
}

TEST(psi_detects_shift) {
    std::mt19937 rng(3);
    std::normal_distribution<double> a(0, 1), b(1.5, 1);
    std::vector<double> ref, same, shifted;
    for (int k = 0; k < 2000; ++k) {
        ref.push_back(a(rng));
        same.push_back(a(rng));
        shifted.push_back(b(rng));
    }
    CHECK(ta::psi(ref, same) < 0.1);
    CHECK(ta::psi(ref, shifted) > 0.25);
}

TEST(deflated_sharpe_penalises_many_trials) {
    std::mt19937 rng(5);
    std::normal_distribution<double> g(0.001, 0.01);
    std::vector<double> r;
    for (int k = 0; k < 750; ++k) r.push_back(g(rng));
    const double one = ta::deflated_sharpe(r, 1, 0.0);
    const double many = ta::deflated_sharpe(r, 1000, 0.002);
    CHECK(one > many);
    CHECK(ta::expected_max_sharpe(1000, 0.002) > ta::expected_max_sharpe(10, 0.002));
}

TEST(pbo_noise_vs_real_edge) {
    std::mt19937 rng(9);
    std::normal_distribution<double> noise(0, 0.01);
    std::vector<std::vector<double>> pure(320, std::vector<double>(8)), edge = pure;
    for (auto& row : pure) {
        for (auto& v : row) v = noise(rng);
    }
    for (auto& row : edge) {
        for (std::size_t j = 0; j < row.size(); ++j) row[j] = noise(rng) + (j == 0 ? 0.004 : 0.0);
    }
    const double p_noise = ta::pbo_cscv(pure, 8);
    const double p_edge = ta::pbo_cscv(edge, 8);
    CHECK(p_noise > 0.3);
    CHECK(p_edge < 0.1);
}

TEST(robustness_helpers) {
    auto t = trades_with_r({-1, -1, -1, 10});
    CHECK_NEAR(ta::expectancy_without_top(t, 0.01), -1.0, 1e-12);
    auto mixed = trades_with_r({1, 1}, "2023");
    auto more = trades_with_r({3}, "2024");
    mixed.insert(mixed.end(), more.begin(), more.end());
    CHECK_NEAR(ta::max_year_share(mixed), 0.6, 1e-12);
    CHECK_NEAR(ta::trades_needed(0.25, 1.5), 324, 1e-9);  // the rulebook's worked example
    std::vector<double> flat(100, 0.001);
    CHECK_NEAR(ta::bootstrap_drawdown_p95(flat, 50, 5, 1), 0.0, 1e-12);
}

TEST(kill_rules) {
    std::vector<double> neg50(50, -0.1), pos100(100, 0.2);
    CHECK(ta::a1_kill_rule(trades_with_r(neg50)).action == ta::KillAction::Halve);
    std::vector<double> neg100(100, -0.1);
    CHECK(ta::a1_kill_rule(trades_with_r(neg100)).action == ta::KillAction::Off);
    CHECK(ta::a1_kill_rule(trades_with_r(pos100)).action == ta::KillAction::None);
    CHECK(ta::a1_kill_rule(trades_with_r(pos100), 1.6).action == ta::KillAction::Off);

    std::vector<double> bad40(40, -0.2);
    CHECK(ta::a2_kill_rule(trades_with_r(bad40), 0.1, 50, 0).action == ta::KillAction::Off);
    CHECK(ta::a2_kill_rule({}, -0.01, 120, 0).action == ta::KillAction::Off);
    CHECK(ta::a2_kill_rule({}, 0.2, 120, 0.3).action == ta::KillAction::Halve);

    CHECK(ta::b_kill_rule(0.29, 0.19).action == ta::KillAction::Off);
    CHECK(ta::b_kill_rule(0.20, 0.19).action == ta::KillAction::None);

    std::vector<double> d1neg(30, -0.05);
    CHECK(ta::d1_kill_rule(trades_with_r(d1neg)).action == ta::KillAction::Off);
    CHECK(ta::screener_rule(-0.01, true, 0.1).action == ta::KillAction::Off);
    CHECK(ta::screener_rule(-0.01, false, 0.1).action == ta::KillAction::None);
    CHECK(ta::screener_rule(0.02, false, -0.1).action == ta::KillAction::Off);
}

TEST(gate1_report) {
    std::mt19937 rng(2);
    std::normal_distribution<double> g(0.5, 1.0);
    std::vector<double> rs;
    for (int k = 0; k < 400; ++k) rs.push_back(g(rng));
    auto trades = trades_with_r(rs);
    for (std::size_t k = 0; k < trades.size(); ++k) trades[k].exit_date = std::to_string(2015 + k % 10) + "-01-01";
    ta::EquityCurve curve;
    double e = 1e6;
    std::normal_distribution<double> d(0.001, 0.005);
    for (int k = 0; k < 2500; ++k) curve.emplace_back(th::date_of(k), e *= 1 + d(rng));
    const auto rep = ta::evaluate_gate1(trades, curve, trades, 9, 0.0005, 0.05);
    CHECK(rep.lines.size() == 8);
    CHECK(!rep.text().empty());
    const auto bad = ta::evaluate_gate1(trades_with_r({-0.1, -0.2}), curve, {}, 9, 0.0005, 0.5);
    CHECK(!bad.pass());
}
