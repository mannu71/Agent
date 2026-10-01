# Measuring, Validating and Maintaining the Accuracy of a Trading Screener / Signal Model (Intraday and Swing)

> Research notes, compiled 2026-10-01. Note on method: direct page fetches (arXiv, NBER, SSRN mirrors, AQR, Wikipedia, KIT, portfoliooptimizer.io) were blocked by the network egress proxy in this session, so most findings come from search-engine extracts of the cited pages, not from reading the full text. Numbers marked "(search extract)" should be spot-checked against the primary PDF before they are quoted as exact. All calculations I ran myself are labeled "my calculation" under Inferences.

## 1. What realistic directional accuracy / hit rates do the best published models achieve (stocks, crypto, India), and how much of the 70-90%+ claims come from leakage?

### Takeaway
Credible, leakage-controlled studies of daily or weekly direction get roughly 51-56% accuracy for large-cap equities and about 50-60% for crypto. That small edge is still worth a lot economically when it is applied across hundreds of names, but it decays over time and can be wiped out by costs. Papers that claim 75-90% next-day accuracy for stocks or indices are, almost without exception, methodological red flags: features that overlap the label, random (non-temporal) splits, or heavy specification search. Treat any screener that claims "80-90% accuracy" as broken until it is proven otherwise.

### Cited Findings
**US equities, benchmark deep-learning study (Fischer & Krauss 2018, EJOR):**
- LSTM networks predicted next-day out-of-sample direction (above or below the cross-sectional median) for S&P 500 constituents, 1992-2015. A daily long-short portfolio of the top and bottom k stocks earned **0.46% per day with a Sharpe ratio of 5.8 before transaction costs** — [Fischer & Krauss working paper, FAU](https://iwf.rw.fau.de/files/2015/12/11-2017.pdf) (search extract)
- The LSTM's classification accuracy was **"almost 55%"**, using 240 lagged daily returns as input (secondary summary) — [Palomar, Portfolio Optimization book, DL case studies](https://bookdown.org/palomar/portfoliooptimizationbook/16.4-case-studies-of-dl-portfolios.html); [Fischer & Krauss WP](https://iwf.rw.fau.de/files/2015/12/11-2017.pdf) (search extract)
- Outperformance was clear from 1992 to 2009, but **"as of 2010, excess returns seem to have been arbitraged away, with LSTM profitability fluctuating around zero after transaction costs."** With 5 bps costs the full-sample Sharpe fell to about 3.8 — [Fischer & Krauss WP](https://iwf.rw.fau.de/files/2015/12/11-2017.pdf) (search extract)
- A follow-up intraday study by Ghosh, Neufeld & Sahoo treats Fischer & Krauss as its benchmark. It reports that Fischer & Krauss's single-feature (close-to-close returns) setup earned 0.41%/day (LSTM) and 0.39%/day (random forest) — [arXiv 2004.10178](https://arxiv.org/pdf/2004.10178)

**US equities, cross-sectional return prediction (Gu, Kelly & Xiu 2020, RFS):**
- The data cover about 30,000 US stocks over 60 years (1957-2016) with more than 900 candidate predictors. Trees and neural networks did best, and their gains come from nonlinear interactions between predictors. The dominant signals are **momentum, liquidity and volatility** variants — [NBER w25398](https://nber.org/papers/w25398); [Chicago Booth Review](https://www.chicagobooth.edu/review/machine-learning-can-help-money-managers-time-markets-build-portfolios)
- A value-weighted long-short decile portfolio sorted on neural-network forecasts earned an **annualized out-of-sample Sharpe ratio of 1.35** — [NBER rev1 PDF](https://www.nber.org/system/files/working_papers/w25398/revisions/w25398.rev1.pdf) (search extract)
- **Conflict or uncertainty on R²:** one search extract of a working-paper version gives monthly out-of-sample R² of "1.08% to 1.80%" for trees and NNs — [NBER rev1 PDF](https://www.nber.org/system/files/working_papers/w25398/revisions/w25398.rev1.pdf). The figure usually quoted for the published RFS version is about **0.40% per month for NN3**, but I could not retrieve that table to confirm it ([RFS PDF](https://academic.oup.com/rfs/article-pdf/33/5/2223/33209812/hhaa009.pdf), not fetched). Either way, monthly stock-level R² is below 2%.

**Chinese A-shares, qlib/Alpha158 LightGBM benchmarks (daily IC, CSI300):**
- Reported ICs vary a lot from paper to paper for the "same" baseline:
  - IC 0.0158 / Rank IC 0.0235 — [Alpha² arXiv 2406.16505](https://arxiv.org/pdf/2406.16505)
  - IC 0.0259 / Rank IC 0.0324 — [arXiv 2306.12964](https://arxiv.org/pdf/2306.12964)
  - IC 0.0269 / Rank IC 0.0412 / ICIR 0.28 / Rank ICIR 0.33 — [arXiv 2511.18850](https://arxiv.org/pdf/2511.18850)
  - Alpha158 IC 3.91% / ICIR 25.8% / RankIC 5.77% / RankICIR 37.6% — [AlphaPROBE arXiv 2602.11917](https://arxiv.org/pdf/2602.11917)
- One 2026 paper reports IC 0.063 and Rank IC 0.176 but ICIR 0.029. That combination is internally implausible and may be a mis-extraction — [arXiv 2608.28632](https://arxiv.org/pdf/2608.28632) (search extract)

**Crypto:**
- Akyildirim, Goncu & Sensoy (Annals of Operations Research, 2021) studied the 12 most liquid cryptocurrencies at daily and minute frequencies using SVM, logistic regression, ANN and random forest. They report about **55-65% average classification accuracy**, with SVM the most consistent — [Bilkent repository](https://repository.bilkent.edu.tr/items/8f518a9d-24b8-4b2a-a6ea-f17847399ccd/full); [ETH Research Collection](https://www.research-collection.ethz.ch/handle/20.500.11850/411171?show=full)
- A literature summary in a Bitcoin efficient-market-hypothesis paper states that studies "yield 50% to 60% accuracy in classifying Bitcoin returns" — [arXiv 2208.07254](https://arxiv.org/pdf/2208.07254) (search extract)
- Jaquart, Dann & Weinhardt (J. Finance & Data Science, 2021) tested horizons of 1-60 minutes. GRU/LSTM and gradient-boosting models worked best. The exact accuracy figures were not retrievable here — [KIT repository](https://publikationen.bibliothek.kit.edu/1000150665); [JFDS award note](https://www.keaipublishing.com/en/journals/the-journal-of-finance-and-data-science/jfds-events/jfds-awards-2022-announced/)

**Indian market, a red-flag example:**
- A NIFTY 50 study (50 stocks, 2013-2018) reports trend-prediction accuracy of **87.35% (SVM), 86.98% (logistic regression) and 75.88% (perceptron)**, rising to **89.93%** after "reorganizing into supervised learning format" — [Central University of Punjab repository](https://kr.cup.edu.in/handle/32116/2699) (search extract)
- Patel, Shah, Thakkar & Kotecha (2015, Expert Systems with Applications) covered CNX Nifty, BSE Sensex, Reliance and Infosys, 2003-2012. Converting 10 technical indicators into "trend deterministic" (+1/-1) inputs "improved" every model — [Nirma University repository](https://repository.nirmauni.ac.in/jspui/handle/123456789/5669)

**Leakage and spurious predictability:**
- Nikolopoulos (2026), "Spurious Predictability in Financial Machine Learning": adaptive specification search and ordinary workflow tuning produce statistically significant backtests **even in synthetic zero-predictability environments**. The paper's case studies suggest "many published predictability claims are methodological artifacts." It proposes a falsification audit using zero-predictability and microstructure placebos — [arXiv 2604.15531](https://arxiv.org/pdf/2604.15531); [summary](https://letsdatascience.com/news/study-reveals-spurious-predictability-in-financial-machine-l-bf0a39e7)
- A 2026 Springer review of ML stock forecasting flags information leakage and look-ahead effects as recurring problems that "inflate apparent predictive performance." It recommends reading gains alongside validation quality, baseline strength, leakage controls and friction assumptions — [Springer, Discover Computing 2026](https://link.springer.com/article/10.1007/s10791-026-10491-5)
- Cross-domain (non-finance) evidence: leakage in preprocessing inflated accuracy by roughly **6-24%** — [ResearchGate multi-domain study](https://www.researchgate.net/publication/382212868_Implications_of_Data_Leakage_in_Machine_Learning_Preprocessing_A_Multi-Domain_Investigation) (search extract)
- "Limits to (Machine) Learning" (2025/26) argues the reverse effect also exists. In high dimensions, finite samples keep even the best ML estimators from recovering the true signal, so honest out-of-sample metrics can *understate* population predictability — [arXiv 2512.12735](https://arxiv.org/pdf/2512.12735)

### Inferences
- **Realistic targets for a screener:**
  - Next-day or next-week direction on liquid stocks: about 51-55% hit rate, measured against the base rate.
  - Crypto: about 50-58%.
  - Above about 60% sustained out-of-sample at daily or weekly horizons on liquid instruments, suspect leakage first.
  - Above 70% almost certainly means leakage, overlapping labels, a mislabeled target (for example, predicting "trend" defined with the same window as the inputs), or a random train/test split.
- The NIFTY "87-90%" results fit the leakage pattern: accuracy *jumps* when data are re-arranged or indicators are discretized into the trend they help define. This matches the mechanisms Nikolopoulos and the Springer review describe. I have not verified each paper's code, so this is an inference.
- Fischer & Krauss show the pattern a screener should expect: a real edge of about 55% accuracy can be very profitable before costs, and still fall to roughly zero after costs once it is widely known (post-2010).
- **My calculation, linking IC to hit rate:** under bivariate normality, the probability that the forecast and the realized return have the same sign is 0.5 + arcsin(IC)/π.
  - IC 0.02 → 50.6%
  - IC 0.05 → 51.6%
  - IC 0.10 → 53.2%
  - IC 0.20 → 56.4%
  - So a "good" equity IC of 0.03-0.05 corresponds to only 51-52% raw sign accuracy across the whole universe. Higher hit rates appear only in the extreme deciles, and only if the signal is monotonic.

### Gaps
- Could not confirm the RFS-published Gu-Kelly-Xiu Table 1 R² values (0.40% NN3, etc.) or the equal-weighted long-short Sharpe (often quoted as about 2.4). The arXiv paper "A Quadratic Link between Out-of-Sample R² and Directional Accuracy" ([arXiv 2602.07841](https://arxiv.org/html/2602.07841v1)) is directly relevant, but its content could not be retrieved.
- Could not retrieve exact accuracy numbers for Jaquart et al. (2021), or the headline accuracy from Patel et al. (2015).
- I found no systematic meta-study that counts what fraction of published 70-90% accuracy claims are due to leakage. The statement "most are leakage" rests on mechanism-level evidence (Nikolopoulos 2026; the Springer review), not a census.
- No high-quality intraday (5-60 min) equity direction benchmark for Indian or US stocks was retrieved.

## 2. Why hit rate alone is misleading: expectancy, profit factor, payoff ratio, Sharpe, drawdown, low-win-rate trend systems and high-win-rate option selling

### Takeaway
A screener's "accuracy" has to be judged by the *size* of its wins and losses, not only how often it is right. Trend systems with 30-45% win rates make money because their winners are 2-3x their losers. Option-selling programs with win rates of 75-90%+ have been wiped out by a single tail event. Report expectancy (in R-multiples or %), profit factor, payoff ratio, Sharpe/Sortino and max drawdown next to the hit rate, after costs.

### Cited Findings
- Trend-following CTAs typically win on **about 35-40% of trades**, with **average winners 2-3x average losers**. The return stream is positively skewed, like a long-option position — [The Science and Practice of Trend-Following Systems, arXiv 2607.19497](https://arxiv.org/pdf/2607.19497) (search extract)
- Trend following typically has 30-45% winners. A few trades that run produce 3R-10R+, while most trades are small losses — [TurtleTrader, "Winning Percentage Means Nothing"](https://www.turtletrader.com/babe-ruth) (search extract)
- **OptionSellers.com (Tampa CTA):**
  - Managed about $150M for 290 clients.
  - On 15 Nov 2018 it told investors all their money was lost and they would likely owe more.
  - The strategy's win rate was "often north of 75% on a monthly basis" — [Early Retirement Now](https://earlyretirementnow.com/2018/12/18/the-optionsellers-debacle/); [The Short Bear](https://theshortbear.substack.com/p/blowing-up-selling-options)
- "High kurtosis and negative skew are the defining characteristics of option selling programs" — [RCM Alternatives via HedgeFundAlpha, "LJM: the autopsy"](https://hedgefundalpha.com/news/ljm-the-autopsy) (search extract)
- Payoff ratio is average win divided by average loss. It has to be read together with win rate — [TradesViz glossary](https://www.tradesviz.com/glossary/payoff-ratio/); [LuxAlgo win-rate concept](https://www.luxalgo.com/library/concept/win-rate/)

### Inferences
**Formulas** (standard definitions):
- **Expectancy per trade** = W × AvgWin − (1−W) × AvgLoss, where W is the win rate. In R-multiples: E[R] = W × avgWinR − (1−W) × avgLossR.
- **Payoff ratio** b = AvgWin / AvgLoss.
- **Breakeven win rate** = 1 / (1 + b).
- **Profit factor** = gross profit / gross loss = W × AvgWin / ((1−W) × AvgLoss).
- **Kelly fraction** (for sizing context) f* = W − (1−W)/b.
- **Sharpe** = mean(excess return)/stdev × √(periods per year).
- **Sortino** uses downside deviation in place of the standard deviation.
- **Max drawdown** = max over t of (peak-to-date − value_t)/peak-to-date.
- **Calmar** = CAGR / |MaxDD|.

**Worked examples (my calculation):**

| Profile | Win rate | Avg win | Avg loss | Expectancy | Profit factor | Verdict |
|---|---|---|---|---|---|---|
| Trend system | 35% | 3R | 1R | **+0.40R/trade** | 1.62 | Profitable |
| "Accurate" screener | 55% | 1.0 | 1.3 | **−0.035 per trade** | 0.94 | Losing |
| Option seller | 90% | 1 | 12 | **−0.30 per trade** | 0.75 | Losing |

- Breakeven win rates: 33% at payoff 2; 50% at payoff 1; 67% at payoff 0.5.

**Recommended screener scorecard**, reported per horizon (intraday, swing) and per regime:
- Hit rate versus the base rate (the share of all candidate stocks that rose over the same horizon)
- Average win and average loss in % and in R (R = distance to stop at entry)
- Expectancy after costs and slippage
- Profit factor (more than about 1.3 after costs is a common practitioner bar; this is not from a cited source)
- Sharpe and Sortino of the daily P&L of "trade every suggestion"
- Max drawdown and longest losing streak
- Skew and kurtosis of trade returns, to catch option-seller-like tail profiles

**Hit rate against base rate matters most for long-only screeners.** In a bull market, 55-60% of stocks may rise over a 5-day window anyway. "60% of picks went up" then shows no skill. Report excess hit rate (pick hit rate minus universe hit rate), or the hit rate of beating the benchmark or sector.

### Gaps
- I did not retrieve LJM Partners' specific loss figures (February 2018). The hedgefundalpha autopsy is listed but its numbers were not extracted.
- I found no authoritative published threshold for an "acceptable" profit factor or expectancy for retail swing systems. Those thresholds are practitioner conventions.

## 3. Ranking-quality metrics for screeners: IC/Rank IC, ICIR, precision@k and decile spreads, hit rate vs base rate, calibration, and the fundamental law

### Takeaway
A screener is a ranking model, so its main quality metrics are cross-sectional:
- Daily or weekly **Rank IC** (Spearman correlation between score and forward return), plus its stability (**ICIR** = mean IC / std IC).
- **Top-minus-bottom quantile spread** and precision@k of the names actually shown to the user.
- **Calibration** of any stated probability ("70% chance up").

Realistic Rank IC for daily or weekly equity signals is about 0.02-0.06. The fundamental law, IR ≈ IC × √breadth, explains why such small ICs still work, but only with many independent bets.

### Cited Findings
- Alphalens defines IC as the **Spearman rank correlation between factor values and forward returns**. It sorts the universe into quantiles and reports returns, IC, turnover and sector breakdowns per quantile — [QuantRocket Alphalens lecture](https://www.quantrocket.com/codeload/quant-finance-lectures/quant_finance_lectures/Lecture38-Factor-Analysis-with-Alphalens.ipynb.html); [Zipline-trader Alphalens docs](https://zipline-trader.readthedocs.io/en/latest/notebooks/Alphalens.html)
- A common practitioner benchmark treats **mean IC > 0.02** as meaningful. A stable IC over time is as important as its level — [sharpely, Decoding alpha analysis report](https://sharpely.in/knowledge-base/alphalab/decoding-alpha-analysis-report); [QuantRocket lecture](https://www.quantrocket.com/codeload/quant-finance-lectures/quant_finance_lectures/Lecture38-Factor-Analysis-with-Alphalens.ipynb.html) (search extract)
- Published daily CSI300 LightGBM/Alpha158 benchmarks report IC of about 0.016-0.039, Rank IC of about 0.024-0.058, and ICIR of about 0.26-0.37 (see Section 1) — [arXiv 2406.16505](https://arxiv.org/pdf/2406.16505); [arXiv 2306.12964](https://arxiv.org/pdf/2306.12964); [arXiv 2511.18850](https://arxiv.org/pdf/2511.18850); [arXiv 2602.11917](https://arxiv.org/pdf/2602.11917)
- **Fundamental law of active management (Grinold & Kahn):** IR = IC × √BR. IC is the correlation between forecast and realized returns, ranging from −1 to 1. BR (breadth) is the number of *independent* bets per year — [CFI](https://corporatefinanceinstitute.com/resources/capital-markets/fundamental-law-of-active-management/); [AnalystPrep](https://analystprep.com/study-notes/frm/part-2/risk-management-and-investment-management/alpha-and-the-low-risk-anatomy/)
- Gu-Kelly-Xiu's neural-network decile long-short has a value-weighted Sharpe of 1.35 despite a very small R² — [NBER rev1](https://www.nber.org/system/files/working_papers/w25398/revisions/w25398.rev1.pdf)

### Inferences
**Metric definitions to implement:**

*IC and rank IC*
- IC_t = Pearson(score_i,t, fwd_ret_i,t→t+h) across stocks i on date t.
- Rank IC_t = Spearman of the same pair.
- Report mean, standard deviation, ICIR = mean/std (annualized: × √(periods per year)), the t-stat = mean/(std/√T), and the % of periods with IC > 0.
- With overlapping horizons (for example, a 5-day forward return computed daily), use Newey-West or non-overlapping samples for the t-stat.

*Quantile spread*
- Average forward return of the top decile or quintile minus the bottom one, per period.
- Check that returns rise monotonically across quantiles.
- Report the turnover of the top bucket, because cost drag scales with turnover.

*Precision@k (screener-specific)*
- Of the k names shown each day, the fraction that beat the universe median or the benchmark over horizon h.
- Also track the mean excess return of the top k.
- Compare with the base rate: the same metric for k random names from the eligible universe.
- This is the metric closest to what a user experiences.

*Calibration (if the screener shows probabilities)*
- Brier score = (1/N) Σ (p_i − o_i)², where o_i ∈ {0,1}. A climatological baseline always predicts the base rate. Brier skill score = 1 − Brier/Brier_baseline.
- Reliability diagram: bin predictions (for example 50-55%, 55-60%), then plot mean predicted against observed frequency. Also compute expected calibration error (ECE).
- A screener saying "80% probability" that realizes 54% is miscalibrated even if its ranking is useful. Fix it with isotonic or Platt recalibration on walk-forward folds.

**Fundamental law examples (my calculation):**

| IC | Breadth (independent bets/yr) | IR |
|---|---|---|
| 0.02 | 6,000 (e.g., 500 stocks × 12 rebalances) | ≈ 1.55 |
| 0.05 | 1,200 | ≈ 1.73 |
| 0.05 | 50 (a concentrated swing trader) | ≈ 0.35 |

- Real breadth is much smaller than the nominal count, because picks are correlated (same sector, same market factor) and holding periods overlap. So the realized IR will fall short of this formula.
- **Implication for a retail screener showing about 5-10 picks a week:** low breadth means even a genuinely good IC gives a noisy, modest IR. Expect long losing stretches.

### Gaps
- I could not retrieve the canonical Grinold & Kahn statement of "typical" ICs (often quoted as 0.05 = good, 0.10 = excellent) from a primary source. The 0.02 threshold comes from practitioner documentation.
- I could not fetch the qlib documentation directly. The qlib IC numbers come from third-party papers that use qlib, and they vary with configuration and dates.

## 4. Validation methodology: walk-forward, purged and embargoed CV, out-of-time holdout, forward-test length, minimum track record length

### Takeaway
Use strictly time-ordered validation:
- Walk-forward / rolling-origin evaluation, or purged and embargoed k-fold / combinatorial purged CV (CPCV) for model selection.
- An untouched out-of-time holdout.
- A *pre-registered*, timestamped forward (paper) test.

The statistics require far more trades than most users expect:
- Distinguishing a 55% hit rate from 50% with 80% power needs about **620 independent trades** (one-sided), or about 780 (two-sided).
- Distinguishing 53% from 50% needs about **1,700-2,200**.
- A strategy with an annualized Sharpe of 1.0 needs about **2.7-3 years** of daily data before it is significant at 95% (MinTRL). Negative skew and fat tails lengthen that.

### Cited Findings
- **Purged CV (López de Prado, 2017):**
  - Purging removes from the training set any observation whose *label window* overlaps a test observation's label window.
  - An **embargo** also drops a buffer of training data immediately after each test block, to absorb serial correlation that outlives the labels.
  - Standard k-fold and plain walk-forward "often yield overly optimistic performance estimates due to information leakage and overfitting" — [Wikipedia: Purged cross-validation](https://en.wikipedia.org/wiki/Purged_cross-validation) (search extract); [LuxAlgo concept page](https://www.luxalgo.com/library/concept/purged-cross-validation/)
- **CPCV:**
  - Splits the data into N contiguous groups and holds out every combination of k groups as test, with purging and embargo.
  - Produces many out-of-sample backtest *paths*, so the result is a distribution of OOS Sharpe ratios rather than one number.
  - That distribution allows an estimate of the **probability of backtest overfitting (PBO)** — [Wikipedia](https://en.wikipedia.org/wiki/Purged_cross-validation); [fynance CPCV docs](https://fynance.readthedocs.io/en/latest/generated/fynance.data.combinatorial_purged_cv.html)
- **Backtest overfitting (Bailey, Borwein, López de Prado & Zhu):**
  - After trying only **7 strategy configurations**, a researcher should expect to find at least one 2-year backtest with an annualized **Sharpe above 1 when the true out-of-sample Sharpe is 0**.
  - They propose a **Minimum Backtest Length (MinBTL)** that rises with the number of trials, and the CSCV method for estimating PBO — [Bailey et al., "Pseudo-Mathematics and Financial Charlatanism"](https://carmamaths.org/jon/backtest.pdf); [WMich ScholarWorks](https://scholarworks.wmich.edu/math_pubs/40/); [Advisor Perspectives](https://www.advisorperspectives.com/articles/2013/10/22/how-many-monkeys-does-it-take-to-find-a-successful-strategy)
- **Probabilistic Sharpe Ratio and Minimum Track Record Length (Bailey & López de Prado 2012):** PSR is the probability that the true Sharpe exceeds a benchmark SR* given sample length, skewness and kurtosis. The MinTRL formula is:
  - MinTRL = 1 + [1 − γ₃·SR + ((γ₄ − 1)/4)·SR²] × (Z_α / (SR − SR*))²
  - SR is the per-period (non-annualized) observed Sharpe, γ₃ is skewness and γ₄ is raw kurtosis (3 for normal).
  - Equivalently 1 + [1 − γ₃SR + ½SR² + (γ₄−3)/4 · SR²] × …
  - Lower skewness or higher kurtosis increases MinTRL — [portfoliooptimizer.io](https://portfoliooptimizer.io/blog/the-probabilistic-sharpe-ratio-bias-adjustment-confidence-intervals-hypothesis-testing-and-minimum-track-record-length/); [PerformanceAnalytics MinTrackRecord](https://www.quantargo.com/help/r/latest/packages/PerformanceAnalytics/NEWS/MinTrackRecord); [Journal of Risk / RePEc](https://ideas.repec.org/a/rsk/journ4/2223785.html); [Trading Strategy glossary](https://tradingstrategy.ai/glossary/probabilistic-sharpe-ratio)
- **Multiple testing:** Harvey, Liu & Zhu (2016, RFS) argue that because the literature has tested hundreds of factors, a new factor should clear **t > 3.0** rather than 2.0, and that "most claimed research findings in financial economics are likely false" — [NBER w20592](https://nber.org/papers/w20592); [SSRN 2513152](https://papers.ssrn.com/abstract=2513152)

### Inferences
**Recommended protocol for the screener:**

1. **Data split.**
   - Development window: walk-forward with an expanding or rolling train window, for example 3-5 years train, then 1-3 months test, rolled forward.
   - Purge gap = label horizon h (1 day intraday-to-close, 5-20 days for swing), plus an embargo of about 1-2% of the sample or at least h bars.
   - Hold out the final 12-24 months as a *single-use* out-of-time test.
2. **Model selection.** Use CPCV or walk-forward folds. Record the **number of configurations tried (N_trials)**. Report the Deflated Sharpe or PBO, not the best backtest.
3. **Forward or paper test.** Freeze the model and the rules, and log each suggestion with its timestamp *before* the outcome is known (see Section 5). Run until the required sample size is reached, not until it "looks good" (no optional stopping).

**Sample sizes for hit-rate tests (my calculation)**, using the normal-approximation binomial test against p₀ = 0.5 with α = 0.05:

| True hit rate | One-sided, 80% power | Two-sided, 80% power | One-sided, 90% power | Just to make an *observed* rate significant (one-sided, no power) |
|---|---|---|---|---|
| 52% | 3,863 | 4,904 | 5,349 | 1,692 |
| 53% | 1,716 | 2,178 | 2,376 | 752 |
| 55% | 617 | 783 | 853 | 271 |
| 60% | 153 | 194 | 211 | 68 |

- Formula: n = [(z_α·√(p₀q₀) + z_β·√(p₁q₁)) / (p₁ − p₀)]².
- These counts assume *independent* trades. Picks made on the same day are correlated, because they share market and sector moves. The effective n is closer to the number of distinct days or periods, so swing screeners need *many months to years* of forward testing.
- At a 55% observed hit rate, the 95% confidence-interval half-width is about ±13.8 pp at n=50, ±9.8 pp at n=100, ±6.2 pp at n=250, ±4.4 pp at n=500 and ±3.1 pp at n=1,000. **After 100 trades you cannot tell a 55% screener from a coin flip.**

**MinTRL at 95% confidence against SR* = 0 (my calculation):**

| Annualized Sharpe | Daily data, normal returns | Daily data, skew −1 / kurtosis 10 | Monthly data, normal | Monthly data, skew −1 / kurtosis 10 |
|---|---|---|---|---|
| 0.5 | about 10.8 years | about 11.2 years | about 11 years | about 13 years |
| 1.0 | about 2.7 years | about 2.9 years | about 2.9 years | about 4.1 years |
| 1.5 | about 1.2 years | — | about 1.4 years | — |
| 2.0 | about 0.7 years | — | about 0.9 years | about 1.7 years |

- With 7+ configurations tried, raise the bar: test against the expected maximum Sharpe under the null (Deflated Sharpe), or require t > 3.

**Intraday screeners** get breadth faster: many independent days, and possibly several signals per day. But microstructure costs (spread, slippage, opening auction) must be included, or the measured edge is fictitious.

### Gaps
- The exact Deflated Sharpe Ratio formula (expected maximum Sharpe given N trials, using the Euler-Mascheroni approximation) was not retrieved from a primary source in this session.
- I found no published consensus on embargo size beyond López de Prado's practical suggestion (commonly cited as about 1% of observations). I could not verify that number directly because Wikipedia and arXiv were blocked.

## 5. Maintaining accuracy: alpha decay, post-publication decay, drift detection, retraining, champion-challenger, auto-disabling, and logging every suggestion

### Takeaway
Edges decay, and that should be expected and planned for:
- Published anomalies lose about 26% of their return out-of-sample and **about 58% after publication**.
- Industry estimates put the cost of alpha decay at about **5.6%/yr in the US and about 10%/yr in Europe**, and rising.
- Up to 65-82% of anomalies fail strict replication.

Maintaining accuracy is therefore an operating process:
- Log every suggestion immutably at decision time.
- Monitor rolling Rank IC, top-k excess return and calibration.
- Detect feature and score drift (PSI, KS) and performance change points (CUSUM, Page-Hinkley).
- Retrain on a schedule plus on triggers. Promote new models only through champion-challenger shadow testing.
- Automatically disable or down-weight signals whose rolling metrics breach preset thresholds.

### Cited Findings
- **McLean & Pontiff (2016, JF)** studied 97 cross-sectional predictors. Portfolio returns are **26% lower out-of-sample and 58% lower post-publication**, which implies about 32% is due to publication-informed trading. Declines are larger for predictors with higher in-sample returns. Returns are higher in high-idiosyncratic-risk, low-liquidity stocks, consistent with limits to arbitrage — [Gwern-hosted PDF](https://www.gwern.net/doc/economics/2016-mclean.pdf); [Ivey/Pontiff PDF](https://ivey.uwo.ca/media/3775549/pontiff.pdf); [CXO Advisory](https://www.cxoadvisory.com/big-ideas/effects-of-market-adaptation)
- **Hou, Xue & Zhang (2020, RFS),** with microcaps controlled (NYSE breakpoints, value weighting): **65% of 452 anomalies** fail |t| ≥ 1.96, and **82.1%** fail a multiple-testing hurdle of 2.78 — [SSRN](https://papers.ssrn.com/sol3/papers.cfm?abstract_id=3275496); [global-q.org PDF](https://global-q.org/uploads/1/2/2/6/122679606/houxuezhang2020rfs.pdf)
- **Counterpoint, Jensen, Kelly & Pedersen (2023, JF):** with a Bayesian approach and CAPM-alpha tests, **82.4% of factors replicate** — [Wiley JF](https://onlinelibrary.wiley.com/doi/full/10.1111/jofi.13249); [NBER w28432](https://www.nber.org/system/files/working_papers/w28432/w28432.pdf)
- **Maven Securities** estimates alpha decay costs about **5.6% annually in US markets and about 9.9-10% in Europe**, rising by about 36 bps/yr in the US and 16 bps/yr in Europe — [Maven Securities](https://www.mavensecurities.com/alpha-decay-what-does-it-look-like-and-what-does-it-mean-for-systematic-traders/) (search extract)
- The Fischer-Krauss LSTM edge was "arbitraged away" after 2010 (see Section 1) — [FAU WP](https://iwf.rw.fau.de/files/2015/12/11-2017.pdf)
- **PSI drift thresholds:** PSI < 0.1 means low drift, 0.1-0.25 minor drift, and ≥ 0.25 significant drift. The recommendation is to **retrain when PSI ≥ 0.25** — [Qlik AutoML docs](https://help.qlik.com/en-US/cloud-services/Subsystems/Hub/Content/Sense_Hub/AutoML/monitor-drift.htm)
- **KS test:** maximum distance between two empirical CDFs. It suits continuous features and is sensitive to tail shifts — [MLflow drift guide](https://mlflow.org/articles/tags/detecting-model-drift/) (search extract)
- **Page-Hinkley:** non-parametric, low memory, suited to streams, robust to outliers, and detects gradual changes in a monitored performance metric — [Menelaus change-detection docs](https://menelaus.readthedocs.io/en/latest/menelaus.change_detection.html) (search extract)
- **CUSUM:** a statistical-process-control test for mean shifts. It can track a model performance metric or a feature mean. Agreement across Page-Hinkley, CUSUM, windowed KS and DDM strengthens confidence that a change is real — [Menelaus docs](https://menelaus.readthedocs.io/en/latest/menelaus.change_detection.html); [giobbu/CUSUM](https://github.com/giobbu/CUSUM); [MetricGate concept-drift docs](https://metricgate.com/docs/concept-drift-detection)
- **LLM-driven alpha mining:** work such as AlphaAgent explicitly targets "alpha decay" with regularized exploration, which shows that decay is treated as a first-order design constraint in current (2025-26) quant ML research — [AlphaAgent arXiv 2502.16789](https://arxiv.org/pdf/2502.16789)

### Inferences
**Monitoring design for the screener** (synthesized from the sources above; the thresholds are suggested starting points, not literature-mandated values):

1. **Immutable suggestion log.** For each pick, record:
   - `suggestion_id`
   - `model_version` and `feature_snapshot_hash`
   - `generated_at` (UTC and exchange-local time)
   - `data_asof` (the timestamp of the latest bar or fundamental used)
   - symbol, side, score or rank, stated probability
   - intended entry rule (for example, next open or limit price), stop, target, horizon
   - universe size and base rate at that moment

   Write it append-only (hash-chained or write-once storage), *before* the outcome. Score outcomes later by a separate job using the pre-declared entry rule. This prevents hindsight edits and survivorship of "good-looking" picks.
2. **Rolling metrics** (for example, 20/60/120 trading-day windows):
   - Rank IC and ICIR
   - Top-k excess return versus the universe
   - Precision@k versus the base rate
   - Expectancy after costs
   - Brier score and ECE
   - Turnover
   - Live slippage versus modeled slippage
3. **Drift detectors:**
   - PSI or KS on each input feature and on the score distribution, weekly. Alert at PSI > 0.1, act at > 0.25.
   - CUSUM or Page-Hinkley on daily Rank IC or on the daily top-k excess return.
4. **Auto-disable or down-weight rules.** Examples:
   - Disable a signal or strategy family if its 60-day rolling Rank IC < 0 *and* its CUSUM alarm has fired.
   - Disable if the 120-day expectancy after costs is < 0.
   - Disable if drawdown exceeds 1.5-2x the max drawdown seen in backtest. A live drawdown much deeper than backtest is evidence of decay or of leakage in the backtest.
   - Re-enable only after it passes on a fresh forward window.
5. **Retraining:**
   - Scheduled: for example monthly for swing, weekly for intraday, on a rolling or expanding window.
   - Triggered: on a drift alarm.
   - Retrained models enter as **challengers**, run in shadow (logged, not shown) for a minimum window, and replace the **champion** only if their forward Rank IC or expectancy is better by a pre-set margin with a significance test.
6. **Expect decay.** Plan the system's economics assuming the backtest edge shrinks by about one-third to two-thirds in live trading, in line with the McLean-Pontiff out-of-sample and post-publication declines.
7. **Signals to treat with extra suspicion:** those built on widely published anomalies (momentum, RSI/MACD crossovers, 52-week highs). Their decay is likely furthest along, and per McLean-Pontiff, the high-in-sample ones decay the most.

### Gaps
- I found no peer-reviewed standard for retraining frequency or auto-disable thresholds specific to trading screeners. The thresholds above are synthesized engineering defaults.
- I found no primary-source documentation (retrievable here) of qlib's rolling-retraining / online-serving module. It is believed to exist (qlib "online serving" / rolling tasks), but this is unverified in this session.
- I found no high-quality study quantifying alpha half-life for retail technical screeners specifically (intraday or swing). The Maven numbers are for institutional systematic strategies.

## 6. Common leakage and look-ahead traps specific to screeners

### Takeaway
Most "too good to be true" screener accuracy comes from a short list of traps:
- Using the bar's close, high or low before the bar has closed.
- Unadjusted or retroactively adjusted corporate-action data.
- Survivorship-biased universes, which drop delisted names.
- Non-point-in-time fundamentals (restated values, or values keyed to period end instead of filing date).
- Full-sample normalization or feature selection.
- Overlapping labels in random CV.
- Timezone or day-boundary mismatches, especially in 24/7 crypto.

Subtle forms of these are often claimed to add a few hundred bps a year or 0.5-1.5 Sharpe to backtests. Those magnitudes come mostly from secondary sources.

### Cited Findings
- **Subtle leakage remains common even when gross look-ahead is rare.** Examples are normalization leakage (scaling features with full-sample statistics rather than an expanding window), survivorship bias and cross-validation contamination — [Nikolopoulos, arXiv 2604.15531](https://arxiv.org/pdf/2604.15531) (search extract)
- Look-ahead bias means using information unavailable at decision time. It produces inflated metrics that "evaporate in real-world deployment" — [arXiv 2601.13770, standardized benchmark of look-ahead bias in point-in-time models](https://arxiv.org/pdf/2601.13770) (search extract; title only partially retrieved)
- **Survivorship bias:**
  - Excluding defunct stocks can overstate annual returns by **about 1-4%** and distort Sharpe and drawdown — [LuxAlgo blog](https://luxalgo.com/blog/survivorship-bias-in-backtesting-explained) (secondary, lower-quality source)
  - Brown, Goetzmann, Ibbotson & Ross found survivorship could inflate Sharpe ratios by up to about 0.5 — [LuxAlgo blog](https://luxalgo.com/blog/survivorship-bias-in-backtesting-explained) (secondary citation of the academic paper)
  - Another practitioner source claims dropping delisted, bankrupt and merged names overstates a long-equity Sharpe by about 0.5-1.5 — [techinterview.org](https://www.techinterview.org/post/3233477314/why-backtest-sharpe-collapses-live/?format=md) (low-quality source; treat as anecdotal)
- **Subtle leakage magnitudes:** using adjusted prices before the adjustment date, using macro data before release, or filtering the universe on future information "can inflate backtest returns by 200-500 bps per year" — [techinterview.org](https://www.techinterview.org/post/3233477314/why-backtest-sharpe-collapses-live/?format=md); [dev.to, survivorship vs lookahead](https://dev.to/tradevodata/survivorship-bias-vs-lookahead-bias-the-two-silent-backtest-killers-pmm) (practitioner sources, not peer-reviewed)
- **Point-in-time fundamentals:** the root cause is data keyed to fiscal period end rather than the filing or disclosure date, plus restatements (original versus latest values). Every value must be what an investor actually had at decision time — [FlashAlpha, point-in-time data](https://flashalpha.com/articles/point-in-time-options-data-backtest-integrity); [dev.to](https://dev.to/tradevodata/survivorship-bias-vs-lookahead-bias-the-two-silent-backtest-killers-pmm)
- **Crypto survivorship:** dead or delisted coins and exchanges disappear from many datasets — [CoinAPI glossary](https://www.coinapi.io/learn/glossary/survivorship-bias)
- **Multiple testing and specification search as leakage:** Harvey-Liu-Zhu (t > 3) and Bailey et al. (7 trials → spurious SR > 1). See Section 4.

### Inferences
**Screener-specific leakage checklist** (synthesized; each item is a known mechanism, magnitudes vary):

1. **Close-before-close.** A daily screener run "at the close" that uses that day's close, high, low or volume as a feature, and assumes entry at that same close.
   - Fix: compute features from bar t−1 and enter at the open of t+1. Or, for intraday, snapshot at time τ using only bars that are complete by τ, and enter at the next tick or bar plus slippage.
   - Also treat stop or target fills inside a single daily bar conservatively: if both high and low could have been hit, assume the stop hit first.
2. **Labels that overlap features.** Example: a "trend up" label defined with a moving average that includes future bars, or a target over [t, t+h] where features use t+1.
   - This is the most likely source of the 85-90% "accuracy" in some academic papers.
3. **Random k-fold on time series, or overlapping forward-return labels without purging.** Neighboring days share most of their 5-20-day forward return, so random splits leak. Use purged and embargoed CV (Section 4).
4. **Full-sample preprocessing.** Scalers, PCA, feature selection, winsorization limits and hyperparameter search fitted on all the data. Fit them inside each training fold only.
5. **Corporate actions.**
   - Back-adjusted prices give a correct return series, but *price-level* features (for example "price < ₹100", round-number breakouts, 52-week highs computed on adjusted data) and volume become look-ahead or distorted.
   - Unadjusted data creates fake gaps on splits or bonuses.
   - Use returns from adjusted series, price-level filters from as-traded series, and an adjustment table that applies only actions known as of t.
   - In India this especially concerns bonus issues, splits and the timing of record versus ex-dates.
6. **Survivorship and universe look-ahead.**
   - Use the index membership or listed universe *as of each date*, including delisted and suspended names, with delisting returns.
   - Do not screen "current NIFTY 500 members" back through 2015.
7. **Point-in-time fundamentals and events.**
   - Key every value to its filing or announcement timestamp, plus a lag for after-hours releases. Results released after market close are tradable only the next session.
   - Use as-first-reported values, not restated ones.
   - Analyst estimates, ratings and news must carry their publication timestamps.
8. **Timezones and day boundaries.**
   - Crypto trades 24/7. Exchanges and data vendors differ on the daily-candle boundary (often 00:00 UTC). Joining a UTC daily crypto bar to an exchange-local equity or macro calendar, or to sentiment stamped in local time, can leak hours of future data.
   - For Indian equities, align NSE/BSE session times (IST, 09:15-15:30) with any global inputs. US closes happen after the IST session, so US data dated the same calendar day is in the future for an Indian close-of-day decision.
   - Store everything in UTC with explicit `available_at` timestamps.
9. **Data-vendor revisions.** Back-filled or corrected bars, especially in crypto and illiquid small caps, did not exist in real time. Snapshot the data as it is ingested.
10. **LLM-based screeners.** An LLM whose training data post-dates the backtest period "knows" the outcomes. Backtests that use LLM features before the model's cutoff are look-ahead-contaminated, which the arXiv 2601.13770 benchmark is about. Evaluate only on post-cutoff periods.
11. **Costs and liquidity.** Ignoring spread, impact, STT/fees (India), funding (crypto perps), and circuit or limit-up/limit-down locks that make the signaled entry impossible.
12. **Selection or search leakage.** Count every variant tried, and deflate (DSR, PBO, t > 3). A model chosen as the best of 50 backtests carries leakage from the test set into the selection.

**Practical "leakage smoke tests":**
- Shift all features forward by one bar. If performance *improves* or barely drops, there is leakage somewhere.
- Shuffle labels within dates (a placebo). Accuracy should fall to the base rate.
- Run the full pipeline on synthetic random-walk prices, as in Nikolopoulos's zero-predictability placebo. Any "significant" result is pipeline-induced.
- Compare backtest hit rate and IC with the first months of the forward log. A large gap means leakage or overfitting.

### Gaps
- I found no peer-reviewed quantification of the magnitude of corporate-action or timezone leakage specifically. Those items are mechanism-level guidance.
- The survivorship and subtle-leakage magnitudes (1-4%/yr; 200-500 bps; Sharpe +0.5 to 1.5) come from practitioner blogs. I could not verify the Brown-Goetzmann-Ibbotson-Ross figure from the original paper, and Elton-Gruber-Blake style mutual-fund survivorship estimates were not retrieved.
- I found no India-specific study (NSE/BSE) quantifying survivorship bias in screener backtests.
