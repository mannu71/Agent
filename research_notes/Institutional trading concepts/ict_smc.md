# ICT (Inner Circle Trader) and Smart Money Concepts (SMC): codeable definitions and evidence

Scope: Michael J. Huddleston's ICT method and the wider "SMC" retail framework, made into exact rules for an algorithmic paper-trading agent (crypto BTC/ETH/SOL, US stocks/ETFs, NSE stocks; intraday and swing). Evidence grades used throughout: **A** = peer-reviewed evidence of profitability after costs; **B** = independent systematic backtests (including negative ones); **C** = practitioner claims only.

Provenance labels used below:
- **[TEACHER]**: rule as described by ICT/SMC teaching material. Usually from secondary glossaries such as the LuxAlgo Library and innercircletrader.net. ICT's own lectures are YouTube videos with no written specification.
- **[CODER]**: rule fixed by a programmer (smartmoneyconcepts Python package, TradingView scripts, backtest authors). These choices are NOT stated by ICT.

---

## Q1. What exact, codeable definitions exist for each concept, and what are the typical entry/stop/target/timeframe and evidence grade?

### Takeaway
Every ICT/SMC concept has at least one open-source mechanical version. The most complete is the `smartmoneyconcepts` Python package (joshyattridge): FVG, swings, BOS/CHoCH, order blocks, liquidity, previous high/low, sessions and retracements. Its parameters are coder choices, though, and it has lookahead built in (centered swing windows, full-sample liquidity tolerance), so it must be lagged or rewritten before live or paper use. Teacher definitions leave key joints discretionary: "displacement", which pool is the "draw on liquidity", and kill-zone boundaries, which vary 30–90 minutes across ICT sources. No concept has grade A. Only FVG, order block, liquidity sweep and OTE (and, via a large SSRN study, about 44 SMC detectors) reach grade B, and the B evidence is null or negative.

### Cited Findings

