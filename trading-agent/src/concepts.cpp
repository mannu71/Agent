#include "ta/concepts.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

namespace ta {

namespace {

constexpr double kNaN = std::numeric_limits<double>::quiet_NaN();

struct Level {
    double price;
    std::size_t idx;  // bar of the swing point
};

// Context shared by every detector, all as of each bar's close.
struct ContextData {
    std::vector<std::string> trend;  // per signal bar
    std::vector<double> vwap;        // session (UTC day) VWAP through the bar
    std::vector<double> atr, atr_long;
};

ContextData build_context(const FlowSeries& bars, const FlowSeries& daily, int tf, const ConceptConfig& cfg) {
    ContextData c;
    c.atr = atr_series(bars, cfg.atr_n);
    c.atr_long = atr_series(bars, 100);
    // Daily trend: last completed UTC day's close vs SMA50 of completed closes.
    std::vector<std::string> day_trend(daily.size());
    double sum = 0;
    for (std::size_t d = 0; d < daily.size(); ++d) {
        sum += daily[d].close;
        if (d >= 50) sum -= daily[d - 50].close;
        day_trend[d] = d + 1 < 50 ? "" : (daily[d].close > sum / 50.0 ? "up" : "down");
    }
    c.trend.resize(bars.size());
    c.vwap.resize(bars.size());
    std::size_t d = 0;
    std::int64_t cur_day = std::numeric_limits<std::int64_t>::min();
    double pv = 0, v = 0;
    for (std::size_t k = 0; k < bars.size(); ++k) {
        const std::int64_t close_t = bars[k].t + tf;
        // daily[d] is complete once its day has ended: daily[d].t + 1440 <= close_t.
        while (d < daily.size() && daily[d].t + 1440 <= close_t) ++d;
        c.trend[k] = d > 0 ? day_trend[d - 1] : "";
        const std::int64_t day = day_of(bars[k].t);
        if (day != cur_day) {
            cur_day = day;
            pv = v = 0;
        }
        const double typical = (bars[k].high + bars[k].low + bars[k].close) / 3.0;
        pv += typical * bars[k].volume;
        v += bars[k].volume;
        c.vwap[k] = v > 0 ? pv / v : bars[k].close;
    }
    return c;
}

std::string session_of(std::int64_t t) {
    const auto h = (t - day_of(t) * 1440) / 60;
    return h < 7 ? "asia" : h < 13 ? "london" : h < 21 ? "ny" : "late";
}

class Emitter {
public:
    Emitter(const FlowSeries& bars, const ContextData& ctx, int tf, const std::string& symbol, const ConceptConfig& cfg,
            std::vector<Setup>& out)
        : bars_(bars), ctx_(ctx), tf_(tf), symbol_(symbol), cfg_(cfg), out_(out) {}

    // Market order at the next bar's open with a fixed-R target.
    void market(std::size_t k, const std::string& pattern, int side, double stop) {
        Setup s = base(k, pattern, side);
        s.order = 'M';
        s.stop = stop;
        s.rr = cfg_.rr;
        push(s, bars_[k].close);
    }
    // Market order with an absolute target (e.g. the other side of a range).
    void market_to(std::size_t k, const std::string& pattern, int side, double stop, double target) {
        Setup s = base(k, pattern, side);
        s.order = 'M';
        s.stop = stop;
        s.target = target;
        push(s, bars_[k].close);
    }
    void limit(std::size_t k, const std::string& pattern, int side, double entry, double stop) {
        Setup s = base(k, pattern, side);
        s.order = 'L';
        s.entry = entry;
        s.stop = stop;
        s.target = entry + side * cfg_.rr * std::fabs(entry - stop);
        s.expiry = s.t + static_cast<std::int64_t>(cfg_.order_expiry_bars) * tf_;
        push(s, entry);
    }

private:
    Setup base(std::size_t k, const std::string& pattern, int side) const {
        Setup s;
        s.t = bars_[k].t + tf_;
        s.pattern = pattern;
        s.symbol = symbol_;
        s.tf = tf_;
        s.side = side;
        s.ref = bars_[k].close;
        s.expiry = s.t + tf_;  // market orders fill on the next bar or not at all
        s.max_hold = static_cast<std::int64_t>(cfg_.hold_bars) * tf_;
        const double ratio = ctx_.atr[k] / ctx_.atr_long[k];
        s.ctx.trend = ctx_.trend[k];
        s.ctx.session = session_of(bars_[k].t);
        s.ctx.vol = std::isnan(ratio) ? "" : ratio < 0.8 ? "low" : ratio > 1.2 ? "high" : "mid";
        s.ctx.vwap = bars_[k].close >= ctx_.vwap[k] ? "above" : "below";
        s.ctx.flow = bars_[k].delta() * side > 0 ? "with" : "against";
        return s;
    }
    void push(const Setup& s, double ref) {
        // Wrong-side stops and stops too tight for the costs are not setups.
        if (s.side * (ref - s.stop) <= 0) return;
        if (std::fabs(ref - s.stop) < cfg_.min_risk_frac * ref) return;
        if (s.target != 0 && s.side * (s.target - ref) <= 0) return;
        out_.push_back(s);
    }

