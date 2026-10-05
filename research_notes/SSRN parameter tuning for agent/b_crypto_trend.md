# Sleeve B: tuning evidence for the crypto Donchian trend ensemble (BTC/ETH, INR perps)

Scope: Zarattini, Pagani and Barbon, "Catching Crypto Trends" (SSRN 5209907), read in full from the authors' own PDF; other crypto time-series momentum evidence; drawdown-stop evidence; funding and crowding evidence. Research date: 5 Oct 2026. SSRN, BIS PDF, IDEAS, FTI Consulting and AUT pages were blocked by Cloudflare or the egress proxy and were not bypassed. Abstracts or summaries were used where noted.

## Q1. What exactly does "Catching Crypto Trends" specify, and where is it inconsistent?

### Takeaway
The full text confirms the rulebook's signal, stop, weighting, ensemble, 10 bps cost and 20% vol-only rebalance threshold. It does **not** state the daily close time or whether the 20% band is relative or absolute. The sample is in-sample only (Jan 2015 to 19 Mar 2025, no hold-out). The MAR column contradicts the MDD column for the BTC Combo in Tables 1 and 3, and the implied drawdowns (26–34%) are far larger than the printed 19%. So `kill.b_backtest_max_dd = 0.19` rests on a number the paper itself contradicts.

