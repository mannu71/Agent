// ta_backtest: backtests every sleeve on local data files, plus the gate-1 report.

#include <algorithm>
#include <cmath>
#include <limits>
#include <map>
#include <set>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>

#include "args.hpp"
#include "ta/a1.hpp"
#include "ta/a2.hpp"
#include "ta/casebook.hpp"
#include "ta/crypto_trend.hpp"
#include "ta/csv.hpp"
#include "ta/d1.hpp"
#include "ta/journal.hpp"
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
  b  <crypto_dir> [--funding-dir DIR] [--cost-bps 25] [--tax 0.312] [--replicate]
                    crypto Donchian ensemble; prints entries per lookback. --replicate runs
                    the paper's setup (10 bps, no funding/tax/overlay) for the reproduce-first check
  d1 <nifty_fut_5min.csv> [--spot F] [--cost-frac 0.0006] [--slippage-points 3] [--skip-days F]
                    Nifty last-half-hour momentum; --spot takes the signal from the spot index
  setups <dir_of_1m_csvs> [--tf 15] [--patterns sweep,fvg,...] [--symbols BTC,ETH]
                    institutional-concept setups on 1-minute bars with taker-buy volume:
                    per-pattern edge vs a matched random entry, then the memory-gated
                    portfolio. --no-memory, --tax 0.312, --cost-mult 2, --cases OUT.csv,
                    --memory-fields concept,tf,side,trend, --trial-log F --label T --gate,
                    --trade-from DATE (earlier setups only warm up the memory)
  options --legs "P:22000:-1:85,P:21800:1:40,..." --expiry YYYY-MM-DD --today YYYY-MM-DD
          --active-book N [--existing N] [--vol-red] [--blackout] [--lot 65]
  a1 also takes --trial-log FILE [--label TEXT]: every configuration run is appended
     (deduplicated by a hash of its settings) and --gate counts all logged trials
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

// Risk keys are written in active-book units (the rulebook's). Convert them exactly as
// the paper runner does, treating --equity as this sleeve's capital, so a backtest
// validates the same risk that is paper-traded.
void sleeve_units(const ta::Config& c, ta::RiskConfig& r, double sleeve_capital, bool is_a2) {
    ta::Allocation alloc;
    ta::apply_settings(c, alloc);
    const double share = is_a2 ? (alloc.a2 > 0 ? alloc.a2 : alloc.reserve) : alloc.a1;
    if (share <= 0) throw std::runtime_error("allocation share for this sleeve is zero");
    const double active_book = sleeve_capital * alloc.active_share() / share;
    ta::to_sleeve_units(r, active_book, sleeve_capital);
    std::cout << "risk units     active book " << std::fixed << std::setprecision(0) << active_book
              << " -> " << std::setprecision(4) << r.risk_per_trade * 100 << "% of sleeve per trade\n";
}

