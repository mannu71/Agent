#include "ta/paper.hpp"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <limits>
#include <set>
#include <sstream>
#include <stdexcept>

#include "ta/csv.hpp"
#include "ta/data.hpp"
#include "ta/journal.hpp"
#include "ta/monitor.hpp"
#include "ta/regime.hpp"
#include "ta/settings.hpp"
#include "ta/validate.hpp"

namespace fs = std::filesystem;

namespace ta {

namespace {

std::vector<KvRecord> read_records(const fs::path& path) {
    std::vector<KvRecord> out;
    std::ifstream in(path);
    std::string line;
    while (std::getline(in, line)) {
        if (!line.empty()) out.push_back(KvRecord::parse(line));
    }
    return out;
}

void write_records(const fs::path& path, const std::vector<KvRecord>& records) {
    const fs::path tmp = path.string() + ".tmp";
    {
        std::ofstream out(tmp);
        if (!out) throw std::runtime_error("cannot write " + tmp.string());
        for (const auto& r : records) out << r.encode() << '\n';
        if (!out) throw std::runtime_error("write failed: " + tmp.string());
    }
    fs::rename(tmp, path);  // atomic replace: a crash never leaves a half-written state file
}

void append_line(const fs::path& path, const std::string& header, const std::string& line) {
    const bool fresh = !fs::exists(path);
    std::ofstream out(path, std::ios::app);
    if (!out) throw std::runtime_error("cannot append to " + path.string());
    if (fresh) out << header << '\n';
    out << line << '\n';
}

std::vector<Trade> read_trades(const fs::path& path) {
    std::vector<Trade> out;
    std::ifstream in(path);
    std::string line;
    bool header = true;
    while (std::getline(in, line)) {
        if (header) {
            header = false;
            continue;
        }
        std::vector<std::string> f;
        std::stringstream ss(line);
        std::string x;
        while (std::getline(ss, x, ',')) f.push_back(x);
        if (f.size() < 11) continue;
        Trade t;
        t.sleeve = f[0];
        t.symbol = f[1];
        t.side = std::stoi(f[2]);
        t.entry_date = f[3];
        t.exit_date = f[4];
        t.entry_price = std::stod(f[5]);
        t.qty = std::stol(f[6]);
        t.risk_per_share = std::stod(f[7]);
        t.pnl = std::stod(f[8]);
        t.r_multiple = std::stod(f[9]);
        t.exit_reason = f[10];
        out.push_back(t);
    }
    return out;
}

// Closed trades of one sleeve, optionally only those closed after `after` (a manual
// reset), so a reviewed and reset sleeve is not immediately re-killed by old history.
std::vector<Trade> of_sleeve(const std::vector<Trade>& all, const std::string& sleeve, const std::string& after = "") {
    std::vector<Trade> out;
    for (const auto& t : all) {
        if (t.sleeve == sleeve && (after.empty() || t.exit_date > after)) out.push_back(t);
    }
    return out;
}

std::string str_or(const Config& c, const std::string& k, const std::string& def = "") {
    const auto it = c.find(k);
    return it == c.end() ? def : it->second;
}

double num_or(const Config& c, const std::string& k, double def) {
    const auto it = c.find(k);
    return it == c.end() ? def : std::stod(it->second);
}

Universe load_dir_if(const std::string& dir) {
    if (dir.empty()) return {};
    if (!fs::is_directory(dir)) throw std::runtime_error("not a directory: " + dir);
    return load_universe(dir);
}

std::string money(double v) {
    std::ostringstream o;
    o << std::fixed << std::setprecision(0) << v;
    return o.str();
}

}  // namespace

struct PaperAccount::Impl {
    fs::path dir;
    Config cfg;
    double capital = 0;
    std::string start;
    Allocation alloc;
    double active_book = 0;

    // Data. Engines keep references, so these must outlive them.
    std::unique_ptr<MarketData> equity_md, crypto_md;
    Series index, nifty_fut;
    std::map<std::string, double> vix;
    EventCalendar events;
    bool has_events = false;
    IntradayData intraday;
    ExclusionList excluded;

