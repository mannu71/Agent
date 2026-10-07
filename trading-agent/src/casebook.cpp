#include "ta/casebook.hpp"

#include <algorithm>
#include <cmath>
#include <queue>
#include <limits>
#include <random>

namespace ta {

Case simulate_case(const Setup& s, const FlowSeries& m1, const ExecCost& cost) {
    Case c;
    c.setup = s;
    const int side = s.side;
    std::size_t j = lower_index(m1, s.t);
    double fill = 0, fee_in = 0;
    bool same_bar_target = true;  // a market fill at the open may reach its target in that minute

    // Entry.
    for (; j < m1.size() && m1[j].t < s.expiry; ++j) {
        const FlowBar& b = m1[j];
        if (s.order == 'M') {
            fill = b.open * (1.0 + side * cost.slippage);
            fee_in = cost.taker;
        } else if (s.order == 'L') {
            if (side > 0 ? b.low > s.entry : b.high < s.entry) continue;
            fill = side > 0 ? std::min(b.open, s.entry) : std::max(b.open, s.entry);
            fee_in = cost.maker;
            same_bar_target = false;  // the target may have traded before the fill in this minute
        } else {
            if (side > 0 ? b.high < s.entry : b.low > s.entry) continue;
            fill = (side > 0 ? std::max(b.open, s.entry) : std::min(b.open, s.entry)) * (1.0 + side * cost.slippage);
            fee_in = cost.taker;
            same_bar_target = false;
        }
        break;
    }
    if (fill <= 0 || j >= m1.size()) return c;
    if (side * (fill - s.stop) <= 0) return c;  // opened beyond the stop: the setup is void
    c.filled = true;
    c.entry_t = m1[j].t;
    c.entry_px = fill;
    c.stop_px = s.stop;
    const double risk = std::fabs(fill - s.stop);
    c.target_px = s.target != 0 ? s.target : (s.rr > 0 ? fill + side * s.rr * risk : 0);
    if (c.target_px != 0 && side * (c.target_px - fill) <= 0) c.target_px = 0;  // already through it: time exit

    // Exit.
    double exit = 0, fee_out = 0;
    std::size_t k = j;
    for (; k < m1.size(); ++k) {
        const FlowBar& b = m1[k];
        if (k > j && b.t >= c.entry_t + s.max_hold) {
            exit = b.open * (1.0 - side * cost.slippage);
            fee_out = cost.taker;
            c.exit_reason = "time";
            break;
        }
        const bool first = k == j;
        const bool gap_stop = !first && (side > 0 ? b.open <= s.stop : b.open >= s.stop);
        const bool stop_hit = side > 0 ? b.low <= s.stop : b.high >= s.stop;
        if (gap_stop || stop_hit) {
            exit = (gap_stop ? b.open : s.stop) * (1.0 - side * cost.slippage);
            fee_out = cost.taker;
            c.exit_reason = "stop";
            break;
        }
        const bool target_hit = c.target_px != 0 && (side > 0 ? b.high >= c.target_px : b.low <= c.target_px) &&
                                (!first || same_bar_target);
        if (target_hit) {
            const bool gap_target = !first && (side > 0 ? b.open >= c.target_px : b.open <= c.target_px);
            exit = gap_target ? b.open : c.target_px;
            fee_out = cost.maker;
            c.exit_reason = "target";
            break;
        }
    }
    if (k >= m1.size()) {
        k = m1.size() - 1;
        exit = m1[k].close * (1.0 - side * cost.slippage);
        fee_out = cost.taker;
        c.exit_reason = "end_of_data";
    }
    c.exit_t = m1[k].t;
    c.exit_px = exit;
    c.r_gross = side * (exit - fill) / risk;
    const double hours = static_cast<double>(c.exit_t - c.entry_t) / 60.0;
    const double costs = fee_in * fill + fee_out * exit + cost.funding_per_8h * fill * hours / 8.0;
    c.r_net = c.r_gross - costs / risk;
    return c;
}

std::string memory_key(const Setup& s, const std::vector<std::string>& fields) {
    std::string k;
    for (const auto& f : fields) {
        if (!k.empty()) k += '|';
        if (f == "concept") k += s.pattern;
        else if (f == "tf") k += std::to_string(s.tf);
        else if (f == "side") k += s.side > 0 ? "long" : "short";
        else if (f == "symbol") k += s.symbol;
        else if (f == "trend") k += s.ctx.trend;
        else if (f == "session") k += s.ctx.session;
        else if (f == "vol") k += s.ctx.vol;
        else if (f == "vwap") k += s.ctx.vwap;
        else if (f == "flow") k += s.ctx.flow;
        else k += "?" + f;
    }
    return k;
}

namespace {

struct Stats {
    std::size_t n = 0;
    double sum = 0, sum_sq = 0;
    void add(double x) {
        ++n;
        sum += x;
        sum_sq += x * x;
    }
};

std::string day_string(std::int64_t t) { return format_minutes(t).substr(0, 10); }

}  // namespace

PortfolioResult run_casebook(const std::vector<Case>& cases, const PortfolioConfig& cfg) {
    PortfolioResult res;
    res.setups = static_cast<int>(cases.size());
    std::vector<std::size_t> idx;
    for (std::size_t i = 0; i < cases.size(); ++i) {
        if (cases[i].filled) idx.push_back(i);
    }
    res.filled = static_cast<int>(idx.size());

    // Pass 1, in decision-time order: what did the memory say when each setup appeared?
    std::sort(idx.begin(), idx.end(), [&](std::size_t a, std::size_t b) {
        return cases[a].setup.t != cases[b].setup.t ? cases[a].setup.t < cases[b].setup.t : a < b;
    });
    std::vector<bool> recall_ok(cases.size(), !cfg.recall.enabled);
    if (cfg.recall.enabled) {
        using Pending = std::pair<std::int64_t, std::size_t>;  // (known-from time, case)
        std::priority_queue<Pending, std::vector<Pending>, std::greater<>> pending;
        std::map<std::string, Stats> memory;
        for (std::size_t i : idx) {
            const Case& c = cases[i];
            // An outcome is known once its exit minute has closed.
            while (!pending.empty() && pending.top().first <= c.setup.t) {
                const Case& done = cases[pending.top().second];
                memory[memory_key(done.setup, cfg.recall.fields)].add(done.r_net);
                pending.pop();
            }
            const auto it = memory.find(memory_key(c.setup, cfg.recall.fields));
            if (it != memory.end() && it->second.n >= cfg.recall.min_cases) {
                const Stats& st = it->second;
                const double n = static_cast<double>(st.n);
                const double mean = st.sum / n;
                const double var = std::max(0.0, (st.sum_sq - st.sum * st.sum / n) / (n - 1));
                recall_ok[i] = mean >= cfg.recall.min_mean_r && mean - cfg.recall.z * std::sqrt(var / n) > 0;
            }
            pending.push({c.exit_t + 1, i});
        }
        // What the memory says now, after every case has finished.
        for (; !pending.empty(); pending.pop()) {
            const Case& done = cases[pending.top().second];
            if (done.exit_reason != "end_of_data") memory[memory_key(done.setup, cfg.recall.fields)].add(done.r_net);
        }
        for (const auto& [key, st] : memory) {
            if (st.n < cfg.recall.min_cases) continue;
            const double n = static_cast<double>(st.n);
            const double mean = st.sum / n;
            const double var = std::max(0.0, (st.sum_sq - st.sum * st.sum / n) / (n - 1));
            const double lower = mean - cfg.recall.z * std::sqrt(var / n);
            res.memory_now[key] = {st.n, mean, lower, mean >= cfg.recall.min_mean_r && lower > 0};
        }
    }
    for (std::size_t i : idx) res.recalled += recall_ok[i] ? 1 : 0;

    // Pass 2, in fill-time order: position limits, sizing and realised equity.
    std::sort(idx.begin(), idx.end(), [&](std::size_t a, std::size_t b) {
        return cases[a].entry_t != cases[b].entry_t ? cases[a].entry_t < cases[b].entry_t : a < b;
    });
    double equity = cfg.equity;
    std::map<std::string, double> day_equity;
    struct Open {
        std::int64_t exit_t;
        std::size_t trade;
        bool operator>(const Open& o) const { return exit_t > o.exit_t; }
    };
    std::priority_queue<Open, std::vector<Open>, std::greater<>> open;
    std::map<std::string, int> held;
    auto close_until = [&](std::int64_t t) {
        while (!open.empty() && open.top().exit_t <= t) {
            Trade& tr = res.trades[open.top().trade];
            equity += tr.pnl;
            day_equity[day_string(open.top().exit_t)] = equity;
            --held[tr.symbol];
            open.pop();
        }
    };
    if (!idx.empty()) day_equity[day_string(cases[idx.front()].entry_t)] = equity;
    for (std::size_t i : idx) {
        const Case& c = cases[i];
        close_until(c.entry_t);
        if (!recall_ok[i] || c.setup.t < cfg.trade_from) continue;
        if (held[c.setup.symbol] > 0 || static_cast<int>(open.size()) >= cfg.max_open) continue;
        const double risk_amt = equity * cfg.risk_frac;
        const double unit_risk = std::fabs(c.entry_px - c.stop_px);
        double pnl = c.r_net * risk_amt;
        if (pnl > 0 && cfg.tax_rate > 0) {
            res.tax_paid += pnl * cfg.tax_rate;
            pnl *= 1.0 - cfg.tax_rate;
        }
        Trade tr;
        tr.sleeve = c.setup.pattern;
        tr.symbol = c.setup.symbol;
        tr.entry_date = format_minutes(c.entry_t);
        tr.exit_date = format_minutes(c.exit_t);
        tr.side = c.setup.side;
        tr.entry_price = c.entry_px;
        tr.qty = std::max(1L, static_cast<long>(std::llround(risk_amt / unit_risk)));
        tr.risk_per_share = unit_risk;
        tr.pnl = pnl;
        tr.r_multiple = pnl / risk_amt;
        tr.exit_reason = c.exit_reason;
        res.trades.push_back(tr);
        res.case_of_trade.push_back(i);
        open.push({c.exit_t, res.trades.size() - 1});
        ++held[c.setup.symbol];
        ++res.taken;
    }
    close_until(std::numeric_limits<std::int64_t>::max());

    // Daily curve with every calendar day carried forward (crypto trades all week).
    if (!day_equity.empty()) {
        const std::int64_t d0 = parse_minutes(day_equity.begin()->first) / 1440;
        const std::int64_t d1 = parse_minutes(day_equity.rbegin()->first) / 1440;
        double e = cfg.equity;
        for (std::int64_t d = d0; d <= d1; ++d) {
            const std::string ds = day_string(d * 1440);
            const auto it = day_equity.find(ds);
            if (it != day_equity.end()) e = it->second;
            res.curve.emplace_back(ds, e);
        }
    }
    return res;
}

std::vector<Case> random_cases(const std::vector<Case>& cases, const std::map<std::string, const FlowSeries*>& m1,
                               const ExecCost& cost, unsigned seed) {
    std::mt19937_64 rng(seed);
    std::vector<Case> out;
    for (const Case& c : cases) {
        if (!c.filled) continue;
        const auto it = m1.find(c.setup.symbol);
        if (it == m1.end() || it->second->size() < 2) continue;
        const FlowSeries& bars = *it->second;
        std::uniform_int_distribution<std::size_t> pick(1, bars.size() - 1);
        const std::size_t j = pick(rng);
        const Setup& o = c.setup;
        const double px = bars[j].open;
        const double ref = o.ref > 0 ? o.ref : c.entry_px;
        Setup s = o;
        s.t = bars[j].t;
        s.ref = px;
        s.expiry = s.t + (o.expiry - o.t);
        if (o.order == 'M') {
            // Same stop distance and same reward:risk as the filled case.
            s.stop = px * (1.0 - s.side * std::fabs(c.entry_px - c.stop_px) / c.entry_px);
            s.target = 0;
            s.rr = c.target_px != 0 ? std::fabs(c.target_px - c.entry_px) / std::fabs(c.entry_px - c.stop_px) : 0;
        } else {
            s.entry = px * o.entry / ref;
            s.stop = px * o.stop / ref;
            s.target = o.target != 0 ? px * o.target / ref : 0;
        }
        s.ctx = c.setup.ctx;
        out.push_back(simulate_case(s, bars, cost));
    }
    return out;
}

}  // namespace ta
