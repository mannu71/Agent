#include "ta/screener.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

#include "ta/indicators.hpp"

namespace ta {

namespace {
constexpr std::size_t kYear = 252;
constexpr double kNaN = std::numeric_limits<double>::quiet_NaN();
}  // namespace

std::vector<double> zscore_clipped(const std::vector<double>& x, double clip) {
    std::vector<double> z(x.size(), 0.0);
    if (x.size() < 2) return z;
    double mean = 0;
    for (double v : x) mean += v;
    mean /= static_cast<double>(x.size());
    double var = 0;
    for (double v : x) var += (v - mean) * (v - mean);
    const double sd = std::sqrt(var / static_cast<double>(x.size() - 1));
    if (sd == 0) return z;
    for (std::size_t k = 0; k < x.size(); ++k) z[k] = std::clamp((x[k] - mean) / sd, -clip, clip);
    return z;
}

bool passes_universe(const Series& s, std::size_t i, const ScreenConfig& cfg) {
    if (i >= s.size() || i < kYear) return false;
    if (s[i].close < cfg.min_price) return false;
    const double atv = avg_traded_value(s, i, 20);
    return !std::isnan(atv) && atv >= cfg.min_avg_traded_value;
}

double setup_pivot(const Series& s, std::size_t i, const ScreenConfig& cfg) {
    const double runup = std::max(ret(s, i, 21), ret(s, i, 63));
    if (std::isnan(runup) || runup < cfg.min_runup) return kNaN;

    const double adr = adr_frac(s, i, 20);
    if (std::isnan(adr) || adr < cfg.min_adr) return kNaN;

    const double sma10 = sma_close(s, i, 10);
    const double sma20 = sma_close(s, i, 20);
    if (!(s[i].close > sma10 && sma10 > sma20)) return kNaN;

    const double pivot = highest_high(s, i, cfg.base_len);
    const double base_range = pivot - lowest_low(s, i, cfg.base_len);
    const double recent_range = highest_high(s, i, 5) - lowest_low(s, i, 5);
    if (std::isnan(base_range) || base_range <= 0) return kNaN;
    if (recent_range > cfg.max_tightness * base_range) return kNaN;

    const double close = s[i].close;
    if (close >= pivot || close < pivot * (1.0 - cfg.max_below_pivot)) return kNaN;
    return pivot;
}

std::vector<Candidate> screen(const std::vector<SymbolView>& views, const ScreenConfig& cfg) {
    std::vector<const SymbolView*> eligible;
    std::vector<double> f6m, f12m, fhigh, f3m;
    for (const auto& v : views) {
        const Series& s = *v.series;
        if (!passes_universe(s, v.idx, cfg)) continue;
        const double vol = ann_vol(s, v.idx, kYear);
        const double hi52 = highest_high(s, v.idx, kYear);
        if (std::isnan(vol) || vol <= 0 || std::isnan(hi52)) continue;
        eligible.push_back(&v);
        f6m.push_back(ret(s, v.idx, 126) / vol);
        f12m.push_back(ret(s, v.idx, kYear) / vol);
        fhigh.push_back(s[v.idx].close / hi52);
        f3m.push_back(ret(s, v.idx, 63));
    }
    if (eligible.empty()) return {};

    const auto z6 = zscore_clipped(f6m), z12 = zscore_clipped(f12m);
    const auto zh = zscore_clipped(fhigh), z3 = zscore_clipped(f3m);

    std::vector<std::pair<double, const SymbolView*>> ranked;
    for (std::size_t k = 0; k < eligible.size(); ++k) {
        ranked.emplace_back((z6[k] + z12[k] + zh[k] + z3[k]) / 4.0, eligible[k]);
    }
    std::stable_sort(ranked.begin(), ranked.end(),
                     [](const auto& a, const auto& b) { return a.first > b.first; });

    const auto keep = static_cast<std::size_t>(
        std::ceil(cfg.top_fraction * static_cast<double>(ranked.size())));
    std::vector<Candidate> out;
    for (std::size_t k = 0; k < keep && k < ranked.size(); ++k) {
        const SymbolView& v = *ranked[k].second;
        const double pivot = setup_pivot(*v.series, v.idx, cfg);
        if (std::isnan(pivot)) continue;
        out.push_back({v.symbol, ranked[k].first, pivot, adr_frac(*v.series, v.idx, 20)});
    }
    return out;
}

}  // namespace ta
