# Institutional-style intraday rules: VWAP trend, Opening Range Breakout (ORB), and market intraday momentum. Evidence, exact rules, replications, and transfer to crypto perps / US / NSE

Evidence grades used throughout:
- **A** = peer-reviewed or academic evidence that holds after costs.
- **B** = independent systematic backtests.
- **C** = practitioner claims only.

The Zarattini/Aziz/Barbon papers are SSRN working papers and Swiss Finance Institute research papers, not peer-reviewed journal articles. They report results net of commissions, but mostly with no or tiny slippage. One co-author (Aziz) runs a day-trading education business. They are graded **B−/C+** on their own. Where independent replications exist, the grade follows the replications.

Sources were checked as of Oct 2026. Primary PDFs were downloaded from concretumgroup.com, nd.edu and other hosts, and numbers were taken from the paper text rather than from secondary summaries, unless a source is marked otherwise.

---

## Q1. What are the exact rules (entry, stop, target, sizing, filters) and the reported results (CAGR, Sharpe, drawdown, trades, period, costs) for each strategy?

### Takeaway
All of the intraday "institutional" strategies with published evidence share one skeleton:
- take a directional signal from early-session price action relative to a reference (first 5-minute bar, VWAP, or a volatility "noise band" around the open);
- hold in the direction of the imbalance;
- exit on a tight structural stop or at the close, never overnight;
- size by risk or volatility target, usually with a 4x leverage cap.

Reported Sharpe ratios run from 1.1 to 2.8 net of commissions, but the per-trade edge is only a few basis points (index ETFs) or about 0.1R (single stocks). The academic "last half-hour" momentum (Gao et al. 2018 JFE; Baltussen et al. 2021 JFE) is the only branch with peer-reviewed status. Its Sharpe is about 1.0–1.7 gross and lower net of costs.

### Cited Findings