    std::unique_ptr<A1Engine> a1;
    std::unique_ptr<A2Engine> a2;
    A2Config a2_cfg;
    std::unique_ptr<CryptoTrendEngine> b;
    std::unique_ptr<D1Engine> d1;
    RiskManager breaker{active_book_breaker_config()};
    std::map<std::string, std::string> reset_dates;  // sleeve -> date of last manual reset
    std::map<std::string, std::string> kill_state;   // sleeve -> last kill-rule action journaled
    std::map<std::string, double> shadow_cache;      // "date|symbol" -> final A2 shadow R
    double cash_parked = 0;   // active-book capital not traded by any engine (C budget, buffer, reserve)
    double last_active = 0;
    std::string last_date;

    fs::path state(const std::string& name) const { return dir / "state" / (name + ".state"); }

    void load() {
        cfg = load_config((dir / "account.cfg").string());
        check_settings_keys(cfg);
        capital = num_or(cfg, "account.capital", 0);
        start = str_or(cfg, "account.start");
        if (capital <= 0) throw std::runtime_error("account.capital must be > 0");
        apply_settings(cfg, alloc);
        const std::string bad = alloc.validate();
        if (!bad.empty()) throw std::runtime_error("allocation: " + bad);
        active_book = alloc.active_share() * capital;

        if (!str_or(cfg, "data.exclusions").empty()) excluded = ExclusionList::load(str_or(cfg, "data.exclusions"));
        if (!str_or(cfg, "data.index").empty()) index = load_series(str_or(cfg, "data.index"));
        if (!str_or(cfg, "data.vix").empty()) vix = load_dated_values(str_or(cfg, "data.vix"));
        if (!str_or(cfg, "data.events").empty()) {
            events = EventCalendar::load(str_or(cfg, "data.events"));
            has_events = true;
        }

        const double a1_cap = alloc.a1 * capital;
        const double a2_cap = num_or(cfg, "paper.a2_capital", (alloc.a2 > 0 ? alloc.a2 : alloc.reserve) * capital);
        const double b_cap = alloc.b * capital;
        // D1 has no live allocation; paper-trade it with headroom above its Rs 15 lakh minimum
        // so one losing day does not stop the forward test.
        const double d1_cap = num_or(cfg, "paper.d1_capital", std::max(alloc.d * capital, 2.0e6));
        // Active-book cash that no engine trades: the broker buffer, the options max-loss
        // budget (Sleeve C is validated, not simulated) and the A2 reserve.
        cash_parked = (alloc.buffer + alloc.c + alloc.reserve) * capital;

        const std::string eq_dir = str_or(cfg, "data.equity_dir");
        if (!eq_dir.empty()) {
            equity_md = std::make_unique<MarketData>(load_dir_if(eq_dir));
            A1Config c1;
            apply_settings(cfg, c1);
            // Rulebook units are fractions of the active book; convert to this sleeve's capital.
            to_sleeve_units(c1.risk, active_book, a1_cap);
            EquityRegimeInputs reg;
            reg.index = index.empty() ? nullptr : &index;
            reg.vix = vix.empty() ? nullptr : &vix;
            reg.events = has_events ? &events : nullptr;
            if (a1_cap > 0) a1 = std::make_unique<A1Engine>(*equity_md, c1, excluded, reg, a1_cap);

            const std::string intra_dir = str_or(cfg, "data.intraday_dir");
            if (!intra_dir.empty() && a2_cap > 0) {
                for (auto& [sym, s] : load_dir_if(intra_dir)) intraday.bars[sym] = std::move(s);
                intraday.finalize();
                A2Config c2;
                apply_settings(cfg, c2);
                to_sleeve_units(c2.risk, active_book, a2_cap);
                A2Inputs in;
                in.intraday = &intraday;
                in.excluded = excluded;
                if (!str_or(cfg, "data.catalysts").empty()) in.catalysts = load_catalysts(str_or(cfg, "data.catalysts"));
                if (!str_or(cfg, "data.bands").empty()) load_bands(str_or(cfg, "data.bands"), in);
                a2_cfg = c2;
                a2 = std::make_unique<A2Engine>(*equity_md, c2, in, a2_cap);
            }
        }

        const std::string cr_dir = str_or(cfg, "data.crypto_dir");
        if (!cr_dir.empty() && b_cap > 0) {
            crypto_md = std::make_unique<MarketData>(load_dir_if(cr_dir));
            CryptoTrendConfig cb;
            apply_settings(cfg, cb);
            std::map<std::string, CryptoAux> aux;
            const std::string fdir = str_or(cfg, "data.crypto_funding_dir");
            const std::string odir = str_or(cfg, "data.crypto_oi_dir");
            for (const auto& [asset, s] : crypto_md->universe()) {
                if (!fdir.empty() && fs::exists(fs::path(fdir) / (asset + ".csv"))) {
                    aux[asset].funding = load_dated_values((fs::path(fdir) / (asset + ".csv")).string());
                }
                if (!odir.empty() && fs::exists(fs::path(odir) / (asset + ".csv"))) {
                    aux[asset].open_interest = load_dated_values((fs::path(odir) / (asset + ".csv")).string());
                }
            }
            b = std::make_unique<CryptoTrendEngine>(*crypto_md, cb, aux, b_cap);
        }

        const std::string fut = str_or(cfg, "data.nifty_fut");
        if (!fut.empty()) {
            nifty_fut = load_series(fut);
            D1Config cd;
            apply_settings(cfg, cd);
            if (!str_or(cfg, "data.d1_skip_days").empty()) {
                for (const auto& d : load_symbol_list(str_or(cfg, "data.d1_skip_days"))) cd.skip_days.insert(d);
            }
            d1 = std::make_unique<D1Engine>(nifty_fut, cd, d1_cap);
        }

        // Restore state.
        if (a1 && fs::exists(state("a1"))) a1->load(read_records(state("a1")));
        if (a2 && fs::exists(state("a2"))) a2->load(read_records(state("a2")));
        if (b && fs::exists(state("b"))) b->load(read_records(state("b")));
        if (d1 && fs::exists(state("d1"))) d1->load(read_records(state("d1")));
        if (fs::exists(state("book"))) {
            for (const auto& r : read_records(state("book"))) {
                if (r.type == "risk") breaker.restore(r);
                if (r.type == "account") {
                    last_date = r.str("last_date");
                    last_active = r.num("last_active");
                }
                if (r.type == "reset") reset_dates[r.str("sleeve")] = r.str("date");
                if (r.type == "kill_state") kill_state[r.str("sleeve")] = r.str("action");
            }
        }
        if (last_active == 0) last_active = active_equity();
    }