int run_a1(const cli::Args& a) {
    const ta::MarketData md(ta::load_universe(a.pos(1)));
    const ta::Config conf = config_of(a);
    ta::A1Config cfg;
    ta::apply_settings(conf, cfg);
    sleeve_units(conf, cfg.risk, a.num("equity", 1e6), false);
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

    // Fingerprint of everything that determines this run, so the trial log can tell a new
    // configuration from a rerun of an old one.
    // Every effective setting is included with numbers normalised, so the same configuration
    // hashes the same whether a value came from a default, a config file or the grid.
    auto fingerprint = [&](const ta::Config& c) {
        std::string f = "a1|" + a.pos(1);
        for (const char* k : {"start", "end", "equity", "cost-rt", "exclude", "index", "vix", "events"}) {
            f += std::string("|") + k + "=" + a.str(k);
        }
        for (const auto& [k, def] : ta::default_settings()) {
            const auto it = c.find(k);
            std::string v = it == c.end() ? def : it->second;
            try {
                std::size_t used = 0;
                const double x = std::stod(v, &used);
                if (used == v.size()) v = ta::fmt_double(x);
            } catch (const std::exception&) {
            }
            f += "|" + k + "=" + v;
        }
        return f;
    };
    const std::string fp = fingerprint(conf);
    const std::string log = a.str("trial-log");
    if (!log.empty()) {
        const bool added = ta::log_trial(log, {"A1", ta::sha256_hex(fp), a.str("label", "run"), r.equity_curve});
        std::cout << (added ? "trial logged" : "trial already in log (same settings)") << " -> " << log << '\n';
    }

    if (a.has("gate")) {
        ta::A1Config stress = cfg;
        stress.cost.buy_frac *= 2;
        stress.cost.sell_frac *= 2;
        const auto r2 = ta::run_a1_backtest(md, stress, excl, equity, a.str("start"), a.str("end"), reg);
        // Every grid point is a trial for the deflated Sharpe ratio and PBO.
        std::vector<ta::TrialRecord> grid;
        for (double runup : {0.25, 0.30, 0.40}) {
            for (double adr : {0.03, 0.04, 0.05}) {
                ta::A1Config g = cfg;
                g.screen.min_runup = runup;
                g.screen.min_adr = adr;
                const auto rg = ta::run_a1_backtest(md, g, excl, equity, a.str("start"), a.str("end"), reg);
                ta::Config gc = conf;
                gc["a1.min_runup"] = ta::fmt_double(runup);
                gc["a1.min_adr"] = ta::fmt_double(adr);
                const std::string tag = "grid runup=" + ta::fmt_double(runup) + " adr=" + ta::fmt_double(adr);
                grid.push_back({"A1", ta::sha256_hex(fingerprint(gc)), tag, rg.equity_curve});
            }
        }
        std::vector<ta::TrialRecord> trials = grid;
        if (!log.empty()) {
            for (const auto& t : grid) ta::log_trial(log, t);
            trials = ta::load_trials(log, "A1");
        } else {
            std::cout << "warning: no --trial-log, so N counts only the 9 grid points; every other\n"
                         "configuration you have tried is missing from the deflated Sharpe and PBO.\n";
        }
        const ta::TrialStats st = ta::trial_stats(trials, 16);
        const auto rep = ta::evaluate_gate1(r.trades, r.equity_curve, r2.trades, st.n, st.var_sharpe, st.pbo);
        const auto rets = ta::periodic_returns(r.equity_curve);
        std::cout << "\n--- gate 1 (" << st.n << " trials, " << st.common_periods << " common periods) ---\n"
                  << rep.text() << "bootstrap 95th-pct drawdown " << ta::bootstrap_drawdown_p95(rets, 500, 20, 7)
                  << "\ntrades needed for t=3 at this edge "
                  << ta::trades_needed(ta::mean(ta::r_multiples(r.trades)), ta::stdev(ta::r_multiples(r.trades)))
                  << "\nAlso required before capital: walk-forward/holdout, paper trading, and beating a\n"
                     "Nifty200 Momentum 30 index fund after tax.\n";
    }
    return 0;
}

