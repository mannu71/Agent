#include "ta/a2.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <limits>
#include <sstream>
#include <stdexcept>

#include "ta/indicators.hpp"
#include "ta/regime.hpp"

namespace ta {

namespace {

constexpr double kNaN = std::numeric_limits<double>::quiet_NaN();
constexpr std::size_t kVars = 6;
constexpr long kShadowUnits = 300;  // divisible by 3 so the partial exit is exact
constexpr long kAll = std::numeric_limits<long>::max();

std::string trim(const std::string& s) {
    const auto b = s.find_first_not_of(" \t\r\n");
    if (b == std::string::npos) return "";
    return s.substr(b, s.find_last_not_of(" \t\r\n") - b + 1);
}

double median(std::vector<double> v) {
    if (v.empty()) return kNaN;
    std::sort(v.begin(), v.end());
    const std::size_t m = v.size() / 2;
    return v.size() % 2 ? v[m] : (v[m - 1] + v[m]) / 2.0;
}

// Day-0 management after an intraday fill: pessimistic stop on the fill bar, stop checks
// on later bars, and the 15:20 weakness exit at the close.
void run_day0(SwingBook& book, const std::string& sym, const std::string& date, const EntryResult& e,
              const Bar* bars, std::size_t n, const A2Config& cfg, std::vector<KvRecord>* events) {
    if (bars[e.bar].low <= e.stop) {
        book.sell(sym, kAll, e.stop, date, "stop_entry_bar", events);
        return;
    }
    for (std::size_t k = e.bar + 1; k < n && book.holds(sym); ++k) {
        const Bar& b = bars[k];
        if (!cfg.weakness_time.empty() && time_of(b.date) >= cfg.weakness_time) {
            if (b.open < e.fill) book.sell(sym, kAll, bars[n - 1].close, date, "weak_day0", events);
            else if (b.low <= e.stop) book.sell(sym, kAll, std::min(b.open, e.stop), date, "stop", events);
            // After 15:20 the position is held overnight unless a later bar hits the stop.
            for (std::size_t j = k + 1; j < n && book.holds(sym); ++j) {
                if (bars[j].low <= e.stop) {
                    book.sell(sym, kAll, std::min(bars[j].open, e.stop), date, "stop", events);
                }
            }
            return;
        }
        if (b.open <= e.stop) book.sell(sym, kAll, b.open, date, "stop_gap", events);
        else if (b.low <= e.stop) book.sell(sym, kAll, e.stop, date, "stop", events);
    }
}

KvRecord event_kv(const GapEvent& ev) {
    KvRecord r;
    r.type = "a2_event";
    r.set("date", ev.date).set("symbol", ev.symbol).set("gap", ev.gap).set("open", ev.open);
    r.set("prior_close", ev.prior_close).set("adr", ev.adr).set("fno", ev.fno).set("upper_band", ev.upper_band);
    for (std::size_t k = 0; k < ev.vars.size(); ++k) r.set("v" + std::to_string(k), ev.vars[k]);
    r.set("neglect", ev.neglect).set("range_breakout", ev.range_breakout).set("early_cycle", ev.early_cycle);
    r.set("score", ev.score).set("approved", ev.approved).set("status", ev.status);
    return r;
}

GapEvent event_from_kv(const KvRecord& r) {
    GapEvent ev;
    ev.date = r.str("date");
    ev.symbol = r.str("symbol");
    ev.gap = r.num("gap");
    ev.open = r.num("open");
    ev.prior_close = r.num("prior_close");
    ev.adr = r.num("adr");
    ev.fno = r.integer("fno") != 0;
    ev.upper_band = r.num("upper_band");
    for (std::size_t k = 0; k < kVars; ++k) {
        const std::string key = "v" + std::to_string(k);
        ev.vars.push_back(r.str(key) == "nan" ? kNaN : r.num(key, kNaN));
    }
    ev.vol_ratio = r.num("vol_ratio");
    ev.rv = r.num("rv");
    ev.neglect = r.integer("neglect") != 0;
    ev.range_breakout = r.integer("range_breakout") != 0;
    ev.early_cycle = r.integer("early_cycle") != 0;
    ev.score = r.num("score");
    ev.approved = r.integer("approved") != 0;
    ev.status = r.str("status");
    return ev;
}

}  // namespace

void IntradayData::finalize() {
    index.clear();
    for (auto& [sym, s] : bars) {
        std::sort(s.begin(), s.end(), [](const Bar& a, const Bar& b) { return a.date < b.date; });
        index[sym] = index_by_day(s);
    }
}

const Bar* IntradayData::day(const std::string& sym, const std::string& date, std::size_t* n) const {
    const auto it = index.find(sym);
    if (it == index.end()) return nullptr;
    const auto d = it->second.find(date);
    if (d == it->second.end()) return nullptr;
    *n = d->second.second - d->second.first;
    return &bars.at(sym)[d->second.first];
}

std::map<std::string, std::vector<std::string>> load_catalysts(const std::string& path) {
    std::ifstream in(path);
    if (!in) throw std::runtime_error("cannot open " + path);
    std::map<std::string, std::vector<std::string>> out;
    std::string line;
    while (std::getline(in, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line.rfind("symbol", 0) == 0) continue;
        const auto comma = line.find(',');
        if (comma == std::string::npos) throw std::runtime_error(path + ": expected symbol,timestamp");
        out[trim(line.substr(0, comma))].push_back(trim(line.substr(comma + 1)));
    }
    for (auto& [sym, v] : out) std::sort(v.begin(), v.end());
    return out;
}

void load_bands(const std::string& path, A2Inputs& in) {
    std::ifstream f(path);
    if (!f) throw std::runtime_error("cannot open " + path);
    std::string line;
    while (std::getline(f, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line.rfind("symbol", 0) == 0) continue;
        const auto comma = line.find(',');
        if (comma == std::string::npos) throw std::runtime_error(path + ": expected symbol,band");
        const std::string sym = trim(line.substr(0, comma));
        const std::string band = trim(line.substr(comma + 1));
        if (band == "FO" || band == "fo") {
            in.fno.insert(sym);
        } else {
            const double b = std::stod(band);
            in.cash_band[sym] = b > 1 ? b / 100.0 : b;
        }
    }
}

EntryResult simulate_entry(const GapEvent& ev, const Bar* bars, std::size_t n, const A2Config& cfg) {
    EntryResult r;
    if (!bars || n == 0 || time_of(bars[0].date) != cfg.or_start) {
        r.reason = "no_opening_bar";
        return r;
    }
    const Bar& b0 = bars[0];
    if (!(b0.close > b0.open)) {
        r.reason = "first_bar_red";
        return r;
    }
    const double trigger = b0.high + cfg.tick;
    const double limit = trigger * (1.0 + cfg.entry_limit_frac);
    r.stop = b0.low - cfg.tick;
    if (ev.fno && trigger >= ev.upper_band * (1.0 - cfg.fno_trigger_margin)) {
        r.reason = "near_upper_band";
        return r;
    }
    if (!ev.fno && ev.open >= cfg.cash_band_skip * ev.upper_band) {
        r.reason = "band_lock";
        return r;
    }
    bool triggered = false;
    for (std::size_t k = 1; k < n; ++k) {
        const Bar& b = bars[k];
        if (time_of(b.date) >= cfg.window_end) break;
        double px = kNaN;
        if (!triggered) {
            if (b.high < trigger) continue;
            triggered = true;
            const double at = std::max(b.open, trigger);
            if (at <= limit) px = at;
            else if (b.low <= limit) px = limit;
        } else if (b.low <= limit) {
            px = std::min(b.open, limit);
        }
        if (std::isnan(px)) continue;
        if (px >= ev.upper_band - cfg.tick / 2) {
            r.reason = "upper_circuit";
            return r;
        }
        r.filled = true;
        r.fill = px;
        r.bar = k;
        return r;
    }
    r.reason = triggered ? "limit_not_reached" : "not_triggered";
    return r;
}

void A2Config::apply_partial_mode() {
    if (partial_mode == "targets") {
        exits.partial_frac = 0;  // the four targets replace the day-3 partial
        exits.r_targets = {{2, 0.25}, {4, 0.25}, {8, 0.25}, {10, 0.25}};
    } else if (partial_mode == "day3") {
        exits.r_targets.clear();
    } else {
        throw std::runtime_error("a2.partial_mode must be day3 or targets");
    }
}

A2Engine::A2Engine(const MarketData& md, A2Config cfg, A2Inputs in, double initial_equity)
    : md_(md),
      cfg_((cfg.apply_partial_mode(), std::move(cfg))),
      in_(std::move(in)),
      risk_(cfg_.risk),
      book_("A2", cfg_.cost, cfg_.exits, initial_equity),
      last_equity_(initial_equity) {
    for (std::size_t k = 0; k < md_.dates().size(); ++k) date_pos_[md_.dates()[k]] = k;
}

std::vector<GapEvent> A2Engine::detect(const std::string& date) const {
    std::vector<GapEvent> out;
    for (const auto& [sym, s] : md_.universe()) {
        std::size_t i = 0;
        const Bar* b = md_.bar(sym, date, &i);
        if (!b || i < 301) continue;
        const Bar& prev = s[i - 1];
        const double gap = b->open / prev.close - 1.0;
        if (gap < cfg_.min_gap) continue;
        if (b->open < cfg_.min_price || in_.excluded.excluded(sym, date)) continue;
        const double atv = avg_traded_value(s, i - 1, 20);
        if (std::isnan(atv) || atv < cfg_.min_avg_traded_value) continue;

        const bool fno = in_.fno.count(sym) != 0;
        double band = cfg_.fno_band;
        if (!fno) {
            const auto it = in_.cash_band.find(sym);
            if (it == in_.cash_band.end() || (std::fabs(it->second - 0.10) > 1e-9 && std::fabs(it->second - 0.20) > 1e-9)) {
                continue;  // only F&O and 10%/20%-band cash stocks
            }
            band = it->second;
        }

        // Catalyst filed after the prior session's close and before today's open.
        if (cfg_.require_catalyst) {
            const auto cat = in_.catalysts.find(sym);
            if (cat == in_.catalysts.end()) continue;
            const std::string from = prev.date + " 15:30", to = date + " " + cfg_.or_start;
            const bool has_catalyst = std::any_of(cat->second.begin(), cat->second.end(),
                                                  [&](const std::string& t) { return t > from && t <= to; });
            if (!has_catalyst) continue;
        }
        // The trader avoided a gap that came right after a gap day.
        if (cfg_.skip_after_gap_day && prev.open / s[i - 2].close - 1.0 >= cfg_.min_gap) continue;

        GapEvent ev;
        ev.date = date;
        ev.symbol = sym;
        ev.gap = gap;
        ev.open = b->open;
        ev.prior_close = prev.close;
        ev.adr = adr_frac(s, i - 1, 20);
        ev.fno = fno;
        ev.upper_band = prev.close * (1.0 + band);

        const double r120 = prev.close / s[i - 121].close - 1.0;
        std::vector<double> atr_hist;
        for (std::size_t k = i - 250; k < i; ++k) atr_hist.push_back(atr_frac(s, k, 50));
        const double atr_ratio = atr_frac(s, i - 1, 50) / median(atr_hist);
        const double vol_ratio = sma_volume(s, i - 1, 20) / sma_volume(s, i - 1, 50);
        const double hh60 = highest_high(s, i - 1, 60);
        const double range60 = (hh60 - lowest_low(s, i - 1, 60)) / prev.close;
        int prior_gaps = 0;
        for (std::size_t k = i - 250; k < i; ++k) {
            if (s[k].open / s[k - 1].close - 1.0 >= cfg_.min_gap) ++prior_gaps;
        }
        const double ext200 = b->open / sma_close(s, i - 1, 200) - 1.0;

        double rv = kNaN;
        if (in_.intraday) {
            std::size_t n = 0;
            const Bar* today = in_.intraday->day(sym, date, &n);
            double base = 0;
            std::size_t cnt = 0;
            for (std::size_t k = i - 1; k + cfg_.rv_days >= i && k > 0 && cnt < cfg_.rv_days; --k) {
                std::size_t m = 0;
                const Bar* d = in_.intraday->day(sym, s[k].date, &m);
                if (d && m > 0) {
                    base += d[0].volume;
                    ++cnt;
                }
            }
            if (today && n > 0 && cnt > 0 && base > 0) rv = today[0].volume / (base / static_cast<double>(cnt));
        }

        // First-5-minute relative volume is a hard gate (Zarattini, Barbon & Aziz: -0.02R below
        // 1x, +0.08R above), standing in for the paper's pre-market volume filter.
        if (cfg_.min_rv > 0 && (std::isnan(rv) || rv < cfg_.min_rv)) continue;

        // Only price information enters the approval score: the trader saw neither volume
        // nor price levels. Volume measures are logged separately.
        ev.vars = {-std::max(r120, 0.0), -atr_ratio, b->open / hh60, -range60,
                   -static_cast<double>(prior_gaps), -ext200};
        ev.vol_ratio = vol_ratio;
        ev.rv = std::isnan(rv) ? 0 : rv;
        ev.neglect = r120 <= 0.15 && atr_ratio < 1.0;
        ev.range_breakout = b->open > hh60 && range60 <= 0.35;
        ev.early_cycle = prior_gaps <= 1 && ext200 <= 0.50;
        out.push_back(std::move(ev));
    }
    return out;
}

void A2Engine::score(GapEvent& ev) {
    const std::size_t pos = date_pos_.at(ev.date);
    std::vector<const GapEvent*> window;
    for (const auto& h : history_) {
        const auto hp = date_pos_.find(h.date);
        if (hp == date_pos_.end() || h.date >= ev.date) continue;
        if (hp->second + cfg_.approval_window >= pos) window.push_back(&h);
    }
    double zsum = 0;
    int zn = 0;
    for (std::size_t k = 0; k < kVars; ++k) {
        if (std::isnan(ev.vars[k])) continue;
        double sum = 0, sum_sq = 0;
        int n = 0;
        for (const auto* h : window) {
            if (std::isnan(h->vars[k])) continue;
            sum += h->vars[k];
            sum_sq += h->vars[k] * h->vars[k];
            ++n;
        }
        if (n < 2) continue;
        const double mean = sum / n;
        const double sd = std::sqrt(std::max(0.0, (sum_sq - sum * sum / n) / (n - 1)));
        if (sd <= 0) continue;
        zsum += std::clamp((ev.vars[k] - mean) / sd, -3.0, 3.0);
        ++zn;
    }
    ev.score = zn > 0 ? zsum / zn : 0;
    if (window.size() < cfg_.min_history_events) {
        ev.status = "warmup";
        return;
    }
    std::vector<double> scores;
    for (const auto* h : window) scores.push_back(h->score);
    if (scores.empty()) {  // only reachable when min_history_events == 0
        ev.approved = true;
        ev.status = "approved";
        return;
    }
    std::sort(scores.begin(), scores.end());
    const auto idx = static_cast<std::size_t>(std::floor(cfg_.approve_pct * static_cast<double>(scores.size() - 1)));
    ev.approved = ev.score > scores[idx];
    ev.status = ev.approved ? "approved" : "below_threshold";
}

void A2Engine::step(const std::string& date, const StepContext& ctx) {
    risk_.start_day(last_equity_);
    book_.exits_at_open(md_, date, ctx.events);

    std::vector<GapEvent> today = detect(date);
    for (auto& ev : today) score(ev);

    std::vector<GapEvent*> approved;
    for (auto& ev : today) {
        if (ev.approved) approved.push_back(&ev);
    }
    std::sort(approved.begin(), approved.end(), [](const GapEvent* a, const GapEvent* b) { return a->score > b->score; });

    const double eq_open = book_.cash() + book_.market_value(md_, date, true);
    const bool can_enter = !ctx.block_new_entries && risk_.allows_new_entries(eq_open);
    int entries = 0;
    for (GapEvent* ev : approved) {
        if (!can_enter) {
            ev->status = "approved_blocked";
            continue;
        }
        if (entries >= cfg_.max_entries_per_day || book_.holds(ev->symbol)) {
            ev->status = "approved_capacity";
            continue;
        }
        const int external = ctx.shared ? ctx.shared->external_positions : 0;
        if (book_.count() + external >= cfg_.risk.max_positions) {
            ev->status = "approved_capacity";
            continue;
        }
        std::size_t n = 0;
        const Bar* bars = in_.intraday ? in_.intraday->day(ev->symbol, date, &n) : nullptr;
        const EntryResult e = simulate_entry(*ev, bars, n, cfg_);
        if (!e.filled) {
            ev->status = "approved_no_fill:" + e.reason;
            continue;
        }
        const double gross = book_.market_value(md_, date, true);
        const SizeDecision d = risk_.size_long(eq_open, book_.cash(), gross, e.fill, e.stop, ev->adr,
                                               cfg_.cost.round_trip(), ctx.risk_multiplier);
        if (d.qty <= 0) {
            ev->status = "approved_rejected:" + d.reject_reason;
            continue;
        }
        if (ctx.shared && ctx.shared->heat_cap > 0 &&
            book_.open_risk() + ctx.shared->external_open_risk + static_cast<double>(d.qty) * (e.fill - e.stop) >
                ctx.shared->heat_cap) {
            ev->status = "approved_rejected:heat_cap";
            continue;
        }
        const Bar* daily = md_.bar(ev->symbol, date);
        book_.open(ev->symbol, date, e.fill, e.stop, d.qty, ev->adr, daily->close, ctx.events);
        ev->status = "entered";
        ++entries;
        run_day0(book_, ev->symbol, date, e, bars, n, cfg_, ctx.events);
    }

    book_.manage_at_close(md_, date, ctx.events);
    last_equity_ = book_.cash() + book_.market_value(md_, date, false);
    last_date_ = date;
    risk_.end_day(last_equity_);

    for (auto& ev : today) {
        if (ctx.events) ctx.events->push_back(event_kv(ev));
        history_.push_back(std::move(ev));
    }
}

void A2Engine::flatten(const std::string& date, const std::string& reason, std::vector<KvRecord>* events) {
    book_.flatten(md_, date, reason, events);
    last_equity_ = book_.cash();
}

std::vector<KvRecord> A2Engine::save() const {
    std::vector<KvRecord> out;
    KvRecord h;
    h.type = "a2";
    h.set("last_date", last_date_).set("last_equity", last_equity_);
    out.push_back(h);
    out.push_back(risk_.snapshot());
    book_.save(out);
    for (const auto& ev : history_) out.push_back(event_kv(ev));
    return out;
}

void A2Engine::load(const std::vector<KvRecord>& records) {
    history_.clear();
    for (const auto& r : records) {
        if (r.type == "a2") {
            last_date_ = r.str("last_date");
            last_equity_ = r.num("last_equity");
        } else if (r.type == "risk") {
            risk_.restore(r);
        } else if (r.type == "a2_event") {
            history_.push_back(event_from_kv(r));
        }
    }
    book_.load(records);
}

double shadow_r(const GapEvent& ev, const MarketData& md, const IntradayData& intraday, const A2Config& config) {
    A2Config cfg = config;
    cfg.apply_partial_mode();
    std::size_t n = 0;
    const Bar* bars = intraday.day(ev.symbol, ev.date, &n);
    const EntryResult e = simulate_entry(ev, bars, n, cfg);
    if (!e.filled || !(e.stop < e.fill)) return kNaN;

    const Series& s = md.series(ev.symbol);
    const MarketData one({{ev.symbol, s}});
    SwingBook book("A2-shadow", cfg.cost, cfg.exits, 1e12);
    const Bar* daily = one.bar(ev.symbol, ev.date);
    if (!daily) return kNaN;
    book.open(ev.symbol, ev.date, e.fill, e.stop, kShadowUnits, ev.adr, daily->close, nullptr);
    run_day0(book, ev.symbol, ev.date, e, bars, n, cfg, nullptr);
    book.manage_at_close(one, ev.date, nullptr);
    std::string last = ev.date;
    for (const Bar& b : s) {
        if (b.date <= ev.date) continue;
        if (!book.holds(ev.symbol)) break;
        book.exits_at_open(one, b.date, nullptr);
        book.manage_at_close(one, b.date, nullptr);
        last = b.date;
    }
    if (book.holds(ev.symbol)) book.flatten(one, last, "end_of_data", nullptr);
    const auto closed = book.drain_closed();
    return closed.empty() ? kNaN : closed.back().r_multiple;
}

A2BacktestResult run_a2_backtest(const MarketData& md, const A2Config& cfg, const A2Inputs& in,
                                 double initial_equity, const std::string& start, const std::string& end) {
    A2Engine eng(md, cfg, in, initial_equity);
    A2BacktestResult r;
    for (const auto& d : md.dates()) {
        if (!start.empty() && d < start) continue;
        if (!end.empty() && d > end) break;
        eng.step(d);
        r.portfolio.equity_curve.emplace_back(d, eng.equity());
    }
    if (!r.portfolio.equity_curve.empty()) {
        eng.flatten(r.portfolio.equity_curve.back().first, "end_of_data", nullptr);
        r.portfolio.equity_curve.back().second = eng.equity();
    }
    r.portfolio.trades = eng.drain_closed();
    r.portfolio.metrics = compute_metrics(r.portfolio.trades, r.portfolio.equity_curve);
    r.portfolio.final_risk_state = eng.risk().state();
    r.events = eng.events();

    std::vector<double> app, rej;
    for (const auto& ev : r.events) {
        const double sr = (in.intraday && ev.status != "warmup") ? shadow_r(ev, md, *in.intraday, cfg) : kNaN;
        r.shadow.push_back(sr);
        if (std::isnan(sr)) continue;
        (ev.approved ? app : rej).push_back(sr);
    }
    auto mean_var = [](const std::vector<double>& v, double& mean, double& var) {
        mean = var = 0;
        if (v.empty()) return;
        for (double x : v) mean += x;
        mean /= static_cast<double>(v.size());
        if (v.size() < 2) return;
        for (double x : v) var += (x - mean) * (x - mean);
        var /= static_cast<double>(v.size() - 1);
    };
    double va = 0, vr = 0;
    mean_var(app, r.approved_mean_r, va);
    mean_var(rej, r.rejected_mean_r, vr);
    r.approved_n = static_cast<int>(app.size());
    r.rejected_n = static_cast<int>(rej.size());
    r.uplift = r.approved_mean_r - r.rejected_mean_r;
    const double se = std::sqrt((app.empty() ? 0 : va / app.size()) + (rej.empty() ? 0 : vr / rej.size()));
    r.uplift_t = se > 0 ? r.uplift / se : 0;
    return r;
}

}  // namespace ta
