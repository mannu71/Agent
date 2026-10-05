#include "ta/monitor.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <numeric>

namespace ta {

namespace {

constexpr double kNaN = std::numeric_limits<double>::quiet_NaN();

std::vector<double> ranks(const std::vector<double>& v) {
    std::vector<std::size_t> idx(v.size());
    std::iota(idx.begin(), idx.end(), 0);
    std::sort(idx.begin(), idx.end(), [&](std::size_t a, std::size_t b) { return v[a] < v[b]; });
    std::vector<double> r(v.size());
    for (std::size_t k = 0; k < idx.size();) {
        std::size_t j = k;
        while (j + 1 < idx.size() && v[idx[j + 1]] == v[idx[k]]) ++j;
        const double avg = (static_cast<double>(k) + static_cast<double>(j)) / 2.0 + 1.0;  // ties share a rank
        for (std::size_t m = k; m <= j; ++m) r[idx[m]] = avg;
        k = j + 1;
    }
    return r;
}

double pearson(const std::vector<double>& x, const std::vector<double>& y) {
    const double n = static_cast<double>(x.size());
    if (n < 2) return kNaN;
    const double mx = std::accumulate(x.begin(), x.end(), 0.0) / n;
    const double my = std::accumulate(y.begin(), y.end(), 0.0) / n;
    double sxy = 0, sxx = 0, syy = 0;
    for (std::size_t k = 0; k < x.size(); ++k) {
        sxy += (x[k] - mx) * (y[k] - my);
        sxx += (x[k] - mx) * (x[k] - mx);
        syy += (y[k] - my) * (y[k] - my);
    }
    return sxx > 0 && syy > 0 ? sxy / std::sqrt(sxx * syy) : kNaN;
}

}  // namespace

double spearman(const std::vector<double>& x, const std::vector<double>& y) {
    if (x.size() != y.size() || x.size() < 3) return kNaN;
    return pearson(ranks(x), ranks(y));
}

double forward_return(const Series& s, std::size_t signal_idx, std::size_t horizon) {
    const std::size_t entry = signal_idx + 1, exit = signal_idx + horizon;
    if (horizon == 0 || exit >= s.size()) return kNaN;
    return s[exit].close / s[entry].open - 1.0;
}

std::vector<DailyScore> score_outcomes(const ScoreLog& log, const MarketData& md, std::size_t horizon,
                                       std::size_t top_k) {
    std::vector<DailyScore> out;
    for (const auto& [date, rows] : log) {
        std::vector<double> sc, fr;
        std::vector<std::pair<double, double>> pairs;
        for (const auto& [sym, score] : rows) {
            std::size_t i = 0;
            if (!md.bar(sym, date, &i)) continue;
            const double f = forward_return(md.series(sym), i, horizon);
            if (std::isnan(f)) continue;
            sc.push_back(score);
            fr.push_back(f);
            pairs.emplace_back(score, f);
        }
        if (sc.size() < 3) continue;
        DailyScore d;
        d.date = date;
        d.n = static_cast<int>(sc.size());
        d.rank_ic = spearman(sc, fr);
        d.base_rate = static_cast<double>(std::count_if(fr.begin(), fr.end(), [](double v) { return v > 0; })) /
                      static_cast<double>(fr.size());
        std::sort(pairs.begin(), pairs.end(), [](const auto& a, const auto& b) { return a.first > b.first; });
        const std::size_t k = std::min(top_k, pairs.size());
        int hits = 0;
        for (std::size_t j = 0; j < k; ++j) hits += pairs[j].second > 0;
        d.precision_at_k = k > 0 ? static_cast<double>(hits) / static_cast<double>(k) : kNaN;
        if (!std::isnan(d.rank_ic)) out.push_back(d);
    }
    return out;
}

double rolling_mean_ic(const std::vector<DailyScore>& s, std::size_t window) {
    if (s.empty()) return kNaN;
    const std::size_t from = s.size() > window ? s.size() - window : 0;
    double sum = 0;
    for (std::size_t k = from; k < s.size(); ++k) sum += s[k].rank_ic;
    return sum / static_cast<double>(s.size() - from);
}

bool Cusum::update(double x) {
    s_ = std::max(0.0, s_ + (target_ - x) - k_);
    if (s_ > h_) alarm_ = true;
    return alarm_;
}

double psi(const std::vector<double>& reference, const std::vector<double>& recent, int bins) {
    if (reference.size() < static_cast<std::size_t>(bins) || recent.empty()) return kNaN;
    std::vector<double> ref = reference;
    std::sort(ref.begin(), ref.end());
    std::vector<double> edges;
    for (int b = 1; b < bins; ++b) edges.push_back(ref[ref.size() * b / bins]);
    auto bucket = [&](double v) {
        return static_cast<int>(std::upper_bound(edges.begin(), edges.end(), v) - edges.begin());
    };
    std::vector<double> pe(bins, 0), pa(bins, 0);
    for (double v : reference) pe[bucket(v)] += 1;
    for (double v : recent) pa[bucket(v)] += 1;
    double out = 0;
    for (int b = 0; b < bins; ++b) {
        const double e = std::max(pe[b] / reference.size(), 1e-4);
        const double a = std::max(pa[b] / recent.size(), 1e-4);
        out += (a - e) * std::log(a / e);
    }
    return out;
}

const char* to_string(KillAction a) {
    switch (a) {
        case KillAction::None: return "none";
        case KillAction::Halve: return "halve";
        case KillAction::Off: return "off";
    }
    return "?";
}

double rolling_expectancy(const std::vector<Trade>& trades, std::size_t n) {
    if (n == 0 || trades.size() < n) return kNaN;
    double sum = 0;
    for (std::size_t k = trades.size() - n; k < trades.size(); ++k) sum += trades[k].r_multiple;
    return sum / static_cast<double>(n);
}

KillDecision a1_kill_rule(const std::vector<Trade>& trades, double slippage_ratio_30) {
    if (trades.size() >= 30 && slippage_ratio_30 > 1.5) return {KillAction::Off, "slippage above 1.5x model over 30 trades"};
    const double e100 = rolling_expectancy(trades, 100);
    if (!std::isnan(e100) && e100 < 0) return {KillAction::Off, "100-trade expectancy below zero"};
    const double e50 = rolling_expectancy(trades, 50);
    if (!std::isnan(e50) && e50 < 0) return {KillAction::Halve, "50-trade expectancy below zero"};
    return {};
}

KillDecision a2_kill_rule(const std::vector<Trade>& trades, double uplift_100, int events_seen, double unfilled_rate) {
    const double e40 = rolling_expectancy(trades, 40);
    if (!std::isnan(e40) && e40 < -0.1) return {KillAction::Off, "40-event expectancy below -0.1R"};
    if (events_seen >= 100 && uplift_100 <= 0) return {KillAction::Off, "approved-minus-rejected uplift <= 0 over 100 events"};
    if (unfilled_rate > 0.20) return {KillAction::Halve, "band-lock/unfilled rate above 20%: review the fill model"};
    return {};
}

KillDecision b_kill_rule(double drawdown, double backtest_max_drawdown) {
    if (backtest_max_drawdown > 0 && drawdown > 1.5 * backtest_max_drawdown) {
        return {KillAction::Off, "drawdown above 1.5x the backtest maximum"};
    }
    return {};
}

KillDecision d1_kill_rule(const std::vector<Trade>& trades, double cost_ratio) {
    if (cost_ratio > 1.5) return {KillAction::Off, "realised cost above 1.5x the model"};
    // Trades are daily, so the last 60 trades approximate the last 60 trading days.
    const double e60 = rolling_expectancy(trades, std::min<std::size_t>(60, trades.size()));
    if (trades.size() >= 20 && !std::isnan(e60) && e60 < 0) return {KillAction::Off, "60-day expectancy below zero"};
    return {};
}

KillDecision screener_rule(double ic_60, bool cusum_alarm, double expectancy_120) {
    if (!std::isnan(ic_60) && ic_60 < 0 && cusum_alarm) return {KillAction::Off, "60-day rank IC < 0 with CUSUM alarm"};
    if (!std::isnan(expectancy_120) && expectancy_120 < 0) return {KillAction::Off, "120-day expectancy after costs < 0"};
    return {};
}

}  // namespace ta
