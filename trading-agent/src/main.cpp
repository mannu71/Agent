// ta_backtest: backtests every sleeve on local data files, plus the gate-1 report.

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>

#include "args.hpp"
#include "ta/a1.hpp"
#include "ta/a2.hpp"
#include "ta/crypto_trend.hpp"
#include "ta/csv.hpp"
#include "ta/d1.hpp"
#include "ta/options.hpp"
#include "ta/regime.hpp"
#include "ta/settings.hpp"
#include "ta/validate.hpp"

namespace fs = std::filesystem;

namespace {

const char* kUsage = R"(usage: ta_backtest <sleeve> ... [options]

  a1 <equity_dir>   momentum-leader breakouts (daily NSE CSVs, one per symbol)
       --exclude F --index F --vix F --events F   regime and exclusion inputs
       --gate      run the 2x-cost rerun and a 9-trial grid; print the gate-1 report
  a2 <equity_dir> --intraday DIR --catalysts F --bands F
                    episodic-pivot gaps; prints the approved-vs-rejected uplift
  b  <crypto_dir> [--funding-dir DIR] [--cost-bps 10] [--tax 0.312]
                    crypto Donchian ensemble; prints entries per lookback
  d1 <nifty_fut_5min.csv> [--cost-points 19] [--skip-days F]
                    Nifty last-half-hour momentum
  options --legs "P:22000:-1:85,P:21800:1:40,..." --expiry YYYY-MM-DD --today YYYY-MM-DD
          --active-book N [--existing N] [--vol-red] [--blackout] [--lot 65]
                    validate and size a defined-risk structure (Sleeve C)

common: --equity N  --start YYYY-MM-DD  --end YYYY-MM-DD  --config F (key = value overrides)
        --cost-rt FRAC (a1/a2 round-trip cost)  --trades OUT.csv  --equity-out OUT.csv
Results are pre-tax except Sleeve B, which models the worst-case Indian VDA tax.
)";

void print_metrics(const ta::Metrics& m, double periods = 252) {
    std::cout << std::fixed << std::setprecision(3) << "trades         " << m.trades << "\nwin rate       "
              << m.win_rate << "\navg R          " << m.avg_r << "\nprofit factor  " << m.profit_factor
              << "\ntotal return   " << m.total_return << "\nCAGR           " << m.cagr << "\nmax drawdown   "
              << m.max_drawdown << "\nSharpe         " << m.sharpe << "  (" << periods << " periods/yr)\n";
}

void write_outputs(const cli::Args& a, const std::vector<ta::Trade>& trades, const ta::EquityCurve& curve) {
    if (a.has("trades")) {
        std::ofstream out(a.str("trades"));
        out << ta::trades_csv_header() << '\n';
        for (const auto& t : trades) out << ta::to_csv(t) << '\n';
    }
    if (a.has("equity-out")) {
        std::ofstream out(a.str("equity-out"));
        out << "date,equity\n";
        for (const auto& [d, e] : curve) out << d << ',' << std::fixed << e << '\n';
    }
}

ta::Config config_of(const cli::Args& a) {
    if (!a.has("config")) return {};
    auto c = ta::load_config(a.str("config"));
    ta::check_settings_keys(c);
    return c;
}