### Cited Findings
- **Version and source.** The PDF on Concretum is "First Version: April 4, 2025; This Version: April 9, 2025" — [Concretum PDF](https://concretumgroup.com/wp-content/uploads/2026/02/Catching-Crypto-Trends.pdf). Barbon's page also shows "Last Revision: 2025-04-09" — [abarbon.com](https://abarbon.com/papers/catching-crypto-trends). A search-engine summary of the RePEc listing says the paper is SFI Research Paper No. 25-80, "revised October 2, 2025". IDEAS itself was blocked, so I could not check whether the October SSRN revision changes any numbers — [IDEAS (unverified)](https://ideas.repec.org/p/chf/rpseri/rp2580.html).
- **Data.** CoinMarketCap survivorship-free panel of 21,616 coins, Jan 2010 to Mar 2025, with stablecoins, wrapped tokens and NFT tokens excluded. Prices are "aggregated across multiple exchanges" per CMC methodology. **No close time or cut-off is stated anywhere in the text** — [PDF §3](https://concretumgroup.com/wp-content/uploads/2026/02/Catching-Crypto-Trends.pdf).
- **Channels.** The channels use closes, not highs and lows: Up = max(Close_t…Close_{t−n+1}), Down = min of the same closes, Mid = 0.5·(Up+Down) — [PDF §4.1](https://concretumgroup.com/wp-content/uploads/2026/02/Catching-Crypto-Trends.pdf).
- **Entry and stop.** Enter when Close_t = Up_t(n). The trailing stop is TS_{t+1} = max(TS_t, Mid_t): "the prevailing Trailing Stop used in day t+1 is the maximum between the Trailing Stop used in day t and the middle line of the Donchian channel computed at the end of day t". The initial stop is Mid at entry. Exit when Close_t ≤ TS_t — [PDF §4.1, §4.3, footnote 3](https://concretumgroup.com/wp-content/uploads/2026/02/Catching-Crypto-Trends.pdf).
- **Sizing.** w_t^n = min(0.25/σ_t, 200%) × Pos_t^n, where σ_t is the 90-day (3-month) annualised volatility of returns. The annualisation factor and simple-versus-log returns are not stated. Combo weight = (1/9)·Σ w^n. Returns are r_t = w_{t−1} × ret_t, so the position is set at the signal close with no extra lag — [PDF §4.2–4.3](https://concretumgroup.com/wp-content/uploads/2026/02/Catching-Crypto-Trends.pdf).
- **Sizing drawback, by the authors' own account.** Vol scaling "reduces exposure just when the opportunity for outsized returns is highest" (late 2020). They say alternative sizing gave "notable" improvement but do not publish it — [PDF §4.2](https://concretumgroup.com/wp-content/uploads/2026/02/Catching-Crypto-Trends.pdf).
- **Rebalance band (verbatim).** "the portfolio is only rebalanced if the difference between the current and target allocation exceeds 20%". The band applies only to vol-driven changes; signal changes trade immediately. **The text does not say whether 20% is relative to the target or absolute** — [PDF §5.1](https://concretumgroup.com/wp-content/uploads/2026/02/Catching-Crypto-Trends.pdf).
- **Costs.** Sensitivity was run at 0, 10, 25 and 50 bps, following Le et al. (2023). The authors say typical BTC exchange costs are "generally below 5 bps". At 50 bps the 5-day model's CAGR falls "from 34% to 18%", and the 20% threshold recovers "approximately 100 basis points per year" at 50 bps. All later tables use 10 bps (citing Liu and Tsyvinski) plus the 20% threshold — [PDF §5.1, Fig. 3](https://concretumgroup.com/wp-content/uploads/2026/02/Catching-Crypto-Trends.pdf).
- **Table 1 (BTC, gross, 1 Jan 2015 to 19 Mar 2025).**

  | Lookback | CAGR | Vol | Sharpe | Sortino | MDD | MAR |
  |---|---|---|---|---|---|---|
  | 5d | 36% | 19% | 1.66 | 1.87 | 25% | 1.41 |
  | 10d | 32% | 18% | 1.55 | 1.64 | 27% | 1.19 |
  | 20d | 34% | 18% | 1.60 | 1.60 | 26% | 1.32 |
  | 30d | 34% | 19% | 1.61 | 1.61 | 24% | 1.41 |
  | 60d | 28% | 19% | 1.30 | 1.25 | 19% | 1.46 |
  | 90d | 27% | 20% | 1.20 | 1.15 | 24% | 1.12 |
  | 150d | 21% | 20% | 0.99 | 0.97 | 29% | 0.74 |
  | 250d | 25% | 20% | 1.13 | 1.15 | 33% | 0.76 |
  | 360d | 29% | 20% | 1.28 | 1.27 | 34% | 0.83 |
  | Combo | 30% | 17% | 1.58 | 2.03 | 19% | **0.88** |

  The Combo has alpha 14% and beta 0.17. Every alpha is significant at the 2.5% level except 150d and 250d — [PDF Table 1](https://concretumgroup.com/wp-content/uploads/2026/02/Catching-Crypto-Trends.pdf).
- **Table 2 (BTC trades per lookback, gross).**

  | Lookback | Trades | Win rate | Gain:loss |
  |---|---|---|---|
  | 5d | 292 | 41% | 3.7 |
  | 10d | 156 | 40% | 4.5 |
  | 20d | 78 | 47% | 5.2 |
  | 30d | 49 | 49% | 6.7 |
  | 60d | 28 | 46% | 9.2 |
  | 90d | 20 | 60% | 6.5 |
  | 150d | 15 | 60% | 4.1 |
  | 250d | 9 | 78% | 4.7 |
  | 360d | 5 | 80% | 12.8 |

  The worst single trade is −5% to −11% per unit — [PDF Table 2](https://concretumgroup.com/wp-content/uploads/2026/02/Catching-Crypto-Trends.pdf).
- **Table 3 (Combo net of 10 bps with the 20% threshold), selected assets.**

  | Asset | CAGR | Vol | Sharpe | Sortino | MDD | MAR |
  |---|---|---|---|---|---|---|
  | BTC | 30% | 17% | 1.56 | 1.23 | 19% | 1.15 |
  | ETH | 27% | 16% | 1.51 | 1.22 | 15% | 0.96 |
  | SOL | 27% | 14% | 1.68 | 1.64 | 12% | 2.04 |
  | BNB | 17% | 15% | 1.06 | 0.99 | 17% | 0.77 |
  | XRP | 18% | 17% | 1.00 | 0.97 | 14% | 1.10 |
  | LTC | 11% | 14% | 0.72 | 0.53 | 29% | 0.44 |
  | XMR | 11% | 16% | 0.67 | 0.53 | 35% | 0.36 |
  | FIL | 5% | 13% | 0.39 | 0.36 | 30% | 0.39 |

  Across the 40 coins, Sharpe ranges from 0.21 (BSV) to 1.68 (SOL) — [PDF Table 3](https://concretumgroup.com/wp-content/uploads/2026/02/Catching-Crypto-Trends.pdf). The authors acknowledge selection bias in this table ("affected by selection bias").
- **Top-B rotation rules.** Rebalanced at month-end. To be eligible a coin must be listed for at least 365 calendar days, must not be a wrapped token, stablecoin or NFT collectible, and must have a 30-day median daily volume of at least $2M. Eligible coins are ranked by median daily volume in the month and the top B are kept, each with 1/B of capital, compounding through the month. A coin is removed if its 30-day median daily volume falls below $1M, **or** its "median daily price change" over 30 days is below 0.5% (the text does not say "absolute") — [PDF §7.1–7.2](https://concretumgroup.com/wp-content/uploads/2026/02/Catching-Crypto-Trends.pdf).
- **Table 4 (long-only rotation, net).**

  | Universe | CAGR | Vol | Sharpe | Sortino | MDD | MAR | Alpha | Beta |
  |---|---|---|---|---|---|---|---|---|
  | Top 5 | 18% | 10% | 1.44 | — | 14% | — | — | — |
  | Top 10 | 18% | 9% | 1.50 | — | 12% | — | — | — |
  | Top 20 | 18% | 9% | 1.57 | 1.97 | 11% | 1.61 | 10.8% | 0.08 |
  | Top 50 | 16% | 8% | 1.54 | — | 11% | — | — | — |

  — [PDF Table 4](https://concretumgroup.com/wp-content/uploads/2026/02/Catching-Crypto-Trends.pdf)
- **Liquidity deciles.** Over 2020–2025 the authors find "no meaningful size effect" across the top 100 coins. The average 6-month rolling correlation with the SG Trend Index is about 7.4% — [PDF §7.3, §8](https://concretumgroup.com/wp-content/uploads/2026/02/Catching-Crypto-Trends.pdf).
- **Long-short appendix (Table 5, gross).** The BTC Combo makes 27% CAGR at 18% vol, Sharpe 1.31, MDD 19% (MAR printed as 0.63), so it is worse than long-only. Single-lookback MDDs reach 43% (360d) — [PDF Appendix](https://concretumgroup.com/wp-content/uploads/2026/02/Catching-Crypto-Trends.pdf).
- **No out-of-sample test.** There is no hold-out, walk-forward or post-publication test. Every table covers the full 2015–Mar 2025 sample, and the authors call the results "historical backtests" — [PDF disclaimer](https://concretumgroup.com/wp-content/uploads/2026/02/Catching-Crypto-Trends.pdf).
- **Inconsistencies a reviewer flagged.** Combo MAR 0.88 against 1.15; Sortino 2.03 falling to 1.23 while Sharpe barely moves; "from 34%" in §5.1 against 36% in Table 1. The same reviewer found that, on Binance closes, 0.25/σ (√365) exceeded the 1× cap on 0.0% of days for BTC, ETH and BNB, with median raw weights of 0.32–0.43 — [GitHub review](https://github.com/alfred1123/Quant_Strategies/pull/61).

### Inferences
- **The MAR and MDD columns disagree, and the implied drawdowns are large (my arithmetic).**
  - In Table 1, every single-lookback row satisfies MAR ≈ CAGR/MDD (5d: 36/25 = 1.44 against 1.41; 360d: 29/34 = 0.85 against 0.83). Only the Combo row breaks it: 30/19 = 1.58 against 0.88, and 0.88 implies an MDD of about **34%**.
  - In Table 3, BTC 30/1.15 implies an MDD of about **26%**, and ETH 27/0.96 about **28%**. Large mismatches also appear for SHIB (2.17 against 6.64) and LUNC.
  - Table 4 is internally consistent (top 20: 18/11 = 1.64 against 1.61).
  - So for BTC and ETH, either the MDD column or the MAR column is wrong. The single-lookback BTC MDDs of 19–34% make a Combo MDD near 19% plausible but not certain. **Treat the paper's 19% and 15% as unreliable lower bounds, and set `kill.b_backtest_max_dd` from the agent's own replication, not from 0.19.**
- **Sortino 2.03 (gross) against 1.23 (net) at an almost unchanged Sharpe (1.58 against 1.56)** cannot come from 10 bps costs. Either a different downside-deviation convention was used in Table 3, or there is a typo. Do not use Sortino as an acceptance gate.
- **Another internal inconsistency.** The captions of Figures 7 and 8 say "top 10" while the §8 text says "applied to 20 assets".
- **Band type.** The Combo weight is a mean of nine weights of roughly 0–0.43 each. An *absolute* 20-percentage-point band would almost never trigger a vol-only rebalance, which would make it effectively "no vol rebalancing". That would not be described as a tool that recovers about 100 bps a year at 50 bps costs. So the *relative* reading (|cur − tgt| > 0.20·tgt), which the agent already implements, is the more plausible one. The absolute version should be run only as a labelled sensitivity.
- **Possible implementation mismatch in `trading-agent/src/crypto_trend.cpp`.**
  - `update_signals()` sets `sig.stop = max(sig.stop, mid_t)` and *then* tests `close_t <= sig.stop`. Today's mid therefore enters today's exit test.
  - The paper tests Close_t against TS_t = max(TS_{t−1}, Mid_{t−1}); today's mid only affects tomorrow.
  - The code can therefore exit one day earlier than the paper when an old low drops out of the window and raises Mid_t above Close_t. This is a correctness fix toward the published rule, not a new trial, and it should be made before checking trade counts against Table 2.
- **Close time.** The paper is silent. The agent's 00:00 UTC is an assumption to sensitivity-test (00:00 UTC against, say, 12:00 UTC, labelled as a robustness check rather than a tuned choice).

### Gaps
- I could not tell whether the 2 Oct 2025 SSRN revision changes any table, because SSRN and IDEAS were blocked. The April 2025 PDF is the only full text read.
- The paper does not state the volatility estimator (log or simple returns, annualisation factor), the exact close time, the band type, or "absolute" in the 0.5% exit rule.
- No per-lookback results are given for ETH. The authors say these are available on request.

## Q2. What does other crypto time-series momentum evidence say about lookbacks, vol targeting, 2022–2026 performance and decay?

### Takeaway
Independent evidence supports crypto time-series momentum, including at short (daily and weekly) horizons, and supports volatility scaling for BTC. However, I found **no independent, peer-reviewed, out-of-sample test of this specific Donchian ensemble after March 2025**. Evidence for 2022–2024 comes only from low-credibility preprints. Lookback choice should stay as the published nine-lookback ensemble rather than be tuned.

### Cited Findings
- **Liu and Tsyvinski (RFS 2021, NBER w24877).** They find "a strong time-series momentum effect" for BTC, ETH and XRP at daily and weekly frequencies. Bitcoin's average next-week return is 11.2% for the top quintile of last-week returns against 2.6% for the bottom quintile (Sharpe 0.45 against 0.19). For BTC daily returns, momentum is significant at the 1–5 day horizon; for ETH it is significant at the 1-day horizon — [NBER w24877](https://www.nber.org/papers/w24877); [NBER PDF](https://www.nber.org/system/files/working_papers/w24877/w24877.pdf); quintile figures via [CXO Advisory summary](https://www.cxoadvisory.com/currency-trading/crypto-asset-risks-and-returns/).
- **Han, Kang and Ryu, "Time-Series and Cross-Sectional Momentum in the Cryptocurrency Market: A Comprehensive Analysis under Realistic Assumptions".** Once transaction costs and daily price paths are accounted for, "many momentum portfolios are liquidated", and many with statistically significant mean returns earn insignificant profits. "Evidence of time-series momentum is strong, whereas evidence of cross-sectional momentum is weak." The effect is concentrated in large winners. The paper uses 15 bps a trade. Only the abstract and a search summary were available; the AUT host was blocked — [AUT PDF](https://acfr.aut.ac.nz/__data/assets/pdf_file/0009/918729/Time_Series_and_Cross_Sectional_Momentum_in_the_Cryptocurrency_Market_with_IA.pdf).
- **Man Group (practitioner research).** Volatility-scaling bitcoin ex ante to 30% volatility, from Sep 2012 to Dec 2024, "achieves an increase of around 40 Sharpe points in each instance, irrespective of the responsiveness, or half-life, of the volatility estimates used". It also improves vol-of-vol and the left tail. BTC's volatility range has fallen from 20–130% in the 2010s to about 30–60% in the 2020s — [Man Group, "Crypto: too hot to handle"](https://www.man.com/insights/crypto-too-hot-to-handle).
- **AdaptiveTrend (Nguyen, arXiv 2602.11708, Feb 2026).** This is a single-author, unreviewed paper from "Talyxion Research". Its headline Sharpe of 2.41 comes with in-sample monthly parameter optimisation, so it carries low credibility. Its *benchmark* table for Jan 2022 to Dec 2024 is more useful:

  | Strategy | Return | Vol | Sharpe | MDD |
  |---|---|---|---|---|
  | Vol-scaled TSMOM (10% vol target) | 22.8% | 10.0% | 1.83 | −16.1% |
  | TSMOM-1M | 18.4% | 21.3% | 0.65 | −34.8% |
  | TSMOM-3M | 15.1% | 19.7% | 0.54 | −38.2% |
  | BTC buy-and-hold | 12.6% | 48.7% | 0.17 | −64.1% |

  Costs were 4 bps taker plus slippage plus funding — [arXiv 2602.11708](https://arxiv.org/abs/2602.11708).
- **Momentum Trading in Cryptocurrencies (Baltic Journal / VU, 2025–26).** Covering eight coins from Jan 2020 to Oct 2025, it reports TSMOM at 31.96% a year *gross*. The authors state that costs, slippage and funding are excluded — [VU journal](https://www.journals.vu.lt/BATP/en/article/view/44540).
- **Within-paper lookback evidence.** On BTC, the 5–30-day models had the best gross Sharpe (1.55–1.66). The 150d and 250d alphas were not significant. Short lookbacks suffer most from costs (5d CAGR 34% → 18% at 50 bps) — [Catching Crypto Trends PDF](https://concretumgroup.com/wp-content/uploads/2026/02/Catching-Crypto-Trends.pdf).

### Inferences
- The short-horizon persistence in Liu and Tsyvinski matches the paper's finding that 5–30-day lookbacks were strongest. This argues *against* dropping the short lookbacks to save costs unless the venue's real cost exceeds about 25 bps a side.
- Choosing a lookback subset from Table 1 would be selection on in-sample results. The nine-lookback equal-weight ensemble is the pre-committed rule, and changing it adds trials.
- No 2025–2026 out-of-sample evidence for this ensemble exists in what I could access. The agent's own replication through Sep 2026 (out of sample relative to the paper's March 2025 cut-off) will be the only genuine OOS evidence, and should be reported separately.

### Gaps
- I did not obtain full text of Detzel, Liu, Strauss, Zhou and Zhu (technical analysis and Bitcoin moving-average predictability, Financial Management).
- I found no paper titled Huang et al. "Time-series momentum in crypto". This may be a mis-citation, so it should not be cited.
- I found no independent post-March-2025 performance of the Concretum ensemble, and no Concretum update after publication.
- No source covers the funding drag of a vol-targeted long/flat perp trend strategy specifically.

## Q3. Do sleeve-level drawdown stops help or hurt trend strategies? What stop is defensible given expected 20–30% drawdowns?

### Takeaway
The theory and statistics argue against a −7.5% halve and −15% latched-off stop for a sleeve with roughly 15% vol and a realistic Sharpe of about 0.8. Under a trend-strategy model, a healthy sleeve's *median* 3-year maximum drawdown is about 18–19%, and its one-in-ten worst case is about 33%. A −15% latched stop would therefore switch off a working strategy roughly half the time within 2–3 years. The defensible design is to **set dollar risk through `b.vol_target` and capital allocation, and put the hard stop at the statistical "worry line" of about 30%**, consistent with `kill.b_backtest_max_dd`.

### Cited Findings
- **Rej, Seager and Bouchaud (CFM, arXiv 1707.01457), drifting Brownian PnL over 10 years.**
  - Fitted rules of thumb, in units of annual volatility: the 5% extreme drawdown *depth* is d5% = 1.50 × SR⁻¹, and the 5% *length* is ℓ5% = 2.14 × SR⁻² years.
  - For SR = 0.5 there is "a 5% chance for the drawdown … to last 7 years or more".
  - "Both managers and investors tend to underestimate the length and depth of drawdowns consistent with the Sharpe ratio."
  - [arXiv 1707.01457](https://arxiv.org/abs/1707.01457)
- **Landolfi (arXiv 2608.00127, Jul 2026), Monte Carlo extension of Rej et al. ("decision panel").** Maximum drawdown in annual-vol units, median → 90th percentile:

  | Sharpe | 1 year | 2 years | 3 years |
  |---|---|---|---|
  | 0.5 | 0.90 → 1.57 | 1.23 → 2.11 | 1.45 → 2.45 |
  | 1.0 | 0.77 → 1.32 | 1.01 → 1.68 | 1.16 → 1.89 |

  - For a "Trend / breakout" archetype at Sharpe 1 over 3 years, MDD is 1.24 (median) and 1.97 (90th percentile). The 3-year 90th-percentile MDD for trend is 2.62 at S = 0.5, 1.98 at S = 1.0 and 1.57 at S = 1.5.
  - Uncertainty about the Sharpe ratio alone lifts near-worst MDD by about 1.23×.
  - A reading past the 90th percentile is "the evidence-based moment to revise the Sharpe down or cut".
  - [arXiv 2608.00127](https://arxiv.org/abs/2608.00127)
- **Kaminski and Lo, "When do stop-loss rules stop losses?" (J. Financial Markets 2014).**
  - Under a random walk, simple 0/1 stop-loss rules *always reduce* expected return. They add value only when returns show momentum (positive serial correlation).
  - Empirically (1950–2004), certain stop rules added 50–100 bps a month to a buy-and-hold equity portfolio *during stop-out periods*.
  - [MIT DSpace](https://dspace.mit.edu/handle/1721.1/114876); [IDEAS](https://ideas.repec.org/a/eee/finmar/v18y2014icp234-254.html)
- **Grossman and Zhou (Mathematical Finance 1993).** With a constraint that wealth stays above α × its running maximum, the optimal CRRA policy invests in risky assets in proportion to the *surplus* W_t − αM_t. This is a smooth, CPPI-like de-risking, not a binary switch. The model assumes continuous, costless rebalancing. "Decayed drawdown" variants trade drawdown control for growth — [IDEAS](https://ideas.repec.org/a/bla/mathfi/v3y1993i3p241-276.html).
- **Man Group's Harvey, Hoyle, Rattray and Van Hemert.** They study drawdown controls among other risk-management tools, with an out-of-sample COVID-2020 test. I found only summaries, not the drawdown-control result itself — [CXO summary](https://cxoadvisory.com/volatility-effects/effectiveness-of-various-risk-controls-during-the-covid-19-crash); [Man book page](https://www.man.com/sites/default/files/uploads/embed/strategic-risk-management-book/index.html).
- **Current code behaviour.** `RiskManager` halves at `drawdown_halve = 0.075` and goes Off at `drawdown_off = 0.15`, and **Off is latched until manual_reset()** (`trading-agent/src/risk.cpp`, `include/ta/risk.hpp`). `b_kill_rule` fires Off when drawdown exceeds 1.5 × `kill.b_backtest_max_dd` (default 0.19, so 28.5%) (`trading-agent/src/monitor.cpp`, `src/paper.cpp`). With these defaults, **the 15% latched stop always fires before the 28.5% kill rule can be reached**, so the two documents' rules are not just in tension: the kill rule is dead code.

### Inferences (author's arithmetic, U)
- **Sleeve volatility.** BTC Combo vol is 17% and ETH is 16% (Table 3). With an assumed strategy correlation of about 0.6, an equal-capital BTC+ETH sleeve has vol of roughly 15%.
- **Healthy-strategy drawdowns at a realistic Sharpe of about 0.8** (50% haircut from 1.56; trend archetype interpolated between S = 0.5 and S = 1):
  - median 1-year MDD ≈ 0.8 vol ≈ 12%;
  - median 3-year MDD ≈ 1.3 vol ≈ 19%;
  - 90th-percentile 3-year MDD ≈ 2.2 vol ≈ **33%**.
  - Rej et al.'s d5% = 1.5/0.8 = 1.9 vol ≈ 28% (10-year horizon, last drawdown).
  - All of these match the rulebook's "plan for 25–30% drawdowns".
- **What the current stops do to a healthy sleeve.**
  - −7.5% (≈ 0.5 vol) will be crossed in most single years.
  - −15% (≈ 1.0 vol) is about the *median* 2-year MDD at S = 1, so it fires on a healthy strategy about half the time within two years and then latches.
  - Halving at −7.5% also slows recovery to the high-water mark, because half-size gains take twice as long. This makes the latched −15% more likely after a halving.
- **Why stops fit trend sleeves poorly.** Under Kaminski and Lo, a stop on a *strategy's* equity curve helps only if that strategy's PnL is serially correlated. A trend sleeve already de-risks internally: its Donchian stops exit losing trades within −5% to −11% per unit (Table 2), and vol targeting cuts size as vol rises. A second outer stop mostly cuts exposure after losses have already happened, and misses the recovery trend that historically produced the stair-step equity curve (Fig. 5). I found no study showing that sleeve-level drawdown stops improve net Sharpe for diversified trend followers.
- **The fix is sizing, not stops.** Volatility scaling keeps Sharpe roughly the same when the 2× and 1× caps do not bind (the reviewer found the 1× cap never binds at 0.25). So `b.vol_target` and sleeve capital, not the stop, should make a 30% sleeve drawdown fit the active-book budget. For example, if sleeve B is 15% of the active book, a 30% sleeve drawdown costs 4.5% of the active book, inside the book-level halve at 10% and stop at 20%.
- **Smooth de-risking instead of a switch.** If de-risking is wanted, a Grossman–Zhou or CPPI-style linear taper is better grounded than a binary halve: exposure multiplier = clamp((0.30 − DD)/0.10, 0.5, 1) for DD between 20% and 30%. This adds one trial if adopted.

### Gaps
- I found no crypto-specific empirical study of sleeve-level drawdown stops on trend strategies.
- I could not read the Man Group drawdown-control results.
- The Landolfi Monte Carlo figures are a 2026 unreviewed preprint, though they reproduce Rej et al.'s analytic d5% = 1.498/SR to three figures.

## Q4. How does funding affect perp-based trend returns, and do funding or open-interest crowding signals predict crashes?

### Takeaway
High crypto carry (the futures basis, closely tied to perp funding) is documented to predict crashes (BIS WP 1087). Perp mispricing and funding levels have fallen sharply since 2022. Minute-level evidence on seven liquidation cascades from 2022 to 2025 finds **no reliable per-event early warning**, including for the October 2025 $19B event. A funding-plus-OI halving gate is defensible as an unoptimised risk overlay. Its threshold should not be tuned on the few historical events.

### Cited Findings
- **BIS WP 1087 (Schmeling, Schrimpf, Todorov, Apr 2023).**
  - Crypto carry averages "above 10% annually" and can reach "up to 60% p.a.".
  - It is driven by "trend-chasing and attention by smaller investors seeking leveraged upside exposure … in boom periods" and by scarce arbitrage capital.
  - "A high crypto carry predicts future price crashes", and rises in carry go with higher prices of crash insurance.
  - The PDF was behind a JS challenge, so exact coefficients are unavailable.
  - [BIS page](https://www.bis.org/publ/work1087.htm); [Mondo Visione reproduction of the BIS summary](https://mondovisione.com/media-and-resources/news/bis-crypto-carry-202344/)
- **He, Manela, Ross and von Wachter, "Fundamentals of Perpetual Futures" (arXiv 2212.06888 v7, Sep 2026).** Perp-spot deviations from no-arbitrage have a mean absolute value of about 60–90% a year across coins, with standard deviation over twice the mean. "After 2022, the average deviation reduces by almost 80% with volatility shrinking by more than 50% for most tokens" — [arXiv 2212.06888](https://arxiv.org/abs/2212.06888).
- **Garcia Seuma (arXiv 2607.27070, Jul 2026, unreviewed).**
  - He studies seven BTC cascades (May 2022, Nov 2022, Aug 2024, Dec 2024, Feb 2025, Apr 2025, Oct 2025) using minute prices and 5-minute leverage and order-flow data.
  - "No variable is event-invariant." Only taker order-flow variance compression survives a placebo test, as "a population-level precursor, not a per-event alarm".
  - The October 2025 event (more than $19B liquidated across about 1.6M accounts, triggered by a 100% China tariff announcement) "turns out to be the outlier".
  - [arXiv 2607.27070](https://arxiv.org/abs/2607.27070)
- **Pre-crash conditions, October 2025 (search-engine summary, not verified).** One summary, apparently of an FTI Consulting article, said BTC and ETH perp funding rose from about 10% to nearly 30% annualised by 6 Oct 2025, and BTC open interest fell by about $20B in a day. I could not verify this because the FTI page was blocked — [FTI Consulting (unverified)](https://www.fticonsulting.com/insights/articles/crypto-crash-october-2025-leverage-met-liquidity).

### Inferences
- **The October 2025 case and the 30% threshold.** If the roughly 30% figure is right, the October 2025 build-up only just reached a 30%-a-year gate. This illustrates the threshold's coarseness rather than validating it.
- **Gate frequency.** Since funding and basis levels have fallen about 80% after 2022, a 30% gate will seldom fire today. Testing 20% is reasonable as a sensitivity, but adopting whichever threshold backtests best would overfit to a handful of events.
- **Funding is a direct cost for long/flat perp trend.** At average carry above 10% and a typical Combo exposure of about 0.2–0.4×, the drag is roughly 2–4% a year in boom years and less after 2022. This should come from actual venue funding history, not a default. The code's `funding_annual_default = 0` understates cost on days without data; use a conservative default such as 0.10 when history is missing.

### Gaps
- I could not obtain BIS crash-regression coefficients or thresholds.
- I found no academic evidence on INR-settled Indian perp funding levels or how they track offshore funding.
- I found no academic paper specifically on "funding > X% with rising OI" as a de-risking rule for trend strategies.

## Q5. Parameter-by-parameter recommendation (config key → proposed value; trials impact)

### Takeaway
Keep the published signal and ensemble unchanged, fix the stop-timing bug, keep the vol target and cap, and raise the cost base for INR venues. Resolve the drawdown conflict by moving the sleeve stop to about 30% and aligning it with a replication-derived kill threshold. Only the band-type test, any funding-threshold change and any taper rule add trials.

### Cited Findings
- The paper's base is 10 bps, 20% threshold, 0.25 vol target, 2× per-lookback cap and nine lookbacks. Its net BTC and ETH MDDs print as 19% and 15%, while MAR implies about 26% and 28% — [PDF](https://concretumgroup.com/wp-content/uploads/2026/02/Catching-Crypto-Trends.pdf).
- The 1× cap bound on 0.0% of days for BTC, ETH and BNB — [GitHub review](https://github.com/alfred1123/Quant_Strategies/pull/61).
- At a 3-year horizon, the 90th-percentile trend MDD is about 2.0–2.6 vol for Sharpe 0.5–1 — [arXiv 2608.00127](https://arxiv.org/abs/2608.00127).
- Under a random walk, stop-losses always reduce expected return — [Kaminski and Lo](https://dspace.mit.edu/handle/1721.1/114876).

### Inferences (proposals, U)

| Key | Current | Proposed | Rationale | Adds a trial? |
|---|---|---|---|---|
| `b.vol_target` | 0.25 | **0.25 (keep)**; lower the *sleeve capital* or set 0.20 only to fit the rupee drawdown budget | Pure scaling while caps do not bind, so Sharpe is unchanged; this is the right lever for dollar risk (Man: vol scaling adds about 0.4 Sharpe for BTC) | No (scaling only), as long as 0.25/σ < 2 and the 1× cap does not bind |
| `b.asset_cap` | 1.0 | **1.0 (keep)** | Never binds at 0.25; serves only as a legal and venue limit | No |
| `b.rebalance_band` | 0.20 relative | **0.20 relative (keep)**; run absolute 0.20 as a labelled sensitivity only | Paper text is ambiguous; relative is the only reading consistent with "~100 bps/yr recovered" | Yes, +1 if both are tested and one chosen; keep relative pre-committed |
| `b.cost_bps` | 10 | **25 base, 50 stress, 10 as the optimistic paper case** | The paper's 10 bps is for global venues where BTC fees are under 5 bps; INR perp venues add GST on fees and wider spreads (cost level unsourced, see gap) | No (an assumption, not a selected parameter) |
| `b.drawdown_halve` | 0.075 | **0.20**, or replace with the linear taper above between 20% and 30% | 7.5% ≈ 0.5 vol, crossed most years by a healthy sleeve | The taper adds +1; a fixed 0.20 set ex ante does not, if not optimised |
| `b.drawdown_off` | 0.15 (latched) | **0.30**, latched to human review with a written re-entry rule | ≈ 2.0 vol ≈ 90th-percentile 3-year MDD at S ≈ 0.8; matches the rulebook's "plan for 25–30%" | No if set ex ante |
| `kill.b_backtest_max_dd` | 0.19 | **The agent's own replicated net Combo MDD for the BTC+ETH sleeve, 2015–Mar 2025; provisional 0.20** (so 1.5× ≈ 30% coincides with `b.drawdown_off`) | The paper's 19% conflicts with its own MAR (26–34% implied); the kill rule is unreachable today | No |
| `regime.funding_red_annual` | 0.30 | **0.30 (keep, unoptimised)**; report 0.20 as a sensitivity only | BIS supports carry-predicts-crash; post-2022 funding is about 80% lower; cascade warnings are unreliable per event | +1 if changed on backtest results |
| `b.funding_annual_default` (code) | 0 | **0.10** when venue history is missing | BIS: average carry above 10% a year | No |
| Lookbacks | {5,…,360} | **Keep all nine** | Ensemble is pre-committed; short lookbacks are supported by Liu and Tsyvinski | Dropping any adds a trial |
| Stop timing (code) | Today's Mid in today's exit test | **Use TS_t = max(TS_{t−1}, Mid_{t−1})** | Matches paper §4.1 and footnote 3 | No (bug fix) |

- **Resolving the drawdown conflict.** Adopt one coherent rule: halve (or taper) at 20%, off for human review at 30%, and a kill threshold equal to 1.5 × the replicated backtest MDD, aligned at about 30%. Make sleeve B's capital share small enough that a 30% sleeve drawdown is under about 5% of the active book. The blueprint's generic −7.5% and −15% should be marked as not applicable to sleeve B, with this evidence cited.
- **Acceptance gate.** BTC trade counts must be within ±5% of Table 2 *after* the stop-timing fix. Set the MDD gate from the replication, not from the paper.

### Gaps
- I found no sourced Indian INR-perp fee and spread figures; the 25 bps base is a judgement.
- The BTC–ETH strategy correlation (0.6) used for the 15% sleeve-vol estimate is an assumption. The replication should measure it.
- No post-publication out-of-sample evidence for the paper exists.
