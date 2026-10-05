#pragma once

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "ta/market.hpp"
#include "ta/trade.hpp"

namespace ta {

// ---------------------------------------------------------------- screener scorecard

double spearman(const std::vector<double>& x, const std::vector<double>& y);

// date -> (symbol, score) for every eligible stock, as logged at that close.
using ScoreLog = std::map<std::string, std::vector<std::pair<std::string, double>>>;

// Forward return from the next session's open to the close `horizon` sessions after the
// signal date. NaN when the data does not reach that far (the outcome is not known yet).
double forward_return(const Series& s, std::size_t signal_idx, std::size_t horizon);

struct DailyScore {
    std::string date;
    double rank_ic = 0;        // Spearman(score, forward return) across the universe
    double precision_at_k = 0; // share of the top-k with a positive forward return
    double base_rate = 0;      // share of all scored stocks with a positive forward return
    int n = 0;
};

std::vector<DailyScore> score_outcomes(const ScoreLog& log, const MarketData& md, std::size_t horizon,
                                       std::size_t top_k);

double rolling_mean_ic(const std::vector<DailyScore>& s, std::size_t window);

// One-sided CUSUM for a drop in the mean of a series below `target`.
class Cusum {
public:
    Cusum(double target, double slack, double threshold) : target_(target), k_(slack), h_(threshold) {}
    bool update(double x);  // true once the alarm fires (latched)
    double value() const { return s_; }
    bool alarm() const { return alarm_; }

private:
    double target_, k_, h_;
    double s_ = 0;
    bool alarm_ = false;
};

// Population stability index between a reference and a recent sample (10 quantile bins of
// the reference). Alert above 0.1, act at 0.25.
double psi(const std::vector<double>& reference, const std::vector<double>& recent, int bins = 10);

// ---------------------------------------------------------------- kill / disable rules

enum class KillAction { None, Halve, Off };
const char* to_string(KillAction a);

struct KillDecision {
    KillAction action = KillAction::None;
    std::string reason;
};

// Mean R of the last n trades (NaN when fewer than n).
double rolling_expectancy(const std::vector<Trade>& trades, std::size_t n);

// A1: 50-trade expectancy < 0 -> halve; 100-trade < 0 -> off; realised slippage above
// 1.5x the model over 30 trades -> off.
KillDecision a1_kill_rule(const std::vector<Trade>& trades, double slippage_ratio_30 = 1.0);
// A2: 40-event expectancy < -0.1R -> off; approved-minus-rejected uplift <= 0 over 100
// events -> off; band-lock or unfilled-trigger rate above 20% -> review (reported as Halve).
KillDecision a2_kill_rule(const std::vector<Trade>& trades, double uplift_100, int events_seen,
                          double unfilled_rate);
// B: sleeve drawdown above 1.5x the backtest maximum -> off.
KillDecision b_kill_rule(double drawdown, double backtest_max_drawdown);
// D1: 60-day expectancy < 0 or realised cost above 1.5x the model -> off.
KillDecision d1_kill_rule(const std::vector<Trade>& trades, double cost_ratio = 1.0);
// Screener: 60-day rank IC < 0 together with a CUSUM alarm, or 120-day expectancy after
// costs < 0 -> that signal family is disabled.
KillDecision screener_rule(double ic_60, bool cusum_alarm, double expectancy_120);

}  // namespace ta