    const FlowSeries& bars_;
    const ContextData& ctx_;
    int tf_;
    std::string symbol_;
    const ConceptConfig& cfg_;
    std::vector<Setup>& out_;
};

double max_high(const FlowSeries& b, std::size_t from, std::size_t to) {
    double h = -std::numeric_limits<double>::infinity();
    for (std::size_t i = from; i < to; ++i) h = std::max(h, b[i].high);
    return h;
}
double min_low(const FlowSeries& b, std::size_t from, std::size_t to) {
    double l = std::numeric_limits<double>::infinity();
    for (std::size_t i = from; i < to; ++i) l = std::min(l, b[i].low);
    return l;
}

}  // namespace

void swing_points(const FlowSeries& bars, int n, std::vector<bool>& swing_high, std::vector<bool>& swing_low) {
    const std::size_t m = bars.size();
    swing_high.assign(m, false);
    swing_low.assign(m, false);
    const auto un = static_cast<std::size_t>(n);
    for (std::size_t i = un; i + un < m; ++i) {
        bool hi = true, lo = true;
        for (std::size_t j = i - un; j <= i + un && (hi || lo); ++j) {
            if (j == i) continue;
            if (bars[j].high >= bars[i].high) hi = false;
            if (bars[j].low <= bars[i].low) lo = false;
        }
        swing_high[i] = hi;
        swing_low[i] = lo;
    }
}

std::vector<double> atr_series(const FlowSeries& bars, int n) {
    std::vector<double> out(bars.size(), kNaN);
    double atr = 0;
    for (std::size_t k = 1; k < bars.size(); ++k) {
        const double pc = bars[k - 1].close;
        const double tr = std::max({bars[k].high - bars[k].low, std::fabs(bars[k].high - pc), std::fabs(bars[k].low - pc)});
        const auto kn = static_cast<std::size_t>(n);
        if (k < kn) {
            atr += tr;
        } else if (k == kn) {
            atr = (atr + tr) / n;
            out[k] = atr;
        } else {
            atr = (atr * (n - 1) + tr) / n;
            out[k] = atr;
        }
    }
    return out;
}

ValueArea value_area(const FlowSeries& bars, std::size_t from, std::size_t to, int bins, double share) {
    ValueArea va;
    if (from >= to || bins < 1) return va;
    const double lo = min_low(bars, from, to), hi = max_high(bars, from, to);
    if (!(hi > lo)) {
        va.poc = va.val = va.vah = lo;
        return va;
    }
    const double w = (hi - lo) / bins;
    std::vector<double> vol(static_cast<std::size_t>(bins), 0.0);
    double total = 0;
    for (std::size_t i = from; i < to; ++i) {
        const auto b0 = std::min(bins - 1, static_cast<int>((bars[i].low - lo) / w));
        const auto b1 = std::min(bins - 1, static_cast<int>((bars[i].high - lo) / w));
        const double per = bars[i].volume / (b1 - b0 + 1);
        for (int b = b0; b <= b1; ++b) vol[static_cast<std::size_t>(b)] += per;
        total += bars[i].volume;
    }
    auto poc = static_cast<int>(std::max_element(vol.begin(), vol.end()) - vol.begin());
    // Expand from the POC, adding the larger neighbouring bucket first, until `share` is covered.
    int a = poc, b = poc;
    double inside = vol[static_cast<std::size_t>(poc)];
    while (inside < share * total && (a > 0 || b < bins - 1)) {
        const double up = b < bins - 1 ? vol[static_cast<std::size_t>(b + 1)] : -1;
        const double dn = a > 0 ? vol[static_cast<std::size_t>(a - 1)] : -1;
        if (up >= dn) inside += vol[static_cast<std::size_t>(++b)];
        else inside += vol[static_cast<std::size_t>(--a)];
    }
    va.poc = lo + (poc + 0.5) * w;
    va.val = lo + a * w;
    va.vah = lo + (b + 1) * w;
    return va;
}

std::vector<Setup> detect_setups(const FlowSeries& bars, const FlowSeries& daily, int tf, const std::string& symbol,
                                 const std::set<std::string>& concepts, const ConceptConfig& cfg) {
    std::vector<Setup> out;
    if (bars.size() < 120) return out;
    const ContextData ctx = build_context(bars, daily, tf, cfg);
    Emitter emit(bars, ctx, tf, symbol, cfg, out);
    auto on = [&](const char* c) { return concepts.count(c) != 0; };

    std::vector<bool> sh, sl;
    swing_points(bars, cfg.swing_n, sh, sl);
    const auto n = static_cast<std::size_t>(cfg.swing_n);

    std::vector<Level> open_highs, open_lows;      // unswept, unbroken swing levels (sweep)
    std::vector<Level> highs, lows;                // every confirmed swing point (structure)
    bool high_broken = true, low_broken = true;    // has the latest swing high / low been broken?
    std::size_t spring_cool = 0, upthrust_cool = 0;
    std::int64_t va_day = std::numeric_limits<std::int64_t>::min();
    ValueArea va;
    bool va_ok = false, va_done = false;
    int va_inside = 0, va_from = 0;  // va_from: +1 opened below value (long), -1 above (short)
    std::size_t day_start = 0, prev_day_start = 0;
    double day_low = 0, day_high = 0;

    for (std::size_t k = 2; k < bars.size(); ++k) {
        const FlowBar& b = bars[k];
        const double atr = ctx.atr[k];
        if (std::isnan(atr) || std::isnan(ctx.atr_long[k])) continue;
        const double buf = cfg.stop_buffer_atr * atr;

        // A swing point at k - n is confirmed by this bar's close.
        if (k >= 2 * n) {
            const std::size_t i = k - n;
            if (sh[i]) {
                highs.push_back({bars[i].high, i});
                open_highs.push_back({bars[i].high, i});
                high_broken = false;
            }
            if (sl[i]) {
                const bool higher_low = !lows.empty() && bars[i].low > lows.back().price;
                const bool higher_high = highs.size() >= 2 && highs.back().price > highs[highs.size() - 2].price;
                lows.push_back({bars[i].low, i});
                open_lows.push_back({bars[i].low, i});
                low_broken = false;
                if (on("hl_pullback") && higher_low && higher_high) emit.market(k, "hl_pullback", 1, bars[i].low - buf);
            }
            if (sh[i] && on("hl_pullback")) {
                const bool lower_high = highs.size() >= 2 && bars[i].high < highs[highs.size() - 2].price;
                const bool lower_low = lows.size() >= 2 && lows.back().price < lows[lows.size() - 2].price;
                if (lower_high && lower_low) emit.market(k, "hl_pullback", -1, bars[i].high + buf);
            }
        }

        // Structure: CHoCH (first break against the trend) and the order block of any break.
        const bool up = highs.size() >= 2 && lows.size() >= 2 && highs.back().price > highs[highs.size() - 2].price &&
                        lows.back().price > lows[lows.size() - 2].price;
        const bool down = highs.size() >= 2 && lows.size() >= 2 && highs.back().price < highs[highs.size() - 2].price &&
                          lows.back().price < lows[lows.size() - 2].price;
        if (!highs.empty() && !high_broken && b.close > highs.back().price) {
            high_broken = true;
            if (on("choch") && down && !lows.empty()) emit.market(k, "choch", 1, lows.back().price - buf);
            if (on("ob")) {
                const std::size_t from = lows.empty() ? (k > 20 ? k - 20 : 0) : lows.back().idx;
                for (std::size_t j = k; j-- > from;) {
                    if (bars[j].close < bars[j].open) {
                        if (bars[j].high < b.close) emit.limit(k, "ob", 1, bars[j].high, bars[j].low - buf);
                        break;
                    }
                }
            }
        }
        if (!lows.empty() && !low_broken && b.close < lows.back().price) {
            low_broken = true;
            if (on("choch") && up && !highs.empty()) emit.market(k, "choch", -1, highs.back().price + buf);
            if (on("ob")) {
                const std::size_t from = highs.empty() ? (k > 20 ? k - 20 : 0) : highs.back().idx;
                for (std::size_t j = k; j-- > from;) {
                    if (bars[j].close > bars[j].open) {
                        if (bars[j].low > b.close) emit.limit(k, "ob", -1, bars[j].low, bars[j].high + buf);
                        break;
                    }
                }
            }
        }

        // Liquidity sweeps: wick beyond a swing level, close back inside.
        auto sweep = [&](std::vector<Level>& levels, int side) {
            double swept = kNaN;
            std::vector<Level> keep;
            for (const Level& l : levels) {
                if (k - l.idx > static_cast<std::size_t>(cfg.sweep_lookback)) continue;  // stale
                const bool beyond = side < 0 ? b.high > l.price : b.low < l.price;
                if (!beyond) {
                    keep.push_back(l);
                    continue;
                }
                const bool back = side < 0 ? b.close < l.price : b.close > l.price;
                if (back && (std::isnan(swept) || side * (swept - l.price) > 0)) swept = l.price;
            }
            levels.swap(keep);
            if (std::isnan(swept)) return;
            const double stop = side < 0 ? b.high + buf : b.low - buf;
            if (on("sweep")) emit.market(k, "sweep", side, stop);
            // Absorption: aggressive orders pushed through the level and still failed.
            if (on("sweep_absorb") && b.delta() * side < 0) emit.market(k, "sweep_absorb", side, stop);
            // Osler (2005) and the G10 FX tests: stop runs tend to continue, not reverse.
            if (on("sweep_cont")) emit.market(k, "sweep_cont", -side, side < 0 ? b.low - buf : b.high + buf);
        };
        sweep(open_highs, -1);
        sweep(open_lows, 1);

        // Fair value gaps with displacement.
        if (on("fvg")) {
            const FlowBar &c1 = bars[k - 2], &c2 = bars[k - 1];
            const bool disp = (c2.high - c2.low) >= cfg.displacement_atr * ctx.atr[k - 1];
            if (disp && b.low > c1.high && b.low - c1.high >= cfg.fvg_min_atr * atr) {
                emit.limit(k, "fvg", 1, (b.low + c1.high) / 2.0, c1.low - buf);
            }
            if (disp && b.high < c1.low && c1.low - b.high >= cfg.fvg_min_atr * atr) {
                emit.limit(k, "fvg", -1, (b.high + c1.low) / 2.0, c1.high + buf);
            }
        }

        // Wyckoff spring / upthrust out of a trading range.
        const auto L = static_cast<std::size_t>(cfg.range_len);
        if ((on("spring")) && k > L) {
            const double hi = max_high(bars, k - L, k), lo = min_low(bars, k - L, k);
            if (hi - lo <= cfg.range_max_atr * ctx.atr[k - 1]) {
                double vol = 0;
                for (std::size_t j = k - L; j < k; ++j) vol += bars[j].volume;
                const bool climax = b.volume >= cfg.climax_vol_mult * vol / static_cast<double>(L);
                if (climax && b.low < lo && b.close > lo && k >= spring_cool) {
                    emit.market_to(k, "spring", 1, b.low - buf, hi);
                    spring_cool = k + L / 2;
                }
                if (climax && b.high > hi && b.close < hi && k >= upthrust_cool) {
                    emit.market_to(k, "spring", -1, b.high + buf, lo);
                    upthrust_cool = k + L / 2;
                }
            }
        }

        // Dalton's 80% rule on the prior UTC day's value area.
        if (on("va80")) {
            const std::int64_t day = day_of(b.t);
            if (day != va_day) {
                prev_day_start = day_start;
                day_start = k;
                va_ok = va_day != std::numeric_limits<std::int64_t>::min() && day == va_day + 1;
                va_day = day;
                if (va_ok) va = value_area(bars, prev_day_start, day_start, cfg.profile_bins, cfg.value_area);
                va_done = false;
                va_inside = 0;
                va_from = !va_ok ? 0 : b.open < va.val ? 1 : b.open > va.vah ? -1 : 0;
                day_low = b.low;
                day_high = b.high;
            }
            day_low = std::min(day_low, b.low);
            day_high = std::max(day_high, b.high);
            if (va_from != 0 && !va_done) {
                const bool inside = b.close > va.val && b.close < va.vah;
                va_inside = inside ? va_inside + 1 : 0;
                if (va_inside >= 2) {
                    if (va_from > 0) emit.market_to(k, "va80", 1, day_low - buf, va.vah);
                    else emit.market_to(k, "va80", -1, day_high + buf, va.val);
                    va_done = true;
                }
            }
        }
    }
    return out;
}

}  // namespace ta
