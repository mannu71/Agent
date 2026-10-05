# Sleeve A1 (NSE momentum-leader breakouts): SSRN-listed evidence for parameter tuning

Scope: the A1 thresholds in `reports/Price action reading strategy rules.md` ("Sleeve A1") and `trading-agent/README.md` ("Rules implemented"), config keys as read by `trading-agent/src/settings.cpp` (`a1.min_price`, `a1.min_avg_traded_value`, `a1.top_fraction`, `a1.min_runup`, `a1.min_adr`, `a1.base_len`, `a1.max_tightness`, `a1.max_below_pivot`, `a1.entry_limit_frac`, `a1.stop_adr_mult`, `a1.partial_day`, `a1.time_stop_day`, `a1.max_hold_days`, `a1.risk_per_trade`, `a1.cost_round_trip`, `regime.*`).

Access notes (Oct 2026): SSRN, concretumgroup.com, freefincal.com, stockviz.substack.com, apps.olin.wustl.edu, MIT DSpace PDFs and journals.sagepub.com PDFs were blocked or returned challenge/HTML pages from this environment. Full texts actually read: Zarattini, Pagani and Wilcox (2025) via a Webflow CDN copy of SSRN 5084316; Han, Zhou and Zhu (2016 version) via a mirror of SSRN 2407199; George and Hwang (2004) from the author's Houston page; the IIMA Fama-French-Momentum working paper and drawdown tables; the BacktestIndia study that Freefincal re-reported. Rajan Raju's four India momentum papers (SSRN only) and Kaminski and Lo were seen as abstracts or search summaries only; numbers from them are therefore limited to what the abstracts state.

## Q1. Indian momentum evidence: formation/holding periods, skip month, crashes and fixes, liquidity/size

### Takeaway
Momentum (11-month formation with a 1-month skip, and 52-week-high nearness) is a real, large but crash-prone premium in Indian data (IIMA WML about 11% a year gross, worst drawdowns of 62% and 53%); India-specific work by Raju finds that volatility-adjusted momentum has the best Sharpe ratio, 52-week-high strategies give more stable alpha with weaker reversals, and returns decay as holding/rebalancing periods lengthen. The 2026 "liquidity illusion" result splits by turnover relative to market cap (not by absolute traded value), is unrefereed, and argues against favouring heavily traded momentum names rather than for raising the absolute ₹ liquidity floor.