#### 1. 5-minute ORB on QQQ/TQQQ: Zarattini & Aziz, "Can Day Trading Really Be Profitable?" (SSRN 4416622, 2023; rev. Apr 2024). Grade B− (C+ standalone)
- **Data:** QQQ (and TQQQ) 5-minute bars, Jan 1 2016 to Feb 17 2023 — [Concretum PDF](https://concretumgroup.com/wp-content/uploads/2026/02/Can-Day-Trading-Really-Be-Profitable.pdf)
- **Direction and entry:** the first 5-minute candle (09:30–09:35 ET) sets the direction. If it closed up, go long at the open of the second 5-minute candle; if down, go short. No trade on a doji (open = close) — [Concretum PDF](https://concretumgroup.com/wp-content/uploads/2026/02/Can-Day-Trading-Really-Be-Profitable.pdf)
- **Stop:** the low of the first 5-minute candle for longs, the high for shorts. Risk $R = entry − stop.
- **Target and sizing:** profit target 10R, otherwise exit at end of day. Risk 1% of equity per trade, chosen because "the historical average daily move on QQQ is 1%". Maximum leverage 4x (FINRA). Starting capital $25,000 — [Concretum PDF](https://concretumgroup.com/wp-content/uploads/2026/02/Can-Day-Trading-Really-Be-Profitable.pdf)
- **Costs:** commission $0.0005/share and **no slippage**. The paper itself says this could "be considered unrealistic" for large accounts — [Concretum PDF](https://concretumgroup.com/wp-content/uploads/2026/02/Can-Day-Trading-Really-Be-Profitable.pdf)
- **QQQ results:** 1,795 trades (51% long / 49% short); annualized return 31%; Sharpe 1.12; annualized alpha 33% net of commissions; about 675% total over 7 years. The leverage cap binds on most trades, so many trades risk less than 1% — [Concretum PDF](https://concretumgroup.com/wp-content/uploads/2026/02/Can-Day-Trading-Really-Be-Profitable.pdf)
- **TQQQ results** (used to get around the 4x cap): total return 1,484%; 48%/yr; volatility 39%; Sharpe 1.19; max drawdown 28%. Buy-and-hold TQQQ returned 438%, Sharpe 0.69, max drawdown 82%. The paper also shows a TQQQ variant with the stop at 5% of the 14-day ATR and an end-of-day exit. It warns that this stop is about $0.08 on a $25 stock and "will likely be exceeded" with size — [Concretum PDF](https://concretumgroup.com/wp-content/uploads/2026/02/Can-Day-Trading-Really-Be-Profitable.pdf)
- **Per-trade stats** (as quoted by a replicator): hit rate about 24% and about +0.13R per trade — [MQL5 replication blog](https://www.mql5.com/en/blogs/post/776235)

#### 2. 5-minute ORB on "Stocks in Play": Zarattini, Barbon & Aziz, "A Profitable Day Trading Strategy for the U.S. Equity Market" (SSRN 4729284, first version Feb 16 2024; SFI RP 24-98). Grade B− (C+ standalone)
- **Universe:** more than 7,000 US stocks, Jan 1 2016 to Dec 31 2023 — [Concretum PDF](https://concretumgroup.com/wp-content/uploads/2026/02/A-Profitable-Day-Trading-Strategy-For-The-U.S.-Equity-Market.pdf)
- **Filters, all required:**
  1. opening price > $5;
  2. 14-day average volume ≥ 1,000,000 shares;
  3. 14-day ATR > $0.50;
  4. **Relative Volume ≥ 100%**;
  5. trade only the **top 20 by Relative Volume** that day.
  — [Concretum PDF](https://concretumgroup.com/wp-content/uploads/2026/02/A-Profitable-Day-Trading-Strategy-For-The-U.S.-Equity-Market.pdf)
- **Relative Volume definition:** volume in today's first 5 minutes ÷ the average first-5-minute volume over the previous 14 days — [Concretum PDF](https://concretumgroup.com/wp-content/uploads/2026/02/A-Profitable-Day-Trading-Strategy-For-The-U.S.-Equity-Market.pdf)
- **Entry** (differs from the QQQ paper): a **stop order** at the 5-minute high if the first candle (09:30–09:35) was bullish, or at the 5-minute low if bearish. No order on a doji — [Concretum PDF](https://concretumgroup.com/wp-content/uploads/2026/02/A-Profitable-Day-Trading-Strategy-For-The-U.S.-Equity-Market.pdf)
- **Stop:** 10% of the 14-day ATR from the fill price. Worked example: BLDR with ATR $5 gives a $0.50 stop. **No profit target**; exit at the close if not stopped. That example made 13.62R — [Concretum PDF](https://concretumgroup.com/wp-content/uploads/2026/02/A-Profitable-Day-Trading-Strategy-For-The-U.S.-Equity-Market.pdf)
- **Sizing:** 1% of capital lost if stopped; 4x maximum leverage; $25,000 start — [Concretum PDF](https://concretumgroup.com/wp-content/uploads/2026/02/A-Profitable-Day-Trading-Strategy-For-The-U.S.-Equity-Market.pdf)
- **Costs:** commission **$0.0035/share** (IB Pro entry tier). The text found no slippage assumption, so results are "net" of commissions only — [Concretum PDF](https://concretumgroup.com/wp-content/uploads/2026/02/A-Profitable-Day-Trading-Strategy-For-The-U.S.-Equity-Market.pdf)
- **Results, base ORB on all eligible stocks (no Relative Volume filter):** 29% total, 3.2% IRR, volatility 6.6%, Sharpe 0.48, max drawdown 13%. Average PnL is −0.02R when Relative Volume < 100% and +0.08R when it is > 100%. Profitability rises monotonically with Relative Volume — [Concretum PDF](https://concretumgroup.com/wp-content/uploads/2026/02/A-Profitable-Day-Trading-Strategy-For-The-U.S.-Equity-Market.pdf)
- **Results, ORB + Relative Volume (top 20):** total 1,637% ($25k to about $435k); IRR 41.6%; volatility 14.8%; **Sharpe 2.81**; daily hit ratio 48.4%; max drawdown 12%; worst day −1.61%; alpha 35.8%; beta 0.00. The S&P 500 returned 198%, Sharpe 0.78, max drawdown 34% — [Concretum PDF](https://concretumgroup.com/wp-content/uploads/2026/02/A-Profitable-Day-Trading-Strategy-For-The-U.S.-Equity-Market.pdf)
- **Other opening-range windows** (each with its own Relative Volume filter):

  | Opening range | Total return | IRR | Sharpe | Max drawdown |
  |---|---|---|---|---|
  | 5-minute | 1,637% | 41.6% | 2.81 | 12% |
  | 15-minute | 272% | 17.4% | 1.43 | 11% |
  | 30-minute | 21% | 2.3% | 0.21 | 35% |
  | 60-minute | 39% | 4.1% | 0.40 | 21% |
  | Equal-weight combo | 234% | 15.8% | 1.99 | 7% |

  The edge is concentrated in the first 5 minutes — [Concretum PDF](https://concretumgroup.com/wp-content/uploads/2026/02/A-Profitable-Day-Trading-Strategy-For-The-U.S.-Equity-Market.pdf)

#### 3. VWAP trend trading on QQQ/TQQQ: Zarattini & Aziz, "Volume Weighted Average Price (VWAP): The Holy Grail for Day Trading Systems" (SSRN 4631351, Nov 2023). Grade C+/B−
- **Data:** 1-minute bars, regular trading hours only; QQQ and TQQQ; Jan 2 2018 to Sep 28 2023 — [Concretum PDF](https://concretumgroup.com/wp-content/uploads/2026/02/Volume-Weighted-Average-Price.pdf)
- **VWAP:** computed from the HLC average of each minute, session hours only — [Concretum PDF](https://concretumgroup.com/wp-content/uploads/2026/02/Volume-Weighted-Average-Price.pdf)
- **Rule:** wait for the first 1-minute candle to close after 09:30. If it closes above VWAP, go long at the next candle; if below, go short. Exit and reverse when a 1-minute candle closes on the other side of VWAP. Hold until 16:00 otherwise. The strategy is always in a position, so there are many trades per day in chop — [Concretum PDF](https://concretumgroup.com/wp-content/uploads/2026/02/Volume-Weighted-Average-Price.pdf)
- **Sizing:** 100% of equity with no leverage. A fixed-risk size is impossible because the stop (VWAP) moves.
- **Costs:** commission $0.0005/share and **no slippage assumed** ("we assumed no slippage in our order fills"). The authors say the system is not suitable for multi-million-dollar funds — [Concretum PDF](https://concretumgroup.com/wp-content/uploads/2026/02/Volume-Weighted-Average-Price.pdf)
- **Results:**

  | Strategy | Total return | Per year | Volatility | Sharpe | Max drawdown |
  |---|---|---|---|---|---|
  | QQQ VWAP | 671% | 43% | 18% | 2.1 | 9.4% |
  | TQQQ VWAP | 8,242% | 116% | 54% | 1.7 | 36.1% |
  | QQQ buy-and-hold | 126% | — | — | 0.7 | 35.6% |

  QQQ: 21,967 trades, hit ratio 17%, gain:loss 5.7. Simple moving-average comparators did worse; for example, SMA9 made 202%, Sharpe 1.3, from 107,067 trades — [Concretum PDF](https://concretumgroup.com/wp-content/uploads/2026/02/Volume-Weighted-Average-Price.pdf)
- About 56% of QQQ 1-minute candles closed above VWAP over the sample. The paper's motivating statistic is that the sum of 1-minute changes after an "above VWAP" candle is much larger than after a "below VWAP" candle — [Concretum PDF](https://concretumgroup.com/wp-content/uploads/2026/02/Volume-Weighted-Average-Price.pdf)

#### 4. SPY "Noise Area" intraday momentum: Zarattini, Aziz & Barbon, "Beat the Market: An Effective Intraday Momentum Strategy for S&P500 ETF (SPY)" (SSRN 4824172, May 2024; SFI RP 24-97; 4th place, Quantpedia Awards 2025). Grade B (independently replicated to 2024; see Q2)
- **Data:** IQFeed 1-minute SPY, May 2007 to April 2024 — [UniSG PDF](https://alexandria.unisg.ch/server/api/core/bitstreams/a99aba00-f967-49b3-aceb-f544dc386e0b/content)
- **Noise area:** for each day t−i (i = 1…14) and time-of-day HH:MM, compute move = |Close_HH:MM / Open_9:30 − 1|. σ_HH:MM is the 14-day average of that move.
  - UpperBound = max(Open_today, Close_yesterday) × (1 + VM·σ)
  - LowerBound = min(Open_today, Close_yesterday) × (1 − VM·σ)
  - VM (volatility multiplier) = 1 in the base case. Total return falls as VM rises.
  — [UniSG PDF](https://alexandria.unisg.ch/server/api/core/bitstreams/a99aba00-f967-49b3-aceb-f544dc386e0b/content)
- **Entry:** checked only at HH:00 and HH:30, so the first possible trade is 10:00. Long if price > UpperBound, short if price < LowerBound — [UniSG PDF](https://alexandria.unisg.ch/server/api/core/bitstreams/a99aba00-f967-49b3-aceb-f544dc386e0b/content)
- **Exit:** the final version uses a trailing stop at max(UpperBound, VWAP) for longs and min(LowerBound, VWAP) for shorts, also checked only at :00 and :30. Everything is flat at 16:00. VWAP is computed from market-hours data only — [UniSG PDF](https://alexandria.unisg.ch/server/api/core/bitstreams/a99aba00-f967-49b3-aceb-f544dc386e0b/content)
- **Sizing:** shares = AUM_{t−1} × min(4, σ_target / σ_SPY,t) / Open, with σ_target = 2% daily and σ_SPY the 14-day realized daily volatility. Leverage is capped at 4x — [UniSG PDF](https://alexandria.unisg.ch/server/api/core/bitstreams/a99aba00-f967-49b3-aceb-f544dc386e0b/content)
- **Costs:** commission $0.0035/share plus slippage $0.001/share. The slippage figure comes from the authors' live-order measurements: median below that, average about $0.001 — [UniSG PDF](https://alexandria.unisg.ch/server/api/core/bitstreams/a99aba00-f967-49b3-aceb-f544dc386e0b/content)
- **Results by version:**

  | Version | Total return | Per year | Volatility | Sharpe | Hit ratio | Max drawdown |
  |---|---|---|---|---|---|---|
  | Opposite-band exit, 100% notional | 178% | 6.2% | 10.9% | 0.61 | — | — |
  | Current band + VWAP stop, 100% notional | 380% | 9.7% | 7.7% | 1.24 | 43% | 12% |
  | Final, volatility-targeted | 1,985% | 19.6% | 14.3% | 1.33 | 43% (daily) | 25% |

  The final version had alpha 19.6% and beta −0.07, and made **7,668 trades**. Sharpe rises with VIX at the open: about 1.5 at VIX > 6 and about 3.5 at VIX > 40 — [UniSG PDF](https://alexandria.unisg.ch/server/api/core/bitstreams/a99aba00-f967-49b3-aceb-f544dc386e0b/content)
- **Day of week:** across 2,620 days the unconditional average PnL is 12 bps/day (t = 5.34). Wednesday is the best day at 18 bps (t = 3.42); Monday is not significant. The authors link the Wednesday effect to FOMC days — [UniSG PDF](https://alexandria.unisg.ch/server/api/core/bitstreams/a99aba00-f967-49b3-aceb-f544dc386e0b/content)
- **Follow-up from the authors (Pagani & Zarattini, Feb 2026, "QuanTip: Fast Alphas", SSRN 6391638):** SPY 5-minute bars, Jan 2007 to Jan 2026.
  - Simpler band: session open ± ½·ATR(14).
  - Entry checks at :00/:15/:30/:45. Stop when price returns to the session open. Flat at end of day. 2% daily volatility target.
  - Result: CAGR above 13% and Sharpe about 0.87 net of IBKR tiered fees, no slippage.
  - Timing overlay: delay entry until a 5-minute counter-move (a 5-minute down bar before a long entry), and delay stop exits the same way. This raises Sharpe from 0.87 to 0.99, and CAGR by about 200 bps.
  - A standalone 5-minute mean-reversion "fast alpha" earned about 31.9% CAGR at zero cost and became unprofitable under standard IBKR commissions.
  — [Concretum PDF](https://concretumgroup.com/wp-content/uploads/2026/02/Improving-Performance-with-Fast-Alphas-A-Tactical-Overlay-for-Intraday-Trend-Trading.pdf)

#### 5. Market intraday momentum: Gao, Han, Li & Zhou (2018), "Market Intraday Momentum", Journal of Financial Economics. Grade A− (peer-reviewed; survives spread costs in-sample)
- **Data:** SPY from TAQ, Feb 1 1993 to Dec 31 2013 — [paper PDF (working-paper version)](https://c.mql5.com/forextsd/forum/173/intraday_momentum_-_the_first_half-hour_return_predicts_the_last_half-hour_return.pdf)
- **Predictor and target:** the "first half-hour" return runs from the **previous day's 16:00 close to 10:00 ET**, so it includes the overnight gap. The target is the last half-hour return, 15:30–16:00 — [paper PDF](https://c.mql5.com/forextsd/forum/173/intraday_momentum_-_the_first_half-hour_return_predicts_the_last_half-hour_return.pdf)
- **Predictability:**
  - In-sample R² is 1.6%; 2.6% when combined with the 12th half-hour (15:00–15:30); 3.3% on high-volatility days.
  - Out-of-sample R² is 1.2% (1.8% combined).
  - Predictability is stronger on volatile days, high-volume days, recession days, and macro-news days, and stronger when the first half-hour is positive.
  — [paper PDF](https://c.mql5.com/forextsd/forum/173/intraday_momentum_-_the_first_half-hour_return_predicts_the_last_half-hour_return.pdf); [WUSTL profile](https://profiles.wustl.edu/en/publications/market-intraday-momentum/)
- **Timing rule:** at 15:30, go long SPY if the first half-hour return > 0, short if < 0, and exit at the 16:00 close. This made **6.67%/yr with standard deviation 6.19%, Sharpe 1.08**. Certainty-equivalent gains were 6.35–6.44%/yr — [paper PDF](https://c.mql5.com/forextsd/forum/173/intraday_momentum_-_the_first_half-hour_return_predicts_the_last_half-hour_return.pdf)
- **After costs** (post-decimalization, Jul 2001 onward): entering at the 15:30 ask or bid and exiting at the closing auction, which has no spread, gives 4.46%/yr (2.47% lower), standard deviation 6.10%, M² 14.88%/yr. The two-signal version gives 4.30%/yr after costs, M² 19.87% — [paper PDF](https://c.mql5.com/forextsd/forum/173/intraday_momentum_-_the_first_half-hour_return_predicts_the_last_half-hour_return.pdf)
- The effect also held in 10 other actively traded ETFs and 2 international index futures — [Alpha Architect summary](https://alphaarchitect.com/2014/08/attention-prop-traders-the-first-half-hour-of-trading-predicts-the-last-half-hour/)

#### 6. Baltussen, Da, Lammers & Martens (2021), "Hedging Demand and Market Intraday Momentum", Journal of Financial Economics 142:377–403. Grade A− (peer-reviewed; gross of costs, with one net check on S&P futures)
- **Data:** more than 60 futures (equity, bond, commodity, FX), Dec 1974 to May 2020, tick data — [JFE PDF (nd.edu)](https://www3.nd.edu/~zda/intramom.pdf)
- **Predictor:** the best predictor is r_ROD, the return from the **previous close to 30 minutes before the close**, used to predict the last 30 minutes. The first-half-hour signal r_ONFH is weaker.
  - Pooled equity out-of-sample R²: 2.22% for r_ROD vs −1.71% for r_ONFH; 2.88% combined.
  - r_ROD has a positive, significant out-of-sample R² in 14 of 17 equity index futures.
  — [JFE PDF](https://www3.nd.edu/~zda/intramom.pdf)
- **Timing strategies** (long or short in the last 30 minutes by the sign of the signal), on a 1/N equity index futures portfolio:

  | Signal | Return/yr | Std dev | Sharpe | Success rate |
  |---|---|---|---|---|
  | r_ONFH (first half hour) | 4.21% | 3.95% | 1.07 | 55% |
  | Both signals agree | 5.47% | — | 1.60 | 61% |
  | r_ROD (rest of day) | 6.86% | 3.96% | 1.73 | 55% |

  Other asset classes, r_ROD Sharpe: bonds 1.62, commodities 1.42, currencies about 0.87. A last-half-hour "always long" control had Sharpe 0.11 — [JFE PDF](https://www3.nd.edu/~zda/intramom.pdf)
- **Mechanism:** short-gamma hedging by options dealers and leveraged-ETF rebalancing pushes the last 30 minutes in the direction of the day's move. The effect partially reverts over the next days — [JFE PDF](https://www3.nd.edu/~zda/intramom.pdf); [Erasmus repository](https://pure.eur.nl/en/publications/hedging-demand-and-market-intraday-momentum/)
- **Costs:** "we do not consider transaction costs". With one tick of cost in S&P 500 futures, the strategy still has a positive net Sharpe — [JFE PDF](https://www3.nd.edu/~zda/intramom.pdf)

#### 7. Other academic intraday momentum (equities)
- Li, Sakkas & Urquhart (2022, Journal of Financial Markets 57) studied intraday time-series momentum in **16 developed markets**. It is significant in- and out-of-sample in most of them, and stronger when liquidity is low, volatility is high, and news is discrete — [RePEc](https://ideas.repec.org/a/eee/finmar/v57y2022ics138641812100001x.html). Grade A− (country-level numbers not extracted).

#### 8. Crypto intraday studies
- **Shen, Urquhart & Wang (2022), "Bitcoin Intraday Time Series Momentum", Financial Review 57(2).** Grade A− (peer-reviewed; cost treatment not verified).
  - Because Bitcoin trades 24/7 with no clear open or close, the study uses **trading volume as a proxy for market trading time**.
  - The first half-hour return positively predicts the last half-hour return. Predictability is greatest when the "first session" is the one with the highest volume or volatility.
  - Momentum timing yields "substantial economic gains", especially in Bitcoin downturns. The driver is liquidity provision, not late-informed trading.
  — [Birmingham research portal](https://research.birmingham.ac.uk/en/publications/bitcoin-intraday-time-series-momentum/)
- **Wen, Bouri, Xu & Zhao (2022), North American Journal of Economics and Finance 62.**
  - Bitcoin high-frequency data, Mar 3 2013 to May 31 2020.
  - Both intraday **momentum and reversal** occur. Which one dominates depends on price jumps, FOMC announcements, liquidity, and COVID. The patterns extend to ETH, LTC and XRP.
  - Timing strategies beat always-long and buy-and-hold.
  — [RePEc](https://ideas.repec.org:443/a/eee/ecofin/v62y2022ics1062940822000833.html). Grade B+/A− (numbers not extracted).
- **Quantpedia, Bitcoin time-of-day study.** Grade C+/B− (one non-peer-reviewed study, no costs).
  - Gemini hourly data, Oct 9 2015 to Feb 3 2022.
  - The strongest returns come in **21:00–23:00 UTC**, after the US cash close and before Asia, when all major stock exchanges are closed. The worst hours are 03:00–04:00 UTC.
  - Rule: buy at 21:00 UTC, sell at 23:00 UTC. Result: 33% annualized, volatility 20.93%, max drawdown −22.45%. No transaction costs are stated.
  - Friday is the best day for the window, then Thursday. The window works better in uptrends.
  — [Quantpedia](https://quantpedia.com/are-there-seasonal-intraday-or-overnight-anomalies-in-bitcoin/)
  - A later Quantpedia update is reported at **40.64% annualized, Calmar 1.79, max drawdown −22.7%** (search-snippet figures; the page was not fetched) — [Quantpedia "The Seasonality of Bitcoin"](https://quantpedia.com/the-seasonality-of-bitcoin/)
- **Session returns** (practitioner research, grade C):
  - NYDIG, Jan 2019 to Jan 2021: the US session had positive average returns and the Asia session negative ones. The pattern flips across regimes; returns around the 2017 peak, and the drawdown after it, were Asia-dominated — [NYDIG](https://nydig.com/research/a-look-at-geographic-drivers-of-returns)
  - K33 reported that "Bitcoin mostly sleeps during Asian market hours" and that 2022's downside was not in the European session (titles seen in search; content not verified) — [K33](https://k33.com/research/archive/articles/bitcoin-mostly-sleeps-during-asian-market-hours)
  - An academic study with more than 15 million observations found time-of-day, day-of-week and month effects that are **time-varying, with no consistent pattern** — [RePEc, Finance Research Letters 34 (2020)](https://ideas.repec.org/a/eee/finlet/v34y2020ics1544612319301904.html)

#### 9. VWAP as an institutional benchmark, and why price "reacts" to VWAP. Grade A for the benchmark facts, C for VWAP as support/resistance
- Berkowitz, Logue & Noser (1988, Journal of Finance) proposed daily VWAP as an unbiased estimate of the price a non-strategic trader faces that day, and it became the standard execution benchmark — [Madhavan 2002, "VWAP Strategies", Investment Guides / Transaction Performance](https://www.smallake.kr/wp-content/uploads/2014/07/TP_Spring_2002_Madhavan.pdf)
- **How it is achieved:** passive participation algorithms trade in proportion to the expected intraday volume profile; guaranteed-VWAP bids and crossing are alternatives. VWAP benchmarks can be gamed, and they understate cost when the stock is trending — [Madhavan 2002](https://www.smallake.kr/wp-content/uploads/2014/07/TP_Spring_2002_Madhavan.pdf)
- VWAP orders are said to be roughly half of institutional trading (secondary claim) — [Otago working paper, "Improving VWAP strategies"](https://ourarchive.otago.ac.nz/bitstream/handle/10523/1540/ImprovingVWAPStrategiesADynamicalVolumeApproach2006.pdf?sequence=3)
- **Anchored VWAP (Brian Shannon), practitioner rules:**
  - Anchor at significant events: earnings, gaps, swing highs and lows, IPO, start of the quarter or year.
  - Price above the anchored VWAP means buyers since the anchor are in profit (support); below means sellers are in control.
  - Treat reclaims and rejections of the anchored VWAP as entry triggers.
  - No systematic peer-reviewed or independent backtest was found. One claim of "67% win rate, 1:2 R:R" is unsourced marketing.
  — [takeprofitapp explainer](https://takeprofitapp.com/en/learn/anchored-vwap-trading); [MQL5 blog on AVWAP](https://www.mql5.com/en/blogs/post/774013). Grade C.
- **VWAP standard-deviation bands:** no published systematic evidence was found. The closest evidence-based analogue is Concretum's "noise area" bands, which are volatility bands around the open, not around VWAP. Concretum publishes a TradingView "Concretum Bands" indicator (14-day lookback, multiplier 1) — [TradingView Concretum Bands](https://in.tradingview.com/script/CUpWCZhe-Concretum-Bands)

### Inferences
- **Shared mechanism:** institutional flow (benchmark-tracking execution spread across the day, dealer gamma hedging, leveraged-ETF rebalancing near the close) creates **intraday trend persistence after early-session imbalance**. VWAP serves as the cost basis of the day's participants, and as a trailing stop it was the single best exit improvement in the SPY paper: it doubled Sharpe from 0.61 to 1.24.
- **For a rules engine, the most defensible parameter set is the frozen paper set:**
  - 14-day lookbacks;
  - 5-minute opening range;
  - Relative Volume ≥ 1 with top-N selection;
  - 10%-ATR stop (stocks) or opposite end of the opening range (ETF);
  - VWAP or band trailing stop;
  - checks every 30 minutes;
  - flat at end of day;
  - 1% risk per trade or 2% daily volatility target;
  - 4x leverage cap.
- **Where the edge sits:** the 5-minute window does most of the work. Stocks-in-Play Sharpe falls from 2.81 (5-minute) to 0.21–0.40 (30/60-minute). In the SPY noise area, the edge is concentrated in high-VIX regimes.

### Gaps
- The Stocks-in-Play paper's exact slippage treatment: the text says only commissions ($0.0035/share). No slippage figure was found.
- Shen, Urquhart & Wang: data period, exchange, the exact volume-defined session boundary, and strategy Sharpe/return numbers could not be extracted (PDF host unreachable).
- Li, Sakkas & Urquhart country-level and India-specific numbers were not extracted. The study covers developed markets only.
- No peer-reviewed study tests VWAP or anchored VWAP as support/resistance for price. The "price reacts to VWAP" claim rests on the Zarattini VWAP paper (C+/B−) and practitioner lore.

---

## Q2. Independent replications: did the results hold after publication and after realistic costs? Is there evidence of decay?

### Takeaway
- Gross results replicate closely: trade counts, hit rates and R-per-trade match the papers.
- Net results are fragile. The QQQ/index ORB edge (~0.13R gross) is about the size of realistic costs and nets to zero on five index CFDs from 2015 to 2026.
- The SPY noise-area strategy replicated well through 2024 (Sharpe 1.4–2.0 per year) but had **Sharpe ≈ 0 or negative in 2025–2026** on both SPY and ES.
- The Stocks-in-Play ORB has no published independent net replication; community tests report weak or negative recent expectancy.
- The academic last-half-hour momentum survives costs in-sample, and its causal mechanism (gamma hedging) has peer-reviewed support. A small NSE test found the edge (~1.5 bps) far below costs.

### Cited Findings
- **QQQ/index ORB, MQL5 replication** (published Sep 2026). Grade B.
  - Paper rules applied to NQ, SPX, Dow, DAX and FTSE CFDs, Jan 2015 to Jun 2026, about 2,900 sessions each.
  - Gross: **+0.131R on NQ** vs the paper's +0.13R; hit rate 23.2% vs 24%.
  - Net of spread and slippage (NQ 2.5 pts, SPX 0.8, Dow 4.0, DAX 2.5, FTSE 1.5): NQ **+0.002R**; "no market is distinguishable from zero and four of five are negative".
  - The first candle contains roughly 0.1R of edge, about equal to typical costs.
  — [MQL5 blog](https://www.mql5.com/en/blogs/post/776235)
- **QQQ ORB, giovannibrusco GitHub replication** (Jan 2016 to Feb 2023). Grade B.
  - No slippage: 1,775 trades vs 1,795; Sharpe 1.06 vs 1.12; CAGR 30.4%; max drawdown 22.4%.
  - With $0.02/share entry slippage and $0.04/share stop slippage: Sharpe 0.23; CAGR 2.7%; max drawdown 43.9%. Break-even slippage is about 2.2¢/share.
  - 2022 produced 38% of the PnL.
  — [zarattini-2023-orb-qqq](https://github.com/giovannibrusco/zarattini-2023-orb-qqq) (as summarized in prior project notes: research_notes/Price action reading strategy rules/related_rule_based_papers.md)
- **SPY noise area, giovannibrusco replication.** Grade B.
  - Alpaca IEX 1-minute SPY, Jul 2020 to Jul 2026; ES futures May 2024 to Jul 2026.
  - Sharpe 1.11 (paper 1.33); alpha +16.7%/yr (t = 2.85); beta ≈ 0; +2.6 bps/trade; win rate 41%; payoff 1.69.
  - 2020–2024: Sharpe 1.4–2.0 every year, and +25.8% in 2022 while SPY fell 19.5%.
  - **2025–2026: "Recent Sharpe ≈ 0 on both instruments" (edge compressed).** SPY and ES correlate at 0.97, which rules out a data artefact. ES round-trip cost is about 0.4 bps.
  - A 27-variant grid picked the original paper configuration. Quarterly walk-forward reselection gave Sharpe 0.57 vs 0.92 for the fixed configuration.
  — [zarattini-2024-momentum-spy](https://github.com/giovannibrusco/zarattini-2024-momentum-spy)
  - Prior project notes record 2025 at −4.9% (Sharpe −0.27) and 2026 YTD at −12.9% (Sharpe −1.91), and that $0.01/share slippage cuts Sharpe from 1.11 to 0.94 — [same repo, via prior notes](https://github.com/giovannibrusco/zarattini-2024-momentum-spy)
  - Quantitativo's ES/NQ replication: about +2 bps/trade, win rate about 36% — [Quantitativo](https://www.quantitativo.com/p/intraday-momentum-for-es-and-nq) (via prior notes; not re-fetched)
- **Authors' own update** (Pagani & Zarattini, Feb 2026). The ATR-band variant on SPY from 2007 to Jan 2026 still shows CAGR > 13% and Sharpe about 0.87 net of commissions, no slippage. That is a much lower Sharpe than the 1.33 in the original paper. The full-sample number hides the yearly breakdown, so 2025 performance is not separately visible — [Concretum PDF](https://concretumgroup.com/wp-content/uploads/2026/02/Improving-Performance-with-Fast-Alphas-A-Tactical-Overlay-for-Intraday-Trend-Trading.pdf)
- **Stocks-in-Play ORB, QuantConnect implementation.**
  - 1,000 most liquid US equities, top 20 "in play"; 2016 backtest Sharpe 2.396, beta −0.042; 17 of 25 parameter combinations beat the benchmark.
  - Commenters report that "the backtest doesn't look good for other years", that the strategy collapses in other regimes, that stop orders placed after entry differ from live fills, and that costs are large: about 25% of gross even with 6 symbols.
  — [QuantConnect forum 18444](https://quantconnect.com/forum/discussion/18444). Grade B−/C (forum; numbers only for 2016).
  - Another full-market replication (POB-CL-DT PR #12, with $0.01/share stop slippage) reported negative expectancy over its last 5–9 months — [GitHub PR](https://github.com/patrickobirmingham-eng/POB-CL-DT/pull/12) (via prior notes)
- **Maróy (2025, SSRN 5095349)** optimized the noise-area exits (VWAP, ladder) to claim Sharpe > 3. Treat this as in-sample data mining — [SSRN](https://papers.ssrn.com/sol3/papers.cfm?abstract_id=5095349) (via prior notes)
- **Gao et al. extensions:**
  - Baltussen et al. found it across 60+ futures from 1974 to 2020 and in both subsamples, 1974–1999 and 2000–2020. Grade A — [JFE PDF](https://www3.nd.edu/~zda/intramom.pdf)
  - Li, Sakkas & Urquhart found it in 16 developed markets, out-of-sample. Grade A — [RePEc](https://ideas.repec.org/a/eee/finmar/v57y2022ics138641812100001x.html)
  - Baltussen et al. note that in Feb–May 2020 the effect was very strong (COVID gamma) — [JFE PDF](https://www3.nd.edu/~zda/intramom.pdf)
- **General decay benchmark:** McLean & Pontiff found anomaly returns 26% lower out-of-sample and 58% lower in the five years after publication — [Quantpedia summary](https://quantpedia.com/how-do-investment-strategies-perform-after-publication/)
- **Conflict of interest:** Andrew Aziz runs Bear Bull Traders, a day-trading education business, and promotes the ORB papers in its marketing — [Bear Bull Traders](https://bearbulltraders.com/category/andrews-newsletter/page/3/); [BBT VWAP post](https://bearbulltraders.com/?p=2606339)
- **VWAP Holy Grail (QQQ/TQQQ):** no independent net-of-slippage replication was found. QuantConnect embedded-backtest pages exist, but their parameters and authorship were not verifiable — [QuantConnect backtest](https://www.quantconnect.com/terminal/cache/embedded_backtest_0cab7e083e99d5a372e02fe882bbf078.html)

### Inferences
- **Strategy grades after replication:**

  | Strategy | Grade | Basis |
  |---|---|---|
  | Last-half-hour momentum (Gao; Baltussen) | **A−** | Peer-reviewed, multi-market, survives one-tick costs in futures |
  | SPY noise area + VWAP stop | **B** | Replicates through 2024; decayed or paused 2025–2026 |
  | QQQ/index ORB | **B, net ≈ 0** | Gross edge replicates; zero after costs |
  | Stocks-in-Play ORB | **B−/C+** | No published net replication; community reports weak recent results |
  | VWAP Holy Grail | **C+** | No slippage modeled, about 22k trades; likely fragile to 1–2¢ slippage |
  | Anchored VWAP / VWAP bands | **C** | Practitioner claims only |

- **Why the VWAP strategy is likely fragile:** at a 17% hit rate with about 22k trades on QQQ over 5.7 years (about 15 trades per day), each $0.01/share of slippage on a roughly $300 QQQ is about 0.33 bps per side. That costs several percentage points per year at 100% notional. This is a rough calculation, not tested.
- **Common failure mode:** per-trade edges of 2–3 bps (index) or 0.08–0.13R (ORB) mean that **execution cost determines whether the strategy is viable**. Profits cluster in high-volatility years (2008, 2020, 2022). A paper-trading agent should freeze the paper parameters, model pessimistic fills (stop-through slippage, same-bar ambiguity), and gate live use on forward performance. Walk-forward re-optimization hurt in the SPY replication.
- **2025–2026 compression:** this is consistent with crowding after publication and the Quantpedia award. It may also relate to the rise of 0DTE options changing dealer gamma positioning; the Beat the Market paper itself mentions 0DTE altering expiry effects. This is speculation; no source tests it.

### Gaps
- No independent Stocks-in-Play ORB replication with full net numbers through 2025–2026 is publicly visible.
- No post-2013 SPY-only replication of the exact Gao et al. 15:30 timing rule with yearly results was found. Decay of the simple first-half-hour signal specifically after 2013 is unverified, although Baltussen's r_ROD variant holds to 2020.
- Quantpedia's own strategy-database page for "Market Intraday Momentum" (post-publication performance) was not accessible.

---

## Q3. Which rules transfer to crypto (24/7, no opening auction; which "session open" do studies use) and to India (NSE 09:15 open, STT costs)?

### Takeaway
- **Crypto:**
  - No study uses a natural "open". Shen et al. define sessions by trading volume. Quantpedia uses UTC clock hours; the best window, 21:00–23:00 UTC, sits around the US close.
  - Practitioner crypto ORB indicators use the **US equity open (13:30/14:30 UTC)** as the session anchor.
  - Taker fees (0.05% per side, so 10 bps round trip) are an order of magnitude larger than the 2–3 bps edges of US index intraday momentum. Only maker-entry or low-turnover variants are plausible.
- **India:**
  - The rules port mechanically: 09:15 open, flat by about 15:20–15:25.
  - Costs are prohibitive. Futures STT rose to 0.05% (sell side) from Apr 1 2026. Cash-intraday STT is 0.025% on the sell side, plus about 5 bps slippage per side.
  - The one public NSE test found ORB and first-to-last half-hour momentum both net negative.

### Cited Findings
- **Crypto session definitions used in studies:**
  - Shen, Urquhart & Wang use trading volume as a proxy for trading time because Bitcoin "has not got a clear opening and closing period". The first high-volume, high-volatility session predicts best — [Birmingham](https://research.birmingham.ac.uk/en/publications/bitcoin-intraday-time-series-momentum/)
  - Quantpedia uses UTC clock hours. The largest returns come at 22:00–23:00 UTC, when all major exchanges are closed — [Quantpedia](https://quantpedia.com/are-there-seasonal-intraday-or-overnight-anomalies-in-bitcoin/)
  - Session splits in practitioner research: North America 9am–5pm ET, Asia 5pm–1am ET, Europe 1am–9am ET. BTC volume and volatility peak in the hours when European and US equity markets overlap — [NYDIG](https://nydig.com/research/a-look-at-geographic-drivers-of-returns); [Amberdata](https://blog.amberdata.io/trading-between-hours-volatility-dispersion-across-multiple-regions)
  - TradingView "BTC US opening range breakout" scripts use the first hour of the US cash session as the opening range — [TradingView](https://in.tradingview.com/script/03DoIeHA-BTC-US-opening-range-breakout)
- **Crypto weekends:** in Quantpedia's split, intraday returns are stronger on weekends and holidays, while on NYSE business days the "overnight" (non-US-hours) component dominates — [Quantpedia](https://quantpedia.com/are-there-seasonal-intraday-or-overnight-anomalies-in-bitcoin/)
- **Crypto momentum vs reversal:** intraday momentum and reversal coexist and depend on jumps and FOMC days (BTC, ETH, LTC, XRP; 2013–2020) — [RePEc, Wen et al. 2022](https://ideas.repec.org:443/a/eee/ecofin/v62y2022ics1062940822000833.html). Calendar effects are time-varying and not persistent — [RePEc, FRL 2020](https://ideas.repec.org/a/eee/finlet/v34y2020ics1544612319301904.html)
- **Noise area on 24/7 crypto:** Concretum's ATR-band variant uses the "session open". For crypto, the anchor must be chosen arbitrarily (00:00 UTC, or the US open). No published noise-area or VWAP-trend backtest on BTC/ETH/SOL perps was found.
- **India, ORB conventions:** the opening range is commonly 09:15–09:30 IST (15-minute) on Bank Nifty/Nifty futures — [OneTradeJournal](https://onetradejournal.com/strategies/fifteen-minute-orb-strategy)
  - A 2010–2018 Bank Nifty ORB AmiBroker backtest reported "17,000+ points", with **no costs or slippage** — [Marketcalls](https://www.marketcalls.in/amibroker/backtestable-open-range-breakout-orb-study-for-amibroker.html). Grade C.
- **India, STT:**
  - Futures STT rose to **0.05% from 0.02%** and options premium STT to **0.15% from 0.1%**, effective **April 1 2026** (Union Budget 2026-27) — [5paisa](https://www.5paisa.com/news/stt-hike-on-fo-to-take-effect-from-april-1-amid-rising-options-activity); [Outlook Money](https://www.outlookmoney.com/invest/stt-hike-from-april-1-2026-budget-what-it-means-for-futures-and-options-traders); [ICICI Direct](https://www.icicidirect.com/ilearn/futures-and-options/articles/stt-changes-in-budget-2026-what-f-o-traders-should-know)
  - Before that, futures STT was 0.0125%, raised to 0.02% from Oct 1 2024 — [Business Standard](https://www.business-standard.com/budget/news/budget-2024-steep-stt-increase-to-tame-retail-frenzy-in-derivatives-market-124072301145_1.html) (via prior notes)
- **India, one public net test** (md0n-cmd/indian-equity-backtests, Nifty 100 with survivorship bias, 2021–2025). Grade B−.
  - Costs: STT 0.025% on the sell side; brokerage ₹20 or 0.03%; exchange charges; stamp duty; GST; slippage 0.05%/side. Round trip is about 20 bps.
  - The 15-minute ORB with a Relative Volume filter lost **−21.8% over 4 years**: ₹173 gross profit on 759 trades against ₹2,357 of costs.
  - First-30-minutes-predicts-last-30-minutes momentum captured about **1.5 bps per trade against about 20 bps of costs**.
  — [GitHub](https://github.com/md0n-cmd/indian-equity-backtests) (via prior notes)
- **Nifty 50 time-of-day effects:** an Indian study found significant time-of-day effects, including an "open jump" effect and a persistent end-of-session effect (abstract only) — [i-scholar, Ajm](https://i-scholar.in/index.php/Ajm/article/view/179374)

### Inferences
- **Crypto transfer (BTC/ETH/SOL perps on Delta Exchange India, 0.02% maker / 0.05% taker):**
  - A taker-taker round trip is 10 bps; a maker entry plus taker stop is 7 bps. Indian GST on fees, if charged, would add more (not verified).
  - Gao/Baltussen-style last-half-hour timing earns about 2–3 bps per day in US index futures (6.67%/yr over 252 trades). It is **not viable on crypto with taker fees** unless crypto's per-trade edge is several times larger. Shen et al.'s magnitudes were not extracted, so this is unverified.
  - The **ORB/noise-area structure (few trades, wide R)** is the better fit. Use 10%-ATR or opening-range stops so that R is many times the 7–10 bps cost. Keep at most one or two trades per day.
  - **Recommended anchors to test:**
    - the 13:30 UTC US cash open (14:30 in northern winter), where volume and volatility spike and where institutional flow (ETF creations, CME basis desks) is concentrated;
    - 00:00 UTC (the Binance/Delta daily candle, where funding and index resets cluster);
    - a volume-defined "first session" following Shen et al.
  - Evaluate the 21:00–23:00 UTC drift and weekend behaviour as filters, not standalone strategies.
  - Use the session VWAP from the chosen anchor as the trailing stop, following the SPY paper.
  - Perp funding is paid every 8 hours (or as the venue defines it). It is negligible intraday but matters if a position spans a funding timestamp.
- **US stocks/ETFs:** run the papers as written (best fit). Add pessimistic slippage of at least $0.01–0.02/share, and treat 2025–2026 noise-area weakness as a live regime risk. The ETF last-half-hour signal is cheap to execute through MOC orders (no spread at the close).
- **India (NSE stocks, Nifty/Bank Nifty):**
  - **Timing:** OR = 09:15–09:20 (5-minute) or 09:15–09:30. Half-hour checks from 09:45. The Gao-style "first half hour" = previous close to 09:45. The "last half hour" = 15:00–15:30, exited through the closing session or by about 15:25.
  - **Futures costs:** STT is now 0.05% on the sell side, about 5 bps per round trip from STT alone, before brokerage, exchange charges and slippage. That is larger than the 2–3 bps edge of index intraday momentum, so an index-futures intraday-momentum sleeve is likely **uneconomic after Apr 2026** unless signal filters (high-volatility days, macro-news days) raise per-trade expectancy several-fold.
  - **Stocks:** the Stocks-in-Play ORB is the only family whose per-trade R (0.08R average, higher at extreme Relative Volume) might clear costs. That requires R to be wide relative to about 20 bps of cost, for example by keeping only Relative Volume ≥ 3–5x names or using wider stops than 10% ATR. This is an untested adaptation.
  - **Price filters** need rescaling to rupee and turnover terms.
  - **Circuit risk:** upper and lower circuit limits can make stops unfillable on NSE stocks, especially for shorts.

### Gaps
- No published BTC/ETH/SOL backtest of the ORB, noise area or VWAP trend that includes perp taker fees or funding was found. Crypto transfer is untested.
- Shen et al.'s per-trade magnitudes and exact session definition are needed to judge viability under 0.05% taker fees, and could not be retrieved.
- Whether Delta Exchange India charges 18% GST on trading fees was not verified in this pass.
- No academic study of first-half-hour to last-half-hour momentum on Nifty or Bank Nifty futures with costs was found. The only evidence is one small GitHub test on stocks.
- NSE lot sizes, MIS margin rules and the 2026 STT impact on cash-intraday (equity) STT were not verified. The cash-intraday rate of 0.025% on the sell side comes from the GitHub cost model, not a primary source.
