# Wyckoff, VSA, Supply/Demand Zones, Market Structure (HH/HL) and Al Brooks Price Action: Codifiable Rules and Evidence Quality

Scope: turn each concept into an exact rule for an algorithmic paper-trading agent (crypto BTC/ETH/SOL, US stocks/ETFs, Indian NSE stocks; intraday and swing), and grade the evidence.
Grading key (from the brief): A = peer-reviewed evidence of profitability after costs; B = independent systematic backtests; C = practitioner claims only.
Research date: 2026-10-07. About 30 tool calls (searches and fetches). Anything marked "PROPOSED" under Inferences is my own codification for the agent. It is not taken from a source, and its parameters are starting values that need tuning on held-out data.

---

## Q1. What codified or algorithmic versions exist, with exact parameters? (definitions, mechanical rules, entry/stop/target/timeframe, data, grade per concept)

### Takeaway
None of these five schools has an official, canonical algorithm. Wyckoff Analytics itself publishes no quantitative thresholds for event size, phase length or volume. The only exact parameters available come from practitioner code (Pine Script lookbacks of 15–25 bars, a 20-bar volume moving average, "volume lower than the previous two bars", a signal-bar close in the top or bottom 30%, and the Brooks glossary's bar-counting rules for H1/H2). So every rule set the agent uses will be a researcher-chosen parameterisation. Each one has to be validated out of sample, and each one counts as an extra trial for data-snooping purposes.

### Cited Findings

**Wyckoff method: canonical (qualitative) definitions**
- The source applies no quantitative thresholds to events, phase durations or volume. Wyckoff Analytics' own method page says the method "relies on pattern recognition and relative comparisons" (summary of the page content). — [Wyckoff Analytics: Wyckoff Method](https://www.wyckoffanalytics.com/wyckoff-method/)
- Accumulation events, as defined there:
  - PS (preliminary support): "volume increases and price spread widens" after a prolonged downtrend.
  - SC (selling climax): panic selling absorbed by professionals; price often "close[s] well off the low".
  - AR (automatic rally): the bounce after the SC, driven by short covering and demand.
  - ST (secondary test): a revisit of the SC area on "diminished" volume and spread.
  - Spring: a dip below trading-range (TR) support that quickly reverses back into the range.
  - Test: a "higher low on lesser volume".
  - SOS (sign of strength): an advance "on increasing spread and relatively higher volume".
  - LPS (last point of support): a pullback low after the SOS "on diminished spread and volume".
  - BU (back-up): a test of former resistance before markup continues.
  — [Wyckoff Analytics](https://www.wyckoffanalytics.com/wyckoff-method/)
- Distribution events, as defined there:
  - PSY (preliminary supply): volume expands and spread widens after an uptrend.
  - BC (buying climax): often "coincides with great earnings or good news".
  - AR (automatic reaction) and ST (secondary test): as in accumulation, mirrored.
  - UT (upthrust) and UTAD (upthrust after distribution): a move above resistance that reverses.
  - SOW (sign of weakness): a move down to or through the bottom of the TR "on increased spread and volume".
  - LPSY (last point of supply): a "feeble rally on narrow spread".
  — [Wyckoff Analytics](https://www.wyckoffanalytics.com/wyckoff-method/)
- Phases, as defined there:
  - Phase A: the prior trend stops (PS/SC/AR/ST, or PSY/BC/AR/ST).
  - Phase B: "building a cause".
  - Phase C: a test, either a spring/shakeout or a UT/UTAD.
  - Phase D: demand (or supply) dominates and price travels to or through the far edge of the TR.
  - Phase E: price leaves the TR; markup or markdown.
  — [Wyckoff Analytics](https://www.wyckoffanalytics.com/wyckoff-method/)
- The three laws are supply/demand, cause/effect and effort/result. Effort vs result example: "High-volume but narrow-range price bars after a rally with failure to make new highs suggests big interests are unloading shares." — [Wyckoff Analytics](https://www.wyckoffanalytics.com/wyckoff-method/)
- The composite operator ("Composite Man") is a heuristic that stands for all large operators acting together and running campaigns: accumulation, markup, distribution, markdown. — [Wyckoff Analytics](https://www.wyckoffanalytics.com/wyckoff-method/)
- Point-and-figure (P&F) cause/effect count:
  - Method: count the columns along a count line inside the TR, then compute columns × box size × reversal amount.
  - Add the result to three baselines: the TR low (minimum target), the halfway point between the low and the count line, and the count line (maximum target).
  - Worked example: DJIA with 100-point boxes and a 3-box reversal: 10 columns → 10 × 100 × 3 = 3,000 points.
  - The targets are "points where you should stop, look and listen", not exact turning points.
  — [Wyckoff Analytics](https://www.wyckoffanalytics.com/wyckoff-method/)

**Wyckoff: algorithmic spring detection (Wyckoff Analytics, 3 June 2025)**
- A valid spring cannot close at the low of the bar; the bar needs a significant lower tail.
- The spring bar may close below the 50% level of its range and still be valid.
- Volume should come in as price penetrates support, but the bar must not close below support (the penetration is temporary).
- A distance of more than 10% between consecutive lows is "unacceptable" because it makes stop placement impractical.
- The Pine Script baseline lookback for the lowest low is 15 bars, with 20–25 bars suggested as alternatives.
- The page gives no backtest results, no volume threshold, and no entry, stop or target levels.
— [Wyckoff Analytics: Identifying Wyckoff Springs with Algorithms](https://www.wyckoffanalytics.com/identifying-wyckoff-springs-with-algorithmic-trading-strategies/)
- Wyckoff Analytics has also published a webinar titled "Back-Testing and Validating Your Trading Plan (2017)". I did not retrieve its content, so I cannot report any results from it. — [Wyckoff Analytics](https://www.wyckoffanalytics.com/demand/back-testing-and-validating-your-trading-plan-2017/)

**Wyckoff: practitioner TradingView event detectors**
- Typical inputs:
  - Volume MA length: default 20.
  - Price-pattern lookback: default 20.
  - A "Volume Climax Multiplier" for spike sensitivity.
  - Minimum label spacing: 5 bars.
- Typical logic: `volMA = ta.sma(volume, len)`, `ta.highest(high, lookback)`, `ta.lowest(low, lookback)`. Climaxes, springs and upthrusts are flagged as volume spikes at extremes of the lookback window.
— [TradingView Wyckoff script search (Alpha Extract "Wyckoff Event Detection")](https://vn.tradingview.com/scripts/search/Wyckoff); [Wyckoff Detector (BidWhales), protected source](https://www.tradingview.com/script/De6tPtJ3-Wyckoff-Detector-BidWhales)
- Academic and ML attempts:
  - A 2024 paper in the Journal of Information Systems Engineering and Management describes detecting Wyckoff patterns (accumulation, the trading range and the secondary test) with CNNs (for spatial data) and LSTMs (for temporal data). It reports the "efficacy of deep learning models in detecting" the patterns, which is a detection result, not trading profitability after costs.
  - The search results also cite Pal (2024, arXiv 2403.18839), which uses Wyckoff phases in an LSTM pattern-recognition model for currency trading.
  — [JISEM 2024 "Wyckoff Theory in the Mind of the Market"](https://www.jisem-journal.com/download/60_Wyckoff%20Theory.pdf); [arXiv 2403.18839](https://arxiv.org/html/2403.18839v1)
- A practitioner strategy site says the difficulty of backtesting Wyckoff "stems from its qualitative assessments". — [PapersWithBacktest: Wyckoff](https://paperswithbacktest.com/strategies/wyckoff-trading-strategy)

**VSA (Volume Spread Analysis: Tom Williams' extension of Wyckoff)**
- VSA reads each bar as EFFORT (volume) versus RESULT (the spread, plus where the bar closed within its range). Heavy volume on a narrow spread means absorption. — [TradersUnion VSA](https://tradersunion.com/az/technic-analysis/volume-spread-analysis/)
- VSA works bar by bar on volume (or relative volume), the close and the range, to judge the contest between supply and demand. It came out of Wyckoff's work and was developed by Tom Williams. — [Academia.edu "Volume Spread Analysis"](https://www.academia.edu/40243204/VOLUME_SPREAD_ANALYSIS)
- No-demand bar (exact rule): an up bar (close above the prior close) with a narrow spread and volume lower than EACH of the previous two bars. It counts as evidence, not as a trigger, and becomes actionable only against a background of strength or weakness plus a confirming next bar. No-supply is the mirror image (a down bar, narrow spread, volume below each of the prior two bars). — [LuxAlgo: No-demand / No-supply bars](https://www.luxalgo.com/library/concept/no-demand-no-supply-bars/); [TAC VSA indicator, ProRealCode](https://prorealcode.com/prorealtime-indicators/tac-vsa-volume-spread-analysis-2)
- One source claims a win rate of about 70% "on quality setups" for VSA. This is a general practitioner claim, not a documented backtest. — [TradersUnion VSA](https://tradersunion.com/az/technic-analysis/volume-spread-analysis/)
- Many published VSA implementations (TradingView, ProRealCode, cTrader) are quantified approximations that differ from interpretive, context-dependent VSA. — [TradersUnion](https://tradersunion.com/az/technic-analysis/volume-spread-analysis/); [ProRealCode VSA IQ](https://www.prorealcode.com/prorealtime-indicators/volume-spread-analysis-iq/)

**Supply and demand zones**
- Pattern names: Rally-Base-Rally (RBR) is a demand zone in an uptrend continuation; Drop-Base-Drop (DBD) is a supply zone; Drop-Base-Rally (DBR) and Rally-Base-Drop (RBD) are the reversal versions. — [LuxAlgo: Supply and demand zones](https://www.luxalgo.com/blog/supply-and-demand-zones-core-trading-strategies/)
- Codified form found in a TradingView script ("Zone Forge"):
  - A zone is "born from a run of compressed candles (the base) followed by an impulsive leg away of a configurable ATR multiple within a fixed window".
  - Zone lifecycle runs one way: Fresh → Tested → Broken. A broken zone never returns to fresh, and each extra test weakens the zone.
  - The script's author notes that the doctrines (fresh zones beat tapped ones; heavy-volume bases beat quiet ones) are "measured almost nowhere".
  — [TradingView scripts (Supply & Demand Zones – Zone Forge [AFD])](https://br.tradingview.com/scripts/trendanalysis/page-8/)
- Optuma forum users have tried to backtest zone bounces systematically. I found no published result. — [Optuma forum](https://forum.optuma.com/t/supply-and-demand-zones/3392)

**Market structure (HH/HL) and swing-point detection**
- An uptrend is a sequence of higher highs (HH) and higher lows (HL); a downtrend is lower highs (LH) and lower lows (LL). A break in the sequence is "the primary signal that the trend itself may be changing". — [DayTradingToolkit HH/HL strategy](https://daytradingtoolkit.com/strategies/higher-high-higher-low-market-structure-strategy); [CrossTrade market structure](https://crosstrade.io/learn/price-action/market-structure)
- Swing points are "the atoms of market structure". Lag in detecting them "is the price of an objective definition, and it matters whenever swing-based signals are backtested or automated". A fractal or pivot swing high with N bars on each side is confirmed only N bars after the pivot bar. — [LuxAlgo: Swing High/Low](https://www.luxalgo.com/library/concept/swing-high-low/)

**Al Brooks price action**
- High 1 (H1): "a bar with a high above the prior bar in a bull flag or near the bottom of a trading range".
- High 2 (H2): if a bar with a lower high then follows (one or several bars later), the next bar in that correction whose high goes above the prior bar's high is an H2.
- The third and fourth such bars are H3 and H4. L1, L2 and so on mirror this in bear flags.
— [Brooks Trading Course forum / glossary: H1 H2 L1 L2](https://www.brookstradingcourse.com/support-forum/01-terminology/h1-h2-l1-l2-etc/paged/2/); [definition of H1 thread](https://brookstradingcourse.com/support-forum/09-pullbacks-and-bar-counting/definition-of-h1-confusing)
- The H2 is a two-legged pullback (a "complex" bull flag), often shaped like a double bottom inside the flag. Brooks calls it his most reliable with-trend entry. This is a practitioner claim. — [NexusFi: Two-legged pullback trading](https://nexusfi.com/a/strategies/two-legged-pullback-trading)
- An open-source TradingView "Al Brooks Second Entry" indicator codifies this as:
  - Trend read from new highs and lows and inside/outside bars, plus an EMA trend filter (length is a user input).
  - H1/L1 marked as a trend starts; H2/L2 marked after a pullback resumes.
  - Optional strong-signal-bar filter: the H2 (L2) bar must close in the top (bottom) 30% of its range.
  - The count resets on new swing highs/lows or when the trend bias changes.
  - No performance claims.
  — [TradingView: Al Brooks Second Entry](https://www.tradingview.com/script/YUixSlPL-Al-Brooks-Second-Entry/)

### Inferences

**PROPOSED exact rules for the agent.** These are my codifications, not quotes from any source. Notation: N_v = 20-bar volume SMA; ATR = ATR(14); R = bar range (high − low); closePos = (close − low) / R.

**Swing points (shared by all modules).**
- A pivot high at bar t exists if high[t] > max(high[t−k..t−1]) and high[t] ≥ max(high[t+1..t+k]). Pivot lows mirror this.
- k = 2–3 for intraday and k = 3–5 for swing; the pivot is confirmed only at bar t + k (no lookahead).
- Alternative: a ZigZag that needs a reversal of at least m × ATR, with m = 1.5–3. This avoids the "too many pivots" problem in noisy crypto 1-minute data.

**Trend state (HH/HL).**
- UP when the last two confirmed pivot highs and the last two confirmed pivot lows are both rising.
- DOWN is the mirror image. Anything else is RANGE.
- Break of structure: a close below the most recent confirmed HL flips the state to RANGE or DOWN.
- Entry ("buy the higher low"): in an UP state, after a pullback of at least 0.5 × ATR that does not undercut the prior HL, buy a stop order 1 tick above the high of the first bar that takes out the prior bar's high. This is effectively a Brooks H1 or H2.
- Stop: below the new swing low, or below the prior HL as the conservative choice.
- Target: a 1R/2R ladder, or a measured move.

**Al Brooks H1/H2.**
- Keep a bull-flag leg counter. A pullback starts when the trend state is UP and a bar makes a lower high than the bar before it.
- H1: the first later bar whose high exceeds the prior bar's high. An H2 can only occur after another lower-high bar has followed the H1.
- Reset the count if price makes a new trend high, or if the pullback low undercuts the start of the trend leg.
- Trend filter: close > EMA(20). Brooks' charts use a 20-bar EMA, but I did not verify this against a source here.
- Signal-bar filter: closePos ≥ 0.7, following the TradingView 30% convention.
- Entry: buy stop at the H2 signal-bar high + 1 tick. Stop: the signal-bar low, or the pullback low. Target: at least 1R, or the prior trend high (a scalp to the test).
- L1/L2 mirror this.

**Wedge.** Three pushes, meaning three confirmed pivot highs (or lows) in the same direction, where the slope from pivot 1 to 2 is greater than the slope from pivot 2 to 3 (converging). Fade it after a reversal bar, with the stop beyond the third push.

**Measured move.** Target = entry-leg start + (leg 1 length). This is the AB = CD equal-leg projection; leg 1 is measured from the first-leg pivot.

**Always-in direction.** A simple proxy: always-in long after a bull breakout bar with R ≥ 1.5 × ATR, closePos ≥ 0.75, that closes above the prior swing high, until the opposite condition fires. This proxy is my own simplification. The real Brooks concept is discretionary.

**Breakout / failed breakout.**
- Breakout: a close beyond the TR boundary by at least 0.25 × ATR.
- Failed breakout: within the next 1–5 bars, price closes back inside the TR. Trade the failure (fade) with the stop beyond the breakout extreme.
- This is the same structure as a Wyckoff spring or UTAD.

**Wyckoff trading range (TR).**
- After a decline of at least X% (for example a 20-bar return below −2 × ATR × √20, or below the 20th percentile of 20-bar returns), mark the SC as the bar with the lowest low in the window and volume ≥ 2 × N_v (climax multiplier between 1.8 and 3; this needs tuning). R ≥ 1.5 × ATR is optional.
- AR = the highest high within the next 3–15 bars. The TR is the band from the SC low to the AR high.
- Require width ≥ 1.5 × ATR and ≤ 6 × ATR (to exclude trends).
- ST = a bar within 0.5 × ATR of the SC low with volume < 0.8 × the SC's volume.

**Wyckoff spring (sourced parts plus proposed parts).**
- Sourced: low < the TR low (or the lowest low of the last 15–25 bars); the close is back above the TR low; the bar is not closing at its low.
- Proposed: closePos ≥ 0.4. Penetration depth ≤ 0.5 × ATR, or ≤ 10% of TR width; deeper is a breakdown, not a spring. The sourced rule is that consecutive lows more than 10% apart are unacceptable.
- Two volume variants to test separately:
  - (a) "Low-volume spring": volume < N_v, entry on the bar close.
  - (b) "Shakeout": volume > 1.5 × N_v, entry on a successful test, meaning a higher low on lower volume within 1–10 bars.
- Stop: the spring low − 0.1 × ATR.
- Targets: T1 = the TR midpoint, T2 = the TR top (Creek/AR). T3 = the P&F count, if box and reversal are defined (for example box = ATR-scaled, 3-box reversal).
- Distribution (UTAD) mirrors all of this.

**SOS and LPS.**
- SOS: a close above the AR high with R ≥ 1.2 × ATR and volume ≥ 1.5 × N_v.
- LPS: the first pullback after the SOS whose low holds above the TR midpoint (or above the AR high, which is the back-up/BU case), with volume < N_v. Entry: break of the LPS bar high.
- SOW and LPSY mirror this.

**VSA signs.**
- No demand / no supply: as sourced above. "Narrow spread" = R < 0.7 × ATR (proposed).
- Stopping volume: a down bar into a 20-bar low with volume ≥ 2 × N_v, closePos ≥ 0.5 and R ≥ ATR.
- Upthrust: high > the 20-bar high, close in the lower third (closePos ≤ 0.33), volume > N_v.
- Test: a down bar that makes a lower low intrabar, closes in the upper half, and has volume < each of the prior two bars, at or near support.
- Effort vs result: volume ≥ 2 × N_v and R ≤ 0.7 × ATR (absorption). An up bar of that shape after an up-leg is bearish, and the mirror case is bullish.
- Use VSA signs as filters or confirmations for the Wyckoff and S/D modules, not as standalone triggers. VSA's own sources say the same.

**Supply/demand zone.**
- Base: 1–6 consecutive bars, each with R ≤ 0.5–0.7 × ATR, and a total base range ≤ 1 × ATR.
- Departure: within 1–3 bars after the base, a move of at least 2 × ATR away from the base edge (between 1.5 and 3, to tune).
- Zone: from the base's lowest low to its highest body (demand), or the mirror (supply).
- Status: Fresh until first touched, then Tested (count touches), then Broken on a close beyond the far edge.
- Trade: first touch of a fresh zone in the direction of the higher-timeframe trend. Limit at the proximal edge, stop 0.25 × ATR beyond the distal edge, target at least 2R or the origin of the departure leg.

**Timeframes, as hypotheses for the agent.**
- Intraday: 5–15 minute bars for US/NSE equities; 15-minute to 1-hour bars for crypto. Use higher-timeframe trend (1-hour or 4-hour) as a filter.
- Swing: daily bars, with weekly trend as a filter.
- Wyckoff/P&F causes are classically built on daily/weekly charts over weeks to months, so Wyckoff TR detection on 1-minute crypto data will produce many spurious ranges.

**Data needed.**
- OHLCV for everything. Bid/ask or tick data improves intraday stop and fill realism.
- Volume caveats:
  - Crypto: volume is fragmented across exchanges, and one exchange's volume (for example Binance spot or perpetuals) is a proxy for total demand, not the whole of it.
  - US equities: consolidated tape volume works. Note that off-exchange volume is about 40%+ of the total, and it is included in the consolidated figures.
  - NSE: exchange volume is clean, plus there is delivery-volume data, which could enrich effort/result checks.
- P&F counts need a box size and a reversal amount. For crypto, use percentage boxes or ATR-scaled boxes.

**Grades.**
- Wyckoff (all events, P&F counts, composite operator): C. No peer-reviewed or independent systematic evidence of profitability; algorithmic work is detection-only.
- VSA: C.
- Supply/demand zones: C for the zone strategy itself. Adjacent academic evidence (Osler, see Q2) shows support/resistance levels have predictive content for bounces, but it does not show profitability.
- HH/HL market structure as a literal swing-pivot rule: C. Its close cousin, time-series momentum/trend, is A (see Q3).
- Al Brooks H1/H2, wedges, always-in, measured moves: C.

### Gaps
- I found no published canonical numeric thresholds from Wyckoff Analytics (Bogomazov/Hank Pruden), the Stock Market Institute or David Weis for TR width, climax volume multiples or spring penetration depth. Their materials stay qualitative or relative. I did not access David Weis' *Trades About to Happen* directly, so his wave-volume parameters are not reported here.
- I did not retrieve the Tom Williams originals (*Master the Markets* / *Undeclared Secrets*) to confirm exact VSA parameters beyond the no-demand/no-supply rule.
- I could not fetch the official Brooks glossary page (HTTP 404). The H1/H2 definitions come from the Brooks forum and glossary excerpts in search results. I found no exact Brooks rules for "always-in", wedge geometry or measured-move legs.
- Pal (2024) arXiv and JISEM 2024: I did not verify whether either reports after-cost trading results.

---

## Q2. Independent systematic backtests and academic studies, including NEGATIVE results; data-snooping caveats

### Takeaway
I found no peer-reviewed or independent systematic backtest of Wyckoff, VSA, supply/demand zones or Al Brooks setups that shows profit after costs. These methods are untested, not disproven. The closest academic evidence is mixed:
- Osler (2000) shows published support/resistance levels predict intraday FX "bounces" (60.8% vs 56.2% for random levels), which is evidence of predictive content but is not a profitability test.
- Lo, Mamaysky and Wang (2000) find algorithmically detected chart patterns carry "incremental information".
- The broad technical-rule literature shows profits that fade after the early 1990s and disappear under data-snooping corrections and modest transaction costs.

### Cited Findings

**Osler (2000), FRBNY Economic Policy Review: "Support for Resistance"**
- Design:
  - Data: minute-by-minute quotes for USD/DEM, USD/JPY and USD/GBP, New York hours, January 1996 – April 1998.
  - Levels tested: the support/resistance levels that six trading firms sent their customers each day.
  - A "hit" = bid (ask) within 0.01% of the level; 0.00% and 0.02% were also tested.
  - A "bounce" = price still on the original side of the level 15 minutes later; 30 minutes was also tested.
  - Significance came from a bootstrap against arbitrary (randomly generated) levels.
  — [Osler 2000, FRBNY EPR](https://www.newyorkfed.org/medialibrary/media/research/epr/00v06n2/0007osle.pdf)
- Results:
  - Rates bounced off published levels 60.8% of the time on average, versus 56.2% for arbitrary levels.
  - All 16 firm-currency pairs beat the arbitrary levels on average, and the result was significant at 5% for all but three pairs.
  - The excess bounce frequency was 4.2 points (mark), 5.6 (yen) and 4.0 (pound). The best case (Firm 1, yen) was +9.2 points.
  - Predictive power lasted at least five business days after the levels were published.
  — [Osler 2000](https://www.newyorkfed.org/medialibrary/media/research/epr/00v06n2/0007osle.pdf)
- Round numbers: more than 70% of published levels ended in 0, and 96% ended in 0 or 5. — [Osler 2000](https://www.newyorkfed.org/medialibrary/media/research/epr/00v06n2/0007osle.pdf)
- The paper's summary notes that the firms could identify turning points but did not correctly judge the relative strength of their levels. It tests predictive power, not profitability after costs. — [Osler 2000](https://www.newyorkfed.org/medialibrary/media/research/epr/00v06n2/0007osle.pdf); [NY Fed staff report 125](https://www.newyorkfed.org/medialibrary/media/research/staff_reports/sr125.html)
- Osler's follow-up, "Currency orders and exchange rate dynamics: an explanation for the predictive success of technical analysis" (Journal of Finance, 2003), links these effects to order clustering:
  - Take-profit orders cluster at round numbers, which explains reversals at those levels.
  - Stop-loss orders cluster just beyond round numbers, which explains trend acceleration after a level breaks.
  - Caveat: I took this content from prior knowledge; the abstract page timed out when fetched, so the title is confirmed but the summary is not. — [SUFE academic newsletter listing](https://academicnewsletter.sufe.edu.cn/info/361290)

**Lo, Mamaysky and Wang (2000), Journal of Finance 55(4): 1705–1765**
- They used nonparametric kernel regression to detect patterns such as head-and-shoulders and double bottoms automatically, on US stocks from 1962 to 1996. Comparing conditional and unconditional return distributions, "several technical indicators do provide incremental information and may have some practical value". — [MIT page](https://web.mit.edu/Alo/www/Papers/techanal.html); [NBER w7613](https://www.nber.org/papers/w7613)
- The abstract does not test trading profitability. — [MIT page](https://web.mit.edu/Alo/www/Papers/techanal.html)
- This is the methodological template for coding "subjective" chart concepts such as Wyckoff ranges, wedges and S/D bases: smooth the price, find local extrema, then match a geometric pattern.

**Brock, Lakonishok and LeBaron (1992), and its out-of-sample failure**
- Brock et al. tested 26 rules (moving averages and trading-range breakouts) on DJIA data from 1897 to 1986.
- Sullivan, Timmermann and White (1999, Journal of Finance) widened the rule universe, applied White's Reality Check bootstrap, and replicated the rules on the next 10 years (1987–1996). The best rule earned a statistically insignificant 8.63% per year before transaction costs.
— [Sullivan, Timmermann & White listing](https://academicnewsletter.sufe.edu.cn/info/361689); [LSE Research Online](https://researchonline.lse.ac.uk/id/eprint/119144)
- Trading-range breakout rules are the closest academic relative of "support/resistance breakout" and Brooks/Wyckoff breakout logic.

**Bajgrowicz and Scaillet (2012), Journal of Financial Economics 106(3): 473–491**
- They revisited technical rules on DJIA daily data from 1897 to 2011, using the False Discovery Rate to correct for data snooping.
- Persistence tests show "an investor would never have been able to select ex ante the future best-performing rules".
- Even in sample, performance "is completely offset by the introduction of low transaction costs".
— [RePEc](https://ideas.repec.org:443/a/eee/jfinec/v106y2012i3p473-491.html); [CFA Digest summary](https://rpc.cfainstitute.org/research/cfa-digest/2013/02/technical-trading-revisited-false-discoveries-persistence-tests-and-transaction-costs-digest)

**Park and Irwin (2007) survey**
- Of 92 "modern" studies, 58 found positive results for technical trading, 24 negative and 10 mixed.
- Profits were consistent "until the early 1990s".
- Most studies suffer from data snooping, ex post rule selection, and problems estimating risk and transaction costs.
— [AgEcon working paper version](https://ageconsearch.umn.edu/record/37487/files/AgMAS04_04.pdf); [RePEc](https://ideas.repec.org/p/ags/uiucrr/37487.html)

**India**
- A 27-year study of beta-sorted NSE 500 portfolios with SMA/EMA rules (5–100-day windows) finds positive alphas before transaction costs. Its emphasis on trading frequency shows that costs matter, especially for short windows. — [Investment Management and Financial Innovations 2025 (Bhama)](https://businessperspectives.org/journals/investment-management-and-financial-innovations/issue-501/can-technical-analysis-create-returns-for-beta-based-portfolios-in-the-indian-market)
- Another Indian-index MA study, as summarised in search results: the rules capture direction and give significant gross returns, but "cannot be exploited fully due to real world transaction costs". I could not confirm the exact paper behind this summary; it is likely the Quantitative Finance 2011 article. — [RePEc Quant Finance 2011 listing](https://ideas.repec.org/a/taf/quantf/v11y2011i2p287-297.html)
- A BSE study of RSI and MACD across market cycles found the RSI rule failed to deliver positive returns even before costs. MACD sell signals beat the mean in bear periods, before costs. — [Colombo Business Journal](https://mgmt.cmb.ac.lk/cbj/index.php/testing-the-profitability-of-technical-trading-rules-across-market-cycles-evidence-from-india/)

**Practitioner backtests of Wyckoff, VSA, zones and Brooks**
- Searches turned up only forum attempts (Aussie Stock Forums "Backtesting Wyckoff Method", the Optuma forum on zones) and indicator code without performance statistics. No documented sample sizes or after-cost results. — [Aussie Stock Forums](https://www.aussiestockforums.com/threads/backtesting-wyckoff-method.33512/); [Optuma forum](https://forum.optuma.com/t/supply-and-demand-zones/3392); [TradingView Al Brooks Second Entry (no performance claims)](https://www.tradingview.com/script/YUixSlPL-Al-Brooks-Second-Entry/)

### Inferences

**Data-snooping exposure.**
- Every module in Q1 has 3–8 free parameters: the climax multiplier, spring depth, base length, ATR departure multiple, pivot k, EMA length, signal-bar closePos threshold, and target R multiple. A modest grid of 3 values each gives 3^6 = 729 configurations per concept per market per timeframe.
- That is exactly the setting in which Sullivan-Timmermann-White and Bajgrowicz-Scaillet show apparent edges disappear.
- The agent should therefore:
  - pre-register one configuration per concept;
  - count every tested configuration as a trial;
  - use White's Reality Check, Hansen's SPA test or an FDR correction, or the deflated Sharpe ratio;
  - test against random-entry baselines matched on holding period and direction. This mirrors Osler's arbitrary-level bootstrap and is the natural null for "zone" and "spring" entries.

**Costs dominate.** Most of these setups are short-horizon: intraday bounces and spring fades with tight stops.
- Osler's +4–5 point bounce edge over 15 minutes is small relative to a typical spread plus slippage on equities or crypto alts.
- Expected gross edge per trade must be compared with round-trip costs:
  - Crypto perpetuals: about 4–10 basis points taker per side.
  - NSE: STT, exchange and stamp charges plus slippage. STT is higher on delivery than on intraday.
  - US: spread plus SEC/FINRA fees.

**Where these concepts have any academic support,** it is for level-reaction effects (round numbers, order clustering) and for trend persistence. The agent's best-grounded variants of "Wyckoff/S&D/Brooks" are therefore:
- (1) fade or bounce rules at clustered, round-number, prior-swing levels, with the stop just beyond the level (Osler's stop-loss cascade implies a break beyond the level should be respected); and
- (2) with-trend pullback entries (Q3).

**Regime decay.** Technical-rule profits on US large caps decayed after the early 1990s (Park & Irwin). Less efficient venues (NSE small and mid caps, crypto alts) may retain more edge, but they also carry higher costs and data problems.

### Gaps
- I found no peer-reviewed study that tests Wyckoff schematics, VSA bar signs, RBR/DBR zones or Brooks H2 setups for profitability after costs. I also found no independent, documented large-sample backtest from QuantifiedStrategies or Quantpedia on these specific methods; their search results surfaced only RSI and moving-average pullbacks.
- I did not retrieve the full results tables of Lo-Mamaysky-Wang (which patterns were significant on NYSE/AMEX versus Nasdaq).
- I did not quantify the break-even transaction costs for the Brock et al. rules.
- I could not fetch the Osler (2003) JF abstract (timeout). The order-clustering findings stated above come from prior knowledge and should be verified.

---

## Q3. Evidence on swing-point / pullback-in-trend entries that can stand in for "trade higher lows in an uptrend"

### Takeaway
The strongest evidence near "trade higher lows in an uptrend" comes from time-series momentum (trend), not from swing pivots themselves. Moskowitz, Ooi and Pedersen (2012, JFE: 58 futures, 1985–2009) and Hurst, Ooi and Pedersen (trend profitable since 1880) give A-grade support for being positioned with the 1–12-month trend. Liu and Tsyvinski (RFS 2021) find strong time-series momentum in crypto at weekly horizons. Pullback timing inside a trend (buying a dip while price is above its 200-day MA) is backed only by practitioner backtests (B/C). Those show high win rates but low exposure and returns well below buy-and-hold over the last decade.

### Cited Findings

**Time-series momentum in futures**
- Moskowitz, Ooi and Pedersen (2012, Journal of Financial Economics) studied 58 liquid futures (equity indices, bonds, commodities, currencies) over 1985–2009.
- Time-series momentum was positive in every asset class. Returns persist over 1 to 12 months and partially reverse over longer horizons.
- A diversified TSMOM portfolio earned substantial abnormal returns with little exposure to standard factors, and performed best in extreme markets.
— [SSRN abstract 2089463](https://papers.ssrn.com/abstract=2089463); [Quantpedia: Time-series momentum effect](https://quantpedia.com/strategies/time-series-momentum-effect/)

**A century of trend following**
- Hurst, Ooi and Pedersen built a time-series momentum strategy back to 1880 and found it "consistently profitable" with low correlation to traditional assets, including diversification benefits in equity bear markets.
- I did not read the exact net-of-cost figures in the excerpts retrieved.
— [AQR: A Century of Evidence on Trend-Following Investing](https://www.aqr.com/Insights/Research/Journal-Article/A-Century-of-Evidence-on-Trend-Following-Investing); [PDF mirror](https://fairmodel.econ.yale.edu/ec439/hurst.pdf)

**Crypto**
- Liu and Tsyvinski, "Risks and Returns of Cryptocurrency" (Review of Financial Studies 34(6), 2021, pp. 2689–2727), found a strong time-series momentum effect in Bitcoin, Ripple and Ethereum. Investor-attention proxies also forecast returns.
- Example: for the top fifth of past-week Bitcoin returns, the average next-week return was 11.2%, versus 2.6% for the bottom fifth. The figures are as reported via secondary coverage; the sample period for this example was not recorded.
— [NBER w24877](https://nber.org/papers/w24877); [SSRN 3226806](https://papers.ssrn.com/abstract=3226806)

**Practitioner pullback-in-uptrend backtests on SPY**
- QuantifiedStrategies "RSI pullback" strategy on SPY (exact rule parameters not captured from the snippet): 184 trades, 0.7% average gain per trade, 78% win rate, profit factor 2.8, CAGR 3.9%. — [QuantifiedStrategies: RSI pullback strategy](https://quantifiedstrategies.substack.com/p/rsi-pullback-strategy)
- Connors-style RSI(2) on the S&P 500. Rules: buy when RSI(2) < 5 and close > the 200-day SMA; exit when the close is above the 5-day SMA or below the 200-day SMA.
- Its October 2016 – October 2026 backtest returned +18.1% in total versus +255.4% for buy-and-hold. That is +1.7% per year, with a −21.5% maximum drawdown and a 71.2% win rate.
- This is a NEGATIVE result for absolute return over the most recent decade, even though the win rate is high.
— [Backtrex: Connors RSI(2) on S&P 500](https://backtrex.com/en/backtests/connors-rsi-2-sp-500)
- QuantifiedStrategies also publishes a long-term pullback system that combines the 200-day and 20-day moving averages. I did not capture its statistics. — [QuantifiedStrategies: A long-term pullback trading strategy](https://quantifiedstrategies.substack.com/p/a-long-term-pullback-trading-strategy)

### Inferences

**Use TSMOM as the backbone.** For "trade higher lows in an uptrend", the evidence-backed core is the trend filter: the sign of the 3–12-month return (swing) or a multi-week return (crypto). The pullback or H2 or HL trigger is a timing overlay whose incremental value has to be shown against a plain trend-following baseline that ignores timing.
- The recommended test for the agent: compare "TSMOM long plus HL/H2 entry" against "TSMOM long, entered at a random time within the trend" and against "TSMOM long, always invested". Use identical costs for all three.

**Pullback rules cut market exposure sharply.** In a strong bull decade, that drives CAGR far below buy-and-hold even with a 70–78% win rate. The Backtrex RSI(2) result shows this.
- Evaluate on risk-adjusted, exposure-adjusted metrics such as return per unit of time in the market, not on win rate.

**Horizon mismatch.** TSMOM evidence is at 1–12 months (futures) and 1–4 weeks (crypto). Intraday HH/HL trading on 1–15-minute bars has no comparable academic support in these sources, so treat intraday versions as C-grade until tested.

**Grades for this question.**
- With-trend positioning (TSMOM): A for futures and multi-asset portfolios. For crypto it is peer-reviewed predictability evidence (RFS); after-cost profitability was not verified here.
- Pullback-in-trend entries (RSI(2) / 200-day MA, H2): B. These are systematic backtests by practitioners, mostly on US indices; results are mixed and recently weak in absolute terms.
- Literal swing-pivot HH/HL rules: C.

### Gaps
- I found no peer-reviewed paper that tests swing-pivot (fractal or ZigZag) HH/HL definitions or Brooks bar-counting entries directly, before or after costs.
- I did not retrieve the exact Sharpe ratios or after-cost numbers for Moskowitz-Ooi-Pedersen or Hurst-Ooi-Pedersen.
- I found no NSE-specific or SOL-specific evidence on pullback-in-trend entries.
- Short-term reversal literature (Jegadeesh 1990; Lehmann 1990), which could justify "buy the dip within momentum", was not retrieved in this pass.