### Cited Findings
- IIMA Indian four-factor data (Agarwalla, Jacob and Varma): momentum return at month-end t is the 11-month return from end of t−12 to t−1 (one-month skip); winners/losers are the top/bottom 30% by that return, crossed with size; WML is the average of small and big winner-minus-loser legs, rebalanced monthly — [IIMA working paper](https://faculty.iima.ac.in/iffm/Indian-Fama-French-Momentum/four-factors-India-90s-onwards-IIM-WP-Version.pdf)
- Over Oct 1993 – Dec 2014 the average annual momentum (WML) factor return was 21.9%; the authors note WML is "not strictly comparable" to other factors because monthly rebalancing implies higher trading cost (returns are gross of costs) — [IIMA working paper](https://faculty.iima.ac.in/iffm/Indian-Fama-French-Momentum/four-factors-India-90s-onwards-IIM-WP-Version.pdf)
- Updated IIMA drawdown tables (data run into 2025): WML annualised return 10.96%, volatility 17.30%; worst drawdowns −62.2% (Mar 2000 → trough Dec 2000 → recovered Mar 2005), −53.1% (Dec 2008 → trough Sep 2009 → recovered Dec 2011), −44.6% (1997–98), −37.3% (Aug 2013 → May 2014), −28.2% (Mar 2020 → Dec 2020 → recovered Jan 2022) — [IIMA drawdown tables](https://faculty.iima.ac.in/~iffm/Indian-Fama-French-Momentum/drawdown.php)
- Earlier IIMA/Vikalpa summary: WML 17.3% annualised with 17.06% volatility; worst drawdown −48.5% during the post-GFC rally (13 Mar 2009 – 16 Aug 2011), "momentum tends to underperform during sharp rallies that follow a market crash" — [Vikalpa 2017, Agarwalla, Jacob & Varma](https://journals.sagepub.com/doi/full/10.1177/0256090917733848) (search summary; PDF blocked). Note the different WML figures (21.9%, 17.3%, 10.96%) reflect different sample ends and return conventions.
- US crash mechanism: momentum crashes are partly forecastable, occurring in "panic states" following market declines and when market volatility is high, contemporaneous with market rebounds — [Daniel & Moskowitz, JFE 2016](https://www.kentdaniel.net/papers/published/jfe_16.pdf) (search summary). The Indian 2009 and 2020 WML drawdowns match this timing (IIMA table above).
- Raju, "Shades of Momentum" (Dec 2008 – Sep 2024, Indian equities, decile and long-short portfolios, EW and MW): academic, volatility-adjusted, idiosyncratic and information-discreteness momentum all work; volatility-adjusted momentum delivers the highest Sharpe ratio; extending holding from 3 to 12 months weakens the effect significantly — [SSRN 4977717](https://papers.ssrn.com/sol3/papers.cfm?abstract_id=4977717) (abstract only; numbers not accessible)
- Raju, "Timing the Tide" (Jan 2024): shorter rebalancing periods capture academic momentum more effectively in Indian equities — [SSRN 4687044](https://papers.ssrn.com/sol3/papers.cfm?abstract_id=4687044) (abstract only)
- Raju, "The 52-Week High Effect and Momentum Investing: Evidence from India" (Oct 2004 – Aug 2023): stocks near 52-week highs earn higher returns and Sharpe ratios after controlling for size, across weighting schemes; "more stable alpha than academic momentum"; weaker long-term reversals — [SSRN 4587697](https://papers.ssrn.com/sol3/papers.cfm?abstract_id=4587697) (abstract only)
- Raju & Chandrasekaran (2019): a monthly-rebalanced long-only top-decile momentum portfolio from the NIFTY100 significantly outperforms the NIFTY100, with higher volatility and occasional crashes — [SSRN 3510433](https://papers.ssrn.com/sol3/papers.cfm?abstract_id=3510433) (abstract only)
- "Liquidity illusion" study (BacktestIndia, re-reported by Freefincal, March 2026): universe NSE top 200 with PE > 0; top 30 momentum stocks split into two groups of 15 by Scaled Turnover = daily volume ÷ market cap; annual December rebalance; costs 0.11% + 0.05% slippage; STCG 20%/LTCG 12.5% applied; Dec 2006 – Dec 2025 (one block says Jun 2025). Net CAGR / vol / max DD / recovery: low-turnover 19.43% / 22.87% / −66.41% / 61 months; base 30-stock momentum 14.60% / 23.02% / −70.61% / 67 months; Nifty 50 10.41% / 20.56% / −55.12% / 60 months; high-turnover 8.51% / 25.18% / −75.09% / 100 months — [BacktestIndia](https://backtestindia.com/blog/momentum-factor-india-liquidity-premium-scaled-turnover); [Freefincal](https://freefincal.com/is-indian-momentum-investing-just-a-liquidity-illusion/)
- Consistent US evidence on turnover: firms with high past turnover show glamour characteristics and lower future returns, and "high volume winners experience faster reversals" — [Lee & Swaminathan, JF 2000](https://www.johnhcochrane.com/s/lee_swaminathan_returns_volume_JF.pdf) (abstract via [IDEAS](https://ideas.repec.org/a/bla/jfinan/v55y2000i5p2017-2069.html))
- Volatility scaling: momentum risk is time-varying and predictable; scaling by trailing 6-month realised variance "virtually eliminates crashes and nearly doubles the Sharpe ratio" (US, 1926–2011) — [Barroso & Santa-Clara, JFE 2015, via CXO](https://www.cxoadvisory.com/momentum-investing/avoiding-momentum-strategy-crashes/); [UNSW BusinessThink](https://www.businessthink.unsw.edu.au/articles/next-wave-yes-momentum-investing-can-be-rewarding)

### Inferences
- A1's composite score (vol-adjusted 6m and 12m return, close/52-week high, 63-day return) is directionally supported in India: vol-adjusted momentum has the best Sharpe (Raju) and 52-week-high nearness is the more stable signal (Raju; George & Hwang, Q2). Keep `a1.top_fraction` = 0.20 (between the 30% academic cut and the 10–15% index cuts); no evidence favours changing it, so changing it would be a pure trial.
- Skip month: the academic Indian factor uses 12-1. A1 deliberately wants recent strength (63-day return, 30% run-up), which is a breakout design choice, not a momentum-literature one. A low-cost robustness trial is to compute the 12-month component as 12-1 (one trial); keep the 63-day term since the breakout trigger needs it.
- Holding: cross-sectional Indian momentum decays over 3–12 months (Raju), which supports a cap on holding — but see Q2 for trade-level evidence that the right tail needs long holds. These do not conflict: decay is about average portfolio alpha; tail capture is about a minority of trades.
- Crash protection: A1's regime gate (`regime.trend_sma`, `regime.vol_red_pct`) aligns with the Daniel–Moskowitz state dependence and Indian 2009/2020 crashes; keep it. Note the crash risk is mainly the short (loser) leg in long-short WML; a long-only sleeve is exposed mainly to sharp reversals in winners during rebounds.
- Liquidity: the 8.51% vs 19.43% result is about *relative* turnover inside large caps, not about the ₹10 crore floor. It is unrefereed, uses one split, 15-stock portfolios and annual rebalancing, so it is weak evidence. But it plus Lee & Swaminathan imply that A1's tendency to select high-ADR, heavily traded leaders is a risk. Recommend logging Scaled Turnover per trade and testing an exclusion of the top tercile by volume/market-cap (one trial), rather than raising `a1.min_avg_traded_value`.

### Gaps
- Raju's papers' numeric results (Sharpe by metric, decay per holding period, 52-week-high alpha, cost treatment, OOS split) could not be read: full texts are SSRN-only and SSRN serves a bot challenge.
- No Indian residual (idiosyncratic) momentum crash study with net-of-cost numbers was found beyond Raju's abstract; no Indian test of Barroso–Santa-Clara scaling was read in full.
- Freefincal's page itself was blocked; numbers are from the BacktestIndia original it reports.

## Q2. Breakout / 52-week-high / all-time-high entries, ATR stops and trailing exits that survive costs

### Takeaway
The best-documented mechanical analogue is Zarattini, Pagani & Wilcox (2025): all-time-high close → buy next open, exit on a ratcheting stop of ATH − 10×ATR(42), 0.50% round-trip cost, 43.9% win rate, avg +0.50R/trade, with profits concentrated in fewer than 7% of trades whose winners last on average 370 days. That evidence supports A1's 52-week/ATH-proximity selection and its 0.5% cost assumption, but it argues that A1's 120-day cap, day-20 time stop and day-3 partial cut the very right tail that pays for trend trading.

### Cited Findings
- Zarattini, Pagani & Wilcox, "Does Trend Following Still Work on Stocks?" (SSRN 5084316): survivorship-bias-free US data (Norgate), all liquid stocks 1950 – Nov 2024, >66,000 trades; OOS = 2005–2024 (after the Wilcox & Crittenden 2005 white paper) — [PDF](https://cdn.prod.website-files.com/6666e810f1ece690fd70bf82/688d233e0fe06c2b1af5e460_Does%20Trend%20Following%20Still%20Work%20on%20Stocks_ssrn-5084316_%20(2).pdf)
- Universe filters: unadjusted close > $10 and 42-day average dollar volume > $1,000,000 (CPI-deflated back in time: ~$600k in 2005, $300k in 1980); positions that later fail the filters are held until the stop — [PDF](https://cdn.prod.website-files.com/6666e810f1ece690fd70bf82/688d233e0fe06c2b1af5e460_Does%20Trend%20Following%20Still%20Work%20on%20Stocks_ssrn-5084316_%20(2).pdf)
- Entry: close ≥ highest adjusted close in history → buy at next open. Exit: trailing stop = ATH_t × (1 − 10·ATR42_t/ATH_t), i.e. ATH − 10×ATR(42), updated daily, never lowered; close below → sell next open. Costs: 0.50% per round turn — [PDF](https://cdn.prod.website-files.com/6666e810f1ece690fd70bf82/688d233e0fe06c2b1af5e460_Does%20Trend%20Following%20Still%20Work%20on%20Stocks_ssrn-5084316_%20(2).pdf)
- Trade statistics: average +0.50R per trade; average winner +1.90R; average loser −0.70R; win rate 43.90%; winners' average duration 370 days; trades held ~2 years often ≈ +2R while trades < 1 year cluster below 1R; 8% of trades lost more than 1R (overnight gaps); 22% made more than +1R; 56% of trades lost, ~37% roughly broke even, and fewer than 7% generated all profits — [PDF](https://cdn.prod.website-files.com/6666e810f1ece690fd70bf82/688d233e0fe06c2b1af5e460_Does%20Trend%20Following%20Still%20Work%20on%20Stocks_ssrn-5084316_%20(2).pdf)
- Decay: average PnL per trade fell from 0.39R pre-2005 to 0.31R post-2005, partially offset by more trades (note: these yearly-average figures differ from the 0.50R pooled average; the paper does not reconcile them) — [PDF](https://cdn.prod.website-files.com/6666e810f1ece690fd70bf82/688d233e0fe06c2b1af5e460_Does%20Trend%20Following%20Still%20Work%20on%20Stocks_ssrn-5084316_%20(2).pdf)
- Portfolio: 1991–2024 gross CAGR 15.19%, annualised alpha 6.18%; the base system is "not viable" for AUM < $1M after costs because of daily turnover; a Turnover Control algorithm restores viability — [PDF abstract](https://cdn.prod.website-files.com/6666e810f1ece690fd70bf82/688d233e0fe06c2b1af5e460_Does%20Trend%20Following%20Still%20Work%20on%20Stocks_ssrn-5084316_%20(2).pdf); a search summary instead quotes CAGR 15.02%, alpha 6.19%, max DD 31.75% — [search summary of the same paper/AAII](https://www.aaii.com/investor-update/article/277265-all-time-high-trend-strategies-are-not-without-costs-and-risks) (minor conflict, likely version difference)
- George & Hwang (JF 2004), US July 1963 – Dec 2001, top/bottom 30%, 6-month hold, with a skip: 52-week-high (price ÷ 12-month high) WML 0.45%/month vs JT 6-month momentum 0.48% (all months); excluding Januaries 1.23% (t = 7.06) vs 1.07% (t = 6.97); 52-week-high nearness "dominates and improves upon" past returns and its forecasts "do not reverse in the long run" — [George & Hwang PDF](https://www.bauer.uh.edu/tgeorge/papers/gh4-paper.pdf)
- International: the 52-week-high strategy is profitable in 18 of 20 markets, significant in 10 — [Liu, Liu & Ma, JIMF 2011 (IDEAS)](https://ideas.repec.org/a/eee/jimfin/v30y2011i1p180-204.html) (search summary; India coverage not confirmed)
- India: Raju (2004–2023) finds the 52-week-high effect robust and more stable than academic momentum — [SSRN 4587697](https://papers.ssrn.com/sol3/papers.cfm?abstract_id=4587697) (abstract)

### Inferences
- Liquidity floor: $1M ADV ≈ ₹8.5–8.8 crore at an assumed ₹85–88/USD (author's arithmetic, FX assumed). A1's ₹10 crore 20-day floor is therefore in line with the only full-text breakout study's floor; **keep `a1.min_avg_traded_value` = 1e8 (₹10 crore)**. The $10 price floor targets microstructure noise; there is no India-specific evidence on ₹100 — **keep `a1.min_price` = 100** (no evidence to change; changing it is a trial).
- Selection: ATH/52-week-high proximity is the best-supported component in both US and India; A1's `a1.max_below_pivot` = 0.15 (close within 15% of base high) and the close/52w-high score term are consistent. Keep.
- Exits: the ZPW system has an *initial* risk of ~10×ATR42, against A1's 1.0×ADR20 (roughly 1 ATR). A1 is a different (short-horizon, tight-stop) design, so ZPW cannot validate `a1.stop_adr_mult` = 1.0; but its evidence that profits come from <7% of trades held ~1 year implies A1's hard cap of 120 days and early profit-taking truncate the tail. Proposed variant: for the final runner portion, replace the SMA10/SMA20 trail with a ratcheting high-water − k×ATR(42) stop (k ∈ {3, 5, 10}) and lift the max hold.
- **`a1.max_hold_days`: raise from 120 to 250 (test; 1 trial)**, or remove it when the ATR trail variant is used. No source supports 120.
- Costs: ZPW uses 0.50% round trip for US stocks and still finds a profit, so A1's 0.5% base / 1.0% stress (`a1.cost_round_trip`) is not generous; keep.

### Gaps
- The CDN copy read appears to contain the trade-level part only; ATR-multiple sensitivity tables (e.g. 5× vs 10×), the Turnover Control details and net-of-cost portfolio tables by AUM were not in the extracted text, and concretumgroup.com was blocked. No ATR-multiple optimisation evidence was obtained.
- No full-text Indian ATH/52-week-high *breakout-entry* (event) study with costs was found; Indian 52-week-high evidence is portfolio-sort only.

## Q3. Stop-loss rules and time stops in momentum

### Takeaway
Stop-losses add value precisely when returns are serially correlated (momentum), per Kaminski & Lo; in US momentum, a 10–15% stop measured from the formation price, checked on daily lows, roughly doubles the long-short Sharpe ratio and cuts the worst month from −49.79% to about −15%, with break-even extra costs of 3–5%. Most of that gain comes from the short (loser) leg, so the benefit to a long-only sleeve is smaller. No academic evidence was found for a "day-20 below +1R" time stop or a "sell 1/3 on day 3" partial.

### Cited Findings
- Han, Zhou & Zhu, "Taming Momentum Crashes: A Simple Stop-Loss Strategy" (SSRN 2407199, Sep 2016 version), CRSP Jan 1926 – Dec 2013, monthly 11/1 momentum deciles — [PDF mirror](https://smallake.kr/wp-content/uploads/2017/03/SSRN-id2407199.pdf)
- Rule: P0 = month-start price; each day check worst return from P0 using the day's low; when ≤ −L, place a sell limit the next day at (1 − 0.5L)×(1 − L)×P0, else sell at the close; proceeds earn T-bills for the rest of the month (stop resets monthly) — [PDF](https://smallake.kr/wp-content/uploads/2017/03/SSRN-id2407199.pdf)
- Results (EW WML): original SD 6.01%, monthly Sharpe 0.165, worst month −49.79%; L = 15%: mean 1.93%/mo, SD 4.85%, Sharpe 0.399, worst −17.43% (VW: −64.97% → −22.10%); L = 10%: mean 2.32%/mo, SD 4.61%, Sharpe 0.504, skewness 1.54, worst −15.37%; L = 20% similar; "loss reduction ... mainly coming from the losers portfolio" — [PDF](https://smallake.kr/wp-content/uploads/2017/03/SSRN-id2407199.pdf)
- Turnover/costs: monthly turnover rises from 45.05%/39.11% (losers/winners) to 58.57%/49.08%; break-even extra transaction costs 4.00% (equal mean), 4.82% (equal Sharpe), 3.18% (insignificant difference); 7.18%/8.65%/5.71% using median turnover increases — [PDF](https://smallake.kr/wp-content/uploads/2017/03/SSRN-id2407199.pdf)
- Kaminski & Lo (J. Financial Markets 18, 2014, 234–254): under the Random Walk Hypothesis, simple 0/1 stop-loss rules always reduce expected return; with momentum they can add value; at longer sampling frequencies some stop policies raise expected return while substantially reducing volatility; US 1950–2004, 10% stop switching into long-term bonds — [MIT DSpace record](https://dspace.mit.edu/handle/1721.1/114876); [SIFR WP abstract](https://swopec.hhs.se/sifrwp/abs/sifrwp0063.htm) (abstract and search summaries only)
- Trend-following trade data show long-held trades produce the right tail (winners average 370 days; <7% of trades produce all profit) — [ZPW PDF](https://cdn.prod.website-files.com/6666e810f1ece690fd70bf82/688d233e0fe06c2b1af5e460_Does%20Trend%20Following%20Still%20Work%20on%20Stocks_ssrn-5084316_%20(2).pdf)

### Inferences
- Initial stop: A1's 1.0×ADR (≈ 4–8% for qualifying stocks with ADR 4–8%) is tighter than the HZZ 10–15% levels. HZZ's 10% is a fixed percentage from formation price, monthly reset; A1's is volatility-scaled per trade. There is no direct evidence to move `a1.stop_adr_mult`; **keep 1.0** (rulebook's skip rule at > 1.0×ADR and 1.5× absolute cap stays). A test of 1.5 is a reasonable single trial given HZZ's finding that 10/15/20% all work similarly (low sensitivity).
- The HZZ benefit is concentrated in the short leg; for long-only A1 expect a smaller crash benefit from stops, so the regime gate remains the primary crash defence.
- `a1.time_stop_day` = 20 (exit if < +1R): no supporting evidence; ZPW shows short trades cluster below 1R yet the portfolio is profitable because a few long ones run. A time stop that culls laggards is plausible but unproven. **Keep as design (U), add one trial with the time stop off** to measure its effect.
- `a1.partial_day` = 3 (sell 1/3): practitioner rule only; it lowers exposure to the right tail ZPW identifies. **Keep as primary; one trial with partial off.** If trades must be cut, the rulebook's "test day 5 and 1/2" adds two more trials.

### Gaps
- No full text of Kaminski & Lo was obtained (MIT PDF returned an HTML page); their stopping-premium magnitudes are not reported here.
- No study of time stops (exit if not +kR by day N) in equity momentum/breakout trading was found.
- No Indian-data stop-loss momentum study with costs was found.

## Q4. Volatility filters (ADR/ATR minimums), price/liquidity filters and transaction costs in Indian equities

### Takeaway
There is no academic evidence for a specific ADR ≥ 4% minimum; the relevant evidence cuts both ways (vol-adjusted ranking is best in India, but high-turnover/high-attention momentum names reverse faster and, in India, underperformed). Indian statutory delivery costs are about 0.23–0.25% round trip before impact, so A1's 0.5% base / 1.0% stress is reasonable for buy-stop breakout fills in mid-caps.

### Cited Findings
- Indian delivery STT 0.1% on both buy and sell; stamp duty 0.015% on the buy side; discount brokers charge zero delivery brokerage, traditional brokers 10–20 bps — [ClearTax](https://cleartax.in/s/equity-investment-cost); [Zerodha charges](https://www.zerodha.com/charges) (search summary)
- An older NSE working paper costing a ₹1 lakh trade lists STT 12.5 bps (then-rate), stamp duty 1 bp and impact cost 6 bps for large caps — [NSE WP 2](https://nsearchives.nseindia.com/research/content/NSEWP_2.pdf) (search summary; NSE site blocked; rates outdated)
- The Indian liquidity study assumed only 0.11% + 0.05% slippage per trade, for annual rebalancing of Nifty 200 names — [BacktestIndia](https://backtestindia.com/blog/momentum-factor-india-liquidity-premium-scaled-turnover)
- US breakout study used 0.50% per round turn, citing estimates of 5–25 bps (large-cap broker data), 40–70 bps round trip (spreads, largest US stocks) — [ZPW PDF](https://cdn.prod.website-files.com/6666e810f1ece690fd70bf82/688d233e0fe06c2b1af5e460_Does%20Trend%20Following%20Still%20Work%20on%20Stocks_ssrn-5084316_%20(2).pdf)
- Volatility-adjusted momentum delivers the highest Sharpe ratio in India (Dec 2008 – Sep 2024) — [Raju SSRN 4977717](https://papers.ssrn.com/sol3/papers.cfm?abstract_id=4977717)
- High-volume winners reverse faster (US) — [Lee & Swaminathan](https://ideas.repec.org/a/bla/jfinan/v55y2000i5p2017-2069.html); high relative-turnover Nifty 200 momentum stocks: 8.51% net CAGR, −75.09% max DD (India, 2006–2025, unrefereed) — [BacktestIndia](https://backtestindia.com/blog/momentum-factor-india-liquidity-premium-scaled-turnover)

### Inferences
- `a1.min_adr` = 0.04: practitioner threshold with no academic test; sizing by stop distance (risk ÷ ADR) already makes position size inverse to volatility, which is consistent with Barroso–Santa-Clara scaling. **Keep 0.04; the rulebook's planned 0.03 and 0.05 tests are 2 trials.** Watch for the high-turnover reversal risk (log volume/market-cap per trade).
- `a1.cost_round_trip` = 0.005 base, 0.01 stress: statutory part ≈ 0.2% STT + 0.015% stamp + small exchange/SEBI/GST fees (author's arithmetic from cited rates; exchange/GST amounts not sourced), leaving ~0.25% for spread, buy-stop slippage and the exit gap. **Keep.** Do not adopt the 0.16% figure used by annual-rebalance studies.
- `a1.risk_per_trade` = 0.004: no academic evidence found either way; keep (judgment). With an expected edge near 0–0.2R/trade (rulebook) and the ZPW 56% loss rate, losing streaks of 10+ are plausible; 0.40% × 10 = 4% of the active book is inside the sleeve's −7.5% halve threshold.
- `a1.min_runup` = 0.30, `a1.base_len` = 20, `a1.max_tightness` = 0.5, `a1.max_below_pivot` = 0.15, `a1.entry_limit_frac` = 0.005: practitioner-only (Kullamägi), no academic evidence found; **keep and do not tune** (each tune is a trial with no prior).

### Gaps
- No peer-reviewed Indian study of minimum-volatility (ADR/ATR) filters for momentum or breakouts was found.
- No current, sourced measurement of NSE mid-cap impact cost or buy-stop slippage at breakout was found (NSE research pages blocked).
- No study on NSE price-level filters (₹50 / ₹100 / ₹250) or on the effect of the new tick-size regime was found.

## Q5. Consolidated recommendations by config key, with trial accounting

### Takeaway
Keep almost every threshold; the evidence supports the selection layer (vol-adjusted momentum, 52-week-high nearness, ₹10 crore floor, 0.5% cost) and is silent on the Kullamägi mechanics. The one evidence-backed change is to stop truncating the right tail: lengthen `a1.max_hold_days` and test an ATR-based runner trail. Every change below is a trial for deflated-Sharpe/PBO.

### Cited Findings
- Right-tail concentration and long winner durations — [ZPW PDF](https://cdn.prod.website-files.com/6666e810f1ece690fd70bf82/688d233e0fe06c2b1af5e460_Does%20Trend%20Following%20Still%20Work%20on%20Stocks_ssrn-5084316_%20(2).pdf)
- Stop-level insensitivity (10/15/20% all similar) — [HZZ PDF](https://smallake.kr/wp-content/uploads/2017/03/SSRN-id2407199.pdf)
- Vol-adjusted and 52-week-high momentum best in India — [Raju 4977717](https://papers.ssrn.com/sol3/papers.cfm?abstract_id=4977717); [Raju 4587697](https://papers.ssrn.com/sol3/papers.cfm?abstract_id=4587697)
- Repo convention: "Every threshold marked U ... is a trial: change it, and the deflated Sharpe and PBO numbers must account for it" — `trading-agent/README.md`

### Inferences

| Config key | Current | Recommendation | Evidence basis | New trials |
|---|---|---|---|---|
| `a1.min_price` | 100 | keep | none India-specific; US analogue $10 | 0 |
| `a1.min_avg_traded_value` | ₹10 crore (20d) | keep | ≈ ZPW $1M 42-day floor at ₹85–88/$ | 0 |
| (new, diagnostic) scaled turnover cap | none | log volume/market-cap; test excluding top tercile | Lee & Swaminathan; BacktestIndia 8.51% vs 19.43% | 1 |
| `a1.top_fraction` | 0.20 | keep | academic 30%, indices 10–15% | 0 |
| score: 12-month term | 12m (no skip) | test 12-1 | IIMA, George & Hwang use a skip | 1 |
| `a1.min_runup` | 0.30 | keep (practitioner) | none | 0 |
| `a1.min_adr` | 0.04 | keep; run planned 0.03 / 0.05 | none direct | 2 |
| `a1.base_len`, `a1.max_tightness`, `a1.max_below_pivot` | 20, 0.5, 0.15 | keep | 52w-high nearness supports a near-high pivot | 0 |
| `a1.stop_adr_mult` | 1.0 | keep; optional 1.5 test | HZZ stop-level insensitivity | 0–1 |
| `a1.partial_day` | 3 (1/3) | keep; test "off" | ZPW tail evidence argues against early trimming | 1 (+2 if day 5 and 1/2 tested) |
| trailing exit | SMA10/SMA20 | keep; variant: runner on high-water − k×ATR42, k ∈ {3,5,10} | ZPW 10×ATR42 | 3 |
| `a1.time_stop_day` | 20 (< +1R) | keep (U); test "off" | none | 1 |
| `a1.max_hold_days` | 120 | **raise to 250** (or none with ATR trail) | ZPW winners avg 370 days | 1 |
| `a1.risk_per_trade` | 0.004 | keep | none | 0 |
| `a1.cost_round_trip` | 0.005 (stress 0.01) | keep | ZPW 0.50%; Indian STT/stamp | 0 (stress run is not a trial) |
| `regime.*` | as set | keep | Daniel–Moskowitz; IIMA 2009/2020 drawdowns | 0 |

- Total additional trials if all variants are run: about 10–13, on top of the 9-trial grid already in `ta_backtest --gate`. Deflated Sharpe should use the full count (≈ 19–22), and PBO/CSCV should include every variant actually run, not only the chosen one.
- Prioritise: (1) `a1.max_hold_days` 250, (2) ATR runner trail k = 10 (the single published value, avoids a 3-way search), (3) partial off, (4) time stop off. Stopping at these four keeps added trials at 4.
- Expectation management: ZPW's US post-publication edge was ~0.31R/trade with a 10×ATR risk unit, and the Indian liquid-momentum result was below the index; the rulebook's planning range of 0 to +0.2R per trade after costs remains appropriate.

### Gaps
- No out-of-sample, net-of-cost Indian test exists for any of the Kullamägi setup thresholds (`min_runup`, `min_adr`, base tightness, day-3 partial, SMA trails, day-20 time stop); these remain unverified and must be validated by the agent's own walk-forward backtest with the trial counts above.
