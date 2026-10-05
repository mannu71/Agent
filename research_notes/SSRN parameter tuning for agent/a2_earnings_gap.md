# Sleeve A2 (NSE earnings/results-gap "episodic pivot"): SSRN-listed evidence for tuning each threshold

Scope note: both Concretum PDFs were downloaded and read in full as text (pdftotext); quotes and numbers below come from that text. SSRN itself was not touched. Several mirrors were blocked by the egress proxy: nowpublishers, emerald, upenn repository, alphaarchitect, haslam.utk.edu, scirp and the UCLA page via WebFetch (the UCLA page was read via curl). Findings from those sources are marked "(search-snippet only)" and should be treated as lower confidence. "V" = verified in a primary full text read for this note; "S" = from search snippet / secondary summary only.

## Q1. What exactly does "The Power of Price Action Reading" (Zarattini & Stamatoudis, SSRN 4879527) specify, and what in our rulebook is wrong?

### Takeaway
The paper's gap universe is only three filters: open ≥ 6% above prior close, opening price ≥ $2, and ≥ 200,000 shares pre-market. It has no catalyst, ADV or market-cap filter. The "4 Targets" are 25% each at **2R, 4R, 8R and 10R**, not unknown. The only cost modelled is $0.01/share per execution, applied to the micromanaged equity curve only. There is no slippage model and no out-of-sample split: it is one 2016–2023 sample with one trader, who is also a co-author. Our rulebook has four errors: it says the target levels are unknown, it lists three selection preferences instead of four, it calls every mechanical variant "negative", and it implies the trader saw volume.

