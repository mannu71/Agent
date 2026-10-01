# Swing Trading Strategies (2 days to 6 weeks) for Indian Equities and Crypto: Rules, Net-of-Cost Evidence, Costs and Overnight Risks (as of Oct 2026)

*Method note for the report writer: page fetching (WebFetch) was blocked by the network egress proxy for every domain tried (niftyindices.com, iima.ac.in, nber.org, arxiv.org, ssrn-type sites, quantpedia, alphaarchitect, 5paisa, fyers). All findings below come from web-search result summaries of the cited pages, not from reading the full documents. Numbers are reported as the summaries gave them; where a summary looked garbled or internally inconsistent, this is flagged. High-stakes figures (index CAGRs, drawdowns, tax rates) should be spot-checked against the primary PDF before publication.*

---

## 1. Time-series and cross-sectional momentum (weekly/monthly), 52-week-high momentum, and Indian momentum evidence (indices, IIMA data, crashes)

### Takeaway
Momentum is the best-documented swing/position-horizon effect worldwide and in India. Indian momentum premia have been large: IIMA reports a WML factor of about 21-22% a year for 1994-2014, and the Nifty200 Momentum 30 TRI has returned about 18.8% CAGR against 13.9% for the Nifty 200 TRI since 2005. But the premium comes with crashes of 50-70% (2000, 2008, 2020, 2024-25), high turnover, and costs that are material at holding periods under a month. One 2026 practitioner backtest finds that net-of-cost Indian momentum alpha sits almost entirely in the less-liquid half of the momentum basket. Classic cross-sectional momentum is a 3-12 month effect. At 1-4 week horizons, the cross-section tends to show *reversal* in equities, so "swing momentum" is better framed as trading in the direction of 3-12 month leadership than as ranking on last week's return.

### Cited Findings