int run_a1(const cli::Args& a) {
    const ta::MarketData md(ta::load_universe(a.pos(1)));
    ta::A1Config cfg;
    ta::apply_settings(config_of(a), cfg);
    if (a.has("cost-rt")) cfg.cost.buy_frac = cfg.cost.sell_frac = a.num("cost-rt", 0.005) / 2;
    const ta::ExclusionList excl = a.has("exclude") ? ta::ExclusionList::load(a.str("exclude")) : ta::ExclusionList{};
    ta::Series index;
    std::map<std::string, double> vix;
    ta::EventCalendar events;
    ta::EquityRegimeInputs reg;
    if (a.has("index")) reg.index = &(index = ta::load_series(a.str("index")));
    if (a.has("vix")) reg.vix = &(vix = ta::load_dated_values(a.str("vix")));
    if (a.has("events")) reg.events = &(events = ta::EventCalendar::load(a.str("events")));
    const double equity = a.num("equity", 1e6);
    const auto r = ta::run_a1_backtest(md, cfg, excl, equity, a.str("start"), a.str("end"), reg);
    std::cout << "symbols        " << md.universe().size() << '\n';
    print_metrics(r.metrics);
    std::cout << "risk state     " << ta::to_string(r.final_risk_state) << "\n(pre-tax; costs "
              << cfg.cost.round_trip() * 100 << "% round trip)\n";
    write_outputs(a, r.trades, r.equity_curve);

    if (a.has("gate")) {
        ta::A1Config stress = cfg;
        stress.cost.buy_frac *= 2;
        stress.cost.sell_frac *= 2;
        const auto r2 = ta::run_a1_backtest(md, stress, excl, equity, a.str("start"), a.str("end"), reg);
        // Every grid point counts as a trial for the deflated Sharpe ratio and PBO.
        std::vector<std::vector<double>> perf;
        std::vector<double> trial_sr;
        for (double runup : {0.25, 0.30, 0.40}) {
            for (double adr : {0.03, 0.04, 0.05}) {
                ta::A1Config g = cfg;
                g.screen.min_runup = runup;
                g.screen.min_adr = adr;
                const auto rg = ta::run_a1_backtest(md, g, excl, equity, a.str("start"), a.str("end"), reg);
                const auto ret = ta::periodic_returns(rg.equity_curve);
                if (perf.empty()) perf.assign(ret.size(), {});
                for (std::size_t t = 0; t < ret.size() && t < perf.size(); ++t) perf[t].push_back(ret[t]);
                const double sd = ta::stdev(ret);
                trial_sr.push_back(sd > 0 ? ta::mean(ret) / sd : 0);
            }
        }
        const double var_sr = std::pow(ta::stdev(trial_sr), 2);
        const double pbo = ta::pbo_cscv(perf, 16);
        const auto rep = ta::evaluate_gate1(r.trades, r.equity_curve, r2.trades,
                                            static_cast<int>(trial_sr.size()), var_sr, pbo);
        const auto rets = ta::periodic_returns(r.equity_curve);
        std::cout << "\n--- gate 1 (" << trial_sr.size() << " trials) ---\n" << rep.text()
                  << "bootstrap 95th-pct drawdown " << ta::bootstrap_drawdown_p95(rets, 500, 20, 7) << "\n"
                  << "trades needed for t=3 at this edge "
                  << ta::trades_needed(ta::mean(ta::r_multiples(r.trades)), ta::stdev(ta::r_multiples(r.trades)))
                  << "\nAlso required before capital: walk-forward/holdout, paper trading, and beating a\n"
                     "Nifty200 Momentum 30 index fund after tax.\n";
    }
    return 0;
}

int run_a2(const cli::Args& a) {
    const ta::MarketData md(ta::load_universe(a.pos(1)));
    ta::A2Config cfg;
    ta::apply_settings(config_of(a), cfg);
    if (a.has("cost-rt")) cfg.cost.buy_frac = cfg.cost.sell_frac = a.num("cost-rt", 0.005) / 2;
    ta::IntradayData intra;
    if (!a.has("intraday")) throw std::runtime_error("a2 needs --intraday DIR");
    for (auto& [sym, s] : ta::load_universe(a.str("intraday"))) intra.bars[sym] = std::move(s);
    intra.finalize();
    ta::A2Inputs in;
    in.intraday = &intra;
    if (a.has("catalysts")) in.catalysts = ta::load_catalysts(a.str("catalysts"));
    if (a.has("bands")) ta::load_bands(a.str("bands"), in);
    if (a.has("exclude")) in.excluded = ta::ExclusionList::load(a.str("exclude"));
    const auto r = ta::run_a2_backtest(md, cfg, in, a.num("equity", 1e6), a.str("start"), a.str("end"));
    print_metrics(r.portfolio.metrics);
    std::cout << "events         " << r.events.size() << "\napproved R     " << r.approved_mean_r << " (n="
              << r.approved_n << ")\nrejected R     " << r.rejected_mean_r << " (n=" << r.rejected_n
              << ")\nuplift         " << r.uplift << "  t=" << r.uplift_t
              << "\n(pass needs uplift >= 0.15R with t >= 2 and >= 300 out-of-sample events)\n";
    write_outputs(a, r.portfolio.trades, r.portfolio.equity_curve);
    return 0;
}

