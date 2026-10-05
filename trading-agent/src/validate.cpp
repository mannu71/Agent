#include "ta/validate.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <map>
#include <numeric>
#include <random>
#include <sstream>

namespace ta {

namespace {
constexpr double kNaN = std::numeric_limits<double>::quiet_NaN();
constexpr double kEuler = 0.5772156649015329;
}  // namespace

double norm_cdf(double x) { return 0.5 * std::erfc(-x / std::sqrt(2.0)); }

double norm_inv(double p) {
    if (p <= 0 || p >= 1) return p <= 0 ? -std::numeric_limits<double>::infinity() : std::numeric_limits<double>::infinity();
    static const double a[] = {-3.969683028665376e+01, 2.209460984245205e+02, -2.759285104469687e+02,
                               1.383577518672690e+02, -3.066479806614716e+01, 2.506628277459239e+00};
    static const double b[] = {-5.447609879822406e+01, 1.615858368580409e+02, -1.556989798598866e+02,
                               6.680131188771972e+01, -1.328068155288572e+01};
    static const double c[] = {-7.784894002430293e-03, -3.223964580411365e-01, -2.400758277161838e+00,
                               -2.549732539343734e+00, 4.374664141464968e+00, 2.938163982698783e+00};
    static const double d[] = {7.784695709041462e-03, 3.224671290700398e-01, 2.445134137142996e+00,
                               3.754408661907416e+00};
    const double lo = 0.02425, hi = 1 - lo;
    double x;
    if (p < lo) {
        const double q = std::sqrt(-2 * std::log(p));
        x = (((((c[0] * q + c[1]) * q + c[2]) * q + c[3]) * q + c[4]) * q + c[5]) /
            ((((d[0] * q + d[1]) * q + d[2]) * q + d[3]) * q + 1);
    } else if (p <= hi) {
        const double q = p - 0.5, r = q * q;
        x = (((((a[0] * r + a[1]) * r + a[2]) * r + a[3]) * r + a[4]) * r + a[5]) * q /
            (((((b[0] * r + b[1]) * r + b[2]) * r + b[3]) * r + b[4]) * r + 1);
    } else {
        const double q = std::sqrt(-2 * std::log(1 - p));
        x = -(((((c[0] * q + c[1]) * q + c[2]) * q + c[3]) * q + c[4]) * q + c[5]) /
            ((((d[0] * q + d[1]) * q + d[2]) * q + d[3]) * q + 1);
    }
    return x;
}

double mean(const std::vector<double>& v) {
    return v.empty() ? kNaN : std::accumulate(v.begin(), v.end(), 0.0) / static_cast<double>(v.size());
}

double stdev(const std::vector<double>& v) {
    if (v.size() < 2) return kNaN;
    const double m = mean(v);
    double s = 0;
    for (double x : v) s += (x - m) * (x - m);
    return std::sqrt(s / static_cast<double>(v.size() - 1));
}

double t_stat(const std::vector<double>& v) {
    const double sd = stdev(v);
    return sd > 0 ? mean(v) / (sd / std::sqrt(static_cast<double>(v.size()))) : kNaN;
}

double skewness(const std::vector<double>& v) {
    if (v.size() < 3) return 0;
    const double m = mean(v);
    double m2 = 0, m3 = 0;
    for (double x : v) {
        m2 += (x - m) * (x - m);
        m3 += (x - m) * (x - m) * (x - m);
    }
    m2 /= static_cast<double>(v.size());
    m3 /= static_cast<double>(v.size());
    return m2 > 0 ? m3 / std::pow(m2, 1.5) : 0;
}

double kurtosis(const std::vector<double>& v) {
    if (v.size() < 4) return 3;
    const double m = mean(v);
    double m2 = 0, m4 = 0;
    for (double x : v) {
        const double d = (x - m) * (x - m);
        m2 += d;
        m4 += d * d;
    }
    m2 /= static_cast<double>(v.size());
    m4 /= static_cast<double>(v.size());
    return m2 > 0 ? m4 / (m2 * m2) : 3;
}

std::vector<double> periodic_returns(const EquityCurve& curve) {
    std::vector<double> r;
    for (std::size_t k = 1; k < curve.size(); ++k) r.push_back(curve[k].second / curve[k - 1].second - 1.0);
    return r;
}

std::vector<double> r_multiples(const std::vector<Trade>& trades) {
    std::vector<double> r;
    for (const auto& t : trades) r.push_back(t.r_multiple);
    return r;
}

double probabilistic_sharpe(double sr, double sr_benchmark, double n, double skew, double kurt) {
    const double denom = std::sqrt(std::max(1e-12, 1 - skew * sr + (kurt - 1) / 4.0 * sr * sr));
    return norm_cdf((sr - sr_benchmark) * std::sqrt(std::max(1.0, n - 1)) / denom);
}

double expected_max_sharpe(int n_trials, double var_sr) {
    if (n_trials <= 1) return 0;
    const double n = static_cast<double>(n_trials);
    return std::sqrt(std::max(0.0, var_sr)) *
           ((1 - kEuler) * norm_inv(1 - 1 / n) + kEuler * norm_inv(1 - 1 / (n * std::exp(1.0))));
}

double deflated_sharpe(const std::vector<double>& returns, int n_trials, double var_trial_sharpe) {
    const double sd = stdev(returns);
    if (!(sd > 0)) return kNaN;
    const double sr = mean(returns) / sd;
    return probabilistic_sharpe(sr, expected_max_sharpe(n_trials, var_trial_sharpe),
                                static_cast<double>(returns.size()), skewness(returns), kurtosis(returns));
}

double pbo_cscv(const std::vector<std::vector<double>>& perf, int splits) {
    if (perf.empty() || perf[0].size() < 2 || splits < 2 || splits % 2) return kNaN;
    const std::size_t T = perf.size(), N = perf[0].size();
    const std::size_t block = T / static_cast<std::size_t>(splits);
    if (block < 2) return kNaN;

    auto sharpe_over = [&](const std::vector<int>& blocks, std::size_t j) {
        double sum = 0, sum_sq = 0;
        std::size_t n = 0;
        for (int b : blocks) {
            for (std::size_t t = b * block; t < (b + 1) * block; ++t) {
                sum += perf[t][j];
                sum_sq += perf[t][j] * perf[t][j];
                ++n;
            }
        }
        const double m = sum / n;
        const double var = (sum_sq - sum * sum / n) / (n - 1);
        return var > 0 ? m / std::sqrt(var) : 0.0;
    };

    std::vector<int> sel(splits, 0);
    std::fill(sel.begin() + splits / 2, sel.end(), 1);  // lexicographic combinations of halves
    int total = 0, overfit = 0;
    do {
        std::vector<int> is, oos;
        for (int b = 0; b < splits; ++b) (sel[b] ? is : oos).push_back(b);
        std::size_t best = 0;
        double best_sr = -std::numeric_limits<double>::infinity();
        std::vector<double> oos_sr(N);
        for (std::size_t j = 0; j < N; ++j) {
            const double s = sharpe_over(is, j);
            if (s > best_sr) {
                best_sr = s;
                best = j;
            }
            oos_sr[j] = sharpe_over(oos, j);
        }
        // Relative rank of the in-sample winner out of sample, in (0, 1).
        const double below = static_cast<double>(std::count_if(oos_sr.begin(), oos_sr.end(),
                                                                [&](double v) { return v < oos_sr[best]; }));
        const double w = (below + 1) / (static_cast<double>(N) + 1);
        if (std::log(w / (1 - w)) <= 0) ++overfit;
        ++total;
    } while (std::next_permutation(sel.begin(), sel.end()));
    return static_cast<double>(overfit) / total;
}

double expectancy_without_top(const std::vector<Trade>& trades, double frac) {
    std::vector<double> r = r_multiples(trades);
    if (r.empty()) return kNaN;
    std::sort(r.begin(), r.end());
    const auto drop = static_cast<std::size_t>(std::ceil(frac * static_cast<double>(r.size())));
    r.resize(r.size() - std::min(drop, r.size() - 1));
    return mean(r);
}

double max_year_share(const std::vector<Trade>& trades) {
    std::map<std::string, double> by_year;
    double total = 0;
    for (const auto& t : trades) {
        by_year[t.exit_date.substr(0, 4)] += t.pnl;
    }
    for (const auto& [y, p] : by_year) total += std::max(0.0, p);
    if (total <= 0) return 1;
    double best = 0;
    for (const auto& [y, p] : by_year) best = std::max(best, p / total);
    return best;
}

double bootstrap_drawdown_p95(const std::vector<double>& returns, int sims, int block, unsigned seed) {
    if (returns.empty() || sims < 1 || block < 1) return kNaN;
    std::mt19937 rng(seed);
    std::uniform_int_distribution<std::size_t> start(0, returns.size() - 1);
    std::vector<double> dds;
    for (int s = 0; s < sims; ++s) {
        double eq = 1, peak = 1, dd = 0;
        std::size_t produced = 0, pos = start(rng);
        while (produced < returns.size()) {
            if (produced % static_cast<std::size_t>(block) == 0) pos = start(rng);
            eq *= 1 + returns[pos % returns.size()];
            peak = std::max(peak, eq);
            dd = std::max(dd, 1 - eq / peak);
            ++pos;
            ++produced;
        }
        dds.push_back(dd);
    }
    std::sort(dds.begin(), dds.end());
    return dds[static_cast<std::size_t>(0.95 * static_cast<double>(dds.size() - 1))];
}

double trades_needed(double mu, double sd, double t) {
    if (mu == 0) return std::numeric_limits<double>::infinity();
    return std::pow(t * sd / mu, 2);
}

bool GateReport::pass() const {
    return std::all_of(lines.begin(), lines.end(), [](const GateLine& l) { return l.pass; });
}

std::string GateReport::text() const {
    std::ostringstream o;
    o.setf(std::ios::fixed);
    o.precision(3);
    for (const auto& l : lines) {
        o << (l.pass ? "PASS  " : "FAIL  ") << l.name << " = " << l.value << "  (need " << l.requirement << ")\n";
    }
    o << (pass() ? "GATE 1: PASS\n" : "GATE 1: FAIL\n");
    return o.str();
}

GateReport evaluate_gate1(const std::vector<Trade>& trades_1x, const EquityCurve& curve_1x,
                          const std::vector<Trade>& trades_2x, int n_trials, double var_trial_sharpe,
                          double pbo, const GateThresholds& th) {
    GateReport g;
    const auto r1 = r_multiples(trades_1x);
    const auto r2 = r_multiples(trades_2x);
    const double e1 = r1.empty() ? kNaN : mean(r1);
    const double e2 = r2.empty() ? kNaN : mean(r2);
    const double t = t_stat(r1);
    const double dsr = deflated_sharpe(periodic_returns(curve_1x), n_trials, var_trial_sharpe);
    const double dd = compute_metrics(trades_1x, curve_1x).max_drawdown;
    const double top = expectancy_without_top(trades_1x, 0.01);
    const double year = max_year_share(trades_1x);

    auto add = [&](const std::string& name, double v, const std::string& req, bool pass) {
        g.lines.push_back({name, v, req, !std::isnan(v) && pass});
    };
    add("expectancy at 1x costs (R)", e1, ">= " + std::to_string(th.min_expectancy_1x), e1 >= th.min_expectancy_1x);
    add("expectancy at 2x costs (R)", e2, "> 0", e2 > th.min_expectancy_2x);
    add("t-statistic of R", t, "> 3", t > th.min_t);
    add("deflated Sharpe ratio", dsr, ">= 0.95", dsr >= th.min_deflated_sharpe);
    add("probability of backtest overfitting", pbo, "< 0.20", pbo < th.max_pbo);
    add("max drawdown", dd, "<= 0.25", dd <= th.max_drawdown);
    add("expectancy without top 1% (R)", top, "> 0", top > 0);
    add("largest single-year share of P&L", year, "<= 0.40", year <= th.max_year_share);
    return g;
}

}  // namespace ta