### Cited Findings
**Data and universe (V).** [PAR PDF](https://concretumgroup.com/wp-content/uploads/2026/02/The-Power-Of-Price-Action-Reading.pdf), §3:
- NYSE and Nasdaq stocks, 1 Jan 2016 to 31 Dec 2023, about 7,000 stocks, survivorship-bias-free, from CRSP.
- 1-minute intraday data from IQFeed and Polygon, *not* adjusted for splits or dividends.
- Gap event criteria, exactly as written: "(1) An opening price at least 6% higher than the previous closing price. (2) Stocks with a minimum opening price of $2. (3) At least 200,000 shares traded in pre-market." This yields **9,794 events**.
- No news or catalyst filter is applied. "Catalyst days" appears only in passing in §4.

**Event behaviour (V).** [PAR PDF](https://concretumgroup.com/wp-content/uploads/2026/02/The-Power-Of-Price-Action-Reading.pdf), Fig. 1 text:
- The text says the average gap is "around 25%"; the Fig. 1 caption says "an average 28% overnight gap". The paper is internally inconsistent here.
- On average, price drifts down after the gap and settles about 15% above the pre-gap close by day 16, which the paper reads as overshooting on the gap day.
- The pre-gap 15 days show a rise "from approximately −26% towards 0%". The normalisation is unclear.

**The six mechanical rules (V).** [PAR PDF](https://concretumgroup.com/wp-content/uploads/2026/02/The-Power-Of-Price-Action-Reading.pdf), pp. 10–11:

| # | Rule | Specification |
|---|---|---|
| 1 | Open – No Stop | Buy at 9:30, hold 30 days |
| 2 | Open – Stop at 1 ATR | Same, with a 1 ATR stop (footnote: ATR is "typically 14 days"; the length used is not stated explicitly) |
| 3 | All OR | Enter on a break of the 5-minute opening-range high; stop "1 cent below the low of the first 5-minute candle"; hold 30 days or until stopped |
| 4 | Pos OR | Same, only for stocks with "a positive 5-minute opening move" |
| 5 | Pos OR + Trailing | Adds a "trailing stop set using a 10-day simple moving average"; close versus intraday touch is not specified |
| 6 | Pos OR + Trailing + 4 Targets | "Selling 25% of shares at each target set at **2R, 4R, 8R, and 10R**", where R = entry − stop |

- No entry time window is specified for the OR breakout. No cost is mentioned for the six mechanical curves.

**Results by stage (V).** [PAR PDF](https://concretumgroup.com/wp-content/uploads/2026/02/The-Power-Of-Price-Action-Reading.pdf), pp. 11–22:
- Open – No Stop bottoms at **−0.25R on day 8**. The R unit here is 1 ATR.
- Open – Stop at 1 ATR bottoms at **−0.17R on day 8**. The paper adds that "if a stock shows weakness on the gap day, a trader is better off closing the positions quickly".
- All OR, Pos OR and Pos OR + Trailing improve step by step but stay unprofitable. Their numbers appear only in Fig. 2 and are not in the text.
- Pos OR + Trailing + 4 Targets is "slightly profitable from the tenth day onwards", but the paper calls this "marginal".
- With trader selection added (Pos OR + Trailing + 4 Targets + Trader), the curve **peaks at +0.25R on day 12**.
- With micromanagement, the curve is **+0.55R on the gap day, with a local maximum of +0.80R on day 4**. It then pulls back for about 3 days and rises again more slowly.

**Outliers (V).** [PAR PDF](https://concretumgroup.com/wp-content/uploads/2026/02/The-Power-Of-Price-Action-Reading.pdf), Fig. 3:

| Group | Gap-day return | Day-16 return |
|---|---|---|
| Top 10% of events | 15% | 22% |
| Top 2.5% | 37% (day 1) | >60% |
| Top 1% | 60% (day 1) | about 85% |

**Selection protocol (V).** [PAR PDF](https://concretumgroup.com/wp-content/uploads/2026/02/The-Power-Of-Price-Action-Reading.pdf), pp. 14–16:
- The trader saw a **two-year daily chart** with 6-month, 1-year and 2-year views and moving averages.
- The software removed dates, tickers, sector, news, **"price levels and volumes"**. The trader therefore did not see volume.
- Events were shown in random order with no feedback until the study ended.
- The trader approved **1,721 of 9,794 events (≈17.6%)**.
- The four favoured factors were:
  1. gaps after a neglect period;
  2. multi-week or multi-month range breakouts;
  3. gaps early in the momentum cycle;
  4. **"Avoiding Gaps Following Consecutive Gaps: gaps that occur immediately after a gap on the previous day are not favored … potential exhaustion."**

**Micromanagement protocol (V).** [PAR PDF](https://concretumgroup.com/wp-content/uploads/2026/02/The-Power-Of-Price-Action-Reading.pdf), pp. 18–22:
- Entry was chosen bar by bar on 1-minute data, with **one entry attempt only**. The trader often waited, because "most gaps … filled (or partially filled)". He waited for a prior daily support area or higher lows, and judged EMAs and VWAP by eye.
- Stops were "typically placed just below the low of the day".
- Up to **4 partials of 25%** each, with management for up to **50 days**.
- Exits trailed the 10-day and 20-day MAs "when a candle **closes** below these averages". The 50-day MA was used once a trade was several R up. Partials were taken into parabolic moves.
- The full position was usually held for the first 3 days before partials began.

**Portfolio results, micromanaged trades only (V).** [PAR PDF](https://concretumgroup.com/wp-content/uploads/2026/02/The-Power-Of-Price-Action-Reading.pdf), Table 1 and Table 2:
- Risk was **0.25% of equity per trade**. The only cost was "**$0.01 per share** … in each execution".
- **1,580 trades**: 18% win rate, average gain +10.10R, average loss −1.02R, **average PnL +1.03R**.
- CAGR 59.1%, volatility 29.9%, Sharpe 1.70, max drawdown 35% (2021), daily skew 3.01.
- About 1,600R cumulative over the sample.
- Yearly returns:

| 2016 | 2017 | 2018 | 2019 | 2020 | 2021 | 2022 | 2023 |
|---|---|---|---|---|---|---|---|
| 14% | 116% | 4% | 0% | 34% | 239% | 64% | 59% |

- Most months from May 2018 to December 2019 show 0.0%, which means almost no trades in that stretch.
- The text says February 2021 was +44.5%, but Table 2 prints **74.0**. Another internal inconsistency.

### Inferences
- **Correction 1 (partials).** The "4 Targets" levels are known: 2R/4R/8R/10R at 25% each. The rulebook's variant of +1R/+2R/+3R/trail is not the paper's. Replacing it with the paper's levels is a spec correction, not a new trial.
- **Correction 2 (selection factors).** There are **four** trader factors, not three. The fourth, no gap on the previous day, is not coded. The current proxy (at most one prior ≥6% gap in 250 days) is a different and looser test.
- **Correction 3 (baseline).** "Unfiltered skeleton is negative" is true for 5 of the 6 mechanical rules. The 4-Targets variant turns slightly positive from day 10, but gross of costs, because no cost is stated for those curves.
- **Correction 4 (volume).** The trader **did not see volume**. Neither the vol20/vol50 neglect proxy nor the first-5-minute RV term in the score replicates the trader. They are additions, mostly from the Zarattini–Barbon–Aziz (ZBA) paper, and should be labelled as such.
- **The paper's own volume filter was pre-market volume ≥ 200k shares.** That is an absolute share count with no NSE analogue. The closest NSE analogue is pre-open auction volume (09:00–09:08) or first-5-minute RV. Historical NSE pre-open data is likely unavailable, so first-5-minute RV ≥ 1 is the practical substitute.
- **Table 1's +1.03R average is not comparable to the 30-day curves.** It covers a management window of up to 50 days, with discretionary entries that are not ORB entries. Expectations for a coded A2 should be anchored on the +0.25R filtered stage (gross), not on 1.03R.
- **Approval rate.** 1,721/9,794 = 17.6%, which matches approving above about the 82nd percentile. Our 0.80 is a close and defensible mirror.
- **Trade count versus approvals.** 1,580 trades against 1,721 approvals implies that about 8% of approved gaps were never entered (inference). This supports logging unfilled approvals separately.
- **Regime dependence.** The near-zero 2018–2019 activity, alongside 2017 and 2021 bursts, means event flow is regime- and season-clustered. A per-day entry cap will bind in bursts.

### Gaps
- The values of the All OR, Pos OR and Pos OR + Trailing curves are only in figures; the text gives no numbers.
- ATR length for rule 2 is not stated beyond "typically 14".
- No per-event cost or slippage is modelled for the mechanical rules. There is no breakdown by gap size, liquidity, catalyst type or year for the mechanical rules.
- Whether "positive 5-minute opening move" means close > open of the first bar, or close > prior close, is not stated. ZBA uses close versus open of the first bar (see Q3), so we assume the same.

## Q2. Earnings-gap continuation versus reversal, PEAD and its decay, the announcement premium, overnight versus intraday, and India

### Takeaway
For liquid stocks, the academic evidence says earnings information is now priced by the next open:
- PEAD has been absent for non-microcaps since about 2006.
- A 2025 replication finds the price-based earnings-drift factor insignificant once microcaps are excluded (t = 1.43).
- After-hours price discovery in 50 very liquid US stocks became efficient after 2016.

Two survivals remain:
- large **information-backed** price jumps and shocks continue for 1–3 months, while no-information shocks reverse;
- earnings-momentum profits accrue **overnight**, not intraday.

India showed PEAD in 2002–2017 data, but the studies are small and pre-date the 2020s market structure. This supports keeping the catalyst requirement and holding overnight. It does not support an expectation of large mechanical drift.

### Cited Findings
**PEAD decay (V).** [Martineau, CFR 2022 draft](https://cfr.ivo-welch.info/published/papers/martineau2021rest.pdf):
- Samples: Compustat 1973–2019 (593,654 announcements) and I/B/E/S 1984–2019 (312,462).
- "For large stocks, PEAD have been non-existent since 2006 but has only disappeared recently for microcap stocks." Microcap means below the NYSE 20th percentile of market cap.
- Analyst surprises "fail to positively predict post-announcement returns over 60 days" for all-but-microcaps since 2006, and for microcaps since 2016.
- Announcement-date price response to surprises is **6x** larger for all-but-microcaps and **3x** larger for microcaps in 2016–2019 than in 1984–1990.
- Pre-announcement drifts have also weakened.
- With random-walk surprises, drift for all-but-microcaps lasts "not more than five days".

**2025 rebuttals and the reconciliation (V for the article; the papers themselves were not read).** [UCLA Anderson Review on Subrahmanyam](https://anderson-review.ucla.edu/is-post-earnings-announcement-drift-a-thing-again/) and [Subrahmanyam SSRN 5930255](https://papers.ssrn.com/sol3/papers.cfm?abstract_id=5930255):
- Dickerson, Julliard and Mueller (2025) find earnings drift "remains a major market factor". Their factor is built from **abnormal returns around earnings days**, which is price-based rather than accounting-surprise-based.
- Hirshleifer, Peng and Wang (2025) report PEAD with a t-statistic around 14.
- Subrahmanyam replicated the earnings-drift factor on US data from Feb 2001 to Dec 2024:

| Universe | t-statistic |
|---|---|
| All stocks | 2.18 |
| **Excluding microcaps** | **1.43** |

- He argues the Hirshleifer et al. result is a market-cap effect. Microcaps are about 3% of US market value.

**Speed of pricing after 2016 (V).** [Christensen, Timmermann & Veliyev, arXiv 2601.08962 (v2 Jan 2026)](https://arxiv.org/pdf/2601.08962):
- 50 very liquid US stocks, 2008–2020, tick data.
- Prices jump after more than 90% of earnings announcements.
- A surprise-signed trade on the first post-announcement print earns, per trade:

| Execution | Mean return |
|---|---|
| Frictionless | 1.80% |
| At midquote | 1.50% |
| At actual spreads | 0.72% |
| 5-second delay | 0.41% |
| Longer delays | Not significant |

- In 2016–2020 the strategy is insignificant once spreads are paid, and turns negative with delays. The authors call this "consistent with efficient price formation after 2016".
- The universe is 50 mega-liquid names only, so this is not evidence about small or mid caps.

**Fast numbers, slow language (V, abstract).** [arXiv 2606.29734](https://arxiv.org/abs/2606.29734), S&P 1500, 2022–2025:
- The quantitative EPS/revenue surprise "is largely eliminated by the next market open".
- Sentiment in the conference-call transcript "peaks on the next trading day, real and tradeable".

**Overnight versus intraday (V).** [Lou, Polk & Skouras, JFE 2019](https://personal.lse.ac.uk/polk/research/TugOfWar.pdf), US 1993–2013:
- "100% of the returns to SUE [earnings momentum] occur overnight": the long-short CAPM alpha is 0.56%/month (t = 3.20), and the intraday alpha is "indistinguishable from zero".
- Price momentum: overnight alpha 0.98%/month (t = 3.84), intraday −0.02%.
- Overnight-winner minus overnight-loser decile: three-factor **overnight** alpha +3.47%/month (t = 16.83), **intraday** alpha −3.02%/month. This is the cross-period "tug of war".
- More of momentum's negative skew arrives intraday: skewness is −1.53 intraday versus −1.08 overnight.

**Information shocks continue; no-information shocks reverse (S, abstracts).**
- [Jiang & Zhu, JFE 2017](https://ideas.repec.org/a/eee/jfinec/v124y2017i1p43-64.html) (search-snippet only): long positive-jump and short negative-jump stocks "earn significantly positive returns over the next one- to three-month horizons". The effect is stronger for overnight jumps, works beyond earnings surprises, and is attributed to limited attention.
- [Savor, JFE 2012](https://repository.upenn.edu/handle/20.500.14332/34495) (search-snippet only): price shocks accompanied by analyst reports drift, while no-information shocks reverse. Drift exists only when the analyst recommendation change has the same sign as the price move.

**Earnings-day return components (S, abstract).** [Ben-Rephael, "Mind the Gap" working paper](https://haslam.utk.edu/wp-content/uploads/2024/11/Ben-Rephael-Paper.pdf) (search-snippet only):
- The part of the earnings-day return not explained by the earnings surprise (the "Return-Earnings Gap") is about 50% reversed, but slowly, over about 3 years.
- The surprise-explained part continues and does not revert.

**Drift turning to reversal (S).** [Clinch, He, Landsman & Li, SSRN 5836584](https://papers.ssrn.com/sol3/papers.cfm?abstract_id=5836584) (search-snippet only): PEAD continuation at the *next* quarter's announcement "has been replaced by a price reversal in the past two decades".

**Pre-announcement run-up (S).** [Aboody, Lehavy & Trueman, RAST 2010, via Alpha Architect](https://alphaarchitect.com/2013/04/go-long-make-money-go-short-make-money-all-good/) (search-snippet only):
- Stocks with the strongest prior-12-month returns earn +1.58% market-adjusted in the 5 days *before* earnings and **−1.86% in the 5 days after**.
- The pattern is significant after transaction costs and is attributed to small-investor attention.

**Earnings announcement premium (S).**
- [Barber, De George, Lehavy & Trueman, "The earnings announcement premium around the globe"](https://lbsresearch.london.edu/id/eprint/391) (search-snippet only): announcement-month returns exceed non-announcement months across countries.
- ["The Disappearing Earnings Announcement Premium" (IIMB ARC 2020)](https://www.iimb.ac.in/ARC2020/Papers/The_Disappearing_Earnings_Announcement_Premium.pdf) (search-snippet only; the PDF download returned HTML): the premium has faded and shifted toward 8-K filing periods, consistent with uncertainty-based explanations.

**India: PEAD and announcement timing (S).**
- [PEAD Anomaly in India, 2002–2017 (SCIRP)](https://scirp.org/journal/paperinformation?paperid=88060) (search-snippet only): statistically significant PEAD; the top coded-surprise group beats the bottom by about 6% over 64 days. Robust to beta, size, P/B, illiquidity and idiosyncratic volatility.
- [Sehgal & Bijoy, Vision 2015](https://doi.org/10.1177/0972262914564042) (search-snippet only), 2002–2011: significant post-event abnormal returns in 35 of 37 quarters.
- [Value-glamour study, 100 NSE firms, 1,130 observations, 2014–2018](https://journals.vut.cz/index.php/trends/article/download/541/521/1870) (search-snippet only): a *negative* association between abnormal returns and earnings surprise.
- [Asian Journal of Accounting Research, 2019](https://www.emerald.com/insight/content/doi/10.1108/AJAR-04-2019-0023/full/html) (search-snippet only), 30 BSE Sensex firms:
  - market responses differ significantly between during-hours and after-hours announcements;
  - firms with significant surprises tend to announce after hours;
  - after-hours announcements drew a negative response.
- A financial-sector Indian study (snippet only; URL not identified) finds no significant difference between during-hours and after-hours abnormal returns.

**Practitioner gap statistics (low quality; not used).** Claims such as "12%+ gaps fill 38% of the time", "20%+ earnings moves stall more than 8–15% moves" and "earnings gap-ups hold 71% within 5 days versus 43% for non-earnings" come from blogs with no methodology ([PaperTradingJournal](https://papertradingjournal.com/2026/07/15/how-often-do-stocks-gap-up-vs-gap-down-after-earnings/), [StockAlarm](https://pro.stockalarm.io/blog/stock-gap-up-gap-down-explained)). They are not evidence.

### Inferences
- **Keep the catalyst requirement.** Savor 2012 and Jiang & Zhu 2017 imply that the *information* content of a jump separates drift from reversal. ZS 2024 had no catalyst filter, so on NSE "results-filed" is a plausible improvement over the paper. It is not a replication, though, and the baseline should be run both ways: "any 6% gap" to reproduce the paper's negative result, and "results-only".
- **Do not expect classic PEAD.** For F&O names, which are mostly large and liquid, the US evidence says the surprise is priced by the open. Any edge must come from selection, meaning information jumps in neglected or base-breakout names, as Jiang & Zhu argue through limited attention. That fits the ZS trader factors, but it is inference.
- **Overnight holding matters.** Earnings and price momentum accrue overnight, while intraday returns of overnight winners revert (tug of war). Three implications:
  - the gap-day "15:20 weakness exit" cuts losers that the intraday clientele is pushing down, which is consistent with ZS's 1-ATR observation;
  - a strict early entry window may be buying into the intraday reversal leg;
  - a later or pullback entry, like the micromanaging trader's, may be cheaper.
  This is untested on NSE.
- **The pre-gap run-up matters.** Aboody et al. show that extreme prior-12-month winners reverse after earnings. This supports the "early cycle" and "neglect" proxies: open/SMA200 ≤ 50%, and 120-day return ≤ 15% or negative.
- **The ZS Fig. 1 pattern is consistent with the literature.** Prices on average give back about 10 of roughly 25 gap points by day 16, which matches overshoot/reversal of the non-fundamental part of the gap (Ben-Rephael). The mechanical baseline should be negative on NSE too.
- **India-specific.** Indian firms do release results during market hours, which our A2 excludes. Evidence that firms with big surprises choose after-hours timing (AJAR 2019) suggests the overnight-filed sample is where large surprises concentrate. That supports the 15:30-to-09:15 filter, though it is weak evidence from 30 firms. The Indian PEAD studies end by 2017–2018 and are small. There is no published evidence that NSE PEAD survived the post-2020 retail and F&O boom.

### Gaps
- Not read in full (blocked or SSRN): Dickerson-Julliard-Mueller (2025), Hirshleifer-Peng-Wang (2025), Subrahmanyam (SSRN 5930255), Jiang & Zhu, Savor, Aboody et al., Ben-Rephael, Clinch et al., and the Indian studies.
- Specific thresholds are unknown, such as Savor's price-shock threshold and Jiang & Zhu's jump-test details and return magnitudes.
- No academic paper was found that conditions post-earnings drift on **gap size buckets** (5/6/8/10%+) or on **first-minutes relative volume** for gap-ups specifically. The high-volume return premium (Gervais, Kaniel & Mingelgrin) was not retrieved.
- No 2020s NSE study of results-gap continuation, mid-session results or post-results drift was found. Primary NSE pages are blocked.

## Q3. Evidence on opening-range-breakout entry and intraday stops: Zarattini, Barbon & Aziz, "A Profitable Day Trading Strategy for the U.S. Equity Market" (SSRN 4729284)

### Takeaway
ZBA verify our relative-volume definition: first-5-minute volume divided by the mean of the prior **14 days'** first-5-minute volume. The selection is RV ≥ 100%, then the top 20 by RV, with direction set by the first bar and dojis skipped. The stop is **10% of ATR(14)** from the fill, the exit is at the close, and risk is 1% with a 4x leverage cap. The only cost is commission of $0.0035/share. There is no slippage model and no out-of-sample period (2016–2023, the same period as ZS). The 10%-ATR stop belongs to an **intraday** strategy and should not replace A2's first-bar-low stop. The RV result is the best evidence available for an RV ≥ 1 hard gate.

### Cited Findings
All findings in this section are V, from the [ZBA PDF](https://concretumgroup.com/wp-content/uploads/2026/02/A-Profitable-Day-Trading-Strategy-For-The-U.S.-Equity-Market.pdf).

**Universe:**
- More than 7,000 US stocks, 2016–2023.
- Filters: opening price > $5; average volume over the previous 14 days ≥ 1,000,000 shares/day; ATR over the previous 14 days > $0.50.

**Rules:**
- **Entry.** A stop order at the 5-minute high if the first candle (9:30–9:35) is bullish, at the 5-minute low if it is bearish, and "In the case of a doji … no order was placed".
- **Stop.** "a stop loss order at a 10% ATR distance from the executed entry price". If the stop is not hit, the position closes at 16:00.
- **Sizing.** 1% of capital lost if stopped, with a 4x leverage cap. Starting capital $25,000.
- **Costs.** Commission $0.0035/share (IBKR Pro Tiered). No slippage is mentioned.
- **Relative volume.** RV(t,j) = ORVolume(t,j) / [(1/14) · Σ(i=1..14) ORVolume(t−i,j)], where ORVolume is "volume traded … during the first 5-minutes".

**Average PnL by RV bucket (net of commissions):**

| Relative volume | Average PnL |
|---|---|
| < 100% | −0.02R |
| > 100% | **+0.08R** |
| > 30x | **+0.38R** |

**Performance:**

| Strategy | Total return | IRR | Volatility | Sharpe | Hit ratio | Max drawdown | Worst day | Alpha |
|---|---|---|---|---|---|---|---|---|
| Base (all stocks, both directions) | 29% | 3.2% | 6.6% | 0.48 | 41.4% | 13% | — | 3.3% |
| ORB + RelVol (RV ≥ 100%, top 20) | 1,637% | 41.6% | 14.8% | 2.81 | 48.4% | 12% | −1.61% | 35.8% (beta 0.00) |

**Other opening-range lengths** (same RV ≥ 100% rule):

| ORB length | Sharpe |
|---|---|
| 5 minutes | 2.81 |
| 15 minutes | 1.43 |
| 30 minutes | 0.21 |
| 60 minutes | 0.40 |
| Equal-weight combination | 1.99 |

- The authors claim robustness because the "parameters were minimal and based on economic rationale". There is no holdout.
- No time-of-day cutoff for entry is stated; orders appear to stay live through the session.

**Replication in India (secondary, from the rulebook).** An NSE ORB replication was −21.8% ([GitHub](https://github.com/md0n-cmd/indian-equity-backtests), cited in the existing rulebook and not re-verified here).

### Inferences
- **RV definition (rulebook "U" becomes V).** Our score's "volume ÷ its 14-day average at the same time" matches ZBA exactly.
- **RV ≥ 1 as a hard gate.** ZBA show the ORB edge is concentrated in RV > 1, and is negative below 1, *for intraday ORB*. ZS required ≥ 200k pre-market shares, which is also an "in play" filter. A hard gate of RV(first 5 minutes) ≥ 1.0 is the closest NSE analogue to ZS's pre-market volume rule, and is more defensible than folding log-RV into a z-score average.
  - This is a code rule change. If added as a replacement for the paper's pre-market filter it adds 1 trial; if RV is also kept in the score, count 2.
- **Top-20 selection.** This is irrelevant on NSE at A2 event counts (a few events a day at most). The analogue is `a2.max_entries_per_day` with ranking by score.
- **Stop.** ZBA's 10%-ATR stop is designed for same-day exits. Over a 30-day hold, such a tight stop would make R tiny and blow up cost-to-R; at 0.5% round-trip costs, it would likely consume more than 0.3R per trade on typical NSE ATRs (inference). Keep ZS's first-bar-low − 1 tick for A2. The "skip if stop distance > 1.0 × ADR20" cap is ours (U), not from either paper.
- **Entry timing.** The 5-minute OR beats 15/30/60-minute ORs in ZBA, so keep the 5-minute bar. Neither paper imposes an entry deadline, so a 10:15 cutoff is our own restriction.

### Config recommendations for Sleeve A2

Trial accounting: "adds trials" means the value enters the deflated-Sharpe / PBO count.

| Key / rule | Current | Recommendation | Evidence | Adds trials? |
|---|---|---|---|---|
| `a2.min_gap` | 0.06 | **Keep 0.06** as the frozen default (ZS spec, V). Run 0.05/0.08/0.10 only as a reported sensitivity, not for selection. No academic source ties drift to a specific gap size. NSE bands (10%/20% cash; F&O dynamic) truncate the gap distribution far below ZS's ~25% average, so NSE 6% gaps are a different population. | ZS §3; no gap-bucket literature found | Grid adds 3 |
| `a2.window_end` | "10:15" | For the **replication run, set it to about "15:15"** (whole gap day), because neither ZS nor ZBA specifies a deadline. Keep 10:15 as a variant. The tug-of-war evidence gives no support for forcing early entries. | ZS pp. 10–11; ZBA; Lou-Polk-Skouras | 1 (10:15 vs full day); 11:15 and 15:00 add 2 more |
| `a2.approve_pct` | 0.80 | **Keep** (ZS approval 17.6% ⇒ 0.824 would be exact; difference immaterial). Do not tune. | ZS p. 15 | 0 if frozen |
| `a2.min_history_events` | 30 | Keep. There is no evidence either way, but it is a warm-up control rather than an edge parameter. Note that ZS event flow was nearly zero for about 18 months in 2018–2019, so a 250-day window can hold fewer than 30 events in quiet regimes. Report the share of days blocked by warm-up. | ZS Table 2 | 0 if frozen |
| `a2.max_entries_per_day` | 2 | Keep 2, with entries **ranked by score**, and log the skipped approvals. ZS had no cap. Its events cluster (2017, Nov 2020 to Feb 2021), so the cap will truncate bursts. Measure the clipped-event PnL. | ZS Table 2 | 0 if frozen; 1 if 3 is tested |
| `a2.max_hold_days` | 30 | **Keep 30** to match ZS mechanical rules. Test 50 as one variant, since the trader managed up to 50 days, Table 1's +1.03R included long tails, and Jiang–Zhu continuation runs 1–3 months. | ZS pp. 10, 19; Jiang & Zhu | 1 |
| `a2.risk_per_trade` | 0.0025 | **Keep**: exactly the ZS trader's sizing. ZS max drawdown was 35% at this size *with* a +1.03R edge, so a coded edge near zero gives no reason to go higher. | ZS Table 1 | 0 |
| Code: partial variant | 1/3 at day 3; variant +1R/+2R/+3R/trail | **Replace the variant** with the ZS spec: 25% each at **2R, 4R, 8R, 10R**, with the remainder on the SMA10 trail. Keep "1/3 at day 3" as the primary (a coded proxy of "full position first 3 days, then partials"). | ZS pp. 10–11, 22 | 0 (spec fix); 1 if both are compared |
| Code: SMA10 trail trigger | close-based assumed | Keep **close-based**: the micromanaging trader exited "when a candle closes below" the 10/20-day MAs. Mechanical-rule wording is ambiguous. | ZS p. 20 | 0 |
| Code: consecutive-gap exclusion | absent | **Add**: reject if day t−1 was itself a gap ≥ `a2.min_gap`. This is ZS trader factor 4. | ZS p. 16 | 1 (or 0 if treated as paper spec) |
| Code: in-play gate | log-RV inside score | **Add a hard gate** RV(first 5 minutes, 14-day baseline) ≥ 1.0 as the NSE analogue of ZS's ≥ 200k pre-market shares. Move volume terms (vol20/vol50 and RV) out of the "trader-replica" score into a separately reported feature, because the ZS trader never saw volume. | ZBA Fig. 4; ZS §3 and p. 14 | 1–2 |
| Code: pre-earnings run-up | 120-day return ≤ 15% | Keep. Optionally add "prior-12-month return not in the top decile of the universe". | Aboody et al. 2010 (S) | 1 if added |
| Code: catalyst | results filed 15:30 to 09:15 | Keep, but run the "any ≥6% gap" baseline first to reproduce ZS's negative mechanical result. | Savor 2012 (S); Jiang & Zhu (S); ZS has no catalyst | 0 (baseline is a reproduction check) |
| Code: day-0 weakness exit | 15:20 below entry → exit at close | Keep. It agrees with ZS's comment on the 1-ATR variant and with intraday reversal of overnight winners. It is not a ZS rule, so report results with it on and off. | ZS p. 11; Lou-Polk-Skouras | 1 |
| Costs | 0.5% RT base, 1.0% stress | Keep our model. Neither paper gives a usable benchmark: ZS charges only $0.01/share on the micromanaged curve (mechanical curves uncosted), and ZBA charges only $0.0035/share. | ZS Table 1; ZBA §2 | 0 |

### Gaps
- ZBA reports no breakdown of long trades on gap-ups specifically, and no multi-day holding.
- ZBA's universe filters (1M shares, $0.50 ATR) have no published rupee calibration.
- No out-of-sample or post-2023 results exist for either Concretum paper. Neither paper reports bootstrap or t-statistics for the per-trade R.
