#include "harness.hpp"
#include "ta/crypto_trend.hpp"

namespace {

ta::Universe crypto_universe(int days) {
    return {{"BTC", th::random_walk(11, days, 30000, 0.035, 1e9, true)},
            {"ETH", th::random_walk(12, days, 2000, 0.045, 1e9, true)}};
}

double sum_pnl(const std::vector<ta::Trade>& t) {
    double s = 0;
    for (const auto& x : t) s += x.pnl;
    return s;
}

}  // namespace

TEST(crypto_signal_enters_on_new_high_and_exits_on_mid) {
    // 120 quiet days, a 60-day rally (every lookback <= 60 makes new highs), then a crash.
    ta::Series s;
    double c = 100;
    for (int k = 0; k < 120; ++k) {
        c *= (k % 2 ? 1.01 : 0.99);
        s.push_back({th::iso_day(k), c, c, c, c, 1});
    }
    for (int k = 120; k < 180; ++k) {
        c *= 1.02;
        s.push_back({th::iso_day(k), c, c, c, c, 1});
    }
    ta::CryptoTrendConfig cfg;
    cfg.lookbacks = {5, 20, 60};
    const ta::MarketData md({{"BTC", s}});
    ta::CryptoTrendEngine eng(md, cfg, {}, 1e6);
    for (const auto& d : md.dates()) eng.step(d);
    CHECK(eng.exposure("BTC") > 0);
    // The ensemble never exceeds 1x notional (measured before the rebalance fee is paid).
    CHECK(eng.exposure("BTC") * s.back().close <= eng.equity() * (1.0 + 1e-3));

    for (int k = 180; k < 190; ++k) {
        c *= 0.85;
        s.push_back({th::iso_day(k), c, c, c, c, 1});
    }
    const ta::MarketData md2({{"BTC", s}});
    ta::CryptoTrendEngine eng2(md2, cfg, {}, 1e6);
    for (const auto& d : md2.dates()) eng2.step(d);
    CHECK(eng2.exposure("BTC") == 0);  // every lookback's mid-channel stop was hit
    CHECK(eng2.entries_by_lookback().at(5) >= 1);
}

TEST(crypto_accounting_identity_with_tax_and_costs) {
    const ta::MarketData md(crypto_universe(1200));
    ta::CryptoTrendConfig cfg;
    cfg.funding_annual_default = 0.08;
    const auto r = ta::run_crypto_backtest(md, cfg, {}, 1e6);
    CHECK(r.metrics.trades > 5);
    CHECK(r.tax_paid > 0);
    CHECK(r.funding_paid > 0);
    // Trade P&L is after costs and funding, before tax; equity is after tax.
    CHECK_NEAR(r.equity_curve.back().second, 1e6 + sum_pnl(r.trades) - r.tax_paid, 1e-4);
}

TEST(crypto_tax_costs_and_funding_only_reduce_returns) {
    const ta::MarketData md(crypto_universe(1200));
    ta::CryptoTrendConfig clean;
    clean.tax_rate = 0;
    clean.cost_bps = 0;
    const double base = ta::run_crypto_backtest(md, clean, {}, 1e6).equity_curve.back().second;

    ta::CryptoTrendConfig taxed = clean;
    taxed.tax_rate = 0.312;
    CHECK(ta::run_crypto_backtest(md, taxed, {}, 1e6).equity_curve.back().second < base);

    ta::CryptoTrendConfig costly = clean;
    costly.cost_bps = 50;
    CHECK(ta::run_crypto_backtest(md, costly, {}, 1e6).equity_curve.back().second < base);

    std::map<std::string, ta::CryptoAux> aux;
    for (const auto& d : md.dates()) aux["BTC"].funding[d] = 0.0003;  // ~11%/yr, longs pay
    CHECK(ta::run_crypto_backtest(md, clean, aux, 1e6).equity_curve.back().second < base);
}

TEST(crypto_crowding_gate_halves_exposure) {
    ta::Series s;
    double c = 100;
    for (int k = 0; k < 200; ++k) {
        c *= (k < 120 ? (k % 2 ? 1.01 : 0.99) : 1.02);
        s.push_back({th::iso_day(k), c, c, c, c, 1});
    }
    const ta::MarketData md({{"BTC", s}});
    ta::CryptoTrendConfig cfg;
    cfg.lookbacks = {5, 20};
    std::map<std::string, ta::CryptoAux> hot;
    for (const auto& d : md.dates()) hot["BTC"].funding[d] = 0.40 / 365;  // 40%/yr, no OI data
    ta::CryptoTrendEngine normal(md, cfg, {}, 1e6), crowded(md, cfg, hot, 1e6);
    for (const auto& d : md.dates()) {
        normal.step(d);
        crowded.step(d);
    }
    CHECK(crowded.exposure("BTC") > 0);
    CHECK(crowded.exposure("BTC") < normal.exposure("BTC") * 0.75);
}

