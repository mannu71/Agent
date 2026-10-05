#include "ta/a1.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

#include "ta/indicators.hpp"

namespace ta {

A1Engine::A1Engine(const MarketData& md, A1Config cfg, ExclusionList excluded, EquityRegimeInputs regime,
                   double initial_equity)
    : md_(md),
      cfg_(cfg),
      excluded_(std::move(excluded)),
      regime_in_(regime),
      risk_(cfg.risk),
      book_("A1", cfg.cost, cfg.exits, initial_equity),
      last_equity_(initial_equity) {
    if (regime_in_.index && cfg_.vol_scale) {
        for (std::size_t k = 0; k < regime_in_.index->size(); ++k) {
            index_vol_.push_back(ann_vol(*regime_in_.index, k, cfg_.vol_n));
        }
    }
}

double A1Engine::vol_multiplier(const std::string& date) const {
    const double nan = std::numeric_limits<double>::quiet_NaN();
    if (index_vol_.empty()) return nan;
    const Series& idx = *regime_in_.index;
    const auto it = std::upper_bound(idx.begin(), idx.end(), date,
                                     [](const std::string& d, const Bar& b) { return d < b.date; });
    if (it == idx.begin()) return nan;
    const auto j = static_cast<std::size_t>(std::distance(idx.begin(), it) - 1);
    const double now = index_vol_[j];
    if (std::isnan(now) || now <= 0) return nan;
    double target = cfg_.vol_target;
    if (target <= 0) {
        std::vector<double> hist;
        for (std::size_t k = 0; k < j; ++k) {
            if (!std::isnan(index_vol_[k])) hist.push_back(index_vol_[k]);
        }
        if (hist.size() < 252) return nan;  // need a year of history for the median
        std::nth_element(hist.begin(), hist.begin() + static_cast<long>(hist.size() / 2), hist.end());
        target = hist[hist.size() / 2];
    }
    return std::min(cfg_.vol_scale_cap, target / now);
}

void A1Engine::step(const std::string& date, const StepContext& ctx) {
    // Day-start equity for the daily loss limit is yesterday's close mark.
    risk_.start_day(last_equity_);

    book_.exits_at_open(md_, date, ctx.events);
    enter(date, ctx);
    book_.manage_at_close(md_, date, ctx.events);

    last_equity_ = book_.cash() + book_.market_value(md_, date, false);
    last_date_ = date;
    risk_.end_day(last_equity_);
    screen_for_tomorrow(date, ctx.events);
}

void A1Engine::enter(const std::string& date, const StepContext& ctx) {
    std::vector<Candidate> pending;
    pending.swap(pending_);
    const double eq_open = book_.cash() + book_.market_value(md_, date, true);
    if (ctx.block_new_entries || !pending_allowed_ || !risk_.allows_new_entries(eq_open)) return;

    EquityRegime today;
    if (regime_in_.events) {
        today = evaluate_equity_regime({nullptr, nullptr, regime_in_.events, &md_.dates()}, date, cfg_.regime);
    }
    if (today.event_blackout) return;

    const double mult = ctx.risk_multiplier * pending_risk_mult_;
    for (const Candidate& c : pending) {
        if (book_.holds(c.symbol) || excluded_.excluded(c.symbol, date)) continue;
        const int external = ctx.shared ? ctx.shared->external_positions : 0;
        if (book_.count() + external >= cfg_.risk.max_positions) break;
        const Bar* b = md_.bar(c.symbol, date);
        if (!b) continue;

        const double trigger = c.pivot + cfg_.tick;
        if (b->high < trigger) continue;                                  // never triggered
        if (b->open > trigger * (1.0 + cfg_.entry_limit_frac)) continue;  // gapped past the limit
        const double fill = std::max(b->open, trigger);
        const double stop = fill * (1.0 - cfg_.stop_adr_mult * c.adr);

        const double gross = book_.market_value(md_, date, true);
        const SizeDecision d =
            risk_.size_long(eq_open, book_.cash(), gross, fill, stop, c.adr, cfg_.cost.round_trip(), mult);
        if (d.qty <= 0) {
            if (ctx.events) {
                KvRecord r;
                r.type = "reject";
                r.set("sleeve", std::string("A1")).set("date", date).set("symbol", c.symbol);
                r.set("reason", d.reject_reason);
                ctx.events->push_back(r);
            }
            continue;
        }
        if (ctx.shared && ctx.shared->heat_cap > 0) {
            const double add = static_cast<double>(d.qty) * (fill - stop);
            if (book_.open_risk() + ctx.shared->external_open_risk + add > ctx.shared->heat_cap) continue;
        }

        book_.open(c.symbol, date, fill, stop, d.qty, c.adr, b->close, ctx.events);
        // Daily bars cannot say whether the low came before or after the fill, so assume
        // the worst: a low through the stop on entry day is a stop-out.
        if (b->low <= stop) book_.sell(c.symbol, d.qty, stop, date, "stop_entry_day", ctx.events);
    }
}

void A1Engine::screen_for_tomorrow(const std::string& date, std::vector<KvRecord>* events) {
    pending_.clear();
    last_scores_.clear();
    if (risk_.state() == RiskState::Off) return;

    std::vector<SymbolView> views;
    for (const auto& [sym, s] : md_.universe()) {
        if (excluded_.excluded(sym, date)) continue;
        std::size_t i = 0;
        if (md_.bar(sym, date, &i)) views.push_back({sym, &s, i});
    }
    for (const auto& r : score_universe(views, cfg_.screen)) last_scores_.emplace_back(r.symbol, r.score);
    for (auto& c : screen(views, cfg_.screen)) {
        if (!book_.holds(c.symbol)) pending_.push_back(std::move(c));
    }

    const EquityRegime reg =
        evaluate_equity_regime({regime_in_.index, regime_in_.vix, nullptr, nullptr}, date, cfg_.regime);
    pending_allowed_ = reg.allow_new_entries();
    const double vm = cfg_.vol_scale ? vol_multiplier(date) : std::numeric_limits<double>::quiet_NaN();
    pending_risk_mult_ = std::isnan(vm) ? reg.risk_multiplier() : vm;

    if (events) {
        KvRecord r;
        r.type = "regime";
        r.set("sleeve", std::string("A1")).set("date", date).set("trend", std::string(to_string(reg.trend)));
        r.set("vol", std::string(to_string(reg.vol))).set("crash", std::string(to_string(reg.crash)));
        r.set("risk_mult", pending_risk_mult_);
        events->push_back(r);
        for (const Candidate& c : pending_) {
            KvRecord s;
            s.type = "signal";
            s.set("sleeve", std::string("A1")).set("date", date).set("symbol", c.symbol).set("score", c.score);
            s.set("trigger", c.pivot + cfg_.tick).set("adr", c.adr).set("allowed", pending_allowed_);
            events->push_back(s);
        }
    }
}

void A1Engine::flatten(const std::string& date, const std::string& reason, std::vector<KvRecord>* events) {
    book_.flatten(md_, date, reason, events);
    pending_.clear();
    last_equity_ = book_.cash();
}

std::vector<KvRecord> A1Engine::save() const {
    std::vector<KvRecord> out;
    KvRecord h;
    h.type = "a1";
    h.set("last_date", last_date_).set("last_equity", last_equity_);
    h.set("pending_allowed", pending_allowed_).set("pending_risk_mult", pending_risk_mult_);
    out.push_back(h);
    out.push_back(risk_.snapshot());
    book_.save(out);
    for (const Candidate& c : pending_) {
        KvRecord p;
        p.type = "pending";
        p.set("symbol", c.symbol).set("score", c.score).set("pivot", c.pivot).set("adr", c.adr);
        out.push_back(p);
    }
    return out;
}

void A1Engine::load(const std::vector<KvRecord>& records) {
    pending_.clear();
    for (const auto& r : records) {
        if (r.type == "a1") {
            last_date_ = r.str("last_date");
            last_equity_ = r.num("last_equity");
            pending_allowed_ = r.integer("pending_allowed", 1) != 0;
            pending_risk_mult_ = r.num("pending_risk_mult", 1.0);
        } else if (r.type == "risk") {
            risk_.restore(r);
        } else if (r.type == "pending") {
            pending_.push_back({r.str("symbol"), r.num("score"), r.num("pivot"), r.num("adr")});
        }
    }
    book_.load(records);
}

BacktestResult run_a1_backtest(const MarketData& md, const A1Config& cfg, const ExclusionList& excluded,
                               double initial_equity, const std::string& start, const std::string& end,
                               const EquityRegimeInputs& regime) {
    A1Engine eng(md, cfg, excluded, regime, initial_equity);
    BacktestResult r;
    for (const auto& date : md.dates()) {
        if (!start.empty() && date < start) continue;
        if (!end.empty() && date > end) break;
        eng.step(date);
        r.equity_curve.emplace_back(date, eng.equity());
    }
    if (!r.equity_curve.empty()) {
        eng.flatten(r.equity_curve.back().first, "end_of_data", nullptr);
        r.equity_curve.back().second = eng.equity();  // after exit costs
    }
    r.trades = eng.drain_closed();
    r.metrics = compute_metrics(r.trades, r.equity_curve);
    r.final_risk_state = eng.risk().state();
    return r;
}

}  // namespace ta