    // Live sleeves count toward the active book; paper-only shadows (A2 at 0%, D1 at 0%) do not.
    double active_equity() const {
        double e = cash_parked;
        e += a1 ? a1->equity() : alloc.a1 * capital;
        e += b ? b->equity() : alloc.b * capital;
        if (alloc.a2 > 0) e += a2 ? a2->equity() : alloc.a2 * capital;
        if (alloc.d > 0) e += d1 ? d1->equity() : alloc.d * capital;
        return e;
    }

    void save() const {
        fs::create_directories(dir / "state");
        if (a1) write_records(state("a1"), a1->save());
        if (a2) write_records(state("a2"), a2->save());
        if (b) write_records(state("b"), b->save());
        if (d1) write_records(state("d1"), d1->save());
        KvRecord acct;
        acct.type = "account";
        acct.set("last_date", last_date).set("last_active", last_active);
        std::vector<KvRecord> recs = {acct, breaker.snapshot()};
        for (const auto& [sleeve, date] : reset_dates) {
            KvRecord r;
            r.type = "reset";
            r.set("sleeve", sleeve).set("date", date);
            recs.push_back(r);
        }
        for (const auto& [sleeve, action] : kill_state) {
            KvRecord r;
            r.type = "kill_state";
            r.set("sleeve", sleeve).set("action", action);
            recs.push_back(r);
        }
        write_records(state("book"), recs);
    }

    void record(Journal& j, std::vector<KvRecord>& evs) {
        for (const auto& e : evs) j.append(e);
        evs.clear();
    }

