# Directional Intraday Trading Strategies (Trend/Momentum and Mean-Reversion): Mechanics and Net-of-Cost Evidence

Research note scope: ORB, intraday momentum, VWAP/MA/Donchian/Supertrend, mean reversion, pairs/stat-arb, order flow, ML, and retail day-trader outcomes. Equities (US, India) and crypto. Note on method: many primary sources (SSRN, Concretum, Substack, university repositories) were blocked by the network proxy during this research, so several numbers come from search-engine abstracts/summaries of the primary papers rather than full-text reads. Where that is the case it is flagged. Numbers not verified are put in Gaps, not in findings.

## 1. Opening Range Breakout (ORB): mechanics, net-of-cost evidence, India

### Takeaway
The best-known academic ORB evidence (Zarattini & Aziz 2023 on QQQ; Zarattini, Barbon & Aziz 2024 on "Stocks in Play") reports very strong backtests (Sharpe 1.0 to 2.8), but these are in-sample, written by authors with commercial day-trading ties, and an independent replication shows the QQQ version's edge is about 2 cents/share and goes away with realistic slippage. Indian ORB evidence is almost all retail blog backtests, and the ones that model full Indian costs (STT, brokerage, slippage) mostly find little or no net profit.

### Cited Findings
**Zarattini & Aziz (2023), "Can Day Trading Really Be Profitable?" (SSRN 4416622), 5-min ORB on QQQ**
- Rules: take the direction of the first 5-minute candle (bullish means long, bearish means short), enter at the open of the second candle, put the stop at the other end of the first candle, target 10R or exit at the session close. Reported hit rate is about 24% with +0.13R per trade; with 4x leverage the paper reports about 33%/yr on QQQ. — [MQL5 replication blog (search summary)](https://www.mql5.com/en/blogs/post/776235)
- Reported 2016–2023 portfolio return of 1,484% vs 169% for buy-and-hold QQQ. — [MQL5 blog (search summary)](https://www.mql5.com/en/blogs/post/776235); [SSRN abstract page](https://papers.ssrn.com/sol3/papers.cfm?abstract_id=4416622)
- Caveat: the paper charges commission but no spread and no slippage, and it has no out-of-sample period. — [MQL5 blog (search summary)](https://www.mql5.com/en/blogs/post/776235)
- The MQL5 blog is titled "The opening-range breakout paper, replicated on five indices: gross reproduced, net zero" (Sept 2026). Only the title and summary could be seen, because the full text was blocked. — [MQL5 blog](https://www.mql5.com/en/blogs/post/776235)
- Independent replication (Jan 2016 to Feb 2023, 1,775 trades vs 1,795 in the paper): with no execution costs it gets Sharpe 1.06 and 30.4% annualized, matching the paper. The gross edge is about $0.070/share and $138,639 total profit. Adding $0.02/share entry slippage cuts net profit to $4,860 (Sharpe 0.23). **Break-even is at about 2.2¢/share slippage**, which is inside the bid-ask spread. — [giovannibrusco/zarattini-2023-orb-qqq (GitHub)](https://github.com/giovannibrusco/zarattini-2023-orb-qqq)
- In the same replication, a Nasdaq futures (NQ) confirmation filter brings it back to $44,332 profit (Sharpe 0.77, per-trade t=2.05). However, **76% of the filtered P&L comes from 2022 alone**. The filter loses money in 2017, 2020 and early 2023, which suggests a volatility-regime effect rather than a structural inefficiency. — [GitHub replication](https://github.com/giovannibrusco/zarattini-2023-orb-qqq)

**Zarattini, Barbon & Aziz (2024), "A Profitable Day Trading Strategy for the U.S. Equity Market" (SSRN 4729284), 5-min ORB on "Stocks in Play"**
- Universe: more than 7,000 US stocks, 2016–2023. Each day it trades only the top 20 "Stocks in Play" ranked by relative volume in the first 5 minutes. A 5-minute OR beat 15/30/60-minute ORs. — [SSRN](https://papers.ssrn.com/sol3/papers.cfm?abstract_id=4729284); [Concretum page (search summary)](https://concretumgroup.com/a-profitable-day-trading-strategy-for-the-u-s-equity-market/)
- Rules as implemented in a public replication: price > $5, 14-day average volume of at least 1M shares, ATR > $0.50, top 20 by first-5-min relative volume. Direction comes from the first 5-min candle colour. Entry is a stop order at the OR high or low after 9:35. Stop is 10% of the 14-day ATR. Exit at the close. Risk is 1% per slot with a 4x notional cap. — [POB-CL-DT PR #12 (GitHub)](https://github.com/patrickobirmingham-eng/POB-CL-DT/pull/12)
- Reported results: total net return above 1,600%, **Sharpe 2.81, annualized alpha 36%, near-zero beta, hit rate 48.4%, max drawdown 12%, worst day −1.61%**. The S&P 500 over the same period returned 198% with a 34% max drawdown and a −10.9% worst day. — [search summary of Concretum/SSRN paper](https://concretumgroup.com/a-profitable-day-trading-strategy-for-the-u-s-equity-market/); [paper PDF copy](https://tradewithpat.com/wp-content/uploads/2025/09/ssrn-4729284.pdf)
- No independent net-of-slippage replication results were visible. The public replication PR above lists $0.01/share slippage on stop fills and a doubled-slippage robustness test, but its numerical results were not visible. — [POB-CL-DT PR #12](https://github.com/patrickobirmingham-eng/POB-CL-DT/pull/12)

**ORB on Nifty / BankNifty (India)**
- A popular BankNifty variant posted on Zerodha's TradingQnA ("2 PM ORB") marks the day's high and low at 2 PM, buys a break above the high or shorts a break below the low, and exits at 3:20 PM or on the stop. It claims more than 17,000 points over 9 years with a win rate of about 48% (gross). — [TradingQnA (Zerodha)](https://tradingqna.com/t/orb-opening-range-breakout-2pm-banknifty-intraday-strategy/39756)
- One blog backtest of BankNifty ORB reports 352% total return with a 48% drawdown. After adding 0.03% slippage, ₹20/lot brokerage and all government charges, the strategy "was not profitable at all". (Attribution comes from aggregated search results; most likely source shown.) — [saimohanreddy.com BankNifty ORB backtest](https://saimohanreddy.com/orb-backtest-on-banknifty/)
- A vendor/blog "8+ year" Nifty ORB backtest says brokerage, STT and slippage should be expected to cut real returns 15–25% below the raw numbers. — [IntradayLab (vendor)](https://intradaylab.com/blog/nifty-orb-breakout-strategy-backtest)
- Zerodha's Varsity/"In the Money" newsletter has a series on trading ORB through Nifty options (educational, not a performance study). — [In the Money by Zerodha](https://inthemoneybyzerodha.substack.com/p/how-to-trade-opening-range-breakout)

### Inferences
- ORB's typical profile is a **low win rate (about 24% on QQQ with a 10R target; about 48% on Stocks-in-Play with ATR stops) with positive skew**. Profit is concentrated in a small number of large trend days and in high-volatility years such as 2022. Expect long flat or losing stretches in low-volatility, range-bound markets.
- The per-trade edge on liquid index ETFs is about one tick, so the result depends on execution quality. Stop-order entries in fast markets cause adverse slippage exactly when the signal fires. The Stocks-in-Play version has a bigger per-trade edge, but it trades news-driven stocks with wider spreads, and published net-of-slippage replications were not found.
- In India, STT on the sell side, exchange fees, stamp duty and GST add fixed per-trade costs. For index futures and options these are large relative to the small per-trade edge of a basic ORB, which is consistent with the blog results that turn negative once costs are included.

### Gaps
- No peer-reviewed or credible institutional study of ORB on Nifty/BankNifty net of costs was found. Indian evidence is retail blog backtests with unclear methodology.
- The full results of the five-index "net zero" MQL5 replication could not be read (domain blocked).
- No post-publication (2024–2026) live or out-of-sample track record for either Zarattini ORB paper was found.
- Crypto ORB: no academic study found. Crypto has no session open, so "opening range" has to be defined arbitrarily, for example the UTC day start or the US equity open.

## 2. Intraday momentum: Gao-Han-Li-Zhou (first half-hour predicts last half-hour) and Zarattini "Beat the Market" noise-area strategy

### Takeaway
Market intraday momentum (the return up to about 10:00 predicting the last 30 minutes) is an academically established effect. It appears in the US (JFE 2018), in more than 60 futures markets globally (Baltussen et al., JFE 2021, linked to gamma hedging), and in Bitcoin and Indian futures. It is economically small per trade (predictive R² of about 1.6%) and strongest on volatile, high-volume, news days. Zarattini et al.'s more active "noise area + VWAP trailing stop" SPY version reports a Sharpe of 1.33 net of costs over 2007–2024, but it is unreplicated after publication.

### Cited Findings
- **Gao, Han, Li & Zhou (JFE 2018)**: using S&P 500 ETF high-frequency data for 1993–2013, the first half-hour return (previous close to 10:00 ET) predicts the last half-hour return (15:30–16:00). Predictive R² is 1.6%, which "matches or exceeds" typical monthly predictive R². Predictability is stronger on more volatile days, higher-volume days, recession days, and major macro-news days. — [SSRN 2440866](https://papers.ssrn.com/sol3/papers.cfm?abstract_id=2440866); [EconPapers JFE 129(2):394-414](https://econpapers.repec.org/RePEc:eee:jfinec:v:129:y:2018:i:2:p:394-414)
- The effect also held for US ETFs on sectors and emerging markets. — [search summary of Gao et al. and follow-ups](https://www.researchgate.net/publication/325364670_Market_Intraday_Momentum)
- **Baltussen, Da, Lammers & Martens (JFE 2021)**: in more than 60 futures (equities, bonds, commodities, FX) from 1974 to 2020, the return from the previous close to the last 30 minutes predicts the last-30-minute return. This holds "everywhere", is highly significant, and **reverses over the following days**. It is linked to **gamma-hedging demand** from options market makers and leveraged ETFs. — [paper PDF (Notre Dame)](https://academicweb.nd.edu/~zda/intramom.pdf); [SSRN 3760365](https://papers.ssrn.com/sol3/papers.cfm?abstract_id=3760365)
- **India**: a study adapting Baltussen et al. (regressions plus Net Gamma Exposure signals) to Indian equity futures finds "strong intraday momentum towards the last half hour of the trading day" and finds that some regression models predict it. This is a working paper on ResearchGate, not peer-reviewed as far as could be verified. — [Hedging Demand and Intraday Momentum within the Indian Stock Market (ResearchGate)](https://www.researchgate.net/publication/383567351_Hedging_Demand_and_Intraday_Momentum_within_the_Indian_Stock_Market)
- **Crypto**: Shen, Urquhart & Wang (Financial Review 2022) find that Bitcoin's first half-hour positively predicts its last half-hour, using trading volume to define "market time". Predictability is greatest when the first session has the highest volume or volatility, and is attributed to liquidity provision rather than late-informed trading. — [Wiley](https://onlinelibrary.wiley.com/doi/abs/10.1111/fire.12290); [CentAUR accepted version](https://centaur.reading.ac.uk/100181/)
- Wen, Bouri, Xu & Zhao (Economic Modelling 2022), using Bitcoin high-frequency data from March 2013 to May 2020, find **both intraday momentum and intraday reversal**. The pattern changes with large jumps, FOMC announcements, liquidity, and COVID-19. Reversal is unique to crypto and is attributed to overreaction to non-fundamental information. — [SSRN 4080253](https://papers.ssrn.com/sol3/papers.cfm?abstract_id=4080253); [IDEAS](https://ideas.repec.org/a/eee/ecofin/v62y2022ics1062940822000833.html)
- **Zarattini, Aziz & Barbon (2024), "Beat the Market: An Effective Intraday Momentum Strategy for SPY"**: 2007 to early 2024, total return 1,985% **net of costs**, 19.6% annualized, **Sharpe 1.33**. — [Concretum (search summary)](https://concretumgroup.com/beat-the-market-an-effective-intraday-momentum-strategy-for-sp500-etf-spy/); [SFI Research Paper 24-97](https://www.sfi.ch/en/publications/n-24-97-beat-the-market-an-effective-intraday-momentum-strategy-for-s-p500-etf-spy)
- Beat the Market rules (from a replication spec):
  - Noise band: daily open ± the 14-day average absolute move from the open at that minute of the day, adjusted for the overnight gap.
  - Decision times: every 30 minutes from 10:00 to 15:30.
  - Entry: long above the upper band, short below the lower band.
  - Stop: the tighter of the band and the session VWAP. Positions can reverse at check times.
  - Exit: flat at the close every day.
  - Sizing: scaled to a 2% daily volatility target, capped at 4x leverage.
  - The replication tests $0.01/share slippage (and $0.02) plus $0.0035/share commission, and treats June 2024 onward as the true out-of-sample window. Its numerical results were not shown.
  — [POB-CL-DT PR #13 (GitHub)](https://github.com/patrickobirmingham-eng/POB-CL-DT/pull/13)

### Inferences
- The mechanism (gamma hedging by dealers and leveraged-ETF rebalancing near the close) gives this effect a structural explanation that most technical patterns lack. That makes it more likely to persist, but its size depends on options positioning. Negative dealer gamma amplifies the effect; positive gamma dampens it. In India, the explosion of index-options volume makes the dealer-gamma channel plausible, but this is not verified with net-of-cost numbers.
- The effect works best on volatile, news-heavy, trending days. It fails on quiet range days and on days with late-session reversals. Because the signal reverses over subsequent days, positions must be closed at the close.
- The single last-half-hour trade from Gao et al. has a small per-trade return. Realistic implementation needs very cheap execution, such as index futures.

### Gaps
- Gao et al.'s timing-strategy economics (often quoted as about 6.67% annualized; Sharpe around 1) could not be verified from the full text, so they are left out of the findings.
- Beat the Market max drawdown, hit rate and exact cost assumptions were not verified (primary sources blocked).
- No published post-2024 out-of-sample performance for the noise-area strategy was found.
- No peer-reviewed India study with net-of-cost P&L was found, and no evidence on whether the effect has decayed since 2013 in US data was located.

## 3. VWAP strategies, moving-average crossovers, Donchian/channel breakouts, Supertrend

### Takeaway
Rigorous net-of-cost evidence for intraday VWAP, MA-crossover, Donchian or Supertrend systems is thin. The academic literature on technical rules generally finds that apparent profits disappear after correcting for data snooping and costs. Vendor-style backtests (VWAP on QQQ/TQQQ, Supertrend on Nifty) show high gross numbers, and the India Supertrend example's profit was almost entirely eaten by statutory costs and 2 bps of slippage. On daily timeframes, trend-following on crypto (Donchian ensembles) has the strongest recent evidence.

### Cited Findings
- **VWAP trend (Zarattini & Aziz)**: a VWAP-based day-trading strategy on QQQ and TQQQ from Jan 2018 to Sep 2023 reportedly grew $25,000 to $192,656 (QQQ) and to $2,085,417 (TQQQ, leveraged ETF). The paper is titled "VWAP: The Holy Grail for Day Trading Systems". — [ResearchGate](https://www.researchgate.net/publication/376217460_Volume_Weighted_Average_Price_VWAP_The_Holy_Grail_for_Day_Trading_Systems)
- **Supertrend on Nifty**: on 60 days of 15-minute Nifty data, a Supertrend strategy made about ₹57,892 gross on ₹10 lakh capital over 32 trades. After brokerage, statutory charges and 2 bps of slippage, net profit was **₹1,652**, and it turned negative at higher slippage. (Aggregated search result; most likely source is this repo, which uses an Indian cost model.) — [nifty-banknifty-intraday-trend-algo (GitHub)](https://github.com/anirudhatalmale6-alt/nifty-banknifty-intraday-trend-algo)
- A Supertrend backtest on survivorship-free NIFTY/BANKNIFTY data with real costs was rated "plausible but fragile". — [The Honest Quant](https://thehonestquant.com/strategies/supertrend-strategy)
- There is an academic-style 15-minute Supertrend backtest on the top 5 Nifty-50 contributors, but no verified figures were available. — [ResearchGate](https://www.researchgate.net/publication/392479564_Back-Testing_Super_Trend_in_15_Mins_Time_Frame_among_Top_5_Contributors_of_Nifty_50_Stocks)
- An arXiv paper optimises Supertrend parameters with Bayesian optimisation. This illustrates the parameter-fitting risk. — [arXiv 2405.14262](https://arxiv.org/html/2405.14262v1)
- **Technical rules and data snooping**: Bajgrowicz & Scaillet (JFE 2012) re-tested technical trading rules on the DJIA from 1897 to 2011 using a false-discovery-rate control. They show that the economic value of rules must be judged on performance persistence after both transaction costs and data-snooping correction. The paper is widely cited as finding that the apparent outperformance of technical rules does not survive these tests out of sample. — [SSRN 1095202](https://papers.ssrn.com/sol3/papers.cfm?abstract_id=1095202); [ScienceDirect](https://www.sciencedirect.com/science/article/abs/pii/S0304405X1200116X)
- **Crypto Donchian trend (daily, not intraday)**: Zarattini, Pagani & Barbon (2025), "Catching Crypto Trends". An ensemble of Donchian channel models with different lookbacks plus volatility-based sizing, applied to a rotational portfolio of the top 20 liquid coins, gives a **Sharpe above 1.5 net of fees and 10.8% annualized alpha vs Bitcoin**. The universe is survivorship-bias-free and includes all coins since 2015 with median daily volume of at least $2M. — [SSRN 5209907](https://papers.ssrn.com/sol3/papers.cfm?abstract_id=5209907); [CXO Advisory](https://www.cxoadvisory.com/technical-trading/crypto-asset-trend-following-strategies/)
- Concretum has also published on "Seasonality in Bitcoin Intraday Trend Trading" (title only; content blocked). — [Concretum](https://concretumgroup.com/seasonality-in-bitcoin-intraday-trend-trading/)

### Inferences
- VWAP trend ("long above VWAP, short below") and MA crossovers on intraday bars trade often and in small increments. They are among the most cost-sensitive systems, and in India the Supertrend example lost about 97% of gross profit to costs.
- Trend rules work in trending, high-volatility regimes and get whipsawed in choppy, mean-reverting sessions. That is the usual regime for large-cap indices most of the time.
- Leveraged-ETF (TQQQ) results mainly show leverage applied to a 2018–2023 bull market. They should not be read as evidence of edge.
- Crypto trend-following has better evidence on daily bars than on intraday bars. Crypto's strong trending episodes and 24/7 trading suit trend models, but published net-of-fee intraday evidence was not found.

### Gaps
- No peer-reviewed net-of-cost studies were found for intraday Bollinger/VWAP-reversion, intraday MA crossover, or intraday Donchian on Nifty/BankNifty or crypto.
- The cost assumptions and drawdowns in Zarattini's VWAP paper were not verified.
- No credible evidence on Supertrend win rate or drawdown over long samples was found. Indian evidence is short-sample retail backtests.

## 4. Mean reversion: Bollinger, RSI(2), intraday overreaction reversal, gap fade, short-horizon reversal

### Takeaway
Short-horizon reversal is real but is mostly a microstructure effect (bid-ask bounce and temporary liquidity imbalance lasting less than an hour). Academic work finds it is very hard to profit from net of costs unless you are a liquidity provider. RSI(2)-style daily mean reversion has a high win rate and shallow drawdowns in long SPY backtests, but it is a multi-day swing strategy rather than intraday, and is reported to have decayed since about 2015. Crypto shows intraday reversal as well as momentum.

### Cited Findings
- **Heston, Korajczyk & Sadka (JF 2010)**: short-term return reversal is driven by temporary liquidity imbalances lasting less than an hour and by bid-ask bounce. There is also return continuation at half-hour intervals that are exact multiples of a trading day, lasting at least 40 trading days. After transaction costs the strategies are greatly reduced. It is "quite difficult to profit from these types of intraday strategies without some exogenous desire to trade". For medium and large stocks, average decile spreads net of costs were roughly −20 and −14 bps, with one-way costs under 5 bps (2001–2005). — [HKS paper PDF](https://www.bauer.uh.edu/departments/finance/documents/Heston-Korajczyk-Sadka-jf-2010-01-07.pdf); [Wiley](https://onlinelibrary.wiley.com/doi/abs/10.1111/j.1540-6261.2010.01573.x)
- Baltussen, Da & Soebhag have a paper titled "End-of-Day Reversal" (content not read). — [paper PDF](https://academicweb.nd.edu/~zda/EOD.pdf)
- **RSI(2) (Connors)**: buy SPY when it is deeply oversold in an uptrend (2-period RSI very low, price above its 200-day MA) and sell the bounce. A 26-year SPY backtest gives 181 trades with an **82% win rate**, an average hold of 3.7 trading days, and a **max drawdown of −13.8% vs −55.2% for buy-and-hold**. The author ran in-sample vs out-of-sample walk-forward splits to test for decay after 2015, "as most mean-reversion strategies did". — [Substack backtest](https://gurufinanceinsights.substack.com/p/i-backtested-the-classic-rsi2-mean); [QuantifiedStrategies RSI-2](https://www.quantifiedstrategies.com/rsi-2-strategy/)
- **Crypto**: Bitcoin intraday data shows intraday reversal driven by overreaction to non-fundamental information, alongside momentum (2013–2020). — [Wen et al. 2022, SSRN](https://papers.ssrn.com/sol3/papers.cfm?abstract_id=4080253)

### Inferences
- Mean-reversion systems usually have a high win rate (70–80%+) and a low payoff ratio, with negative skew. Losses cluster in trend and crash regimes such as gap-and-go days, news shocks, and crypto liquidation cascades. They work in range-bound, high-liquidity regimes.
- Intraday reversal "edge" measured at the last traded price is partly bid-ask bounce. A taker who crosses the spread captures little of it, which is the main failure mode for retail.
- Gap-fade and intraday overreaction strategies conflict with the intraday momentum evidence from Section 2. On news and high-volatility days, continuation tends to dominate.

### Gaps
- No rigorous net-of-cost study of intraday Bollinger Band reversion or gap fade (US, India or crypto) was found within the search budget.
- No Indian evidence on intraday reversal or gap fade was found.
- The out-of-sample split results of the RSI(2) Substack backtest were not visible in the snippet.

## 5. Pairs trading / statistical arbitrage (incl. Indian intraday pairs)

### Takeaway
Classic distance-based pairs trading earned about 11%/yr excess in 1962–2002 (Gatev et al.). Its profits peaked in the 1970s–80s, declined from the 1990s, and were largely unprofitable after costs post-2002 except in bear markets. High-frequency academic pairs studies report very high Sharpe ratios net of costs, but they assume institutional execution. An Indian 5-minute pairs test turned a small gross profit into a net loss after NSE costs.

### Cited Findings
- **Gatev, Goetzmann & Rouwenhorst (RFS 2006)**: pair stocks by minimum distance between normalized prices, then trade divergences (open at a 2-sigma spread, close on convergence). The method earns up to **11% annualized excess return** for self-financing pair portfolios on daily data from 1962 to 2002, and profits typically exceed conservative transaction-cost estimates. — [SSRN 141615](https://papers.ssrn.com/sol3/papers.cfm?abstract_id=141615); [IDEAS RFS 19(3):797-827](https://ideas.repec.org/a/oup/rfinst/v19y2006i3p797-827.html)
- **Do & Faff (2010)**: on CRSP 1962–2009, pairs profitability peaked in the 1970s–80s and has declined since the 1990s. A rising share of pairs diverge and never converge. The exceptions are the 2000–2002 and 2007–2009 bear markets. **Do & Faff (2012)**: the GGR algorithm is largely unprofitable after 2002 once trading costs are included. — [Do & Faff, "Does Simple Pairs Trading Still Work?"](https://www.researchgate.net/publication/47554136_Does_Simple_Pairs_Trading_Still_Work); ["Are Pairs Trading Profits Robust to Trading Costs?"](https://www.researchgate.net/publication/228259167_Are_Pairs_Trading_Profits_Robust_to_Trading_Costs)
- **High-frequency (US, academic)**: a high-frequency pairs backtest reports **60.61% p.a. and Sharpe 5.30 after transaction costs**. — [Statistical Arbitrage Pairs Trading with High-frequency Data (ResearchGate)](https://www.researchgate.net/publication/338969525_Statistical_arbitrage_pairs_trading_with_high-frequency_data)
- Liu & Chang study intraday pairs trading on high-frequency oil-company stocks. — [ResearchGate](https://www.researchgate.net/publication/303847955_Intraday_pair_trading_strategies_on_high_frequency_data_the_case_of_oil_companies)
- **India, daily**: pairs trading in related NSE stock futures over 2011–2017 is reported to be "significantly profitable", with average annualized profitability up to **34% including transaction costs**. — [Springer, Asia-Pacific Financial Markets](https://link.springer.com/article/10.1007/s10690-020-09317-1)
- **India, intraday (5-min)**: an Engle-Granger cointegration test with a walk-forward z-score and GARCH-adjusted thresholds, run on the closest cointegrated pair (SBIN/BANKBARODA). It makes **+1.3% gross over 44 trading days but −3.7% after realistic NSE costs** (STT, stamp duty, GST, slippage). This is a small hobby/student project. — [avadhi3103/Intraday-Pairs-Trading (GitHub)](https://github.com/avadhi3103/Intraday-Pairs-Trading)
- A PLOS ONE study builds mutual-information stock networks for portfolio selection by intraday traders using Indian high-frequency data. — [PLOS ONE](https://journals.plos.org/plosone/article?id=10.1371%2Fjournal.pone.0221910)
- Short-selling restrictions affect pairs-trading profitability (Taiwan evidence). — [ScienceDirect](https://www.sciencedirect.com/science/article/abs/pii/S1059056017305300)

### Inferences
- Pairs trading is market-neutral and mean-reverting. It works when spreads are stationary and liquidity shocks are temporary. It fails when relationships break structurally (mergers, earnings, regulation), which is the "diverge and never converge" problem, and when crowding compresses the spread.
- Intraday pairs in India face extra frictions. Shorting cash equities intraday is possible, but overnight shorts require F&O (stock futures) or SLB. STT and the cost of two legs double the hurdle. The documented 5-min NSE test going from positive gross to negative net is the expected outcome for retail.
- Sharpe ratios above 5 net of costs in high-frequency academic studies assume trading at observed prices with low latency. They are not achievable at retail latency or cost.

### Gaps
- The exact sample, cost assumption and authorship of the 60.61%/Sharpe 5.30 high-frequency study were not verified from full text.
- No peer-reviewed intraday pairs study on NSE with realistic costs was found (only daily stock-futures pairs and a GitHub project).
- No crypto pairs or stat-arb net-of-cost evidence was gathered within budget.

## 6. Order-flow / microstructure signals (order book imbalance, trade flow): feasibility at retail latency

### Takeaway
Order flow imbalance (OFI) at the best bid and ask explains most contemporaneous short-interval price changes (R² of about 65–70%). That is an explanatory result, not a predictive one, and any predictive component decays within seconds. Profiting from it needs co-located, low-latency infrastructure, so it is generally not feasible as a directional signal for retail traders.

### Cited Findings
- **Cont, Kukanov & Stoikov (2014)**: over short intervals, price changes are mainly driven by order flow imbalance at the best bid and ask. The relationship is linear, with a slope inversely proportional to market depth. The data is NYSE TAQ for 50 US stocks, and the linear model R² is about 65–70%. — [arXiv 1011.6402](https://arxiv.org/pdf/1011.6402); [SSRN 1712822](https://papers.ssrn.com/sol3/papers.cfm?abstract_id=1712822)
- Important caveat: the OFI measured over a period explains the price change **over the same period**. The paper does not show that OFI predicts future prices. — [search summary of CKS](https://quantmemo.com/writing/paper-cont-kukanov-stoikov-order-flow-imbalance); [replication on LOBSTER data](https://github.com/Nouzee/ofi-price-impact)
- OFI has cross-impact across stocks. — [Quantitative Finance 2023](https://www.tandfonline.com/doi/full/10.1080/14697688.2023.2236159)
- OFI amplifies price moves in US Treasuries. — [Federal Reserve FEDS Note, Nov 2025](https://www.federalreserve.gov/econres/notes/feds-notes/order-flow-imbalances-and-amplification-of-price-movements-evidence-from-u-s-treasury-markets-20251103.html)
- Order-book imbalance price impact has been demonstrated in crypto. — [Towards Data Science](https://towardsdatascience.com/price-impact-of-order-book-imbalance-in-cryptocurrency-markets-bf39695246f6/)

### Inferences
- Retail traders using broker APIs (in India typically about 100 ms or more, rate-limited, without full depth-of-book or tick-by-tick data) see OFI after high-frequency market makers have already acted on it. The usable horizon is shorter than the round-trip time, and the edge per trade is smaller than the spread.
- Coarser "trade flow" proxies, such as cumulative delta or volume surges on 1–5 minute bars, are closer to momentum or volume-filter signals (compare the Stocks-in-Play relative-volume filter). They are more realistic for retail, but they are not microstructure alpha.

### Gaps
- No credible study was found that quantifies the profitability of OFI or book-imbalance signals net of costs at retail latencies, for India or crypto.
- No published evidence on NSE tick-by-tick data access or latency for retail algos was gathered.

## 7. Machine-learning intraday prediction and the out-of-sample problem

### Takeaway
Headline ML results (LSTM, gradient boosting, random forests on S&P 500 constituents) are impressive before costs but decay sharply after about 2010. The broader literature shows published anomalies lose 26–58% of their returns once out of sample or published, and that most "discoveries" would fail a t-statistic hurdle of 3. Treat any ML intraday backtest as presumed overfit until it shows walk-forward, net-of-cost, post-publication results.

### Cited Findings
- **Fischer & Krauss (EJOR 2018)**: LSTM networks predicting out-of-sample direction for S&P 500 constituents, 1992–2015, earn **0.46% per day and a Sharpe of 5.8 before transaction costs**. Profits are strong before 2010 and **deteriorate markedly afterwards**. Later replications find all models under 0.1% average daily return after 2008. — [arXiv 2201.08218 (replication/extension)](https://arxiv.org/pdf/2201.08218); [ResearchGate](https://www.researchgate.net/publication/321630147_Deep_learning_with_long_short-term_memory_networks_for_financial_market_predictions)
- **Krauss, Do & Huck (EJOR 2017)**: deep neural nets, gradient-boosted trees and random forests for daily statistical arbitrage on the S&P 500. After transaction costs the annualized returns are 73% (ensemble), 67% (RF), 46% (GBT) and 27% (DNN), in a full sample dominated by earlier decades. — [Semantic Scholar](https://www.semanticscholar.org/paper/Deep-neural-networks,-gradient-boosted-trees,-on-Krauss-Do/b656c3ff78af97ba78eed50f048cd5aa972151b3); [IDEAS](https://ideas.repec.org:443/p/zbw/iwqwdp/032016.html)
- An intraday LSTM long-short strategy for the S&P 500 is summarized by Quant Buffet. — [Quant Buffet](https://quantbuffet.com/en/2024/12/29/predicting-intraday-returns-with-machine-learning-methods/)
- An LSTM direction-prediction study covers S&P 500 vs Nasdaq-100 for different daily periods. — [ScienceDirect 2024](https://www.sciencedirect.com/science/article/pii/S2666827024000938)
- **McLean & Pontiff (JF 2016)**: post-publication anomaly returns are about **26% lower** than in-sample and about **58% lower** than out-of-sample but pre-publication. — [ResearchGate](https://www.researchgate.net/publication/315421495_Does_Academic_Research_Destroy_Stock_Return_Predictability)
- **Harvey, Liu & Zhu (RFS 2016)**: most claimed findings in financial economics are likely false because of multiple testing. They argue the t-statistic hurdle should be raised to about 3.0. — [ResearchGate](https://www.researchgate.net/publication/302561929_and_the_Cross-Section_of_Expected_Returns); [summary](https://atticusli.com/replication-crisis/finance-replication-crisis-harvey-2016/)
- Bajgrowicz & Scaillet: data snooping and transaction costs must both be addressed before claiming technical-rule profitability. — [SSRN](https://papers.ssrn.com/sol3/papers.cfm?abstract_id=1095202)

### Inferences
- The decay of ML stat-arb after about 2010 coincides with the spread of algorithmic and HFT trading. The edges the models found were the same short-term reversal and liquidity effects that high-frequency firms now harvest.
- ML failure modes:
  - Look-ahead bias.
  - Survivorship bias.
  - Optimistic fill assumptions (trading at the close that generated the signal).
  - Non-stationarity and regime change.
  - Hyperparameter search that acts as hidden data snooping.
- Reinforcement learning adds overfitting to simulated execution on top of these.
- A practical rule: require a pre-registered walk-forward test, costs at or above real spread plus fees, a t-stat above 3 on net returns, and live paper trading before allocating capital.

### Gaps
- No credible net-of-cost, out-of-sample evidence was found for ML or RL intraday strategies on Nifty/BankNifty or crypto. Most such papers are low-quality journals or arXiv preprints with gross results.
- The exact post-2001 or post-2010 decay figures for Krauss et al. (2017) were not verified from full text.

## 8. Evidence on retail day-trader performance overall

### Takeaway
Large-sample administrative data from three countries agree: the vast majority of retail day traders lose money net of costs, and very few are persistently profitable. That is under 1% predictably profitable in Taiwan, 97% of persistent traders losing in Brazil, and over 70% of intraday cash traders losing in India in FY23.

### Cited Findings
- **Taiwan, Barber, Lee, Liu & Odean (2014)**: fewer than 1% of individuals who day trade over a year are predictably profitable. — [ResearchGate: Do Individual Day Traders Make Money? Evidence from Taiwan](https://www.researchgate.net/publication/238220682_Do_Individual_Day_Traders_Make_Money_Evidence_from_Taiwan); [The Cross-Section of Speculator Skill (PDF)](https://faculty.haas.berkeley.edu/odean/papers/day%20traders/The%20Cross-Section%20of%20Speculator%20Skill.pdf)
- **Taiwan, "Learning, Fast or Slow" (Barber, Lee, Liu, Odean & Zhang, 2020)**: 3.7 billion TWSE transactions from 1992 to 2006. Day traders lose **23.9 bps per day net of fees** on average, and aggregate performance is reliably negative in **14 of 15 years**. Traders keep trading despite losses. — [AEA 2019 paper](https://www.aeaweb.org/conference/2019/preliminary/paper/ZKnGb4Zh); [summary](https://www.tradicted.com/research/barber-learning-2020/)
- **Brazil, Chague, De-Losso & Giovannetti (2019), "Day Trading for a Living?"**: all individuals who started day trading Brazilian equity futures (mini-index) in 2013–2015. **97% of those who persisted more than 300 days lost money**. Only 1.1% earned more than the minimum wage and only 0.5% more than a bank teller's starting salary. There is no evidence of learning. — [SSRN 3423101](https://papers.ssrn.com/sol3/papers.cfm?abstract_id=3423101)
- **India, SEBI study (July 2024) of the equity cash segment, FY23**:
  - Based on about 7 million investors, **over 70% of individual intraday traders made losses** in FY23, with an average loss of ₹5,371.
  - The number of intraday traders rose about 4x from FY19 to FY23.
  - 76% of traders under 30 lost money.
  - Very frequent traders (more than 500 trades a year) had higher loss rates.
  - For loss-makers, **trading costs added 57% on top of their trading losses**.
  — [Business Standard](https://www.business-standard.com/markets/news/over-70-intra-day-traders-incur-losses-during-fy23-reveals-sebi-study-124072401110_1.html); [Angel One summary](https://www.angelone.in/news/market-updates/sebi-highlights-intraday-trading-risk)
- Taiwan futures day-trading profitability and trader characteristics (Kuo et al.). — [IRABF paper](https://www.irabf.org/upload/journal/prog/2.%20Final%20-%20The%20Profitability%20of%20Day%20Trading%20and%20the%20Characteristics%20of%20Traders%20%20Evidence%20from%20the%20Taiwan%20Futures%20Market.pdf)

### Inferences
- Costs and turnover are the dominant drag. The SEBI cost-to-loss ratio and the finding that frequent traders lose more both point to minimising trade count and per-trade cost. This favours strategies with few trades per day, like the single daily trade in Gao et al. or ORB, over high-turnover systems like Supertrend or VWAP-cross scalping.
- The base rate that a retail intraday strategy is profitable net of costs is very low. Backtests should be judged against that prior.

### Gaps
- SEBI's separate F&O studies (e.g., the share of individual F&O traders losing in FY22 and FY24) were not covered here. They are likely relevant for Nifty/BankNifty options-based intraday trading and should be sourced separately.
- No comparable administrative study of retail crypto day-trader outcomes was found within budget. Exchange-reported "X% lose" figures are mostly unsourced marketing.