TEST(crypto_save_load_matches_continuous_run) {
    const ta::MarketData md(crypto_universe(900));
    const ta::CryptoTrendConfig cfg;
    ta::CryptoTrendEngine full(md, cfg, {}, 1e6);
    for (const auto& d : md.dates()) full.step(d);

    std::vector<ta::KvRecord> saved;
    double equity = 0;
    const auto& dates = md.dates();
    for (std::size_t k = 0; k < dates.size(); k += 77) {
        ta::CryptoTrendEngine eng(md, cfg, {}, 1e6);
        std::vector<ta::KvRecord> reparsed;
        for (const auto& r : saved) reparsed.push_back(ta::KvRecord::parse(r.encode()));
        if (!reparsed.empty()) eng.load(reparsed);
        for (std::size_t j = k; j < std::min(dates.size(), k + 77); ++j) eng.step(dates[j]);
        saved = eng.save();
        equity = eng.equity();
    }
    CHECK_NEAR(equity, full.equity(), 1e-6);
}

TEST(crypto_prime_matches_backtest_signals) {
    const ta::MarketData md(crypto_universe(900));
    const ta::CryptoTrendConfig cfg;
    ta::CryptoTrendEngine full(md, cfg, {}, 1e6);
    for (const auto& d : md.dates()) full.step(d);

    // A paper account opened on day 600 primes signals, so its positions follow the same signals.
    const std::string start = md.dates()[600];
    ta::CryptoTrendEngine late(md, cfg, {}, 1e6);
    late.prime(start);
    for (const auto& d : md.dates()) {
        if (d >= start) late.step(d);
    }
    CHECK((full.exposure("BTC") > 0) == (late.exposure("BTC") > 0));
    CHECK((full.exposure("ETH") > 0) == (late.exposure("ETH") > 0));
}

TEST(crypto_risk_off_goes_flat) {
    ta::Series s;
    double c = 100;
    for (int k = 0; k < 200; ++k) {
        c *= (k < 120 ? (k % 2 ? 1.01 : 0.99) : 1.02);
        s.push_back({th::iso_day(k), c, c, c, c, 1});
    }
    for (int k = 200; k < 203; ++k) {
        c *= 0.80;  // crash before the slow lookbacks' stops are hit
        s.push_back({th::iso_day(k), c, c, c, c, 1});
    }
    s.push_back({th::iso_day(203), c * 1.3, c * 1.3, c * 1.3, c * 1.3, 1});
    const ta::MarketData md({{"BTC", s}});
    ta::CryptoTrendConfig cfg;
    cfg.lookbacks = {150};  // slow stop: still long through the crash
    ta::CryptoTrendEngine eng(md, cfg, {}, 1e6);
    for (const auto& d : md.dates()) eng.step(d);
    CHECK(eng.risk().state() == ta::RiskState::Off);
    CHECK(eng.exposure("BTC") == 0);
}

TEST(crypto_exit_tests_yesterdays_stop) {
    // Lookback 5: long at the close of 90 with stop = mid 70. Next day the 50 drops out
    // of the window, lifting today's mid to 75; a close of 75 is above yesterday's stop
    // (70), so the paper stays long. Ratcheting before the test would exit a day early.
    ta::Series s;
    for (int k = 0; k < 95; ++k) {
        const double c = k % 2 ? 102 : 100;
        s.push_back({th::iso_day(k), c, c, c, c, 1});
    }
    int k = 95;
    for (double c : {50.0, 60.0, 70.0, 80.0, 90.0, 75.0}) {
        s.push_back({th::iso_day(k), c, c, c, c, 1});
        ++k;
    }
    ta::CryptoTrendConfig cfg;
    cfg.lookbacks = {5};
    const ta::MarketData md({{"BTC", s}});
    ta::CryptoTrendEngine eng(md, cfg, {}, 1e6);
    for (const auto& d : md.dates()) eng.step(d);
    CHECK(eng.exposure("BTC") > 0);
}
