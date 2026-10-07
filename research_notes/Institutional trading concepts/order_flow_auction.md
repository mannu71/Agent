# Order-Flow Trading and Auction Market Theory — codable rules and evidence

Scope: delta, CVD, CVD/price divergence, absorption, stacked imbalances, footprint, exhaustion, trapped traders, large-lot prints; volume profile (POC, 70% value area, VAH/VAL, HVN/LVN, naked POC); Market Profile/TPO (initial balance, IB extensions, day types, open types, 80% rule); value migration. Target: an algorithmic paper-trading agent, crypto first (Binance 1-minute klines with taker-buy volume; execution on Delta Exchange India perps at 0.02% maker / 0.05% taker), then US and NSE equities.

Grading key used below: A = peer-reviewed evidence of profitability after costs; B = independent systematic backtests; C = practitioner claims only.

Research budget note: about 20 tool calls. Several primary PDFs (Anastasopoulos & Gradojevic, Journal of Financial Markets 2026) could not be fetched (HTTP 403/503). Their content below comes from search snippets only and is flagged.

---

## Concept catalog: plain-English meaning, mechanical definition, data needed, grade

### Takeaway
None of the listed concepts reaches grade A as a stand-alone retail strategy. Academic work shows that order-flow imbalance strongly explains price moves in the same interval. It predicts future returns only weakly, mostly at horizons of seconds to minutes. In crypto, one peer-reviewed-adjacent study finds the effect is too small to beat a 5 bp taker fee. Volume-profile and Market Profile rules have only vendor or blog statistics (grade B- at best), often without P&L or cost accounting. Everything else (absorption, stacked imbalances, exhaustion, trapped traders, Triple-A) is grade C.

