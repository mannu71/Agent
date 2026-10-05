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

// ---------------------------------------------------------------- trial log

// Every configuration run on a sleeve is a trial for the deflated Sharpe ratio and PBO,
// including ones that were looked at and discarded. The log is append-only; a trial is
// identified by a hash of its exact settings, so re-running the same configuration does
// not count twice.
struct TrialRecord {
    std::string sleeve;
    std::string id;     // settings hash
    std::string label;  // human-readable description
    EquityCurve curve;  // date, equity
};

// Appends the trial unless one with the same sleeve and id is already logged. Returns
// true if it was added.
bool log_trial(const std::string& path, const TrialRecord& t);
std::vector<TrialRecord> load_trials(const std::string& path, const std::string& sleeve);

struct TrialStats {
    int n = 0;             // trials on this sleeve
    double var_sharpe = 0; // variance of their non-annualised Sharpe ratios
    double pbo = 0;        // CSCV over the dates common to every trial (NaN if too few)
    std::size_t common_periods = 0;
};
TrialStats trial_stats(const std::vector<TrialRecord>& trials, int splits = 16);

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