int run_a2(const cli::Args& a) {
    const ta::MarketData md(ta::load_universe(a.pos(1)));
    const ta::Config conf = config_of(a);
    ta::A2Config cfg;
    ta::apply_settings(conf, cfg);
    sleeve_units(conf, cfg.risk, a.num("equity", 1e6), true);
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
    if (a.has("replicate")) {
        cfg.replication_mode();
        std::cout << "replication mode: 10 bps, no funding, no tax, no drawdown/crowding overlay\n";
    }
    cfg.cost_bps = a.num("cost-bps", cfg.cost_bps);
    cfg.tax_rate = a.num("tax", cfg.tax_rate);
    std::map<std::string, ta::CryptoAux> aux;
    if (a.has("funding-dir") && !a.has("replicate")) {
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
    cfg.cost_frac = a.num("cost-frac", cfg.cost_frac);
    cfg.slippage_points = a.num("slippage-points", cfg.slippage_points);
    ta::Series spot;
    if (a.has("spot")) spot = ta::load_series(a.str("spot"));
    if (a.has("skip-days")) {
        for (const auto& d : ta::load_symbol_list(a.str("skip-days"))) cfg.skip_days.insert(d);
    }
    const auto r = ta::run_d1_backtest(bars, cfg, a.num("equity", 2e6), a.str("start"), a.str("end"),
                                       spot.empty() ? nullptr : &spot);
    if (spot.empty()) std::cout << "note: signal from futures bars; pass --spot to avoid roll-day carry\n";
    print_metrics(r.metrics);
    std::cout << "mean move      " << r.mean_move_points << " points in signal direction (t=" << r.move_t
              << ")\ncost           " << r.cost_points << " points round trip (mean)\nslope beta     "
              << r.beta << " (t=" << r.beta_t << "), archive below " << cfg.min_beta << '\n';
    for (const auto& [year, bn] : r.beta_by_year) {
        std::cout << "  " << year << ": beta " << bn.first << " (" << bn.second << " days)\n";
    }
    std::cout << "2022+ net      " << r.net_points_2022_on << " points per trade\nswitch-on gate "
              << (r.passes_gate ? "PASS" : "FAIL")
              << " (>= 2x cost, t > 3, >= 250 trades, beta >= min, 2022+ net > 0)\n";
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

std::vector<std::string> split_list(const std::string& s) {
    std::vector<std::string> out;
    std::string cur;
    for (char ch : s + ",") {
        if (ch == ',') {
            if (!cur.empty()) out.push_back(cur);
            cur.clear();
        } else if (ch != ' ') {
            cur += ch;
        }
    }
    return out;
}

struct RStats {
    std::size_t n = 0;
    double mean = 0, sd = 0;
    double t() const { return n > 1 && sd > 0 ? mean / (sd / std::sqrt(static_cast<double>(n))) : 0; }
};

RStats rstats(const std::vector<double>& v) {
    RStats s;
    s.n = v.size();
    if (v.empty()) return s;
    s.mean = ta::mean(v);
    s.sd = v.size() > 1 ? ta::stdev(v) : 0;
    return s;
}

int run_setups(const cli::Args& a) {
    const std::string dir = a.pos(1);
    const int tf = static_cast<int>(a.num("tf", 15));
    const auto patterns_v = split_list(a.str("patterns", "sweep,sweep_absorb,sweep_cont,fvg,ob,choch,hl_pullback,spring,va80"));
    const std::set<std::string> patterns(patterns_v.begin(), patterns_v.end());
    std::vector<std::string> symbols = split_list(a.str("symbols"));
    if (symbols.empty()) {
        for (const auto& e : fs::directory_iterator(dir)) {
            if (e.path().extension() == ".csv") symbols.push_back(e.path().stem().string());
        }
        std::sort(symbols.begin(), symbols.end());
    }
    const ta::Config conf = config_of(a);
    ta::ConceptConfig cc;
    ta::apply_settings(conf, cc);
    const ta::ExecCost cost = ta::ExecCost{}.scaled(a.num("cost-mult", 1.0));
    const ta::ExecCost cost2 = cost.scaled(2.0);
    const std::int64_t start = a.has("start") ? ta::parse_minutes(a.str("start")) : 0;
    const std::int64_t end = a.has("end") ? ta::parse_minutes(a.str("end")) + 1440 : std::numeric_limits<std::int64_t>::max();

    std::vector<ta::Case> cases, cases2, randoms;
    for (const auto& sym : symbols) {
        const ta::FlowSeries m1 = ta::load_flow((fs::path(dir) / (sym + ".csv")).string());
        const ta::FlowSeries bars = ta::resample(m1, tf);
        const ta::FlowSeries daily = ta::resample(m1, 1440);
        std::size_t n_sym = 0;
        std::vector<ta::Case> sym_cases;
        for (const auto& st : ta::detect_setups(bars, daily, tf, sym, patterns, cc)) {
            if (st.t < start || st.t >= end) continue;
            sym_cases.push_back(ta::simulate_case(st, m1, cost));
            cases2.push_back(ta::simulate_case(st, m1, cost2));
            ++n_sym;
        }
        const std::map<std::string, const ta::FlowSeries*> one = {{sym, &m1}};
        for (auto& c : ta::random_cases(sym_cases, one, cost, 17u + static_cast<unsigned>(randoms.size()))) {
            randoms.push_back(std::move(c));
        }
        cases.insert(cases.end(), sym_cases.begin(), sym_cases.end());
        std::cerr << sym << ": " << m1.size() << " minutes, " << n_sym << " setups\n";
    }

    // Edge of each pattern on its own (every filled setup), against the matched random entries.
    std::cout << std::fixed << std::setprecision(3)
              << "pattern       side   setups filled  net R   t     gross R | random net R  edge vs random  t\n";
    for (const auto& p : patterns_v) {
        for (int side : {1, -1}) {
            std::vector<double> net, gross, rnd;
            std::size_t setups = 0;
            for (const auto& c : cases) {
                if (c.setup.pattern != p || c.setup.side != side) continue;
                ++setups;
                if (c.filled) {
                    net.push_back(c.r_net);
                    gross.push_back(c.r_gross);
                }
            }
            for (const auto& c : randoms) {
                if (c.filled && c.setup.pattern == p && c.setup.side == side) rnd.push_back(c.r_net);
            }
            const RStats n = rstats(net), g = rstats(gross), r = rstats(rnd);
            const double se = std::sqrt((n.n > 1 ? n.sd * n.sd / n.n : 0) + (r.n > 1 ? r.sd * r.sd / r.n : 0));
            std::cout << std::left << std::setw(13) << p << std::setw(6) << (side > 0 ? "long" : "short") << std::right
                      << std::setw(7) << setups << std::setw(7) << n.n << std::setw(8) << n.mean << std::setw(7)
                      << n.t() << std::setw(9) << g.mean << " | " << std::setw(11) << r.mean << std::setw(14)
                      << (n.mean - r.mean) << std::setw(9) << (se > 0 ? (n.mean - r.mean) / se : 0) << '\n';
        }
    }

    ta::PortfolioConfig pc;
    pc.tax_rate = a.num("tax", 0.0);
    pc.recall.enabled = !a.has("no-memory");
    pc.recall.min_cases = static_cast<std::size_t>(a.num("min-cases", 30));
    if (a.has("trade-from")) pc.trade_from = ta::parse_minutes(a.str("trade-from"));
    if (a.has("memory-fields")) pc.recall.fields = split_list(a.str("memory-fields"));
    const auto res = ta::run_casebook(cases, pc);
    const auto res2 = ta::run_casebook(cases2, pc);
    std::cout << "\nportfolio (" << (pc.recall.enabled ? "memory-gated" : "every setup") << ", risk "
              << pc.risk_frac * 100 << "% per trade, max " << pc.max_open << " open, tax " << pc.tax_rate * 100
              << "%)\nsetups " << res.setups << "  filled " << res.filled << "  passed recall " << res.recalled
              << "  taken " << res.taken << "  tax paid " << res.tax_paid << '\n';
    print_metrics(ta::compute_metrics(res.trades, res.curve, 365.0), 365.0);
    write_outputs(a, res.trades, res.curve);
    if (a.has("cases")) {
        std::ofstream out(a.str("cases"));
        out << "time,pattern,symbol,tf,side,order,trend,session,vol,vwap,flow,filled,entry_time,exit_time,entry,stop,"
               "target,exit,r_gross,r_net,exit_reason\n";
        for (const auto& c : cases) {
            const auto& s = c.setup;
            out << ta::format_minutes(s.t) << ',' << s.pattern << ',' << s.symbol << ',' << s.tf << ',' << s.side << ','
                << s.order << ',' << s.ctx.trend << ',' << s.ctx.session << ',' << s.ctx.vol << ',' << s.ctx.vwap << ','
                << s.ctx.flow << ',' << c.filled << ',' << (c.filled ? ta::format_minutes(c.entry_t) : "") << ','
                << (c.filled ? ta::format_minutes(c.exit_t) : "") << ',' << c.entry_px << ',' << c.stop_px << ','
                << c.target_px << ',' << c.exit_px << ',' << c.r_gross << ',' << c.r_net << ',' << c.exit_reason << '\n';
        }
    }

    // Trial log: every configuration run counts toward the deflated Sharpe and PBO.
    std::string fp = "setups|" + dir + "|tf=" + std::to_string(tf) + "|patterns=" + a.str("patterns") +
                     "|symbols=" + a.str("symbols") + "|start=" + a.str("start") + "|end=" + a.str("end") +
                     "|cost=" + a.str("cost-mult") + "|tax=" + a.str("tax") + "|memory=" + (pc.recall.enabled ? "1" : "0") +
                     "|fields=" + a.str("memory-fields") + "|min=" + a.str("min-cases") +
                     "|trade_from=" + a.str("trade-from");
    for (const auto& [k, v] : conf) fp += "|" + k + "=" + v;
    const std::string log = a.str("trial-log");
    if (!log.empty()) {
        const bool added = ta::log_trial(log, {"SETUPS", ta::sha256_hex(fp), a.str("label", "run"), res.curve});
        std::cout << (added ? "trial logged" : "trial already in log (same settings)") << " -> " << log << '\n';
    }
    if (a.has("gate")) {
        if (log.empty()) throw std::runtime_error("--gate needs --trial-log so every tried variant is counted");
        const ta::TrialStats st = ta::trial_stats(ta::load_trials(log, "SETUPS"), 16);
        const auto rep = ta::evaluate_gate1(res.trades, res.curve, res2.trades, st.n, st.var_sharpe, st.pbo);
        std::cout << "\n--- gate 1 (" << st.n << " trials, " << st.common_periods << " common periods) ---\n"
                  << rep.text();
    }
    return 0;
}

}  // namespace

int main(int argc, char** argv) {
    try {
        const cli::Args a(argc, argv, 1, {"gate", "vol-red", "blackout", "replicate", "no-memory"});
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
        if (cmd == "setups") return run_setups(a);
        std::cerr << kUsage;
        return 2;
    } catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << '\n';
        return 1;
    }
}