**General: the smartmoneyconcepts Python package (joshyattridge/smart-money-concepts) [CODER]**
- Public API and defaults: `fvg(ohlc, join_consecutive=False)`, `swing_highs_lows(ohlc, swing_length=50)`, `bos_choch(ohlc, swing_highs_lows, close_break=True)`, `ob(ohlc, swing_highs_lows, close_mitigation=False)`, `liquidity(ohlc, swing_highs_lows, range_percent=0.01)`, `previous_high_low(ohlc, time_frame="1D")`, `sessions(ohlc, session, start_time, end_time, time_zone="UTC")`, `retracements(ohlc, swing_highs_lows)`. README disclaimer: "for educational purposes only. Do not use this indicator as a sole decision maker" — [README](https://raw.githubusercontent.com/joshyattridge/smart-money-concepts/master/README.md)
- Source code inspected directly (smc.py, 987 lines, master branch as of 2026-10-07) — [smc.py](https://raw.githubusercontent.com/joshyattridge/smart-money-concepts/master/smartmoneyconcepts/smc.py). Details per concept below.

**Swing highs/lows (fractals), the foundation of market structure**
- [CODER, smc package] The code sets `swing_length *= 2` and then marks a swing high where `high == high.shift(-(swing_length//2)).rolling(swing_length).max()`. That is a **centered window of about 2×swing_length bars, about 50 bars each side at the default**, so the swing is confirmed only after swing_length future bars. A cleanup loop then forces alternation: of consecutive highs it keeps the higher, and of consecutive lows the lower. The code also forces the first and last bars to opposite-type swings — [smc.py](https://raw.githubusercontent.com/joshyattridge/smart-money-concepts/master/smartmoneyconcepts/smc.py)
- [TEACHER] ICT/SMC teaching gives no fixed N. Teachers refer to "swing points" and "internal vs swing structure". The LuxAlgo indicator exposes separate "Internal Structure" and "Swing Structure" layers, but its public docs page does not list the default lengths — [LuxAlgo SMC docs](https://www.luxalgo.com/library/indicator/smart-money-concepts-smc/)

**Market structure: HH/HL/LH/LL, BOS, CHoCH, MSS**
- [CODER, smc package] Bullish BOS: the last 4 alternating swings are `[-1, 1, -1, 1]` (low, high, low, high) with levels `L[-4] < L[-2] < L[-3] < L[-1]`, i.e. a higher low and a higher high. The broken level is `level_order[-3]`, the prior swing high. Bullish CHoCH: same swing order but `L[-1] > L[-3] > L[-4] > L[-2]`, i.e. the last low undercut the earlier low (a lower low) and then price made a higher high. Bearish versions mirror these. The break is confirmed at the first candle from i+2 onward whose **close** (default `close_break=True`) or high/low crosses the level — [smc.py](https://raw.githubusercontent.com/joshyattridge/smart-money-concepts/master/smartmoneyconcepts/smc.py)
- [TEACHER] MSS in ICT's 2022 model: "an energetic leg that closes through a recent opposing swing point". Displacement is required but not quantified — [LuxAlgo Model 2022](https://www.luxalgo.com/library/concept/model-2022.md)
- [CODER, TradingView "ICT Silver Bullet & 2022 Entry Model" by greymyst] After a sweep, the script takes the minor swing formed before the sweep and labels "ChoCh" when price closes past it — [TradingView script](https://br.tradingview.com/script/edyKC9Ey-ICT-Silver-Bullet-2022-Entry-Model/)

**Fair value gap (FVG) / imbalance (BISI = bullish, SIBI = bearish)**
- [CODER, smc package] Bullish FVG at the middle candle i: `high[i-1] < low[i+1]` AND `close[i] > open[i]`. Bearish: `low[i-1] > high[i+1]` AND `close[i] < open[i]`. Top/bottom are the gap edges. The `MitigatedIndex` is the first later candle that trades into the gap. Optional `join_consecutive` merges adjacent FVGs — [smc.py](https://raw.githubusercontent.com/joshyattridge/smart-money-concepts/master/smartmoneyconcepts/smc.py)
- [CODER, StatOasis backtest] "the high of bar i−2 sits below the low of bar i, leaving a gap. Enter long on the first later bar that retraces into the gap." No candle-colour condition — [StatOasis](https://statoasis.com/overfit/research/ict-backtest-what-survives)
- Note: the smc package requires the middle candle to be up-closing for a bullish FVG. The StatOasis version has no such filter. Detector counts will differ.
- [TEACHER] Consequent encroachment means the 50% midpoint of the gap. Some traders use it as the entry or invalidation threshold — [LuxAlgo Inversion FVG](https://www.luxalgo.com/library/concept/inversion-fvg.md)
- Evidence grade: **B, negative**. See Q2: SPY daily 5-day edge −0.005% (t=−0.08). In a random walk, FVG "fill" rates match those of random levels.

**Inverse / inversion FVG (IFVG) and balanced price range**
- [TEACHER] IFVG: "a candle body closing beyond the far side of the gap" (a wick alone is a probe, not an inversion). A violated bullish FVG becomes resistance and vice versa. Enter on the retest with "the stop just beyond the far side of the gap". Stricter and looser variants exist (full gap closure vs midpoint close). Balanced price range: "two opposing gaps overlapping" — [LuxAlgo Inversion FVG](https://www.luxalgo.com/library/concept/inversion-fvg.md)
- Evidence grade: **C**. No systematic test found.

**Order block (OB)**
- [TEACHER] "the last opposing candles before a move", expected to be defended on return. Once defence fails it becomes a breaker — [LuxAlgo Breaker Block](https://www.luxalgo.com/library/concept/breaker-block.md)
- [CODER, smc package] Bullish OB: when a candle **closes above** the most recent swing high (first cross only), the OB is the candle with the **lowest low** between that swing high and the breaking candle (last occurrence on ties). Its zone is that candle's [low, high]. If no candle lies in between, the previous candle is used. A bullish OB is "mitigated" (flagged as breaker) when low < OB bottom, or with `close_mitigation=True` when min(open, close) < bottom. It is deleted when price later trades above its top. `OBVolume` = volume of the break candle + the 2 prior candles. `Percentage` = min(highVolume, lowVolume)/max(...) — [smc.py](https://raw.githubusercontent.com/joshyattridge/smart-money-concepts/master/smartmoneyconcepts/smc.py)
- [CODER, StatOasis] "a down-close bar confirmed by an up-impulse within the next K bars (the move must clear M × ATR20). The bar's range becomes the zone; enter long on the first later bar that retraces into it." 18 variants of K and M were tested — [StatOasis](https://statoasis.com/overfit/research/ict-backtest-what-survives)
- Evidence grade: **B, weak or null**. Best single-concept result in StatOasis: beat random entry in 81.5% of SPY variants, but t=+1.22 and it never beat buy-and-hold. See Q2.

**Breaker block and mitigation block**
- [TEACHER] Breaker: "an order block that failed and flipped roles". The strict ICT version requires a liquidity run: price raids beyond a prior extreme, reverses, and breaks back through the swing that preceded the failed move. That failed leg's candles (down-closing ones, in the bullish case) become support. Mitigation block: the same role-flip **without** the raid, where the swing fails short of the prior extreme. "Many tools conflate them" — [LuxAlgo Breaker Block](https://www.luxalgo.com/library/concept/breaker-block.md)
- [TEACHER] Mitigation-block entry is the retracement into the block after the structure shift. The stop goes beyond the far side of the block or beyond the failed swing. "they fail routinely, especially against strong momentum" — [LuxAlgo Mitigation Block](https://www.luxalgo.com/library/concept/mitigation-block.md)
- [CODER, smc package] The package marks a mitigated bullish OB as `breaker=True` and resets it if price later exceeds the OB top. It does not test the strict "raid first" condition — [smc.py](https://raw.githubusercontent.com/joshyattridge/smart-money-concepts/master/smartmoneyconcepts/smc.py)
- Evidence grade: **C**. No independent test found. The Tangirala SSRN study (Q2) may include breaker detectors, but this could not be confirmed.

**Liquidity pools: equal highs/lows, buy-side (BSL) and sell-side (SSL) liquidity**
- [CODER, smc package] `pip_range = (max(high) − min(low)) of the WHOLE dataframe × range_percent (0.01)`. Swing highs within ±pip_range of each other are grouped, and at least 2 are needed for a "liquidity" level. **Swept** = first later candle whose high ≥ level + pip_range (for BSL), or whose low ≤ level − pip_range (for SSL) — [smc.py](https://raw.githubusercontent.com/joshyattridge/smart-money-concepts/master/smartmoneyconcepts/smc.py)
- [CODER, smc package] `previous_high_low(time_frame="1D")` resamples to prior-period highs/lows (15m, 1H, 4H, 1D, 1W, 1M) and returns BrokenHigh/BrokenLow flags. These are the usual "PDH/PDL" liquidity references — [README](https://raw.githubusercontent.com/joshyattridge/smart-money-concepts/master/README.md)
- [TEACHER] The LuxAlgo SMC indicator detects "Equal Highs & Lows" with a "Bars Confirmation" parameter (default value not stated on the docs page) — [LuxAlgo SMC docs](https://www.luxalgo.com/library/indicator/smart-money-concepts-smc/)
- Evidence grade: **C** for pools as such. Stop clustering at round numbers is real (Q3), but no test of "equal highs" as a stop magnet was found.

**Liquidity sweep / stop hunt / turtle soup / Judas swing**
- [CODER, StatOasis] Liquidity sweep: "price breaks below the N-bar low intrabar (the 'stop hunt') but closes back above it — enter long." 3 variants — [StatOasis](https://statoasis.com/overfit/research/ict-backtest-what-survives)
- [TEACHER, predates ICT] Turtle Soup (Connors & Raschke, *Street Smarts*, 1995). Buy rule: today prints a new 20-day low while the previous 20-day low is **at least 4 sessions old**. Enter on a buy stop back above that prior low. Initial stop below the new extreme — [LuxAlgo Turtle Soup](https://www.luxalgo.com/library/concept/turtle-soup.md)
- [TEACHER] Judas swing: "a push in one direction, usually shortly after a session open, that runs resting stops before the day's real move develops the other way." The primary reference is the **midnight New York opening price**, and it is expected in the London kill zone (about 02:00–05:00 NY) or at the NY open. Price must reject and break back through structure with displacement. If price holds beyond the swept level it is a trending open, not a Judas swing. "No statistical studies" — [LuxAlgo Judas Swing](https://www.luxalgo.com/library/concept/judas-swing.md)
- Evidence grade: **B, negative for the reversal reading**. The Mahadzva 2026 FX study (reported second-hand) found that sweeps predict **continuation**. The SSRN S&P 500 study found liquidity-sweep detectors had the largest point estimates, 11–12 bp over 10 days, but p ≥ 0.23. See Q2.

**Premium/discount and OTE**
- [TEACHER] Equilibrium = 50% of the dealing range. Above 50% is premium (sell zone), below is discount (buy zone). OTE band = **61.8%–79% retracement** (ICT rounds to 62–79%), "sweet spot" 70.5%. Anchor longs from swing low L0 to swing high H1, with `level(p) = H1 − p × (H1 − L0)`. Stop beyond the anchoring swing. Target the swing extreme first, then extensions. Precondition: "liquidity taken, structure shifted, then a retracement into the band" — [LuxAlgo OTE](https://www.luxalgo.com/library/concept/optimal-trade-entry.md)
- [CODER, smc package] `retracements()` returns current and deepest % retracement of the current swing leg. Bullish: `100 − ((low − bottom)/(top − bottom))×100` — [smc.py](https://raw.githubusercontent.com/joshyattridge/smart-money-concepts/master/smartmoneyconcepts/smc.py)
- [CODER, StatOasis] "after an upward structure shift, enter on the retrace into the 61.8%–78.6% Fibonacci band of the last swing leg." 6 variants — [StatOasis](https://statoasis.com/overfit/research/ict-backtest-what-survives)
- Evidence grade: **B, negative**. OTE beat random entry in 0% of its SPY variants. Its only significant t-stat (DIA, 10-day, t=+2.07) was 1 of 32 tests, which is consistent with chance.

**Kill zones / sessions (time windows)**
- [TEACHER, per LuxAlgo, NY local time]: Asian 20:00–22:00; London Open 02:00–05:00; New York AM 07:00–10:00; London Close 10:00–12:00; New York PM 13:30–16:00. "Window boundaries differ by 30 to 90 minutes across ICT sources". Index futures traders may use 08:30–11:00 for NY AM. Anchor charts to New York time so the windows survive DST — [LuxAlgo Killzones](https://www.luxalgo.com/library/concept/killzones.md)
- [TEACHER, alternative secondary source]: Asia 20:00–22:00 EST, London 02:00–05:00 EST, NY AM 08:00–11:00 EST, NY PM 13:00–15:00 EST — [ChartingLens](https://chartinglens.com/blog/ict-kill-zones-guide) / [MetalsMine](https://www.metalsmine.com/thread/1345607-understanding-ict-kill-zones-tflab-free)
- [CODER, smc package; hard-coded UTC, no DST adjustment]: Sydney 21:00–06:00, Tokyo 00:00–09:00, London 07:00–16:00, New York 13:00–22:00, Asian kill zone 00:00–04:00, London open kill zone 06:00–09:00, New York kill zone 11:00–14:00, London close kill zone 14:00–16:00 (all UTC) — [smc.py](https://raw.githubusercontent.com/joshyattridge/smart-money-concepts/master/smartmoneyconcepts/smc.py)
- Related academic fact: FX intraday activity and volatility follow a U-shape for Tokyo and London participants but not New York (Ito & Hashimoto, NBER w12413) — cited via [ThorTradeCopier](https://thortradecopier.com/blog/does-ict-smart-money-concepts-work), original at [NBER](https://www.nber.org/papers/w12413)
- Evidence grade: **C** for kill zones as an edge. Session volatility and liquidity seasonality itself is well documented academically, but that is not evidence of directional profit.

**Power of 3 / AMD (accumulation–manipulation–distribution)**
- [TEACHER] AMD frames the daily candle. Accumulation is the consolidation, often the Asian range. Manipulation is the Judas swing beyond the open. Distribution is the expansion the other way. The Judas swing is "the middle act" — [LuxAlgo Judas Swing](https://www.luxalgo.com/library/concept/judas-swing.md). Model 2022 is described as "the tradeable core" of the AMD template — [LuxAlgo Model 2022](https://www.luxalgo.com/library/concept/model-2022.md)
- Evidence grade: **C**. No test found.

**ICT Silver Bullet**
- [TEACHER] Three one-hour windows (NY time): **03:00–04:00, 10:00–11:00, 14:00–15:00**. The trade is an FVG entry hunted inside the window, toward a liquidity draw. Unlike Model 2022, it is time-boxed — [LuxAlgo Model 2022](https://www.luxalgo.com/library/concept/model-2022.md); [TradingView greymyst script](https://br.tradingview.com/script/edyKC9Ey-ICT-Silver-Bullet-2022-Entry-Model/)
- [CODER, greymyst open-source script]: (1) a raid of a prior swing high or low (BSL/SSL), (2) ChoCh, a close beyond the minor swing that preceded the sweep, (3) an FVG searched within "the exact 3-5 candles that broke the ChoCh level", (4) entry on touch of the FVG's open edge, (5) **stop 1 tick beyond the raid extreme**, target the opposing major liquidity pool or **1:2 R:R** — [TradingView script](https://br.tradingview.com/script/edyKC9Ey-ICT-Silver-Bullet-2022-Entry-Model/)
- Practitioner claim: independent (non-systematic) backtesters report 50–65% win rates, below the 70–80% promotional figures, with about 1:3 R:R targets. No sample sizes or costs were given — [Backtrex](https://backtrex.com/en/blog/ict-silver-bullet-strategy-trading-guide)
- Evidence grade: **C**.

**ICT 2022 Mentorship Model**
- [TEACHER] A fixed three-beat sequence: **raid → shift → entry**. (1) Price runs a visible pool (an old low for longs), ideally into a higher-timeframe objective. (2) Displacement closes through a recent opposing swing (MSS). (3) Enter on the retracement into the FVG left by the displacement leg. Stop beyond the raided extreme, target liquidity on the far side. Bias and pools are read on 15m–1H charts, execution on 1–5m charts, usually in the London or NY AM kill zones. "Discretionary at every joint"; "no published edge proof" — [LuxAlgo Model 2022](https://www.luxalgo.com/library/concept/model-2022.md)
- Evidence grade: **C**, or **B-negative** if the Mahadzva FX study's CHoCH/sweep rules count as a proxy (see Q2).

### Inferences
- **Lookahead traps in the smc package (critical for a paper-trading agent).**
  1. `swing_highs_lows` uses a centered rolling window built with `shift(-swing_length)`, so a swing at bar i is known only at bar i+swing_length. A live detector must emit the swing with that delay.
  2. `liquidity()` sizes its tolerance from the whole dataframe's high–low range, which includes future bars. Replace it with a trailing ATR-based or %-of-price tolerance.
  3. `ob()` relies on swing indices that came from the centered window.
  4. Session times are fixed UTC and ignore DST. ICT anchors to New York local time, which follows US DST.

  All four follow directly from the code quoted above.
- Suggested rule set for the agent [CODER-INVENTED, to be validated, not teacher-stated]:
  - **Swings**: fractal with N bars each side. N=2–5 for "internal" structure and N=10–50 for "swing" structure. Confirm at bar i+N.
  - **FVG**: bullish if `high[i-2] < low[i]`, with an optional minimum size ≥ k×ATR(14) to filter noise.
  - **Sweep**: bar's low < prior swing low or prior-day low, AND close > that level.
  - **MSS**: close beyond the most recent opposing confirmed swing within M bars of the sweep. "Displacement" = that bar's body ≥ 1.5×ATR or the leg contains an FVG.
  - **Entry**: limit order at the FVG edge or 50% (CE). Stop 1 tick beyond the sweep extreme. Target the opposing pool or 2R.
  - **Time filter**: NY-local windows.

  Each numeric choice must be treated as a free parameter and counted in a multiple-testing correction.
- Crypto (24/7) can use NY-time kill zones directly. For US equities the NY AM window overlaps the cash open (09:30). NSE (09:15–15:30 IST) has no ICT-defined windows. Kill zones for NSE would be coder-invented, e.g. the first and last 60–75 minutes by analogy to the U-shape.
- Daily-bar swing trading versions (e.g. turtle soup on 20-day lows, daily FVG/OB) are the ones with independent tests (Q2), and those tests are null. Intraday versions are essentially untested independently except in FX.

### Gaps
- LuxAlgo SMC Pine source defaults (internal/swing lengths, EQH/EQL threshold as an ATR multiple, OB volatility filter, FVG auto-threshold, premium/discount band %) could not be retrieved. The docs page omits them, the TradingView page did not expose source, and the GitHub code search API was blocked. Read the open-source Pine directly on TradingView (script "Smart Money Concepts (SMC) [LuxAlgo]") before relying on any recollected values.
- No written primary specification from ICT himself was retrieved. His definitions live in YouTube lectures and paid mentorships, so all [TEACHER] items above come from secondary glossaries (mainly LuxAlgo Library). innercircletrader.net returned empty content on fetch.
- No codeable definition of "displacement" from ICT was found. It is described qualitatively everywhere.

---

## Q2. Are there independent systematic backtests of SMC/ICT rules, and what did they show after costs (including negative results)?

### Takeaway
Yes, and they are consistently null-to-negative. (1) A 2026 SSRN working paper (Tangirala) codified 44 SMC detectors from two open-source Pine scripts and tested them on 496 S&P 500 stocks (daily, 2010–Jun 2026, 2.2M events, 17.6M simulated trades). After multiple-testing correction, none beats a matched random-entry null at any horizon. (2) A StatOasis study of 648 daily backtests on SPY/QQQ/DIA/IWM found 0 that beat buy-and-hold, and only 1 of 32 forward-edge t-stats > 2, even with zero costs. (3) A 2026 SSRN FX study (Mahadzva), reported second-hand, found "no evidence that ICT/SMC concepts work as literally taught", and that sweeps predict continuation, not reversal. None is peer-reviewed, so the ceiling is grade B, and that B is negative.

### Cited Findings
- **Tangirala (SSRN 7483658), "Do Smart Money Concepts Predict Returns? Evidence from 44 Formalised Detectors on S&P 500 Daily Bars, 2010-2026"**:
  - Method: translates "two widely used open-source Pine Script implementations into 44 formally specified event detectors" and tests 496 S&P 500 constituents, Jan 2010–Jun 2026. 2.20M events, 17.6M simulated trades, 8 holding horizons.
  - Against a zero-return null, 38 of 44 look significant. Against a **matched null**, with multiple-comparison corrections over 352 concept×horizon hypotheses, "none beats random entry at any horizon and none is significantly worse."
  - The largest well-measured point estimates are 11–12 bp over 10 days, both liquidity-sweep detectors, with p ≥ 0.23.
  - For 10 concepts the data exclude, at 95% confidence, an edge large enough to cover the lowest modelled round-trip cost of 11 bp.
  - Source: [SSRN abstract page](https://papers.ssrn.com/sol3/papers.cfm?abstract_id=7483658). Note: SSRN returned HTTP 403 on direct fetch, so these figures come from the search-engine abstract snippet. The paper is an SSRN working paper, not peer-reviewed.
- **StatOasis, "I Backtested ICT / Smart Money Concepts — What Survives"**:
  - Design: SPY (from 1993), QQQ (1999), DIA (1998), IWM (2000), daily bars only. 648 backtests: OB 18 variants, FVG 18, liquidity sweep 3, OTE 6, plus RSI(14), SMA-pullback and inside-bar controls.
  - Costs: **frictionless** (no commission or slippage), even though the best variants took 76–310 trades.
  - Results: "0 of the 648 backtests beat simply holding the index on net profit." Best SPY net profit: OB $110,040, FVG $100,388, vs buy-and-hold $551,738 and random-entry mean $46,270.
  - SPY 5-day forward edge: OB +0.121% (t=+1.22), FVG −0.005% (t=−0.08), sweep +0.119% (t=+0.94), OTE −0.028% (t=−0.17).
  - "Across all four markets and both measured horizons, the ICT concepts produced 32 t-stats and exactly one crossed 2 (OTE on DIA at the 10-day horizon, t = +2.07)."
  - OB beat random in 81.5% (SPY), 66.7% (DIA), 59.3% (QQQ) and 55.6% (IWM) of variants. OTE beat random in 0% of SPY variants.
  - Self-stated limit: "ICT is taught as an intraday, discretionary craft. This study proves nothing about what a skilled discretionary trader does at 9:47am on a 5-minute chart."
  - Source: [StatOasis](https://statoasis.com/overfit/research/ict-backtest-what-survives)
- **Mahadzva (2026), "Smart Money or Costly Folklore? A Systematic Evaluation of ICT/Smart-Money-Concepts Trading Rules in G10 FX"** (SSRN, doi 10.2139/ssrn.7430998):
  - Markets: EURUSD, GBPUSD, AUDUSD, USDJPY, intraday.
  - "Three separately built liquidity-sweep rules agreed, in 65 of 66 combined fold selections, that a swept level predicts continuation, not the textbook reversal."
  - The flagship survivor is a **faded** (inverted) EURUSD 1-hour CHoCH strategy: p = 0.0192, "+19.3% on 60 holdout trades with a profit factor of 1.50".
  - Abstract: "We find no evidence that ICT/SMC concepts work as literally taught."
  - Source: reported via [ThorTradeCopier blog](https://thortradecopier.com/blog/does-ict-smart-money-concepts-work). Not verified at source: a direct search for the title returned nothing and SSRN blocks fetches.
- **Bindra (2026)**: unreviewed preprint reporting "78.0% win rate and +265.0R on 473 backtested fair value gap trades". The critique notes the abstract "does not mention an out-of-sample test, a random baseline or transaction costs" — reported via [ThorTradeCopier](https://thortradecopier.com/blog/does-ict-smart-money-concepts-work). Treat as a practitioner-grade claim.
- **Practitioner and vendor material** (not systematic): Silver Bullet backtesters report 50–65% win rates vs promotional 70–80%, and the source notes a "gap between the strategy as taught and its creator's documented live results" — [Backtrex](https://backtrex.com/en/blog/ict-silver-bullet-strategy-trading-guide). Vendor pages claim "65-75% win rates" with no reproducible method — [Lunefi](https://lunefi.com/blog/ict-trading-strategy-2026-65-75-win-rates-prop-firm).

### Inferences
- The recurring pattern is that SMC events look "significant" against a zero-return null, simply because the underlying assets drift up, but show nothing against a matched random-entry null. A paper-trading agent must therefore always benchmark each concept against random entries matched on time, direction and holding period, not against zero.
- The one consistent directional hint across studies concerns sweeps. They show small positive drift in equities (Tangirala's best estimates, StatOasis t≈0.9–1.2 for sweep/OB longs), and continuation rather than reversal in FX (Mahadzva). That is consistent with Osler's finding that crossing stop clusters propagates trends (Q3). If anything, the testable hypothesis is "sweep → continuation", the opposite of ICT's reversal framing.
- Untested territory: intraday ICT setups on crypto (BTC/ETH/SOL) and on NSE stocks. No independent systematic backtest was found for either. The agent's own validation would be novel evidence, and must use holdout data, costs and a random baseline.

### Gaps
- Full text of Tangirala and Mahadzva could not be read (SSRN 403), so exact detector definitions, cost models, intraday coverage and per-concept tables are unknown. The Mahadzva figures are second-hand.
- No independent systematic backtest of the Silver Bullet, Model 2022, kill zones, AMD/Power of 3, breaker or mitigation blocks as a full time-boxed intraday system was found, and none with costs.
- No systematic backtests on crypto or NSE were found. YouTube "backtest channels" were not examined (manual replay results are not systematic and were out of reach in this tool budget).

---

## Q3. Is there academic evidence on stop-hunting / liquidity sweeps around round numbers or prior highs/lows?

### Takeaway
Yes, for the *mechanism*, though not for ICT's trading rules. Osler's Federal Reserve Bank of New York work, using a dealing bank's actual order book, shows the following. Stop-loss orders cluster just beyond round numbers: buy-stops just above, sell-stops just below. Take-profits cluster at the round number itself. Crossing such levels produces faster, longer-lasting, self-reinforcing moves ("price cascades"), while take-profit clusters produce more frequent reversals. That supports "liquidity pools exist and get run". It also implies that sweeps of stop clusters tend to *continue*, which contradicts ICT's reversal framing.

### Cited Findings
- **Osler, "Stop-Loss Orders and Price Cascades in Currency Markets"** (FRBNY Staff Report 150):
  - Claim: stop-loss orders "contribute to rapid, self-reinforcing price movements". (1) Trends are unusually rapid when rates reach levels where stop-losses cluster. (2) The response to stop-loss orders is larger than to take-profits. (3) It lasts longer.
  - Persistence: "Most results are statistically significant for hours, although not for days."
  - Order data: a major dealing bank, Aug 1 1999 – Apr 11 2000. **9,655 orders**, over $55 billion face value, in USD/JPY, GBP/USD and EUR/USD. Stop-losses are 43% of orders by volume.
  - Round-number clustering: almost 10% of all orders are at rates ending in 00, about 3% at each other rate ending in 0, about 2% at each rate ending in 5.
  - Stop placement: 14.3% of executed stop-loss **buy** orders end in [01,10], vs 6.9% in [90,99], i.e. just above round numbers. 9.9% of executed take-profits sit exactly on 00, vs 3.8% of stop-losses.
  - Price test: one-minute quotes for DEM/USD, JPY/USD and GBP/USD in New York hours (09:00–16:00), Jan 1996 – Apr 1998. Over the 15 minutes after crossing a round number, dollar-mark moves an average **0.061% vs 0.054%** after crossing an arbitrary number. Round numbers were higher in 51 of 58 ten-day intervals, p < 0.001%.
  - Reversal frequency at round numbers (take-profit effect) stays significant for less than 30 minutes. Movement after crossing stays significant for at least 2 hours.
  - Market lore: "running the stops".
  - Source: [NY Fed SR150 PDF](https://www.newyorkfed.org/medialibrary/media/research/staff_reports/sr150.pdf) (read in full text); [RePEc entry](https://econpapers.repec.org/RePEc:fip:fednsr:150)
- **Osler, "Currency Orders and Exchange-Rate Dynamics: An Explanation for the Predictive Success of Technical Analysis"** (FRBNY Staff Report 125): documents the clustering of stop-loss and take-profit orders that SR150 builds on — [NY Fed SR125](https://www.newyorkfed.org/medialibrary/media/research/staff_reports/sr125.pdf). A secondary summary cites 9,667 orders, Sep 1999–Apr 2000, and 14.4% vs 7.4% clustering above vs below round numbers for buy stops — [ThorTradeCopier](https://thortradecopier.com/blog/does-ict-smart-money-concepts-work). These numbers differ slightly from SR150's (9,655; 14.3% vs 6.9%), probably reflecting different versions of the same dataset.
- **Osler (2000), "Support for Resistance: Technical Analysis and Intraday Exchange Rates"** (FRBNY Economic Policy Review): on 1-minute FX data, Jan 1996 – Mar 1998, rates "bounced off arbitrary support and resistance levels 56.2 percent of the time" vs 60.8% for published levels, a 4.6 pp edge — [NY Fed EPR](https://www.newyorkfed.org/medialibrary/media/research/epr/00v06n2/0007osle.pdf) (figures via [ThorTradeCopier](https://thortradecopier.com/blog/does-ict-smart-money-concepts-work)).
- **Turtle Soup** (Connors & Raschke, 1995) is the pre-ICT practitioner codification of a failed breakout of a prior 20-day extreme — [LuxAlgo Turtle Soup](https://www.luxalgo.com/library/concept/turtle-soup.md). Grade C.

### Inferences
- Grade for the *phenomenon* (stop clustering beyond round numbers and prior extremes, plus cascades): strong empirical support from central-bank research on proprietary order data. This is not grade A as a *trading-profit* claim; Osler measures conditional price behaviour, not net-of-cost strategy P&L.
- Codeable implications for the agent:
  - Treat round numbers (00/50 levels in FX; for BTC e.g. 1,000/5,000/10,000 multiples) and prior swing extremes as candidate stop-cluster levels.
  - Expect *continuation* after a clean break, measured over 15 minutes to 2 hours, and *short-lived reversal* at round-number take-profit clusters (< 30 minutes).
  - Whether this transfers to crypto, US stocks or NSE is untested here.

### Gaps
- No academic study was found that tests "equal highs/lows" specifically, as opposed to round numbers, as stop magnets, and none on crypto or NSE stop-hunt dynamics. This was not searched exhaustively within the tool budget. Crypto liquidation-cascade literature exists and may be relevant, but was not covered here.
- The published journal versions of Osler SR150 and SR125 were not verified in this session (they are commonly cited as *J. Int. Money & Finance* 2005 and *J. Finance* 2003). Cite the FRBNY staff reports, which were read, unless the journal versions are checked.

---

## Q4. What do critics say (unfalsifiable, repackaged classic TA, survivorship of influencer claims)?

### Takeaway
Critics make five main points. (a) The vocabulary renames older ideas: supply/demand zones, gaps, Wyckoff, Turtle Soup, session ranges. (b) The core narrative (an "IPDA" interbank price-delivery algorithm, deliberate manipulation) has no verifiable evidence. (c) Discretion at every step makes it hard to falsify. (d) Headline claims such as "FVGs usually fill" ignore base rates. (e) Influencer win rates lack audited live records. The independent tests in Q2 support (d), and support (c) indirectly.

### Cited Findings
- "The vocabulary (order blocks, fair value gaps, liquidity sweeps, structure breaks, kill zones) largely renames older supply-and-demand, gap and session ideas". "zero verifiable evidence of IPDA's existence beyond what ICT speaks of". There was no peer-reviewed test of ICT/SMC concepts in Crossref searches as of 27 Sep 2026, and no empirical support for OTE, premium/discount, BOS or order blocks "as defined" — [ThorTradeCopier](https://thortradecopier.com/blog/does-ict-smart-money-concepts-work)
- Base-rate critique: in a simulated random walk (1,000,000 bars, seed 42), 29.8% of bars formed a gap. Of 148,789 bullish FVGs, 83.4% were revisited within 20 bars, vs **83.5% of random levels at the same distance**. Full fill was 73.1% for both. Re-run with seed 7: 83.6% vs 83.5%, with the sign of the difference flipping — [ThorTradeCopier](https://thortradecopier.com/blog/does-ict-smart-money-concepts-work) (the blog's own simulation, not peer-reviewed, but easy to reproduce)
- SMC is often described as derived from Wyckoff's century-old accumulation/distribution ideas, with ICT credited with popularising the modern terms — [BabyPips forum](https://forums.babypips.com/t/ict-smc-difference-and-similarities/1223760); [Cal Poly news page](https://grandavehousing.calpoly.edu/news/smart-money-concepts-a-modern) (low-quality sources; opinion only)
- Even sympathetic codifiers concede discretion: Model 2022 is "Discretionary at every joint: pool selection, displacement energy, and gap identification vary by trader", with "No published edge proof" — [LuxAlgo Model 2022](https://www.luxalgo.com/library/concept/model-2022.md). For mitigation blocks: "they fail routinely, especially against strong momentum" — [LuxAlgo Mitigation Block](https://www.luxalgo.com/library/concept/mitigation-block.md)
- Kill-zone definitions themselves vary by 30–90 minutes across ICT sources — [LuxAlgo Killzones](https://www.luxalgo.com/library/concept/killzones.md). This leaves room for post-hoc fitting.
- A practitioner backtest site notes a gap between promoted Silver Bullet win rates (70–80%) and what independent replay testers report (50–65%), and between "the strategy as taught and its creator's documented live results" — [Backtrex](https://backtrex.com/en/blog/ict-silver-bullet-strategy-trading-guide)
- StatOasis concedes the counter-critique: mechanical daily tests "prove nothing" about discretionary intraday execution — [StatOasis](https://statoasis.com/overfit/research/ict-backtest-what-survives). This is the standard ICT-community defence, and it is itself an unfalsifiability problem.

### Inferences
- For an algorithmic agent the discretion critique is moot: whatever is coded is falsifiable. The real risk is the garden of forking paths. Swing N, FVG size filter, displacement threshold, kill-zone boundaries, CE vs edge entry and target choice multiply quickly into hundreds of variants (StatOasis ran 648; Tangirala corrected over 352 hypotheses). Pre-register a small grid and apply multiple-testing correction (e.g. a deflated Sharpe ratio or White's Reality Check).
- Any "fill-rate" or "level touched" statistic must be compared against random levels at the same distance, per the random-walk FVG result.
- Overall evidence grades:

| Concept | Grade |
|---|---|
| FVG | B, negative |
| Order block | B, null |
| Liquidity sweep (reversal) | B, negative; the continuation reading is weakly supported |
| OTE / premium-discount | B, negative |
| Market structure BOS/CHoCH | B, null via the Tangirala detectors (if included) and Mahadzva's faded-CHoCH result |
| IFVG, breaker, mitigation block, Judas swing, AMD/Power of 3, kill zones, Silver Bullet, Model 2022 | C |
| Stop clustering / cascades (the mechanism, not a strategy) | Strong academic support (FRBNY, proprietary order data), but not grade A as a net-of-cost profitability claim |

### Gaps
- No audited live track record of Michael J. Huddleston was found, nor verified claims about trading competitions. Not searched in depth; treat any such claim as unverified.
- No formal academic critique (peer-reviewed commentary) of ICT/SMC was found. The criticism is mostly from blogs, vendors and forums, plus the two SSRN working papers.