### Cited Findings
**Delta / OFI / trade-flow imbalance (academic anchor)**
- Cont, Kukanov & Stoikov define order flow imbalance (OFI) as "the imbalance between supply and demand at the best bid and ask prices." They find "a linear relation between order flow imbalance and price changes, with a slope inversely proportional to the market depth", using NYSE TAQ data for 50 US stocks. The volume-based (square-root) relation was "noisy and less robust." This is a contemporaneous result, not a forecast. — [arXiv 1011.6402](https://arxiv.org/abs/1011.6402)
- On BitMEX XBTUSD perps, Silantyev found that *trade* flow imbalance (signed taker volume, i.e. delta) explains contemporaneous price change better than book OFI. — [IDEAS/RePEc, Digital Finance 2019](https://ideas.repec.org/a/spr/digfin/v1y2019i1d10.1007_s42521-019-00007-w.html)
- On Binance, taker side is identified with the `isBuyerMaker` flag: buyer-initiated when the buyer is the taker. Volume-normalised order imbalance runs from −1 to +1. — [Quarter-Hour Effect, arXiv 2607.09426](https://arxiv.org/html/2607.09426v2)

**CVD / divergence**
- Delta divergence is "a disagreement between price and the aggression behind it". Example: price prints a higher high while cumulative volume delta prints a lower high. — [LuxAlgo concept library](https://www.luxalgo.com/library/concept/delta-divergence.md)
- I found no independent systematic backtest of CVD divergence. Grade C.

**Footprint imbalance / stacked imbalances**
- A footprint imbalance compares *diagonally*: ask volume at price P against bid volume one tick lower. A level is flagged when the ratio clears a threshold, commonly ~300% (3:1). "Stacked" means 3 or more consecutive same-side imbalanced levels in one bar. Practitioners treat them as later support or resistance. — [LuxAlgo bid/ask imbalance](https://www.luxalgo.com/library/concept/bid-ask-imbalance/); [Quantower blog](https://www.quantower.com/blog/imbalance-footprint-chart-and-rithmic-plugin)
- This needs per-price bid/ask traded volume (tick or aggTrades data). It cannot be built from 1-minute klines. Grade C.

**Absorption, big trades, Triple-A (Fabio Valentini-style)**
- Community codifications of Valentini's model use these pieces:
  - Absorption: "high-volume, low-price-movement candles."
  - Big Trades: prints large "relative to average volume."
  - Triple-A: Absorption, then Accumulation (range contraction), then Aggression (breakout with volume).
  - Aggressive entry: in the pullback zone when the opposing side is aggressive but price does not continue, with a tight stop.
  - Confirmation entry: waits for reclaim of structure (higher low or lower high).
  - Uses LVNs, volume profile and delta as context.
  These are TradingView and community scripts, not Valentini's own published rules. — [TradingView "Fabio Style Order Flow System"](https://www.tradingview.com/script/rCKzrbRP-Fabio-Style-Order-Flow-System); [Tradezella order-flow strategy](https://www.tradezella.com/strategies/order-flow-strategy)
- Grade C.

**Exhaustion / trapped traders**
- Orderflows (Michael Valtos) material covers "delta divergence, exhaustion prints, and market weakness detection". I could not retrieve exact thresholds. — [Scribd "Orderflows Unlocked Module 1"](https://www.scribd.com/document/959625681/OrderflowsUnlockedModule1)
- Grade C.

**VPIN (flow toxicity)**
- Andersen & Bondarenko argue that VPIN "is a poor predictor of short run volatility." They say it peaked after, not before, the 2010 flash crash. They attribute its predictive content mainly to "a mechanical relation with the underlying trading intensity", and say it depends on the bucketing start point. — [Kellogg Insight](https://insight.kellogg.northwestern.edu/article/the_trouble_with_vpin); [SSRN 2062450](https://papers.ssrn.com/abstract=2062450)
- Easley, López de Prado & O'Hara replied that "AB attack a methodology we do not advocate, an analysis we never performed, and conclusions we did not draw." — [Kellogg Insight](https://insight.kellogg.northwestern.edu/article/the_trouble_with_vpin); [UTS rejoinder](https://opus.cloud1.lib.uts.edu.au/handle/10453/118180)
- Easley, O'Hara, Yang & Zhang (2024) found that VPIN and Roll measures for BTC and ETH have own-market and cross-market predictive power for price dynamics, stable through the crypto winter. The use cases are market making, hedging and volatility estimation, not directional alpha. — [Cornell PDF, SSRN 4814346](https://stoye.economics.cornell.edu/docs/Easley_ssrn-4814346.pdf)

**Volume profile**
- The value area is the range holding ~68–70% of the session's volume around the POC (the highest-volume price). — [MetroTrade](https://www.metrotrade.com/what-is-the-80-rule-in-futures-trading/)
- Naked/virgin POC: a prior session's POC that price has not traded back through. — [LuxAlgo Naked POC](https://www.luxalgo.com/library/concept/naked-poc/)

**Market Profile open types**
- Ordered from most to least conviction:
  - Open-drive: drives from the first prints and never trades back through the open.
  - Open-test-drive: probes a nearby reference, finds no business, then drives the other way.
  - Open-rejection-reverse: the initial move is rejected and price comes back through the open.
  - Open-auction: rotates around the open.
  — [LuxAlgo open types](https://www.luxalgo.com/library/concept/open-types/)
- The initial balance (ES/NQ convention) is the range from 9:30 to 10:30 ET, the first RTH hour. — [tradingstats.net](https://tradingstats.net/initial-balance-breakout-statistics/)

### Inferences
These are my proposed mechanical codifications for the agent. They are not sourced rules; parameters are design choices to be validated.

1. **Bar delta (kline-level, crypto):** `delta_t = 2*taker_buy_base_t − volume_t`. Normalised imbalance: `imb_t = delta_t / volume_t ∈ [−1, 1]`, which matches the arXiv 2607.09426 normalisation.
   - US and NSE equities have no taker flag in standard OHLCV. There, delta needs tick data plus Lee–Ready or tick-rule classification, or bulk-volume classification (BVC: `buy_frac = Φ(Δp/σ_Δp)`, as used in VPIN). Treat any equity "delta" from bars as an approximation.
2. **CVD:** `CVD_t = Σ_{s=session_start..t} delta_s`. Reset at 00:00 UTC for crypto, or roll over N bars.
   - Divergence rule: a price swing high H2 > H1 (pivots with k=5 bars each side) while CVD at H2 < CVD at H1, so `CVD(H2) − CVD(H1) < 0`. Optionally require the difference to exceed 0.5 × the rolling σ of the session's CVD increments.
   - Mirror the rule for lows. This is computable from 1-minute klines.
3. **Absorption (bar proxy):** `volume_t > Q90(volume, rolling 1 day)` AND `|close−open| / ATR_14 < 0.25` AND `|imb_t| > 0.3`. Aggression is on one side but price did not move. Direction is opposite to the delta sign: large positive delta with no up-move means passive sellers absorbing, which is bearish.
   - True absorption (resting limit orders refilling at one price) needs order-book or tick data. The bar proxy is a lossy approximation.
4. **Exhaustion (bar proxy):** a new N-bar extreme on a bar whose `|imb|` and volume both rank in the bottom quartile of the last 20 bars. Aggression is drying up at the extreme.
   - The footprint version (a low single-digit traded volume at the bar's extreme price on the aggressive side) needs per-price data.
5. **Trapped traders (bar proxy):** a breakout bar beyond a reference level (prior high, VAH, IB high) with `imb > +0.3` and volume > Q75. Within M=3 bars, price closes back inside the reference. The trapped buyers' stop is the fuel; enter short on the close back inside, stop above the breakout bar high.
6. **Large-lot / whale prints:** these need aggTrades or trades streams (Binance `aggTrades` dumps on data.binance.vision). Define a large print as `qty > Q99.9` of trailing 24h trade sizes. They cannot be derived from klines.
7. **Stacked imbalances and footprint:** these need trades bucketed by price tick within each bar. For crypto, this is buildable from Binance Vision `aggTrades` history, which is free. Rule: `ask_vol(P) ≥ 3 × bid_vol(P − tick)`, with a minimum volume floor, on 3 or more consecutive ticks.
   - Use a coarser "tick" for crypto (e.g. 0.01% of price), since BTC has a $0.1 tick and many near-empty levels.
8. **Volume profile:** build from 1-minute klines by distributing each bar's volume uniformly over its [low, high] in bins of e.g. 0.05% (BTC) or 1 tick (equities). Kline-built profiles are an approximation; aggTrades give exact profiles.
   - POC = argmax bin.
   - Value area via the standard CBOT expansion: start at the POC, compare the volume of the next two bins above with the next two below, add the larger pair, and repeat until ≥70% of volume.
   - HVN/LVN = local maxima/minima of the smoothed profile (e.g. 3-bin moving average; LVN requires < 30% of POC volume).
   - Naked POC = prior-session POC not touched by any subsequent bar range.
9. **Session definition for 24/7 crypto:** there is no RTH open. Use the 00:00 UTC daily open, consistent with Binance daily klines. As a sensitivity test, also try US-equity-open-anchored sessions (13:30/14:30 UTC). IB = first 60 minutes of the chosen session.
   - This is a non-standard transplant. The auction-theory premise (an overnight inventory gap, then a "fair" price discovered by day timeframe and other timeframe participants) is weaker for continuous markets.
10. **80% rule (codable):** the session opens outside the prior session's value area [VAL, VAH]. Then two consecutive 30-minute periods trade inside the VA, using the looser mypivots wording below. Enter in the direction of the far side. Target: the opposite VA edge. Stop: beyond the entry-side VA edge or the session extreme. Time stop at session end.
11. **Open types (codable from first 30 minutes on 1-minute bars):**
    - Drive: price never crosses the open after the first 5 minutes, and the range is ≥ X×ATR.
    - Test-drive: first moves ≥ 0.25×ATR one way, crosses the open, then extends beyond it.
    - Rejection-reverse: first move is rejected, price crosses back through the open and closes the 30 minutes beyond it.
    - Auction: everything else.
12. **Value migration:** sign of `POC_today − POC_yesterday`, plus VA overlap classification:
    - Higher value: VAL_t > VAH_{t−1}.
    - Overlapping-higher: POC up with overlap.
    - Inside, outside, lower: defined analogously.
    - Developing value: the developing POC/VA recomputed each bar, with the trend measured as the slope of the developing POC over the last 60 minutes.

**Grades:**
- Bar delta/CVD as a contemporaneous variable: well established (but descriptive).
- Delta/OFI as a predictor at retail costs: C to B-, negative for crypto at 1-minute (see next section).
- CVD divergence: C.
- Absorption: C.
- Stacked imbalances: C.
- Exhaustion and trapped traders: C.
- Whale prints: C. No retail-horizon study was found; Chordia–Subrahmanyam-type evidence is about aggregate imbalance, not single prints.
- VPIN: contested; a risk/volatility input, not a directional signal.
- POC/VA/naked POC: C to B- (vendor statistics only).
- IB breakout: B- (descriptive statistics, no P&L).
- 80% rule: B- (two informal tests report ~60–67%, not 80%).
- Open types, day types and value migration: C.

### Gaps
- I could not retrieve Valtos's exact numeric thresholds (e.g. exhaustion-print size, imbalance ratio) or Axia Futures' written rules; their material is paywalled courses.
- Trader Dale's exact rules were not retrieved. From general knowledge, his approach trades volume-profile cluster or "volume accumulation" zones with fixed-tick stops. This is not verified here.
- I did not verify Dalton's day-type definitions (normal, normal variation, trend, double-distribution, neutral, non-trend) or his exact 80% wording against the book text in this session. The open-type definitions above come from a secondary source (LuxAlgo), not the book.
- The CBOT value-area expansion algorithm is stated from general knowledge, not a fetched source.

---

## Academic evidence: CKS (2014), Chordia & Subrahmanyam, VPIN, crypto studies — horizon, size, cost survival

### Takeaway
Order-flow imbalance is mainly a *contemporaneous* explanatory variable: CKS find price change is linear in OFI, with the slope set by depth. Its *predictive* content decays fast. It lasted up to 30 minutes in NYSE 1996 data, 10 minutes in 1999 and 5 minutes in 2002, and is shorter now.

In Binance perps, 1-minute order imbalance shows little short-horizon predictability. A quarter-hour imbalance effect predicts 4–12 hour returns, but the authors say the predictable component is small relative to a 5 bp taker fee. A 1-second-data ML study at a 3-second horizon is profitable after fees only for small-cap coins, not robustly for BTC. No grade-A result exists for retail-cost trading on taker imbalance.

### Cited Findings
**Cont, Kukanov & Stoikov (J. Financial Econometrics 2014)**
- They report a linear OFI→price-change relation, with slope inversely proportional to depth, on 50 NYSE stocks (TAQ). — [arXiv 1011.6402](https://arxiv.org/abs/1011.6402)
- The abstract does not give the R² or interval length. A commonly cited figure is ~65% average R² over 10-second intervals, but I did not verify it in this session.

**Cont, Cucuringu & Zhang (Quantitative Finance 2023)**
- Multi-level "integrated OFI" explains price impact better than best-level OFI.
- Cross-impact adds nothing contemporaneously.
- Lagged cross-asset OFI improves forecasts, but the effect "mainly manifest[s] at short-term horizons and decay[s] rapidly."
- Data: NASDAQ LOBSTER, top-100 S&P 500 stocks.
— [arXiv 2112.13213](https://arxiv.org/abs/2112.13213v4); [RePEc](https://ideas.repec.org/a/taf/quantf/v23y2023i10p1373-1393.html)

**Chordia & Subrahmanyam (JFE 2004)**
- Daily order imbalances for NYSE stocks, 1988–1998.
- Lagged imbalance positively predicts returns. The relation reverses sign after controlling for the current imbalance, which is consistent with market makers absorbing autocorrelated, split large-trader orders.
- An imbalance-based strategy is profitable on paper, but "transaction costs will mitigate any profits."
— [SUFE newsletter summary](https://academicnewsletter.sufe.edu.cn/info/357679); [UPenn PDF](https://www.cis.upenn.edu/~mkearns/finread/stock-misbalance.pdf)

**Chordia, Roll & Subrahmanyam (JFE 2005), "speed of convergence"**
- Daily returns are not serially correlated, but imbalances are highly persistent.
- Lagged imbalances predict returns up to 30 minutes (1996), 10 minutes (1999) and 5 minutes (2002). Efficiency is achieved in "more than five minutes but less than sixty minutes."
— [CFA Digest summary](https://rpc.cfainstitute.org/en/research/cfa-digest/2005/11/evidence-on-the-speed-of-convergence-to-market-efficiency-digest-summary)

**VPIN**
- The critique and rejoinder are summarised in the catalog section above. — [Kellogg Insight](https://insight.kellogg.northwestern.edu/article/the_trouble_with_vpin)
- Crypto application (Easley et al. 2024): VPIN and Roll have predictive power for price dynamics relevant to market making, hedging and volatility. — [Cornell PDF](https://stoye.economics.cornell.edu/docs/Easley_ssrn-4814346.pdf)

**Crypto: "The Quarter-Hour Effect" (arXiv 2607.09426, 2026)**
- Data: Binance USDT-M perps (BTC, ETH, XRP, SOL, DOGE, ADA), Jan 2021 to Oct 2024, 10-second bars.
- Imbalance is measured from `isBuyerMaker`.
- Results:
  - Quarter-hour opening returns: out-of-sample R² 3.37%, directional accuracy 57.12%.
  - Opening order imbalance predicts cumulative returns over 4–12 hours, with slopes peaking at 8–12 hours.
  - The sign-weighted forecast is ≈0.50 bp per boundary, "one tenth of a single standard-tier taker fee and one twentieth of a round trip."
  - The authors conclude the predictable component is "small relative to trading costs" and **not profitable after costs**.
- Search-snippet summary of the same work: "baseline order imbalance and order imbalance at one-minute openings exhibit little predictive association with subsequent returns at short horizons."
— [arXiv 2607.09426](https://arxiv.org/html/2607.09426v2)

**Crypto: "Explainable Patterns in Cryptocurrency Microstructure" (arXiv 2602.00776, 2026)**
- Data: Binance Futures perps (BTC, LTC, ETC, ENJ, ROSE), 1-second frequency, Jan 2022 to Oct 2025.
- Target: 3-second mid-price log return.
- Order-flow and trade imbalance features dominate SHAP importance, with a monotone, concave-at-extremes effect.
- Taker-strategy annualised compounded return after fees: BTC 0.13, LTC 0.07, ETC 5.78, ENJ 4.06, ROSE 7.00. Outperformance is significant (p<0.05) only for ETC, ENJ and ROSE.
— [arXiv 2602.00776](https://arxiv.org/html/2602.00776v1)

**Crypto: Silantyev (Digital Finance 2019), BitMEX**
- Trade-flow imbalance has higher contemporaneous explanatory power than OFI. Crypto order books are thin, so trades have large impact. — [RePEc](https://ideas.repec.org/a/spr/digfin/v1y2019i1d10.1007_s42521-019-00007-w.html)

**Crypto: Anastasopoulos & Gradojevic, "Order flow and cryptocurrency returns" (J. Financial Markets 2026; EFMA 2025)**
- Search snippets state that findings on order-flow predictability "are robust to economic restrictions such as short-selling constraints and high transaction costs."
- I could not open the paper (403/503), so horizon, data and effect size are unverified.
— [ScienceDirect](https://www.sciencedirect.com/science/article/pii/S1386418126000029); [EFMA PDF](https://www.efmaefm.org/0EFMAMEETINGS/EFMA%20ANNUAL%20MEETINGS/2025-Greece/papers/OrderFlowpaper.pdf); [Guelph thesis](https://atrium.lib.uoguelph.ca/bitstreams/bae607b2-3fff-401a-8412-c34569fd5f98/download)
- These findings conflict in tone with arXiv 2607.09426's not-profitable-after-costs conclusion. The horizons and constructions likely differ (possibly daily or lower-frequency order flow).

**Crypto: Vafin (SSRN 6938742, June 2026)**
- A review and evaluation framework for OFI in crypto (out-of-sample, realistic cost model, data-snooping control). I did not see a result. — [SSRN](https://papers.ssrn.com/sol3/papers.cfm?abstract_id=6938742)

### Inferences
- The microstructure literature consistently shows a horizon ladder:
  - Strong contemporaneous explanation (seconds to minutes).
  - Rapidly decaying lagged predictability: 5–30 minutes in old NYSE data, seconds in modern electronic markets.
  - Weak, slow, small effects at hours to days. These are statistically detectable but not obviously cost-covering for a taker.
- On Delta Exchange, a taker round trip costs 10 bp, or 2 × 0.05% plus slippage. That is 20× the 0.5 bp quarter-hour edge measured on Binance.
- Even maker-maker (4 bp round trip) execution exceeds the documented signal sizes. Maker entries also carry adverse selection precisely when imbalance signals fire.
- Imbalance is most usable as a *filter or conditioning variable*: avoid fading strong one-sided flow, or size down when VPIN-like toxicity is high. That is safer than using it as a stand-alone entry trigger.

### Gaps
- I found no peer-reviewed crypto study showing 1-minute-kline taker imbalance strategies profitable after ≥5 bp taker fees at minute-to-hour horizons.
- The Anastasopoulos & Gradojevic details (horizon, costs assumed) are unverified because the full text was inaccessible.
- I found no US/NSE study for retail-cost intraday imbalance strategies after 2010. Lee–Ready-classified NSE studies were not searched.

---

## Can the predictive part be captured from 1-minute bar taker volume, or only at sub-second horizons?

### Takeaway
Mostly no. Contemporaneous explanation survives at 1-minute aggregation, but next-minute predictability from 1-minute imbalance is weak in Binance perps. The strongest crypto ML results use 1-second features to predict 3-second returns. Those horizons need latency and maker-level fees that a retail agent on Delta Exchange does not have.

The one slower effect (quarter-hour imbalance, then 4–12 hours) is real but sub-basis-point per event. Kline taker volume is adequate for *regime/context* features (CVD trend, divergence, absorption proxy, exhaustion proxy). It is not adequate for footprint, stacked-imbalance, iceberg or whale-print concepts, which need aggTrades or order-book data.

### Cited Findings
- "Baseline order imbalance and order imbalance at one-minute openings exhibit little predictive association with subsequent returns at short horizons." Quarter-hour imbalance predicts 4–12 hour returns at ~0.5 bp per event versus a 5 bp taker fee. — [arXiv 2607.09426](https://arxiv.org/html/2607.09426v2)
- The 3-second horizon on 1-second data is profitable after fees only for small caps (ETC, ENJ, ROSE). BTC's annualised compounded return is 0.13 and not significant. — [arXiv 2602.00776](https://arxiv.org/html/2602.00776v1)
- Predictability windows in US stocks shrank from 30 minutes (1996) to 5 minutes (2002). — [CFA Digest](https://rpc.cfainstitute.org/en/research/cfa-digest/2005/11/evidence-on-the-speed-of-convergence-to-market-efficiency-digest-summary)
- Lagged OFI forecasting power decays rapidly. — [arXiv 2112.13213](https://arxiv.org/abs/2112.13213v4)
- Wider spreads attenuate the imbalance effect, consistent with adverse selection and execution costs (search snippet attributed to the crypto microstructure literature). — [arXiv 2602.00776](https://arxiv.org/html/2602.00776v1)

### Inferences
- Test plan for the agent:
  - (a) Regress forward 1/5/15/60/240-minute returns on `imb_t`, rolling sums of imb, and CVD slope, with HAC errors and walk-forward.
  - (b) Convert the forecast to a strategy with 10 bp taker and 4 bp maker round-trip costs.
  - (c) Compare against a random-entry baseline with an identical holding-time distribution.
  - Expect the gross edge per trade to be below 5 bp for 1–15 minute horizons. Kill the approach unless the gross edge is above 2× the round-trip cost out-of-sample.
- Kline delta loses the intra-bar sequence: which side hit first, and where in the bar. Divergence and absorption proxies built on it are noisier than footprint versions. This penalises any C-grade concept further.

### Gaps
- I found no published study directly comparing kline-aggregated versus tick-level taker imbalance predictability on the same crypto sample.

---

## Independent systematic tests of volume-profile / Market Profile rules (POC reversion, 80% rule, IB breakout), including negative results

### Takeaway
I found no peer-reviewed tests. Available "tests" are vendor or blog statistics on ES/NQ.
- The 80% rule tests at ~60–67%, not 80%. One source says it "should probably be called the 60% rule."
- IB breakouts occur on ~97–98% of ES/NQ days. A breakout by itself is near-certain and carries no edge. The 100% IB extension is reached on only ~13–21% of days.
- The naked POC "80% revisited within 10 sessions" claim comes from vendor content with no baseline comparison.
- None of these report P&L after costs. The grade is B- at best.

### Cited Findings
**80% rule**
- Definition: market opens (or moves) outside value, then trades back inside "for two consecutive 30-min-bars". Target: full value-area fill.
- A loose interpretation is allowed: the first bar enters and closes inside, and the second bar opens inside.
- E-mini S&P testing found it "succeeds only 60% of the time … should probably be called the 60% rule."
— [mypivots](https://www.mypivots.com/dictionary/definition/25/80-rule)
- Another source claims "67% accuracy rate in independent testing" and "65–70% accuracy after slippage and partial fills." No methodology is given. Confirmation is two consecutive TPOs inside the VA, holding 30–60 minutes. It works best on balanced days and fails on trend or news days. — [MetroTrade / FTMO (search summary)](https://www.metrotrade.com/what-is-the-80-rule-in-futures-trading/); [FTMO blog](https://ftmo.com/en/blog/market-profile-master-the-80-trading-strategy-hidden-magnets/)

**IB breakout (ES/NQ, 2015–2025)**
- Sample: 2,686 ES days and 2,833 NQ days; IB = 9:30–10:30 ET; RTH only.
- At least one IB breakout: 97.8% of ES days and 96.2% of NQ days.
- Breakout type (ES / NQ):
  - Single up: 38.3% / 40.6%.
  - Single down: 30.9% / 33.0%.
  - Double: 28.7% / 22.6%.
- 100% extension by close: ES up 18.8%, down 20.5%; NQ up 12.8%, down 15.9%.
- "No P&L analysis was computed."
— [tradingstats.net](https://tradingstats.net/initial-balance-breakout-statistics/)
- Further claims from the same site and TradingView, via search summary:
  - Extreme IB (>1.5×ATR) breaks only 66.7% of the time, with median extension 22.3%.
  - IB close above midpoint → 83.5% upside breakout; close below midpoint → 94.9% downside breakout.
  - When the C-period confirms, 45.5% of ES days reach a 100% upside extension. When it fails, 37.9% close back inside the IB.
  - A non-retested breakout past the 1.1 extension runs to a median ~2.0× IB, versus 1.56× when retested.
  — [tradingstats.net IB retest](https://tradingstats.net/initial-balance-retest-statistics/); [TradingView IB stats indicator](https://www.tradingview.com/script/UYVre3kq-Initial-Balance-Breakout-Extension-Statistics-ES-NQ/)

**Naked POC**
- "Approximately 80% of Naked POCs get revisited within 10 trading sessions." This vendor claim appeared in search results from volume-profile guide sites; the exact originating page and methodology are not verified. — [LuxAlgo Naked POC](https://www.luxalgo.com/library/concept/naked-poc/); [buildix.trade guide](https://www.buildix.trade/blog/volume-profile-trading-strategies-value-area-naked-poc-free-guide-2026)

### Inferences
- Most of these statistics are *base rates*, not edges. For example, "price revisits a nearby level within 10 sessions" holds for most random levels near price too. A naked POC needs comparison against random prior-session prices at equal distance.
- The IB-midpoint statistic is partly mechanical: closing the IB near its high puts price close to the high.
- An 80%-rule win rate of 60–67% is meaningless without the reward:risk ratio. The target (the opposite VA edge) is typically larger than the stop, but the stop placement varies by source.
- These tests are all ES/NQ RTH. Transferring them to 24/7 crypto needs a session definition, and results may not carry over. Each rule should be re-tested on BTC/ETH/SOL with a 00:00 UTC session and a random-level/random-time baseline.

### Gaps
- I found no academic or Quantpedia-grade test of POC reversion, VA rules or IB breakouts with costs.
- I found no crypto or NSE replication.
- I did not search Quantpedia directly within budget, so a Quantpedia entry on market-profile rules may exist.

---

## Practitioner sources: exact stated rules

### Takeaway
Practitioner rules are largely discretionary. Codable cores:
- Dalton: IB = first hour; the 80% rule is two 30-minute TPO periods back inside the prior VA, targeting the opposite VA edge; four open types; day types classified by range extension relative to the IB.
- Footprint (Valtos-style): diagonal 3:1 imbalance, stacked across 3 or more levels.
- Valentini-style (per community scripts): absorption, then range contraction, then aggressive breakout (Triple-A), with entries at LVNs and VA edges, stops just beyond the absorption level.
Exact numeric thresholds from Valtos, Axia and Trader Dale were not retrievable (paywalled). Everything here is grade C.

### Cited Findings
- 80% rule: two consecutive 30-minute bars inside value after opening outside, targeting a full VA fill. — [mypivots](https://www.mypivots.com/dictionary/definition/25/80-rule); [ShadowTrader glossary](https://www.shadowtrader.net/glossary/eighty-percent-rule/)
- The four open types are defined in the catalog section above. — [LuxAlgo open types](https://www.luxalgo.com/library/concept/open-types/)
- Footprint imbalance: diagonal comparison, ~300% threshold, stacked = 3 or more consecutive levels. — [LuxAlgo](https://www.luxalgo.com/library/concept/bid-ask-imbalance/); [Quantower](https://www.quantower.com/blog/imbalance-footprint-chart-and-rithmic-plugin)
- Valentini-style absorption, big trades and Triple-A: summarised in the catalog section above. Stops are tight because "invalidation should happen quickly." — [TradingView script](https://www.tradingview.com/script/rCKzrbRP-Fabio-Style-Order-Flow-System)
- Orderflows/Valtos teaches delta divergence, exhaustion prints and weakness detection. Thresholds are not retrieved. — [Scribd](https://www.scribd.com/document/959625681/OrderflowsUnlockedModule1)

### Inferences
- Suggested default parameters for the first crypto test, to be treated as hyperparameters with walk-forward selection:
  - Imbalance ratio 3.0.
  - Stack length 3.
  - Absorption volume over the 90th percentile with body < 0.25 ATR.
  - Divergence pivots k=5.
  - VA 70%.
  - IB = 60 minutes.
  - 80%-rule confirmation = 2×30-minute periods.
  - Time stop at session end for intraday rules.
  - Swing variants: daily/weekly composite profiles with nPOC targets.
- Stop/target conventions to code:
  - Stops beyond the structural level (VA edge, IB extreme, absorption bar extreme) plus a buffer of 0.1–0.2×ATR.
  - Targets at the next profile reference (POC, opposite VA edge, nPOC, 1.0×/1.5×/2.0× IB extensions).

### Gaps
- Axia Futures' written playbook rules (e.g. their "initiative vs responsive" and "absorption at value-area edge" setups) were not retrieved.
- Trader Dale's specific setups (volume clusters, VWAP + profile) were not retrieved.
- Dalton day-type numeric criteria were not verified against the book text.
- Valentini's own statements were not verified; only third-party scripts.