**US/global foundations**
- Jegadeesh & Titman (1993): buying past winners and selling past losers earns about 1% per month over the following 3-12 months. The 12-month formation / 3-month holding decile strategy earned 1.49% per month "on paper" (gross). — [Wikipedia: Momentum investing](https://en.wikipedia.org/wiki/Momentum_investing); [FA-Mag summary](https://www.fa-mag.com/news/article-858.html)
- Lesmond, Schill & Zhou (2004, JFE) estimate that round-trip trading costs for momentum strategies rarely fall below about 1.5% per trade (1980-1998 sample). Once total trading costs are counted, momentum profits are "eclipsed in large part". — [Lesmond et al. 2004 JFE (PDF)](https://www.bauer.uh.edu/rsusmel/phd/Lesmond_et%20al%20_2004_JFE.pdf); [FA-Mag](https://www.fa-mag.com/news/article-858.html)
- Jegadeesh & Titman (2001, JF): momentum profits continued in the 1990s out of sample, so the original result was "not a product of data snooping". There is no significant reversal 2-3 years after formation, but significant reversals appear at 4-5 years. — [IDEAS/RePEc](https://ideas.repec.org/a/bla/jfinan/v56y2001i2p699-720.html); [NBER w7159](https://www.nber.org/papers/w7159)
- Moskowitz, Ooi & Pedersen (2012, JFE), "Time Series Momentum": significant TSMOM in every one of 58 liquid futures (equity indices, bonds, commodities, FX) over 1985-2009. Returns persist for 1-12 months and partially reverse over longer horizons. The diversified TSMOM portfolio had little exposure to standard factors and did best in extreme markets. — [SSRN abstract](https://papers.ssrn.com/abstract=2089463)
- The follow-up "Trends Everywhere" (Babu, Levine, Ooi, Pedersen, Stamelos) reports *gross* Sharpe ratios of 1.17 for 12-month TSMOM in traditional assets, 1.34 in alternative assets, 0.95 on long-short equity factors, and 1.60 combined. — [Quantpedia summary](https://quantpedia.com/time-series-momentum-works-everywhere/)
- Hurst, Ooi & Pedersen, "A Century of Evidence on Trend-Following" (JPM 2017): 67 markets, Jan 1880-Dec 2016, 1/3/12-month lookbacks. Net returns after a 2%/20% fee assumption were positive in every decade. A CXO Advisory summary describes an average Sharpe ratio of about 0.4 after those fees; this figure could not be verified against the paper. — [AQR](https://www.aqr.com/Insights/Research/Journal-Article/A-Century-of-Evidence-on-Trend-Following-Investing?aqrPDF=1); [CXO Advisory](https://www.cxoadvisory.com/momentum-investing/trend-following-with-intrinsic-momentum-over-the-very-long-run)
- Daniel & Moskowitz, "Momentum Crashes" (US 1927-2013, top-minus-bottom decile on 12-month returns):
  - The two worst months were July and August 1932: past losers returned +232% while past winners returned +32%.
  - In March-May 2009, losers returned +163% against +8% for winners.
  - Crashes happen in "panic states" (after market declines, at high volatility) and coincide with market rebounds. They are partly forecastable, and a dynamic, volatility-scaled momentum strategy roughly doubles the static strategy's alpha and Sharpe ratio.
  - [NBER w20439](https://www.nber.org/system/files/working_papers/w20439/w20439.pdf); [Columbia PDF](https://business.columbia.edu/sites/default/files-efs/pubfiles/4607/mom11.pdf)

**52-week-high momentum**
- George & Hwang (2004, JF): long stocks nearest their 52-week high and short those furthest from it earns about 0.45% per month abnormal return. In Fama-MacBeth tests, nearness to the 52-week high dominates both Jegadeesh-Titman individual-stock momentum and Moskowitz-Grinblatt industry momentum. Its returns do not reverse in the long run. — [CXO Advisory](https://www.cxoadvisory.com/1284/technical-trading/the-52-week-high-as-a-momentum-indicator-for-individual-stocks/); [SUFE newsletter abstract](https://academicnewsletter.sufe.edu.cn/info/361244)
- After transaction costs, one study reports that George-Hwang abnormal returns stay statistically significant. Later studies in other markets (e.g., Australia, where liquidity and costs are examined explicitly) are more mixed. The search summary did not make clear which paper made the "robust after costs" claim. — [ANU: impact of liquidity and transaction costs on 52-week-high momentum in Australia](https://researchportalplus.anu.edu.au/en/publications/the-impact-of-liquidity-and-transaction-costs-on-the-52-week-high/)
- India: Quantpedia hosts an out-of-sample test by Milind Paradkar (QuantInsti) on Indian stocks for 2014-2017. It uses the Hong, Jordan & Liu industry-level 52-week-high rotation. The results were not retrievable. — [Quantpedia: An Analysis of 52-Weeks High Effect in Indian Stocks](https://quantpedia.com/an-analysis-of-52-weeks-high-effect-on-indian-stocks/)

**India: academic and factor-library evidence**
- IIMA (Agarwalla, Jacob & Varma) Indian Fama-French + momentum data library:
  - Built from CMIE Prowess, October 1993 onward. It excludes illiquid firms for investability, corrects for survivorship ("vanishing companies" in the mid-1990s), is updated monthly, and has a new series launched December 2021.
  - [IIMA data library](https://faculty.iima.ac.in/~iffm/Indian-Fama-French-Momentum/); [IIMA working paper (2013)](https://web.iima.ac.in/~iffm/legacy/four-factors-India-90s-onwards-IIM-WP-Version-original-Sep13.pdf)
- IIMA momentum factor (WML) returns and drawdowns:
  - Average annual return 21.9% over Jan 1994-Dec 2014. The original paper (1993-2012) gives 21.2%.
  - Worst drawdown: -62%, March 2000 to December 2000, with a 5.1-year recovery.
  - Second worst: -52.8%, December 2008 to September 2009.
  - [IIMA drawdown page](https://web.iima.ac.in/~iffm/Indian-Fama-French-Momentum/drawdown.php); [IIMA library](https://faculty.iima.ac.in/~iffm/Indian-Fama-French-Momentum/)
- A search summary also cites a WML "annualized return of 11.29% with annualized volatility of 17.44%". The period is not given; it may be the full updated series to date. Treat it as unverified. Taken at face value, it suggests the premium has shrunk since 2014. — [IIMA library](https://faculty.iima.ac.in/~iffm/Indian-Fama-French-Momentum/)
- Sehgal & Jain (BSE 500, Jan 2002-Jun 2010, 493 firms):
  - Price momentum profits persist up to about 6 months.
  - Long-short earnings momentum was the most profitable single sort. A triple sort on price, earnings and revenue momentum earned 2.28% per month.
  - CAPM and Fama-French do not explain these returns. Post-holding overreaction (reversal) appears for both winners and losers.
  - [AAMJAF 2015](https://ejournal.usm.my/aamjaf/article/view/aamjaf_vol11-no1-2015_3); [RePEc](https://ideas.repec.org/a/usm/journl/aamjaf01101_47-84.html)
- NSE data Jan 1997-Mar 2013: significant short-term momentum and long-term overreaction. A separate 1995-2008 NSE study finds momentum at 6- and 12-month horizons, contrarian profits at 3 years, and *reversals* for 1-month/1-month strategies across all winner-loser combinations. — [Publishing India IJFM](https://www.publishingindia.com/archive/ijfm/a-study-of-contrarian-and-momentum-profits-in-indian-stock-market); [NSE research paper](https://nsearchives.nseindia.com/content/research/res_paperfinal223.pdf)

**India: NSE momentum indices**
- Nifty200 Momentum 30 methodology:
  - Holds the top 30 of the Nifty 200 by a "Normalised Momentum Score" built from 6-month and 12-month price returns adjusted for daily return volatility.
  - Weight = free-float market cap × score, capped at the lower of 5% or 5× the stock's free-float-cap weight.
  - Rebalanced semi-annually. Base date 1 April 2005, base value 1000.
  - [NSE factsheet](https://www.niftyindices.com/Factsheet/Factsheet_Nifty200_Momentum30.pdf); [Baroda BNP NFO doc](https://www.barodabnpparibasmf.in/assets/download_documents/BBNPP-Nifty-200-Momentum-30-Index-Fund-NFO.pdf)
- Nifty200 Momentum 30 performance:
  - TRI CAGR 18.8% versus 13.9% for the Nifty 200 TRI since 1 April 2005.
  - Drawdown of -31.79% from 27 Sep 2024 to 7 Apr 2025 (192 days to trough). Earlier drawdowns: -34.21% (2020), -28.39% (2021-22), -26.34% (2010-11).
  - 1-year return -2.5% to 30 June 2026.
  - [HDFC MF Nifty200 Momentum 30 Index Fund PPT, July 2026](https://files.hdfcfund.com/s3fs-public/Others/2026-07/HDFC%20NIFTY200%20Momentum%2030%20Index%20Fund_PPT%20%28July%202026%29.pdf)
- 2008 GFC drawdown: Nifty200 Momentum 30 fell -67.9% from its high, against -64.6% for the Nifty 50. — [Motilal Oswal / HDFC fund documents via search](https://www.motilaloswalmf.com/CMS/assets/uploads/Documents/2733b-motilal-oswal-nifty-200-momentum-30-etf-february-2023.pdf)
- Calendar-year returns of +62.09% in 2021 and -12.09% in 2025, and -5.21% for 2026, appear on a third-party tracker (undated snapshot). The same snapshot shows "1Y +26.10%", which conflicts with HDFC's -2.5% 1-year figure to June 2026. The tracker figures are probably from a different date, so treat them as unreliable. — [sharpely.in](https://sharpely.in/indices/nifty-200-momentum-30/58/index-technicals)
- Nifty Midcap150 Momentum 50: top 50 of the Nifty Midcap 150 by the same normalized momentum score. Base date 1 April 2005; index launched 16 August 2022, so all pre-2022 history is back-calculated. — [Tata MF explainer](https://www.tatamutualfund.com/blogs/nifty-midcap150-momentum-50-index-how-stocks-are-selected-and-rebalanced); [NSE factsheet](https://www.niftyindices.com/Factsheet/Factsheet_NiftyMidcap150Momentum50.pdf)
- Midcap150 Momentum 50 fund data: a tracking fund shows a max drawdown of -24.88% (27 Sep 2024 to 14 Mar 2025). Its *since-fund-inception* CAGR was -2.75% as of July 2026, with 29.6% of rolling 1-year windows negative. These are fund-level figures since the fund's launch (about 2024), not index figures since 2005. No reliable full-history index CAGR was found. — [arthgyaan (Kotak fund page)](https://arthgyaan.com/fund/kotak-nifty-midcap-150-momentum-50-index-fund-direct-growth/152916)
- Rebalancing friction for Nifty200 Momentum 30 (Dec 2025):
  - The rebalance replaced 19 of 30 constituents. Estimated round-trip index-fund flows were ₹16,130 crore, and inflows into 27 stocks could exceed their 1-day average volume (Periscope Analytics estimate).
  - Index-fund portfolio turnover was 124.24% a year as of May 2025. Momentum index fund expense ratios average 0.39%, against 0.18% for Nifty 50 index funds.
  - [Business Standard, Dec 2025](https://www.business-standard.com/markets/news/nifty200-momentum30-a-flip-of-index-switch-jolts-16k-cr-across-the-grid-125121400388_1.html)
- NSE Indices changed the ad-hoc rebalancing method for indices with a variable number of stocks, effective 29 May 2026: an excluded stock's weight is now redistributed proportionally. The Momentum 30 has a fixed stock count, so this may not apply to it directly. NSE also published an April 2026 momentum whitepaper. — [Muthoot Securities news](https://www.muthootsecurities.com/News/Detailed-News/NSE-Indices-revises-rebalancing-methodology-for-Nifty-variable-stock-indices/1694343); [NSE momentum whitepaper 2026](https://www.niftyindices.com/docs/default-source/indices/nifty200-momentum-30/momentum-strategy-whitepaper_2026.pdf)

**India: practitioner net-of-cost evidence**
- Capitalmind, 30-stock monthly-rebalanced momentum: ₹100 in Jan 2007 grew to ₹464 by 2019, a 12.6% CAGR against 8.9% for the Nifty and 8.7% for the Nifty 500. It beat both indices in only 6 of 13 years and badly lagged in 2008 and 2016. Capitalmind says naive monthly momentum has had drawdowns above 50% "every 3-4 years", and that a basic strategy still beat the benchmark after costs and taxes over 15 years. — [Capitalmind: momentum investing basics India](https://www.capitalmind.in/insights/momentum-investing-basics-india); [Substack repost](https://calm.substack.com/p/does-momentum-investing-work-in-india)
- "Is Indian Momentum Investing Just a Liquidity Illusion?" (Freefincal, 27 Mar 2026; T. Desai, BacktestIndia):
  - Setup: the top 30 Nifty 200 stocks by 12-month return, split by "scaled turnover" (daily turnover ÷ market cap). Dec 2006-Jun 2025, annual rebalance, equal weight, net of brokerage, slippage, 20% STCG and 12.5% LTCG.
  - Low-turnover (illiquid) half: 19.43% net CAGR, max drawdown -66.41%.
  - Base momentum 30: 14.60% net CAGR, max drawdown -70.61%.
  - Nifty 50: 10.41% net CAGR, max drawdown -55.12%.
  - High-turnover (liquid) half: 8.51% net CAGR, max drawdown -75.09%.
  - Conclusion: "All the alpha lived in the illiquid half."
  - [Freefincal](https://freefincal.com/is-indian-momentum-investing-just-a-liquidity-illusion/)
- Wright Research (2025) argues momentum is "working exactly as designed" but is in a severe, not unprecedented, drawdown phase. This came as small- and midcap indices fell about 18-22% from their peaks in early 2025. — [Wright Research blog](https://www.wrightresearch.in/blog/momentum-strategies-underperforming-2025-data-insights)

**India: time-series trend on the index**
- 80 trend-following rules on the Nifty, 2005-2012 (long-or-cash, no leverage, *no transaction costs*): they worked well in sharply falling markets but "far less so, or not at all well" in gradually rising, mixed or trendless markets. — [Indian Journal of Finance](https://www.indianjournalofentrepreneurship.com/index.php/IJF/article/view/71980)

### Inferences
- For a swing trader (2 days to 6 weeks), the evidence supports momentum as a **universe filter**: trade only stocks with strong 6-12 month relative strength or near 52-week highs. It does not support ranking on 1-4 week returns, where Indian and US equity evidence shows short-term *reversal*.
- Indian momentum's documented premium is concentrated in less-liquid names. Those names carry the largest slippage, circuit-limit and surveillance (ASM/GSM) risk (see Q6). The gap between paper and executable returns is therefore probably larger in India than in the US.
- Crash risk clusters after sharp market falls followed by rebounds (2000, 2009, 2020 in India; a 2024-25 rotation). Market-regime filters (index above its 200-day average, volatility scaling) have the strongest theoretical support (Daniel-Moskowitz) for cutting momentum crashes.
- Scaling Lesmond-style US costs (≥1.5% round trip in 1980-98) to India in 2026: explicit delivery costs are about 0.22-0.30% round trip plus impact (see Q6). Momentum at 1-month or longer holds survives costs in large/midcaps, but monthly turnover in small, illiquid names can consume much of the premium.

### Gaps
- The full IIMA monthly WML series after 2014, including the 2020 and 2024-25 drawdown depths and post-2014 average return, could not be retrieved (site blocked).
- No verified index-level CAGR or 2008 drawdown was found for the Nifty Midcap150 Momentum 50 since its 2005 base date.
- No peer-reviewed Indian study was found that tests momentum at *weekly* formation and holding periods net of current (post-2024) costs.
- No India-specific academic test of George-Hwang 52-week-high momentum with results was retrieved.

---

## 2. Breakout and trend-template swing methods (Minervini Trend Template/VCP, O'Neil CANSLIM, Darvas box, Donchian/Turtle): rigorous tests

### Takeaway
No peer-reviewed, out-of-sample, net-of-cost test of Minervini's Trend Template/VCP or of Darvas boxes was found. CANSLIM has only small, short-sample tests in minor journals. The closest rigorous evidence is the systematic "buy all-time/52-week highs with a wide trailing stop" literature (Wilcox & Crittenden, Concretum 2025). It shows positive expectancy driven by a very small fraction of huge winners, and holding periods that are mostly *longer* than swing horizons. Broad studies of technical rules on indices find that apparent profits vanish after data-snooping correction and costs.

### Cited Findings
- **Wilcox & Crittenden, "Does Trend Following Work on Stocks?" (Blackstar Funds):** buy stocks at all-time highs and exit on a volatility (true-range) trailing stop, roughly a 20% decline. Their tests show positive mathematical expectancy, with $1,000 growing to about $30,000 over the test period. — [UPenn-hosted PDF](https://www.cis.upenn.edu/~mkearns/finread/trend.pdf); [Daily Speculations](https://dailyspeculations.com/vic/trend.html)
- **Concretum update, Zarattini, Pagani & Wilcox, "Does Trend Following Still Work on Stocks?" (2025):**
  - Data: survivorship-bias-free, all liquid US stocks, 1950-Nov 2024, more than 66,000 long-only trend trades.
  - Results: 15.19% CAGR (gross), 6.18% annualized alpha, max drawdown about -31.75% over 1991-2024. Stable out of sample over 2005-2024.
  - Trades averaged a 15.2% expected return and a **305-day hold**. Fewer than 7% of trades produced the majority of profits, which makes it hard for a small account to pick the right subset.
  - [Concretum Group](https://concretumgroup.com/does-trend-following-still-work-on-stocks/); [Harbourfront Quant](https://harbourfrontquant.substack.com/p/does-trend-following-still-work-on)
- **Bajgrowicz & Scaillet (2012, JFE), "Technical trading revisited":** on daily Dow Jones prices, after controlling false discoveries (FDR), investors could not have picked the best rules in advance, and performance "fully disappears once transaction costs are taken into account". — [RePEc](https://ideas.repec.org/a/eee/jfinec/v106y2012i3p473-491.html); [EFMA PDF](https://www.efmaefm.org/0EFMAMEETINGS/EFMA%20ANNUAL%20MEETINGS/2008-Athens/papers/Bajgrowicz.pdf)
- **CANSLIM:** Lutey & Rayome (Journal of Accounting and Finance) paper-traded a live interpretation of O'Neil's 1988 rules from July 2014 to February 2017. It beat the S&P 500 by 20%, the Nasdaq by 9% and the Dow by 17% over that window. Lutey also published a "simplified CAN SLIM" model (Applied Finance Letters) claimed to beat the S&P 500 consistently. These are short samples, paper trades and small journals. — [JAF: Live out-of-sample testing of CAN SLIM](https://mail.articlegateway.com/index.php/JAF/article/view/5134); [Applied Finance Letters](https://ojs.aut.ac.nz/applied-finance-letters/article/view/632)
- **Minervini:** results come from his own record and contest performance. He reports a 155% return in the 1997 U.S. Investing Championship with $250,000 of his own money. No audited trade-level win rate or payoff statistics were found. TradingView implementations of the Trend Template/VCP are protected or unverified scripts with rough claims (e.g., "2-4 weeks" holds, "20-30% moves") and no rigorous backtests. — [Substack profile](https://caseystubbs.substack.com/p/how-mark-minervini-won-the-us-investing?open=false); [TradingView Trend Template script](https://in.tradingview.com/script/FBD7SZom-Mark-Minervini-Trend-Template-SEPA)
- **Darvas:** no academic test found. Practitioner comparisons describe Darvas boxes (3+ week boxes about 10% deep) as conceptually similar to O'Neil pivot breakouts ("3-weeks tight", flat bases). — [Trade That Swing](https://tradethatswing.com/the-technical-foundations-of-nicolas-darvass-trading-strategy/)
- **Turtle/Donchian rules** as commonly stated (System 1): enter long on a 20-day high breakout, exit on a 10-day low. System 2 uses 55-day entries and 20-day exits. Practitioner sources quote win rates of about 30-40% with average winners 3-5× average losers, and one backtest shows 32% winners. These are low-quality educational sources. — [TakeProfit (Turtle rules)](https://takeprofitapp.com/en/learn/turtle-trading-system); [StrategyQuant blog](https://strategyquant.com/blog/from-the-turtles-to-today-how-a-40-year-old-strategy-still-works/)
- **India index trend rules:** 80 trend rules on the Nifty, 2005-2012, with no costs, profited mainly in the sharply falling market. — [Indian Journal of Finance](https://www.indianjournalofentrepreneurship.com/index.php/IJF/article/view/71980)

### Inferences
- The Minervini trend template is commonly given as: price above the 150- and 200-day moving averages; 150-day above 200-day; 200-day rising for at least 1 month; 50-day above both; price at least 25-30% above its 52-week low and within about 25% of its 52-week high; high relative-strength rank. *(Rules from general knowledge of Minervini's 2013 book; not verified from a fetched source in this session.)* Functionally this is a combined 52-week-high plus momentum filter. Its stock-selection edge is therefore plausibly the George-Hwang and momentum premia (Q1), and the VCP/pivot timing layer is untested.
- Expect breakout systems to show low win rates (about 30-45%), payoffs of 2-5×, and long flat or drawdown stretches in range-bound markets. Concretum shows that cutting holds to 2-6 weeks with tight stops truncates exactly the fat right tail (under 7% of trades) that produces the profits. Swing breakout traders should keep some "runner" logic (a trailing stop) rather than fixed short targets.
- In India, breakouts in non-F&O small caps can hit upper circuits on entry and lower circuits on exit (Q6). Backtests that assume fills at breakout price overstate returns.

### Gaps
- No rigorous, survivorship-free, net-of-cost backtest was found of the Minervini Trend Template/VCP, Darvas boxes or CANSLIM on Indian stocks.
- No credible statistics were found on a "pure" 2-6 week breakout swing system (as opposed to the 300-day trend holds Concretum tested).

---

## 3. Short-term reversal and pullback-in-trend systems (Connors RSI(2), MA pullbacks, 1-week reversal) and evidence of decay

### Takeaway
The 1-week/1-month reversal effect is real in the cross-section but is mostly a small-cap, high-turnover phenomenon. Net of costs, it survives only in large caps or in "residual"/industry-adjusted forms (about 30-50 bps per week reported). The plain monthly version has largely disappeared since 1990 once January seasonality is removed. Connors-style RSI(2) and pullback systems were developed on 1995-2007 data without cost or data-mining controls. The one independent out-of-sample check found (CXO on a Connors product) showed falling win rates and underperformance against SPY after publication. Evidence for MA-pullback rules comes from vendors.

### Cited Findings
- Short-term reversal definition: stocks with low returns over the past week or month earn positive abnormal returns over the next week or month, and recent winners the reverse. — [Quantpedia: Short-term reversal in stocks](https://quantpedia.com/strategies/short-term-reversal-in-stocks)
- Net-of-cost survival: several studies find reversal abnormal returns disappear after trading costs, but this is driven by trading small caps. Restricting to large caps and using smarter construction cuts turnover, and reversal strategies then earn about **30-50 bps per week net of trading costs**. — [EFMA 2011 paper (trading costs and reversal profits)](https://efmaefm.org/0EFMAMEETINGS/EFMA%20ANNUAL%20MEETINGS/2011-Braga/papers/0259_update.pdf); [Quantpedia](https://quantpedia.com/Screener/Details/13)
- The monthly reversal effect is strongly seasonal (January). After adjusting for seasonality it "largely disappears since 1990". Residual reversal, by contrast, remains significant net of costs in large caps after 1990. — [CXO Advisory](https://www.cxoadvisory.com/?p=15847); [EFMA 2012 paper](https://www.efmaefm.org/0EFMSYMPOSIUM/2012/papers/017_update.pdf)
- Robeco (Blitz, van der Grient, Honarvar, Oct 2023) responds to "growing concerns" about short-term reversal. Neutralizing it against short-term industry and factor momentum roughly doubles risk-adjusted returns, and the enhanced version stays effective over time. — [Robeco](https://www.robeco.com/en-uk/insights/2023/10/reversing-the-trend-of-short-term-reversal)
- India: 1-month/1-month reversals appear for all winner-loser combinations (NSE, 1995-2008). Large, liquid Indian stocks reverse after large monthly price moves for up to 6 months. — [Publishing India IJFM](https://www.publishingindia.com/archive/ijfm/a-study-of-contrarian-and-momentum-profits-in-indian-stock-market); [CEEOL article](https://ceeol.com/search/article-detail?id=1054639)
- Connors' "Short Term Trading Strategies That Work" (2008, with Cesar Alvarez): RSI(2), pullback and VIX rules tested on 1995-2007 stock and S&P 500 data. CXO notes the tests do not correct for data-mining bias, do not test out of sample, mostly ignore return variability and drawdowns, and lack subperiod tests. — [CXO Advisory: notes on Short-Term Trading Strategies That Work](https://cxoadvisory.com/technical-trading/a-few-notes-on-short-term-trading-strategies-that-work)
- CXO's live-period review of Connors' "Daily Battle Plan" (Oct 2008-Apr 2011, 3-position limit, 0.67% friction per trade, 128 trades): terminal value about $25,614 against about $42,000 for SPY. The gross win rate fell from 62% to 55% in the later period. — [CXO Advisory: Review of Connors' Daily Battle Plan](https://www.cxoadvisory.com/individual-gurus/review-of-larry-connors-daily-battle-plan/)
- A 2-period RSI practitioner retest is titled "a simple system that still earns its keep". No statistics were retrievable. — [Backtest Substack](https://backtest.substack.com/p/the-2-period-rsi-a-simple-system)
- MA pullbacks (vendor evidence only):
  - Schaeffer's reports that stocks pulling back to the 200-day MA averaged +1.68% over the next month. The "percent positive" after 50-day MA pullbacks was the best of the signals it tested. No costs or statistical tests were reported.
  - Morpheus Trading cites a system buying breakout stocks on pullbacks to the 50-day MA with a "26.8% average gain" (anecdotal).
  - [Schaeffer's Research](https://www.schaeffersresearch.com/content/analysis/2018/04/11/2-of-the-hottest-stock-buy-signals); [Morpheus Trading](https://morpheustrading.com/blog/buy-pullback-50-ma/)

### Inferences
- Commonly cited RSI(2) rules (Connors): long only when the close is above the 200-day MA; buy when 2-period RSI < 5 (or < 10); exit on a close above the 5-day MA. Typical published hold is 3-6 days. *(Rules from general knowledge of the 2008 book, not verified via fetched source.)* With about 0.24-0.30% Indian delivery round-trip costs plus slippage, an average trade edge under about 0.5% would be largely eaten. Mean-reversion swing trading therefore needs liquid large caps or index futures in India.
- Mean-reversion systems usually have high win rates (55-70%) but small payoffs (below 1). Their risk is a left tail from catching falling knives during regime shifts, which is why the 200-day trend filter matters.
- A plausible synthesis for swing traders is a pullback in an uptrend: momentum or trend for selection (Q1), short-term reversal for entry timing. Academic support exists for each leg separately (3-12 month momentum, 1-week residual reversal). No rigorous published test of the combined rule net of Indian costs was found.

### Gaps
- No independent, published post-2008 out-of-sample test of RSI(2) with drawdown and cost statistics was retrievable.
- No academic evidence was found for "pullback to 20/50-DMA in uptrend" rules beyond vendor statistics.
- No India-specific weekly reversal study net of post-2024 costs was found.

---

## 4. Event-driven swing: PEAD (India and US), earnings gaps and results season, delivery-volume signals in India

### Takeaway
In the US, PEAD has been a classic 60-day swing anomaly. A prominent 2022 paper (Martineau) finds it disappeared for non-microcap stocks by about 2006, and 2025 papers dispute this, with the disagreement attributed to research design. Indian evidence is older and mixed. Earlier studies found drift lasting into the third month after an earnings surprise, while a 2014-2018 NSE study of 100 firms found surprises properly priced, and another found drift only after bad news. Indian "delivery percentage" signals are widely promoted by brokers and screeners, but no peer-reviewed evidence was found that they predict returns net of costs.

### Cited Findings
- Classic PEAD: after good (bad) earnings news, abnormal returns drift up (down) for at least 60 days. — [Wikipedia: PEAD](https://en.wikipedia.org/wiki/Post%E2%80%93earnings-announcement_drift); [Quantpedia: Post-Earnings Announcement Effect](https://quantpedia.com/Screener/Details/33)
- Martineau, "Rest in Peace Post-Earnings Announcement Drift" (Critical Finance Review, 2022): prices now fully reflect earnings surprises on announcement day. PEAD has not existed for large stocks since about 2006 and only recently vanished for microcaps. He attributes this to decimalization, electronic arbitrage and 2005 market-structure changes that sped up HFT. — [CFR PDF](https://cfr.ivo-welch.info/published/papers/martineau2021rest.pdf); [UCLA Anderson Review](https://anderson-review.ucla.edu/is-post-earnings-announcement-drift-a-thing-again)
- Two papers accepted in 2025 claim PEAD is "alive and well". Subrahmanyam (UCLA) argues the disagreement with Martineau largely reflects research-design choices. — [UCLA Anderson Review](https://anderson-review.ucla.edu/is-post-earnings-announcement-drift-a-thing-again/)
- India (older evidence): "significant price drifts" into the third month after announcements when there is an earnings surprise. Bucketing on absolute SUE explained drift over 2.5 months with R² = 0.86 and sensitivity 0.63. The sample period was not given in the summary. — [Publishing India PDF](https://www.publishingindia.com/storage/PDFBrochures/3691.pdf); [IIMB repository](https://repository.iimb.ac.in/handle/2074/20501)
- Gupta & Dhusia, 100 NSE firms, 2014-2018, 1,130 observations, earnings response coefficients: abnormal returns were *negatively* associated with earnings surprises, and over- or undervalued stocks were "properly priced" after announcements. No exploitable PEAD was found. — [Trends Economics & Management (VUT)](https://journals.vutbr.cz/index.php/trends/article/view/541)
- An Indian event study (Nottingham) found the market efficient on good earnings news but showed negative drift of more than a week after bad news. — [Nottingham ePrints](https://eprints.nottingham.ac.uk/26569)
- Indian disclosure timing: under SEBI LODR, financial results must reach the exchanges within 30 minutes of the board meeting's conclusion. Results can therefore arrive during market hours, not only after the close. SEBI has proposed tying the clock to the end of the agenda item. — [StudyCafe: LODR FAQs](https://studycafe.in/faqs-on-sebi-lodr-amendments-dated-05th-may-2021-102028.html); [Equities India glossary](https://equitiesindia.com/glossary/material-event-disclosure-lodr)
- Delivery volume:
  - Delivery % = delivery quantity ÷ traded quantity, a uniquely Indian data field. Brokers and screeners promote "high volume + high delivery = sustainable move" with no tests.
  - The only study found (a ResearchGate paper on Bajaj group stocks) reports that the delivery-to-traded ratio "causes" returns only in certain stocks.
  - [Tradejini blog](https://www.tradejini.com/blogs/delivery-volume-in-the-cash-market-a-key-indicator-for-investors); [ResearchGate study](https://www.researchgate.net/publication/377565241_A_Comparative_Study_on_Relationship_between_Delivery_Quantity_to_Total_Quantity_traded_Ratio_and_Stock_returns_in_Bajaj); [share.market](https://www.share.market/buzz/learn/stock-volume-technical-analysis/)
- An older NSE research paper on volume-return links found a positive contemporaneous relation between volume and returns/volatility, but volume depended on the direction of price change in only 60% of sample stocks. — [NSE research paper](https://nsearchives.nseindia.com/content/research/res_paper_final226.pdf)

### Inferences
- Indian results-season swing trading faces intraday disclosure timing, so the "gap" can happen mid-session. Weak, mixed Indian PEAD evidence adds to this. Together they suggest treating results as a *risk event* (cut size or exit before results) rather than a reliable drift edge, unless the trader has their own validated SUE-based backtest on recent data.
- Delivery % is plausibly informative about speculative intraday churn versus position-taking. Without evidence it should be used only as a secondary filter, not as an alpha source.

### Gaps
- No post-2018 Indian PEAD study with SUE-sorted drift returns net of costs was retrievable.
- No statistics were found on earnings-day gap sizes or gap-continuation and gap-fade rates for NSE stocks.
- The titles and authors of the 2025 papers reviving US PEAD were not retrieved.
- No rigorous study of delivery % as a return predictor in NSE stocks was found.

---

## 5. Crypto swing: time-series momentum/trend-following on BTC/ETH and altcoins, cross-sectional momentum, drawdown profile

### Takeaway
Crypto shows strong *time-series* momentum at daily and weekly horizons (Liu & Tsyvinski, RFS 2021) and a 1-4 week cross-sectional momentum factor (Liu, Tsyvinski & Wu, JF 2022, about 2.5-4.1% per week gross on 2014-2018 data). Net of costs, cross-sectional momentum is fragile. Costs eat much of it, alpha concentrates in shorts and illiquid coins, crashes are severe, and a single coin can drive results. Trend-following ensembles on liquid coins (Donchian ensembles with volatility sizing) report net Sharpe ratios above 1.5 on 2015-2025 data. Their main value is avoiding the 70-80% bear-market drawdowns typical of buy-and-hold crypto.

### Cited Findings
- Liu & Tsyvinski, "Risks and Returns of Cryptocurrency" (NBER w24877, RFS 2021) on Bitcoin, Ripple and Ethereum:
  - Crypto returns have no exposure to most stock-market, macro, currency and commodity factors.
  - There is strong TSMOM at daily and weekly frequencies.
  - Investor attention (Google searches) forecasts returns 1 to several weeks ahead: next-week BTC return was 11.2% for the top quintile of last week's search interest against 1.1% for the bottom quintile.
  - A search summary also attributes "11.2% vs 2.6%, Sharpe 0.45 vs 0.19" to *return*-sorted quintiles. This looks conflated with the attention result; verify before use.
  - [SSRN](https://papers.ssrn.com/abstract=3226806); [NBER PDF](https://www.nber.org/system/files/working_papers/w24877/w24877.pdf); [RePEc RFS](https://ideas.repec.org/a/oup/rfinst/v34y2021i6p2689-2727..html)
- Liu, Tsyvinski & Wu, "Common Risk Factors in Cryptocurrency" (JF 2022):
  - Sample: 1,707 coins, Jan 2014-Dec 2018, weekly quintile long-short sorts.
  - Nine factors earn significant weekly excess returns of 2.5-4.1%. Momentum long-short earned 2.7%, 3.3%, 4.1% and 2.5% per week for 1-, 2-, 3- and 4-week lookbacks (gross).
  - Market, size and 3-week momentum factors capture the cross-section.
  - [SSRN](https://papers.ssrn.com/abstract=3379131); [CXO Advisory](https://cxoadvisory.com/size-effect/cryptocurrency-factor-model); [Alpha Architect](https://alphaarchitect.com/2022/06/factors-investing-in-cryptocurrency)
- Costs and robustness:
  - "Momentum and liquidity in cryptocurrencies": momentum prevails in larger coins but incurs substantial trading costs and draws alpha largely from short positions. — [ar5iv 1904.00890](https://ar5iv.arxiv.org/html/1904.00890)
  - Once transaction costs and daily price moves are accounted for, many momentum portfolios liquidate and many statistically significant ones become insignificant. TSMOM evidence is strong while cross-sectional evidence is weak. Daily factor returns are much stronger than weekly, and monthly are insignificant. — [AUT ACFR paper](https://acfr.aut.ac.nz/__data/assets/pdf_file/0009/918729/Time_Series_and_Cross_Sectional_Momentum_in_the_Cryptocurrency_Market_with_IA.pdf); [arXiv 1903.06033](https://www.arxiv.org/pdf/1903.06033)
- Grobys, Kolari, Sandretto, Shahzad & Äijö, "Cryptocurrency momentum has (not) its moments" (Financial Markets and Portfolio Management, 2025): in large-cap coins, momentum suffers severe crashes, and even a single coin can make portfolio returns insignificant. Volatility management helps mitigate crashes. — [Univ. of Vaasa repository](https://osuva.uwasa.fi/handle/10024/20018)
- Factor momentum across more than 3,900 coins (2014-2022): past-winner crypto factors keep outperforming, with magnitude similar to equities. A search summary also states that in a split-sample test only the bid-ask spread factor survived both halves for value-weighted portfolios. Source attribution for that claim is uncertain between the listed papers. — [Cryptocurrency Factor Momentum (ICM)](https://open.icm.edu.pl/handle/123456789/25542); [Quantitative Finance 2023 (RePEc)](https://ideas.repec.org/a/taf/quantf/v23y2023i12p1853-1869.html)
- Zarattini, Pagani & Barbon, "Catching Crypto Trends" (Swiss Finance Institute RP 25-80, revised Apr 2025):
  - Method: an ensemble of Donchian-channel trend models across lookbacks, with volatility-based sizing, on survivorship-bias-free data for all coins since 2015.
  - A rotational top-20-liquid-coin portfolio earned a net-of-fees Sharpe above 1.5 and 10.8% annual alpha against BTC.
  - The paper models transaction costs and proposes a cost-mitigation technique.
  - [RePEc](https://ideas.repec.org/p/chf/rpseri/rp2580.html); [Concretum](https://concretumgroup.com/catching-crypto-trends-a-tactical-approach-for-bitcoin-and-altcoins/); [CXO Advisory](https://www.cxoadvisory.com/technical-trading/crypto-asset-trend-following-strategies/)
- Rozario, Holt, West & Ng, "A Decade of Evidence of Trend Following Investing in Cryptocurrencies" (arXiv 2009.12155, 2020): reports 255% walk-forward annualized returns over BTC's early decade. Sharpe ratios ranged roughly 0.5-1.5 depending on the averaging method. Risk-adjusted returns were similar to commodities, with strong bear-market diversification against equities. — [arXiv](https://arxiv.org/abs/2009.12155); [ar5iv](https://ar5iv.labs.arxiv.org/html/2009.12155)
- A University of Genoa study of BTC (Jan 2012-Aug 2019) found simple moving averages performed best on daily data. — [IRIS UniGe](https://iris.unige.it/handle/11567/1034776)
- "AdaptiveTrend" (arXiv 2602.11708, 2026 preprint) claims a Sharpe of 2.41 and max drawdown of -12.7% across 150+ pairs over 2022-2024 (36 months). It is unreviewed and covers a short window, so overfitting risk is high. — [arXiv](https://arxiv.org/abs/2602.11708v1)
- Perpetual futures carry: funding keeps perp prices near spot, and longs pay shorts when funding is positive (studied on Jan 2020-Dec 2022 data). A Sept 2026 snapshot showed average annualized BTC funding of about +0.8%, ranging from -4.8% (Kraken) to +10.9% (Hyperliquid). — [arXiv 2212.06888 "Fundamentals of Perpetual Futures"](https://arxiv.org/pdf/2212.06888); [DefiRate snapshot](https://defirate.com/perps/snapshot/2026-09-25T1643/)

### Inferences
- Swing-horizon crypto evidence favours **time-series trend on liquid coins** (BTC, ETH, top-20) over cross-sectional altcoin rotation. Cross-sectional profits depend on shorting illiquid altcoins, which Indian residents generally cannot do cheaply or legally on domestic venues.
- Typical crypto trend systems have low win rates and large payoffs, like Turtle systems. Their main benefit is drawdown truncation: bitcoin buy-and-hold has had several 70-80% drawdowns, a historical fact not sourced in this session.
- The headline 2.5-4% weekly cross-sectional returns come from 2014-2018, a small, illiquid market, and are gross. They should not be extrapolated to 2026.

### Gaps
- Exact BTC-only trend-following CAGR and maximum drawdown figures from Zarattini et al. could not be retrieved.
- No study was found that measures crypto swing-strategy returns *after Indian taxes* (30% with no loss offset, 1% TDS).
- Historical average perp funding in bull markets (often reported anecdotally at 10-30% annualized) was not verified.

---

## 6. Costs and taxes for swing trading in India (2026) and overnight-holding risks; crypto 30% tax and 1% TDS impact

### Takeaway
Delivery-based swing trades in Indian equities cost roughly 0.22-0.30% round trip in explicit charges at a zero-brokerage broker; STT of 0.1% on each side dominates. Slippage comes on top, and net gains are taxed at 20% STCG (since 23 July 2024; unchanged in Budget 2026). Budget 2026 raised STT on futures from 0.02% to 0.05% and on options premiums from 0.10% to 0.15%, effective 1 April 2026, which makes index-futures swing trading costlier. Overnight risks unique to India include stock-specific price bands (2/5/10/20%) that can freeze exits, surveillance frameworks (ASM/GSM: 100% margin, trade-to-trade), results disclosed mid-session, and index-rebalance flows. Crypto's 30% flat tax without loss offset and 1% TDS per sale heavily penalize frequent swing trading.

### Cited Findings
**Indian equity tax and STT**
- STCG on listed equity (STT paid) rose from 15% to **20%** for transfers on or after **23 July 2024**. LTCG under Sec 112A rose to 12.5%, and the exemption rose from ₹1 lakh to ₹1.25 lakh from FY2024-25. — [Quicko](https://learn.quicko.com/ltcg-tax-equity-shares); [m.Stock](https://www.mstock.com/articles/tax-on-equity-investments-india)
- Union Budget 2026-27 kept STCG on listed equity and equity MFs at 20%. — [Bajaj Finserv](https://www.bajajfinserv.in/investments/understanding-short-term-capital-gains-tax.html); [Fyers Budget 2026 highlights](https://fyers.in/blog/union-budget-2026-highlights/)
- Budget 2026 STT changes, effective 1 April 2026:
  - Futures: 0.02% → **0.05%**.
  - Options premium: 0.10% → **0.15%**.
  - Options exercise: 0.125% → 0.15%.
  - **Delivery equity: unchanged at 0.1% on buy and 0.1% on sell.**
  - [5paisa](https://www.5paisa.com/news/stt-hike-on-fo-to-take-effect-from-april-1-amid-rising-options-activity); [Outlook Money](https://www.outlookmoney.com/amp/story/invest/stt-hike-from-april-1-2026-budget-what-it-means-for-futures-and-options-traders); [Groww](https://groww.in/blog/what-is-stt); [EY Budget 2026 FS highlights](https://www.ey.com/content/dam/ey-unified-site/ey-com/en-in/services/tax/union-budget-2026/ey-union-budget-2026-financial-services-highlights.pdf)
- Budget 2026 also taxes buyback proceeds as capital gains. — [Outlook Money](https://www.outlookmoney.com/invest/equity/union-budget-2026-buyback-proceeds-to-be-taxed-as-capital-gains-stt-raised-on-futures-and-options)

**Other charges (Zerodha schedule, 2026)**
- ₹0 brokerage on delivery.
- DP charge of ₹13.50 + 18% GST per ISIN per day of sale (CDSL), regardless of quantity.
- Stamp duty 0.015% on the buy side (delivery).
- NSE transaction charge 0.00297% per side (BSE 0.00375%).
- SEBI fee 0.0001% (₹10/crore).
- GST 18% on brokerage + exchange charges (not on STT or stamp duty).
- [StockCalc Zerodha 2026](https://stockcalc.in/blog/zerodha-brokerage-charges-2026-full-breakdown); [Zerodha charges](https://www.zerodha.com/charges)

**Overnight and structural risks (India)**
- Price bands:
  - Stock-specific circuit filters of 2%, 5%, 10% or 20% apply on NSE and BSE, reset daily from the previous close.
  - F&O stocks have no price band, only a 10% "dummy" operating range. Non-F&O stocks that belong to derivative-traded indices are still banded.
  - As of April 2023, 1,595 of 2,241 NSE stocks sat in the 20% band, 295 in 5%, 151 in 10%, 7 in 2%, and 192 had no band.
  - [Chittorgarh circuit-filter report](https://www.chittorgarh.com/report/stock-nse-circuit-filters/120/20/); [Upstox help](https://upstox.com/help-center/t-41535/); [IIFL knowledge centre](https://indiainfoline.com/knowledge-center/online-share-trading/what-are-circuit-filters-limits-and-how-are-they-used)
- Surveillance (ASM/GSM): stocks with abnormal price or volume moves or client concentration can move to ASM or GSM stages. These require 100% upfront margin, often trade-to-trade (T2T) settlement with no intraday netting, and restrictions that tighten through the stages. — [Zerodha support: ASM](https://support.zerodha.com/category/trading-and-markets/trading-faqs/articles/what-is-asm); [Groww: GSM](https://groww.in/blog/graded-surveillance-measure); [Bajaj Broking](https://www.bajajbroking.in/blog/decoding-asm-and-gsm-frameworks-and-stages)
- Results timing: results are disclosed within 30 minutes of the board meeting's conclusion, which can be during market hours. — [StudyCafe](https://studycafe.in/faqs-on-sebi-lodr-amendments-dated-05th-may-2021-102028.html)
- Index-rebalance flows: the Dec 2025 Nifty200 Momentum 30 reshuffle meant about ₹16,130 crore of round-trip flows, exceeding 1-day ADV in 27 stocks. This creates predictable price pressure around rebalance dates for stocks swing traders hold. — [Business Standard](https://www.business-standard.com/markets/news/nifty200-momentum30-a-flip-of-index-switch-jolts-16k-cr-across-the-grid-125121400388_1.html)

**Crypto tax (India)**
- Sec 115BBH: 30% flat tax plus 4% cess on VDA transfer gains, regardless of holding period or slab.
- Sec 194S: 1% TDS on transfers above ₹10,000 a year (₹50,000 for specified individuals/HUFs).
- VDA losses cannot be set off against any other income and cannot be carried forward.
- Budget 2026 left the 30% rate and 1% TDS unchanged. It added reporting penalties from April 2026: ₹200 per day for failing to furnish crypto transaction statements, and a ₹50,000 flat penalty for inaccurate information. These apply to reporting entities, per the summaries.
- [KoinX Schedule VDA guide](https://www.koinx.com/tax-guides/file-crypto-taxes-on-schedule-vda); [TokenTax India 2026](https://tokentax.co/blog/guide-to-crypto-taxes-in-india); [Patron Accounting (Income Tax Act 2025 rules)](https://www.patronaccounting.com/blog/crypto-vda-taxation-income-tax-act-2025-rules)
- Esya Centre study of the 2022 tax regime:
  - The 1% TDS was the most distortionary measure: Indian exchanges lost up to 81% of trading volume in July-October 2022.
  - About US$3.85 billion of volume moved to foreign exchanges in February-October 2022.
  - Projected loss of about US$1.2 trillion in domestic exchange volume over 4 years.
  - Recommendation: cut TDS to 0.1% and allow loss offsets.
  - [Forkast](https://forkast.news/headlines/indian-crypto-exchanges-lose-us1-2-trillion/); [FXStreet](https://www.fxstreet.com/cryptocurrencies/news/indians-moved-over-38b-to-foreign-exchanges-since-crypto-tax-rules-research-study-202301040953)

### Inferences
**Worked cost estimates (author's calculation from the Zerodha schedule above, delivery trade, NSE)**
- ₹1,00,000 round trip:
  - STT ₹200
  - Stamp duty ₹15
  - Exchange charges about ₹5.94
  - SEBI fee about ₹0.20
  - GST about ₹1.10
  - DP charge ₹15.93
  - **Total about ₹238, or about 0.24%**
- ₹25,000 round trip: about ₹71.5, or about 0.29%, because the DP charge is fixed.
- Add bid-ask spread and impact: perhaps 0.05-0.2% in Nifty 100 names and much more in small caps. These are assumptions, not sourced.
- A swing system averaging +2% per winning trade therefore loses about 12-15% of each winner's gross gain to explicit costs before the 20% STCG.

**STCG drag**
- At 20%, a strategy earning 25% a year pre-tax (all short-term) nets about 20%.
- For multi-month holds that cross 12 months, LTCG at 12.5% with the ₹1.25 lakh exemption is a meaningful advantage. Most swing trades never qualify.

**Index futures**
- At 0.05% STT on futures (applied to sell-side value under the existing structure, which was not re-verified), a Nifty futures round trip costs about 0.05-0.07% in statutory charges. That is still cheaper per notional than delivery equity, but margin and MTM apply, and the earlier F&O research covers the regulatory side.

**Crypto expectancy under Indian tax (illustration)**
- Example system: 40% win rate, average win +15%, average loss -5%, giving +3.0% expected value per trade pre-tax.
- If losses cannot offset gains (taxing each winner at about 31.2%): EV ≈ 0.4×15×0.688 − 0.6×5 ≈ **+1.13%**, a cut of more than 60%.
- With full offset, after-tax EV would be about 2.06%.
- Low-win-rate, high-payoff trend systems are hit hardest by the no-offset rule.

**1% TDS cash drag**
- TDS is a credit against final tax liability, so the cost is a cash-flow drag rather than a final cost.
- A trader who turns over capital 30-50 times a year would have 30-50% of capital tied up in TDS credits until filing or refund (author's calculation). This structurally discourages frequent swing trading on Indian VDA platforms.

### Gaps
- The exact 2026 NSE transaction-charge rate could not be verified from NSE's own circular. The 0.00297% figure comes only from a broker-calculator summary.
- Whether frequent swing trading is treated as business income or capital gains (CBDT guidance) and the rules for setting off and carrying forward short-term capital losses were not researched in this session.
- Section renumbering under the Income Tax Act 2025 (effective 1 April 2026) for STCG, LTCG and VDA provisions was only partly confirmed.
- No statistics were found on the frequency or size of overnight gaps in NSE stocks.

---

## 7. Typical realistic metrics of a disciplined swing system (win rate, payoff, drawdown)

### Takeaway
Credible evidence points to modest, lumpy edges. Trend and breakout systems typically win 30-45% of trades and depend on payoffs of 2-5× and a handful of outliers. Mean-reversion systems win 55-65% with payoffs below 1. Systematic momentum strategies in India have had peak-to-trough drawdowns of 25-35% in "normal" crashes and 60-75% in severe ones. The commonly quoted 35-55% win rate is consistent with the evidence, but extraordinary track records such as Minervini's 155% contest year are unaudited and should not be used as benchmarks.

### Cited Findings
- Trend/breakout (US stocks, Concretum 2025): CAGR 15.19% gross over 1991-2024 and max drawdown about 31.75%. Fewer than 7% of trades produced most of the profits, and the average trade returned 15.2% over about 305 days. — [Concretum](https://concretumgroup.com/does-trend-following-still-work-on-stocks/)
- Turtle-style Donchian systems: practitioner sources cite 30-40% win rates with average winners 3-5× average losers, and one backtest showed 32% winners. These are low-quality sources and show the order of magnitude only. — [TakeProfit](https://takeprofitapp.com/en/learn/turtle-trading-system); [StrategyQuant](https://strategyquant.com/blog/from-the-turtles-to-today-how-a-40-year-old-strategy-still-works/)
- Short-term mean reversion (Connors product, live 2008-2011): 62% gross win rate falling to 55%, and underperformance against SPY after 0.67% per-trade friction. — [CXO Advisory](https://www.cxoadvisory.com/individual-gurus/review-of-larry-connors-daily-battle-plan/)
- Indian momentum drawdowns:
  - Nifty200 Momentum 30: -67.9% (2008), -34.21% (2020), -28.39% (2021-22), -31.79% (Sep 2024-Apr 2025). — [HDFC MF PPT Jul 2026](https://files.hdfcfund.com/s3fs-public/Others/2026-07/HDFC%20NIFTY200%20Momentum%2030%20Index%20Fund_PPT%20%28July%202026%29.pdf)
  - IIMA WML: -62% (2000) and -52.8% (2008-09). — [IIMA](https://web.iima.ac.in/~iffm/Indian-Fama-French-Momentum/drawdown.php)
  - Annual-rebalanced momentum 30, net: max drawdown -70.61%. — [Freefincal](https://freefincal.com/is-indian-momentum-investing-just-a-liquidity-illusion/)
  - Naive monthly momentum: drawdowns above 50% every 3-4 years. — [Capitalmind](https://www.capitalmind.in/insights/momentum-investing-basics-india)
- Crypto trend ensembles: net Sharpe above 1.5 on the top-20-coin rotation (2015-2025). Earlier crypto trend studies report Sharpe ratios of about 0.5-1.5. — [RePEc SFI](https://ideas.repec.org/p/chf/rpseri/rp2580.html); [arXiv 2009.12155](https://arxiv.org/abs/2009.12155)
- Diversified multi-asset trend-following (the institutional benchmark): Sharpe about 0.4 after 2/20 fees over 1880-2016 (per CXO summary), against gross Sharpe ratios of 1.0-1.6 in in-sample academic portfolios. — [CXO](https://www.cxoadvisory.com/momentum-investing/trend-following-with-intrinsic-momentum-over-the-very-long-run); [Quantpedia](https://quantpedia.com/time-series-momentum-works-everywhere/)
- Minervini: 155% return in the 1997 U.S. Investing Championship, with average annual returns of 220% claimed in promotional material. No audited trade-level statistics were found. — [Substack](https://caseystubbs.substack.com/p/how-mark-minervini-won-the-us-investing?open=false)

### Inferences
**Realistic planning ranges for a disciplined, rules-based swing system (author's synthesis)**

| | Trend/breakout | Pullback/mean-reversion |
|---|---|---|
| Win rate | 35-45% | 55-65% |
| Average win / average loss | 2-3× | 0.7-1.2× |
| Expectancy per trade, net of Indian costs | about 0.5-1.5% | about 0.3-1.0% |

- Drawdowns: a long-only Indian equity swing portfolio should plan for a 20-35% system drawdown in an ordinary correction and 50%+ in a 2008-type crash without a regime filter.
- Sharpe ratios above about 1.0 net of costs and taxes on Indian single stocks should be treated as suspect unless out of sample.
- Gap between gross and net: academic gross Sharpe ratios of 1.0-1.6 shrink to about 0.4-0.8 after fees and costs in the long-run trend-following record. Swing traders should expect similar or larger haircuts, given 20% STCG and India's higher small-cap impact costs.

### Gaps
- No independent, audited distribution of retail swing-trader outcomes in Indian cash-delivery segments was found. SEBI's published loss studies cover F&O and intraday, which another researcher covers.
- No credible source gives trade-level win rate and payoff statistics for CANSLIM, Minervini SEPA or VCP systems.