    void write_trades(std::vector<Trade> trades) {
        for (const auto& t : trades) append_line(dir / "trades.csv", trades_csv_header(), to_csv(t));
    }
};

void PaperAccount::init(const std::string& dir, double capital, const std::string& start, const Config& settings) {
    if (fs::exists(fs::path(dir) / "account.cfg")) throw std::runtime_error("account already exists in " + dir);
    if (capital <= 0) throw std::runtime_error("capital must be > 0");
    fs::create_directories(fs::path(dir) / "state");
    std::ofstream out(fs::path(dir) / "account.cfg");
    out << "# Paper-trading account. Risk fractions for a1./a2. are fractions of the ACTIVE BOOK\n"
           "# (rulebook units); the runner converts them to each sleeve's capital.\n"
        << "account.capital = " << fmt_double(capital) << "\n"
        << "account.start = " << start << "\n\n# data paths (empty = sleeve disabled)\n";
    for (const char* k : {"data.equity_dir", "data.exclusions", "data.index", "data.vix", "data.events",
                          "data.intraday_dir", "data.catalysts", "data.bands", "data.crypto_dir",
                          "data.crypto_funding_dir", "data.crypto_oi_dir", "data.nifty_fut", "data.d1_skip_days"}) {
        const auto it = settings.find(k);
        out << k << " = " << (it == settings.end() ? "" : it->second) << "\n";
    }
    out << "\n# limits (defaults shown; edit to override)\n";
    // Rulebook risk units relative to the active book for the stock sleeves.
    const std::map<std::string, std::string> rulebook = {
        {"a1.risk_per_trade", "0.004"}, {"a1.max_position_frac", "0.05"}, {"a1.daily_loss_limit", "0.01"},
        {"a2.risk_per_trade", "0.0025"}, {"a2.max_position_frac", "0.05"}, {"a2.daily_loss_limit", "0.01"},
    };
    for (const auto& [k, v] : default_settings()) {
        const auto it = settings.find(k);
        const auto rb = rulebook.find(k);
        out << k << " = " << (it != settings.end() ? it->second : rb != rulebook.end() ? rb->second : v) << "\n";
    }
    out << "\n# kill rules\nkill.b_backtest_max_dd = 0.19\nkill.ic_target = 0.03\n";
    for (const auto& [k, v] : settings) {
        if (k.rfind("paper.", 0) == 0) out << k << " = " << v << "\n";
    }
    Journal j((fs::path(dir) / "journal.log").string());
    KvRecord r;
    r.type = "account_init";
    r.set("capital", capital).set("start", start);
    j.append(r);
}

PaperAccount::PaperAccount(const std::string& dir) : p_(std::make_unique<Impl>()) {
    p_->dir = dir;
    if (!fs::exists(p_->dir / "account.cfg")) throw std::runtime_error("no account in " + dir + " (run init)");
    p_->load();
}

PaperAccount::~PaperAccount() = default;

std::string PaperAccount::run(const std::string& until) {
    Impl& p = *p_;
    Journal journal((p.dir / "journal.log").string());
    std::ostringstream rep;
    std::vector<KvRecord> evs;

    // Kill rules run before every trading day on the closed-trade history up to then, so
    // a long catch-up run behaves exactly like running every evening.
    std::vector<Trade> history = read_trades(p.dir / "trades.csv");
    double a1_mult = 1.0;
    auto since = [&](const std::string& s) {
        const auto it = p.reset_dates.find(s);
        return it == p.reset_dates.end() ? std::string() : it->second;
    };
    auto apply = [&](const std::string& sleeve, RiskManager* rm, const KillDecision& d, const std::string& date) {
        const std::string action = to_string(d.action);
        if (p.kill_state[sleeve] != action) {  // journal changes only
            p.kill_state[sleeve] = action;
            KvRecord r;
            r.type = "kill_rule";
            r.set("sleeve", sleeve).set("date", date).set("action", action).set("reason", d.reason);
            journal.append(r);
            if (d.action != KillAction::None) rep << date << " kill rule " << sleeve << ": " << action << " (" << d.reason << ")\n";
        }
        if (d.action == KillAction::Off && rm && rm->state() != RiskState::Off) rm->force_off();
    };
    auto evaluate_rules = [&](const std::string& date) {
        if (p.a1) {
            const auto d = a1_kill_rule(of_sleeve(history, "A1", since("a1")));
            a1_mult = d.action == KillAction::Halve ? 0.5 : 1.0;
            apply("A1", &p.a1->risk(), d, date);
        }
        if (p.a2) {
            // Uplift over the last 100 events whose shadow outcome is final (older than the
            // 30-day maximum hold plus a margin).
            const auto& ds = p.equity_md->dates();
            const auto now = std::lower_bound(ds.begin(), ds.end(), date) - ds.begin();
            std::vector<double> app, rej;
            int seen = 0;
            const auto& events = p.a2->events();
            for (auto it = events.rbegin(); it != events.rend() && seen < 100; ++it) {
                if (it->status == "warmup") continue;
                const auto at = std::lower_bound(ds.begin(), ds.end(), it->date) - ds.begin();
                if (now - at < 40) continue;
                const std::string key = it->date + "|" + it->symbol;
                auto c = p.shadow_cache.find(key);
                if (c == p.shadow_cache.end()) {
                    c = p.shadow_cache.emplace(key, shadow_r(*it, *p.equity_md, p.intraday, p.a2_cfg)).first;
                }
                if (std::isnan(c->second)) continue;
                (it->approved ? app : rej).push_back(c->second);
                ++seen;
            }
            const double uplift = (app.empty() || rej.empty()) ? 0 : mean(app) - mean(rej);
            apply("A2", &p.a2->risk(), a2_kill_rule(of_sleeve(history, "A2", since("a2")), uplift, seen, 0), date);
        }
        if (p.b) {
            apply("B", &p.b->risk(),
                  b_kill_rule(p.b->risk().drawdown(), num_or(p.cfg, "kill.b_backtest_max_dd", 0.19)), date);
        }
        if (p.d1) apply("D1", &p.d1->risk(), d1_kill_rule(of_sleeve(history, "D1", since("d1"))), date);
    };
    auto record_trades = [&](std::vector<Trade> closed) {
        p.write_trades(closed);
        history.insert(history.end(), closed.begin(), closed.end());
    };

    // Timeline: every date any sleeve has data for, after what was already processed.
    std::set<std::string> timeline;
    auto add_dates = [&](const std::vector<std::string>& ds, const std::string& done) {
        for (const auto& d : ds) {
            if ((!p.start.empty() && d < p.start) || (!done.empty() && d <= done)) continue;
            if (!until.empty() && d > until) continue;
            timeline.insert(d);
        }
    };
    if (p.a1 || p.a2) add_dates(p.equity_md->dates(), p.a1 ? p.a1->last_date() : p.a2->last_date());
    if (p.b) {
        if (p.b->last_date().empty() && !p.start.empty()) p.b->prime(p.start);
        add_dates(p.crypto_md->dates(), p.b->last_date());
    }
    if (p.d1) add_dates(p.d1->dates(), p.d1->last_date());

    KvRecord begin;
    begin.type = "run_start";
    begin.set("from", timeline.empty() ? std::string("") : *timeline.begin());
    begin.set("to", timeline.empty() ? std::string("") : *timeline.rbegin());
    journal.append(begin);

    bool flattened = false;
    for (const auto& date : timeline) {
        evaluate_rules(date);
        p.breaker.start_day(p.last_active);
        const RiskState bs = p.breaker.state();
        const double book_mult = bs == RiskState::Off ? 0.0 : bs == RiskState::Halved ? 0.5 : 1.0;
        if (bs == RiskState::Off && !flattened) {
            if (p.a1) p.a1->flatten(p.a1->last_date().empty() ? date : p.a1->last_date(), "breaker_off", &evs);
            if (p.a2) p.a2->flatten(p.a2->last_date().empty() ? date : p.a2->last_date(), "breaker_off", &evs);
            if (p.b) p.b->flatten(p.b->last_date().empty() ? date : p.b->last_date(), "breaker_off", &evs);
            flattened = true;
        }

        const bool eq_day = p.equity_md && p.equity_md->has_date(date);
        if (eq_day && (p.a1 || p.a2)) {
            const double heat_cap = 0.03 * p.active_book;
            if (p.a2 && (p.a2->last_date().empty() || date > p.a2->last_date())) {
                SharedBook sb{p.a1 ? p.a1->open_risk() : 0, p.a1 ? p.a1->positions() : 0, heat_cap};
                StepContext ctx{&sb, book_mult, bs == RiskState::Off, &evs};
                p.a2->step(date, ctx);
            }
            if (p.a1 && (p.a1->last_date().empty() || date > p.a1->last_date())) {
                SharedBook sb{p.a2 ? p.a2->open_risk() : 0, p.a2 ? p.a2->positions() : 0, heat_cap};
                StepContext ctx{&sb, book_mult * a1_mult, bs == RiskState::Off, &evs};
                p.a1->step(date, ctx);
                for (const auto& [sym, score] : p.a1->last_scores()) {
                    append_line(p.dir / "scores.csv", "date,symbol,score", date + "," + sym + "," + fmt_double(score));
                }
            }
        }
        if (p.b && p.crypto_md->has_date(date) && (p.b->last_date().empty() || date > p.b->last_date())) {
            p.b->step(date, book_mult, &evs);
        }
        if (p.d1 && (p.d1->last_date().empty() || date > p.d1->last_date())) {
            const auto& ds = p.d1->dates();
            if (std::binary_search(ds.begin(), ds.end(), date)) p.d1->step(date, book_mult, &evs);
        }

        p.last_active = p.active_equity();
        p.last_date = date;
        p.breaker.end_day(p.last_active);

        if (p.a1) record_trades(p.a1->drain_closed());
        if (p.a2) record_trades(p.a2->drain_closed());
        if (p.b) record_trades(p.b->drain_closed());
        if (p.d1) record_trades(p.d1->drain_closed());
        p.record(journal, evs);

        std::ostringstream row;
        row << date << ',' << fmt_double(p.a1 ? p.a1->equity() : 0) << ',' << fmt_double(p.a2 ? p.a2->equity() : 0)
            << ',' << fmt_double(p.b ? p.b->equity() : 0) << ',' << fmt_double(p.d1 ? p.d1->equity() : 0) << ','
            << fmt_double(p.last_active) << ',' << to_string(p.breaker.state());
        append_line(p.dir / "equity.csv", "date,a1,a2_paper,b,d1_paper,active_book,breaker", row.str());
    }
    p.save();

    KvRecord end;
    end.type = "run_end";
    end.set("days", static_cast<long>(timeline.size())).set("active_book", p.last_active);
    end.set("breaker", std::string(to_string(p.breaker.state())));
    journal.append(end);

    rep << "processed " << timeline.size() << " day(s)";
    if (!timeline.empty()) rep << " from " << *timeline.begin() << " to " << *timeline.rbegin();
    rep << "\n" << status();
    return rep.str();
}

std::string PaperAccount::status() const {
    const Impl& p = *p_;
    std::ostringstream o;
    o << "account " << p.dir.string() << "  capital " << money(p.capital) << "  active book " << money(p.last_active)
      << " of " << money(p.active_book) << "  breaker " << to_string(p.breaker.state()) << "  last date "
      << (p.last_date.empty() ? "-" : p.last_date) << "\n";
    auto sleeve_line = [&](const std::string& name, double eq, const RiskManager& rm, int positions, const std::string& note) {
        o << "  " << std::left << std::setw(4) << name << " equity " << std::setw(12) << money(eq) << " risk "
          << std::setw(7) << to_string(rm.state()) << " dd " << std::fixed << std::setprecision(1)
          << rm.drawdown() * 100 << "%  positions " << positions << note << "\n";
    };
    if (p.a1) {
        sleeve_line("A1", p.a1->equity(), p.a1->risk(), p.a1->positions(), "");
        for (const auto& [sym, pos] : p.a1->book().positions()) {
            o << "       " << sym << " qty " << pos.qty << " entry " << pos.trade.entry_price << " stop " << pos.stop
              << " day " << pos.days_held << (pos.exit_next_open ? " (exit at next open)" : "") << "\n";
        }
        if (!p.a1->pending().empty()) o << "       next session buy-stops:\n";
        for (const auto& c : p.a1->pending()) {
            o << "         " << c.symbol << " trigger " << (c.pivot + 0.05) << " score " << std::setprecision(2) << c.score
              << " adr " << c.adr * 100 << "%\n";
        }
    }
    if (p.a2) sleeve_line("A2", p.a2->equity(), p.a2->risk(), p.a2->positions(), p.alloc.a2 > 0 ? "" : "  [paper-only shadow]");
    if (p.b) {
        sleeve_line("B", p.b->equity(), p.b->risk(), p.b->holdings(), "");
        for (const auto& [asset, s] : p.crypto_md->universe()) {
            const double u = p.b->exposure(asset);
            if (u > 0) o << "       " << asset << " units " << u << "\n";
        }
        o << "       tax paid " << money(p.b->tax_paid()) << "  funding " << money(p.b->funding_paid()) << "\n";
    }
    if (p.d1) sleeve_line("D1", p.d1->equity(), p.d1->risk(), 0, p.alloc.d > 0 ? "" : "  [paper-only shadow]");
    return o.str();
}

std::string PaperAccount::kill(const std::string& reason) {
    Impl& p = *p_;
    Journal journal((p.dir / "journal.log").string());
    std::vector<KvRecord> evs;
    if (p.a1 && !p.a1->last_date().empty()) p.a1->flatten(p.a1->last_date(), "kill_switch", &evs);
    if (p.a2 && !p.a2->last_date().empty()) p.a2->flatten(p.a2->last_date(), "kill_switch", &evs);
    if (p.b && !p.b->last_date().empty()) p.b->flatten(p.b->last_date(), "kill_switch", &evs);
    if (p.a1) p.a1->risk().force_off();
    if (p.a2) p.a2->risk().force_off();
    if (p.b) p.b->risk().force_off();
    if (p.d1) p.d1->risk().force_off();
    p.breaker.force_off();
    if (p.a1) p.write_trades(p.a1->drain_closed());
    if (p.a2) p.write_trades(p.a2->drain_closed());
    if (p.b) p.write_trades(p.b->drain_closed());
    p.record(journal, evs);
    KvRecord r;
    r.type = "kill_switch";
    r.set("reason", reason);
    journal.append(r);
    p.last_active = p.active_equity();
    p.save();
    return "kill switch fired: all sleeves flat and latched off (" + reason + ")\n" + status();
}

std::string PaperAccount::reset(const std::string& sleeve) {
    Impl& p = *p_;
    RiskManager* rm = sleeve == "a1" && p.a1 ? &p.a1->risk()
                      : sleeve == "a2" && p.a2 ? &p.a2->risk()
                      : sleeve == "b" && p.b ? &p.b->risk()
                      : sleeve == "d1" && p.d1 ? &p.d1->risk()
                      : sleeve == "book" ? &p.breaker
                      : nullptr;
    if (!rm) throw std::runtime_error("unknown or disabled sleeve: " + sleeve);
    rm->manual_reset();
    p.reset_dates[sleeve] = p.last_date;
    Journal journal((p.dir / "journal.log").string());
    KvRecord r;
    r.type = "manual_reset";
    r.set("sleeve", sleeve);
    journal.append(r);
    p.save();
    return "reset " + sleeve + " to normal\n";
}

std::string PaperAccount::scorecard(std::size_t horizon, std::size_t top_k) const {
    const Impl& p = *p_;
    if (!p.equity_md) return "no equity data configured\n";
    ScoreLog log;
    std::ifstream in(p.dir / "scores.csv");
    std::string line;
    bool header = true;
    while (std::getline(in, line)) {
        if (header) {
            header = false;
            continue;
        }
        const auto a = line.find(','), b2 = line.rfind(',');
        if (a == std::string::npos || b2 == a) continue;
        log[line.substr(0, a)].emplace_back(line.substr(a + 1, b2 - a - 1), std::stod(line.substr(b2 + 1)));
    }
    const auto days = score_outcomes(log, *p.equity_md, horizon, top_k);
    std::ostringstream o;
    o << std::fixed << std::setprecision(4);
    o << "screener scorecard: " << days.size() << " scored day(s), horizon " << horizon << ", top " << top_k << "\n";
    if (days.empty()) return o.str() + "not enough forward data yet\n";
    Cusum cusum(num_or(p.cfg, "kill.ic_target", 0.03), 0.01, 0.2);
    double prec = 0, base = 0;
    for (const auto& d : days) {
        cusum.update(d.rank_ic);
        prec += d.precision_at_k;
        base += d.base_rate;
    }
    const double ic60 = rolling_mean_ic(days, 60);
    o << "rank IC  20d " << rolling_mean_ic(days, 20) << "  60d " << ic60 << "  120d " << rolling_mean_ic(days, 120)
      << "\nprecision@" << top_k << " " << prec / days.size() << " vs base rate " << base / days.size()
      << "\nCUSUM " << cusum.value() << (cusum.alarm() ? " ALARM" : " ok") << "\n";
    const auto trades = of_sleeve(read_trades(p.dir / "trades.csv"), "A1");
    std::vector<Trade> recent;
    if (!trades.empty() && !p.last_date.empty()) {
        const long last = days_from_iso(p.last_date);
        for (const auto& t : trades) {
            if (last - days_from_iso(t.exit_date) <= 120 * 7 / 5) recent.push_back(t);
        }
    }
    const double e120 = recent.empty() ? std::numeric_limits<double>::quiet_NaN() : mean(r_multiples(recent));
    const KillDecision d = screener_rule(ic60, cusum.alarm(), e120);
    o << "120-day A1 expectancy " << e120 << " R\nscreener rule: " << to_string(d.action)
      << (d.reason.empty() ? "" : " (" + d.reason + ")") << "\n";
    return o.str();
}

}  // namespace ta