int run_b(const cli::Args& a) {
    const ta::MarketData md(ta::load_universe(a.pos(1)));
    ta::CryptoTrendConfig cfg;
    ta::apply_settings(config_of(a), cfg);
    cfg.cost_bps = a.num("cost-bps", cfg.cost_bps);
    cfg.tax_rate = a.num("tax", cfg.tax_rate);
    std::map<std::string, ta::CryptoAux> aux;
    if (a.has("funding-dir")) {
        for (const auto& [asset, s] : md.universe()) {
            const auto f = fs::path(a.str("funding-dir")) / (asset + ".csv");
            if (fs::exists(f)) aux[asset].funding = ta::load_dated_values(f.string());
        }
    }
    const auto r = ta::run_crypto_backtest(md, cfg, aux, a.num("equity", 1e6), a.str("start"), a.str("end"));
    print_metrics(r.metrics, cfg.periods_per_year);
    std::cout << "tax paid       " << r.tax_paid << "\nfunding paid   " << r.funding_paid << "\ncosts paid     "
              << r.costs_paid << "\nentries by lookback (reproduce-first check vs the paper's BTC counts\n"
              << "292,156,78,49,28,20,15,9,5 within 5%):\n";
    for (const auto& [n, c] : r.entries_by_lookback) std::cout << "  " << n << ": " << c << '\n';
    write_outputs(a, r.trades, r.equity_curve);
    return 0;
}

int run_d1(const cli::Args& a) {
    const ta::Series bars = ta::load_series(a.pos(1));
    ta::D1Config cfg;
    ta::apply_settings(config_of(a), cfg);
    cfg.cost_points = a.num("cost-points", cfg.cost_points);
    if (a.has("skip-days")) {
        for (const auto& d : ta::load_symbol_list(a.str("skip-days"))) cfg.skip_days.insert(d);
    }
    const auto r = ta::run_d1_backtest(bars, cfg, a.num("equity", 2e6), a.str("start"), a.str("end"));
    print_metrics(r.metrics);
    std::cout << "mean move      " << r.mean_move_points << " points in signal direction (t=" << r.move_t
              << ")\ncost           " << r.cost_points << " points round trip\nswitch-on gate "
              << (r.passes_gate ? "PASS" : "FAIL") << " (>= 2x cost, t > 3, >= 250 trades)\n";
    write_outputs(a, r.trades, r.equity_curve);
    return 0;
}

int run_options(const cli::Args& a) {
    const auto s = ta::parse_structure(a.str("legs"), a.str("expiry"), a.num("lot", 65));
    ta::OptionsConfig cfg;
    ta::apply_settings(config_of(a), cfg);
    ta::OptionsContext ctx;
    ctx.today = a.str("today");
    ctx.active_book = a.num("active-book", 0);
    ctx.existing_max_loss = a.num("existing", 0);
    ctx.vol_gate_red = a.has("vol-red");
    ctx.event_blackout = a.has("blackout");
    const auto c = ta::check_structure(s, cfg, ctx);
    std::cout << std::fixed << std::setprecision(2) << "net credit     " << ta::net_credit(s) << "\nmax profit     "
              << ta::max_profit(s) << "\nmax loss       " << ta::max_loss(s) << "\nbreakevens    ";
    for (double b : ta::breakevens(s)) std::cout << ' ' << b;
    std::cout << "\ncredit - costs " << c.credit_after_costs_each << "\nlots allowed   " << c.multiplier << '\n';
    for (const auto& v : c.violations) std::cout << "VIOLATION      " << v << '\n';
    std::cout << (c.ok ? "OK\n" : "REJECTED\n");
    return c.ok ? 0 : 3;
}

}  // namespace

int main(int argc, char** argv) {
    try {
        const cli::Args a(argc, argv, 1, {"gate", "vol-red", "blackout"});
        if (a.positional().empty()) {
            std::cerr << kUsage;
            return 2;
        }
        const std::string cmd = a.pos(0);
        if (cmd == "a1") return run_a1(a);
        if (cmd == "a2") return run_a2(a);
        if (cmd == "b") return run_b(a);
        if (cmd == "d1") return run_d1(a);
        if (cmd == "options") return run_options(a);
        std::cerr << kUsage;
        return 2;
    } catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << '\n';
        return 1;
    }
}
