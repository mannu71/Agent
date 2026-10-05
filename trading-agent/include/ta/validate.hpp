#pragma once

#include <string>
#include <vector>

#include "ta/trade.hpp"

namespace ta {

double norm_cdf(double x);
double norm_inv(double p);  // inverse standard normal CDF (Acklam), p in (0, 1)

double mean(const std::vector<double>& v);
double stdev(const std::vector<double>& v);  // sample
double t_stat(const std::vector<double>& v);
double skewness(const std::vector<double>& v);
double kurtosis(const std::vector<double>& v);  // non-excess (normal = 3)

std::vector<double> periodic_returns(const EquityCurve& curve);
std::vector<double> r_multiples(const std::vector<Trade>& trades);

// Probabilistic Sharpe ratio: P(true SR > sr_benchmark) given n observations of
// non-annualised Sharpe `sr` with the sample's skewness and kurtosis.
double probabilistic_sharpe(double sr, double sr_benchmark, double n, double skew, double kurt);
// Expected maximum Sharpe among n_trials unskilled trials with variance `var_sr`.
double expected_max_sharpe(int n_trials, double var_sr);
// Deflated Sharpe ratio (Bailey & Lopez de Prado) of a periodic return series.
double deflated_sharpe(const std::vector<double>& returns, int n_trials, double var_trial_sharpe);

// Probability of backtest overfitting by combinatorially symmetric cross-validation.
// `perf[t][j]` is strategy j's return in period t; periods are split into `splits` blocks.
double pbo_cscv(const std::vector<std::vector<double>>& perf, int splits = 16);

// Mean R after removing the best `frac` of trades (robustness to a few outliers).
double expectancy_without_top(const std::vector<Trade>& trades, double frac);
// Largest single calendar year's share of total positive P&L (1 when one year made it all).
double max_year_share(const std::vector<Trade>& trades);
// 95th-percentile maximum drawdown from a stationary block bootstrap of periodic returns.
double bootstrap_drawdown_p95(const std::vector<double>& returns, int sims, int block, unsigned seed);
// Trades needed for t = 3 at mean `mu` and standard deviation `sd` (in R).
double trades_needed(double mu, double sd, double t = 3.0);

struct GateLine {
    std::string name;
    double value = 0;
    std::string requirement;
    bool pass = false;
};

struct GateThresholds {
    double min_expectancy_1x = 0.15;  // R
    double min_expectancy_2x = 0.0;
    double min_deflated_sharpe = 0.95;
    double max_pbo = 0.20;
    double min_t = 3.0;
    double max_drawdown = 0.25;
    double max_year_share = 0.40;
};

struct GateReport {
    std::vector<GateLine> lines;
    bool pass() const;
    std::string text() const;
};

// Gate 1 for a swing sleeve: the base-cost run, a 2x-cost run of the same rules, and the
// trial statistics from the grid that produced them.
GateReport evaluate_gate1(const std::vector<Trade>& trades_1x, const EquityCurve& curve_1x,
                          const std::vector<Trade>& trades_2x, int n_trials, double var_trial_sharpe,
                          double pbo, const GateThresholds& th = {});

}  // namespace ta
