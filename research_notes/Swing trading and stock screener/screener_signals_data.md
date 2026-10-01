# Stock and crypto screener for Indian equities (NSE) and crypto: signals, construction, data and regulation (as of Oct 2026)

> **How these notes were made.** The network egress policy blocked WebFetch for almost every primary domain: SSRN, NBER, arXiv, sebi.gov.in, niftyindices.com, Springer, ResearchGate, AlphaArchitect, Quantpedia, Zerodha, CXO Advisory, jrvarma.in, Wharton and substack. Only github.com could be fetched. So most findings below come from **search-engine summaries of the cited pages, not from reading the full text**. Where a number could come from more than one of the listed results, the note says so. Treat exact figures as "verify before quoting in final copy". The report writer should not upgrade any of these to "confirmed from primary text".

---

## Q1. Which features have robust evidence for predicting 1-day to 1-month returns (global, India, crypto)?

### Takeaway
The strongest evidence for short-horizon cross-sectional prediction is for:
- **Momentum / relative strength**, with 52-week-high proximity as a more stable variant in India.
- **Liquidity and volatility** characteristics.
- **Short-term reversal**, mainly in small and illiquid names.

In India, momentum (with the 52-week-high variant) is the best-documented factor and has beaten quality over 21 years. It also suffers severe crashes, for example −31.8% vs −15.4% for the Nifty 50 between Sept 2024 and Apr 2025.

The volume-surge premium is weak in India. Earnings-drift evidence is mixed. Low-volatility mainly pays off over long horizons, not 1–30 days.

In crypto, the market, size and momentum factors (1–4 week) explain the cross-section. Large, liquid coins show short-term momentum, while small, illiquid coins show reversal.

### Cited Findings

**Global / ML-identified dominant signals**
- Gu, Kelly & Xiu (RFS 2020) compared many ML methods. All of them agree on the same dominant predictive signals: "variations on momentum, liquidity, and volatility". Trees and neural networks perform best because they capture nonlinear interactions between predictors. — [SSRN: Empirical Asset Pricing via Machine Learning](https://papers.ssrn.com/abstract=3159577)
- Momentum crashes are partly forecastable. They happen in "panic" states, after market declines and when market volatility is high, and they coincide with market rebounds. A dynamic momentum strategy that scales exposure using forecasts of momentum's mean and variance roughly **doubles the alpha and Sharpe ratio** of static momentum. The result holds across periods, international equity markets and asset classes. Published in JFE 2016. — [NBER w20439: Momentum Crashes (Daniel & Moskowitz)](https://www.nber.org/papers/w20439)

**52-week-high proximity (India)**
- Rajan Raju, "The 52-Week High Effect and Momentum Investing: Evidence from India", uses data from Oct 2004 to Aug 2023:
  - The 52-week-high effect is "distinct and robust" in India. Stocks near their 52-week high earn higher returns and Sharpe ratios after controlling for size.
  - It gives a **more stable alpha than academic (12-1) momentum** and has **weaker long-term reversals**.
  - Indicative backtested return is **20.39% with 13.68% volatility** over 2004–2023.
  - [Quantpedia summary](https://quantpedia.com/an-analysis-of-52-weeks-high-effect-on-indian-stocks/). The paper is also listed on [arXiv 2302.13245](https://www.arxiv.org/pdf/2302.13245); not fetched, so attribution is assumed.

**Momentum / relative strength (India)**
- **Nifty200 Momentum 30** (NSE index) methodology:
  - Selects the top 30 Nifty 200 stocks by a "Normalised Momentum Score". The score is built from **6-month and 12-month price returns adjusted for daily return volatility**.
  - Weight = free-float market cap × score, capped at the lower of 5% or 5× the stock's market-cap weight.
  - Rebalanced **semi-annually (June/Dec)**. Base date is 1 Apr 2005, but the index launched only **25 Aug 2020**, so earlier history is backtested.
  - Sources: [Axis Max Life explainer](https://www.axismaxlife.com/blog/investments/what-is-nifty-200-momentum-30-index); [NSE Indices whitepaper (Sep 2020)](https://www.niftyindices.com/docs/default-source/indices/nifty200-momentum-30-index/nifty200_momentum_30_index_whitepaper_sep_20.pdf?sfvrsn=3eb99734_4)
- Over Apr 2005 to Apr 2026, Nifty200 Momentum 30 returned a **17.6% CAGR vs 15.3% for Nifty200 Quality 30**, an edge of 230 bps. Over Apr 2023 to Apr 2026, momentum's edge was 570 bps. Note that the pre-2020 history is backtested. — [Wright Research Momentum–Quality note, Apr 2026](https://www.wrightresearch.in/media/pms/newsletter/wright_momentum_quality_note_april_26.pdf)
- Crash risk:
  - From **27 Sep 2024 to 7 Apr 2025**, the Nifty 50 fell **15.35%** while the **Nifty200 Momentum 30 fell 31.79%**. — [DSIJ: Active Momentum Funds](https://insights.dsij.in/dsijarticledetail/active-momentum-funds-timing-the-market-or-tapping-the-trend-51070). This comes from a search summary and could not be pinned to a single page.
  - Over the same period the Midcap150 Momentum 50 fell about 24% and "simple momentum strategies" fell about 30%. — [StockViz: Oh Momentum](https://stockviz.substack.com/p/oh-momentum) (search summary)
- Naive monthly-rebalanced momentum portfolios in India have had **drawdowns above 50% every 3–4 years**. — [StockViz: Momentum without the Crash](https://stockviz.substack.com/p/momentum-without-the-crash) (search summary)
- Momentum strategies crashed in 2001, 2009 (a drawdown of more than 70%) and 2023. — [Wright Research: momentum funds in bull/bear markets](https://www.wrightresearch.in/blog/momentum-mutual-funds-in-bull-bear-markets/) (search summary; attribution among the listed results is uncertain)
- Wright Research described momentum as "stalled" and "significantly underperforming" into 2025, while saying it is "working exactly as designed" in a painful but not unprecedented phase. — [Wright Research blog, 2025](https://www.wrightresearch.in/blog/momentum-strategies-underperforming-2025-data-insights)
- **IIMA factor data library** (Agarwalla, Jacob & Varma, IIM Ahmedabad):
  - Provides Indian market, size (SMB), value (HML) and **momentum (WML)** factor returns built from CMIE Prowess.
  - Excludes illiquid firms so portfolios are investable, and **corrects for survivorship ("vanishing companies")**.
  - New releases use Prowess DX and come three times a year (Mar, Sep, Dec). They cover both BSE and NSE firms. Legacy releases, which ended in 2019, used BSE data only.
  - Sources: [IIMA library](https://faculty.iima.ac.in/~iffm/Indian-Fama-French-Momentum/); [IIMA legacy page](https://faculty.iima.ac.in/iffm/legacy/); [working paper 2013-09-05](https://www.iima.ac.in/publication/four-factor-model-indian-equities-market)

**Short-term reversal and horizon dependence (India)**
- A thesis on Indian market anomalies found that at **1-month holding periods only 5 of 12 anomalies predict returns**, including momentum and reversal existing side by side. At longer holding periods idiosyncratic volatility becomes significant. — [Goel, Univ. of Nottingham thesis](https://eprints.nottingham.ac.uk/66413)

**Low volatility (India)**
- The volatility effect exists in India. Annualized excess returns were **11.40% for the low-volatility decile vs 1.30% for the high-volatility decile** over 2001 to June 2015. — Search summary; most likely [Applied Finance Letters (AUT)](https://ojs.aut.ac.nz/applied-finance-letters/article/download/32/31), but attribution among the results is uncertain.
- On NIFTY 500 stocks, the volatility anomaly is "predominant in medium to long term (5 to 10 years)" and "negligible for ultra-short and short time frames (6 months to 3 years)". — Search summary of [IIMB Management Review, June 2023](https://iimb.ac.in/imr/previous-issues/volatility-june-2023.php) / [NMIMS Management Review](https://management-review.nmims.edu/?p=275); attribution uncertain.

**Abnormal volume / high-volume return premium**
- Gervais, Kaniel & Mingelgrin (JF 2001): stocks with **unusually high (low) volume over a day or a week tend to rise (fall) over the following month**. Their explanation is visibility: a shock to trading activity raises later demand for the stock. — [Wharton Rodney White Center WP 9901](https://rodneywhitecenter.wharton.upenn.edu/wp-content/uploads/2014/04/9901.pdf)
- Cross-country evidence (Kaniel, Ozoguz & Starks): the premium is "persistent… in almost all developed equity markets and in emerging equity markets as well". — [SUFE academic newsletter summary](https://academicnewsletter.sufe.edu.cn/info/356889)
- **India (Singh, Wang & Hua, Pacific-Basin Finance Journal 2025)**: a small premium exists, but it is **weaker than in the US**. The value-weighted premium is significant only at the 10% level and the equal-weighted premium at 5%. — [IDEAS/RePEc](https://ideas.repec.org/a/eee/pacfin/v91y2025ics0927538x2500126x.html)
- In India, volume and delivery percentage rise sharply before **bulk deals** and fall after the event day, with large cumulative returns around these trades. — [NDL: Bulk Deals, Abnormal Returns and Front-running](https://rcca.ndl.gov.in/items/4fbafa21-3191-40dd-8288-8fa6019189c0/full)

**Earnings surprise / PEAD (India)**
- 100 NSE firms over 2014–2018 (1,130 observations): the study found a *negative* association between abnormal returns and earnings surprise, and concluded that mispriced stocks are correctly priced after announcements. That means no exploitable drift. — [Trends Economics & Management (VUT Brno)](https://journals.vutbr.cz/index.php/trends/article/view/541)
- Another study found India relatively efficient to good earnings news, but saw a negative drift lasting **more than a week after bad news**. — Search summary; likely [Vision 2015 (RePEc)](https://ideas.repec.org/a/sae/vision/v19y2015i1p25-36.html), attribution uncertain.
- An event study over 120 days found post-earnings drift in Indian share prices. — [Nottingham eprint 26569](https://eprints.nottingham.ac.uk/26569)

**Cross-section of Indian stock returns (multi-characteristic)**
- Lalwani & Meshram (IIM Raipur, *Applied Economics* 2022) tested **35 characteristics on a survivorship-bias-free sample** of Indian listed firms from 1994 to 2019. Fama-MacBeth regressions showed **14 predictors with t-stats above 3**. — [Taylor & Francis](https://www.tandfonline.com/doi/abs/10.1080/00036846.2021.1982132)

**Crypto cross-sectional factors**
- Liu, Tsyvinski & Wu, "Common Risk Factors in Cryptocurrency" (JF 2022, vol. 77, pp. 1133–1177):
  - Three factors, **crypto market, size and momentum**, capture the cross-section of expected coin returns.
  - The sample is coins with market cap above $1M from **2014 to 2018**. They tested 25 candidate factors using weekly quintile sorts.
  - Sources: [SSRN 3379131](https://papers.ssrn.com/abstract=3379131); [Yale PDF](https://economics.yale.edu/sites/default/files/2022-10/LiuTsyvinskiWu2019%20COMMON%20RISK%20FACTORS.pdf)
  - A long-smallest / short-largest size strategy earns **more than 3% per week in excess returns**: 3.4% sorting on market cap, 3.9% on end-of-week price, 4.1% on the week's maximum price.
  - Significant long-short characteristics: market cap, price, max price, **1-, 2-, 3- and 4-week momentum**, dollar volume, and the standard deviation of dollar volume. All are explained by the three-factor model.
  - Sources: [Yale PDF](https://economics.yale.edu/sites/default/files/2022-10/LiuTsyvinskiWu2019%20COMMON%20RISK%20FACTORS.pdf); [Wiley JF](https://onlinelibrary.wiley.com/doi/abs/10.1111/jofi.13119)
- A secondary summary says the later version covers **1,827 coins over 2014–2020** with average payoffs of about **3% per week**. — Search summary near [Vilnius BATP paper](https://www.journals.vu.lt/BATP/lt/article/download/44540/42590/138419); attribution uncertain.
- **Size and liquidity change the sign of short-horizon effects** (Fičura & Colak 2023):
  - **Weekly reversal occurs only in small, illiquid coins.** Large, liquid coins show **weekly momentum**.
  - Distance from the 1-week high predicts returns *negatively* for small/illiquid coins and *positively* for large/liquid coins.
  - Source: [SSRN 4378429](https://papers.ssrn.com/sol3/papers.cfm?abstract_id=4378429)
- Daily data on more than 3,600 coins (2015–2021): coins with low previous-day returns outperform coins with high ones, which is **daily reversal**. However, "the handful of largest and most tradeable coins exhibit daily momentum". — [International Review of Financial Analysis: "Up or down? Short-term reversal, momentum, and liquidity effects in cryptocurrency markets"](https://www.sciencedirect.com/science/article/pii/S1057521921002349)
- Weekly data for the **top 30 coins over 2016–2023**: crypto momentum is **subject to severe crashes** in large-cap and equal-weighted portfolios. — [Financial Markets and Portfolio Management (Springer, 2025): "Cryptocurrency momentum has (not) its moments"](https://link.springer.com/article/10.1007/s11408-025-00474-9)

### Inferences
- **Swing horizon (1–4 weeks) for NSE.** Volatility-adjusted 6- and 12-month momentum plus 52-week-high proximity is the best-supported core score, which is also what NSE uses. Add short-term reversal as a secondary signal mainly for illiquid names. Volume surges deserve at most a small weight or should act only as a confirmation filter, because the Indian volume premium is weak (significant only at 10% value-weighted). Earnings-drift signals look unreliable in India; the evidence is small-sample and mixed.
- **Intraday horizon.** None of the factor evidence above is intraday. Intraday ranking (for example on opening volume or gaps) has no peer-reviewed India-specific support in what was found. Treat it as unproven.
- **Crypto.** Restrict to large, liquid coins and use 1–4 week momentum. Avoid reversal rules on large caps. The size premium is concentrated in micro-caps that may not be tradeable on India-accessible exchanges.
- **Crash risk.** Momentum is the dominant Indian signal and has crash risk (−31.8% vs −15.4% for the Nifty in 2024–25). That argues for volatility-scaling or a regime overlay (see Q2).

### Gaps
- Exact Gervais–Kaniel–Mingelgrin magnitudes (return spread in % at 20 and 50 days, sample 1963–1996) could not be read because the primary PDF was blocked.
- Exact per-horizon weekly returns for Liu–Tsyvinski–Wu momentum (for example 3-week momentum in % per week) were not available.
- Quality/profitability and **analyst-revision** evidence specific to India at 1–30 day horizons was not found. Quality appears mainly as a long-horizon index factor (Nifty200 Quality 30, 15.3% CAGR 2005–2026).
- No India-specific study was found on the short-term (1-week or 1-month) reversal magnitude after 2015.
- Momentum, low-volatility and quality factor factsheet numbers (niftyindices.com) could not be fetched.

---

## Q2. How should multi-factor scores be combined, which universe filters matter in India, and what does a regime filter add?

### Takeaway
Industry-standard composites winsorize each raw factor, convert it to a z-score within the universe, cap z at ±3 and average the z-scores. NSE's momentum index follows the same pattern with volatility-adjusted returns. Rank-based aggregation is a common alternative.

Indian universe filters should exclude:
- ASM/GSM/ESM surveillance stocks (100% margin, shrinking price bands, gross settlement)
- Stocks with tight circuit bands (2%/5%), where you can get stuck at the limit
- Illiquid names

F&O stocks have no fixed circuit, only a dynamic 10% band that flexes in 5% steps.

Regime filters such as trend or volatility conditioning are justified by the momentum-crash literature (Daniel–Moskowitz roughly doubles Sharpe). Evidence for an India-specific 200-DMA filter is practitioner-level only.

### Cited Findings

**Composite scoring**
- **MSCI factor index methodology**:
  - Winsorize raw variables, then compute **z-scores using the mean and standard deviation within the parent index**.
  - Winsorize z-scores at **±3**.
  - The composite equals the **average of descriptor z-scores**. A missing descriptor is set to the universe-average z-score.
  - Final score = 1 + Z if Z ≥ 0, and (1 − Z)^−1 if Z < 0.
  - Source: [MSCI Quality Indexes Methodology (May 2022)](https://www.msci.com/eqb/methodology/meth_docs/MSCI_Quality_Indexes_Methodology_May2022.pdf)
- **NSE's Normalised Momentum Score** blends 6- and 12-month volatility-adjusted returns into a normalized score, and tilts weights as market cap × score. — [Axis Max Life explainer](https://www.axismaxlife.com/blog/investments/what-is-nifty-200-momentum-30-index)

**Indian universe filters: surveillance lists**
- **ASM shortlisting** uses objective dynamic criteria: high-low variation, client concentration, close-to-close variation, market cap, volume variation, delivery %, number of unique PANs, and P/E. — [Bajaj Broking: ASM/GSM stages](https://bajajbroking.in/blog/decoding-asm-and-gsm-frameworks-and-stages); [NSE ASM FAQ (Sep 2025)](https://nsearchives.nseindia.com/web/sites/default/files/inline-files/FAQs%20-%20Additional%20Surveillance%20Measure%20%28ASM%29_5.9.25.pdf)
- **Long-term ASM stages**: Stage 1 carries a 100% margin from T+3. Stages 2 and 3 add 100% margin and step the price band down. Stage 4 adds gross settlement, 100% margin and a **5% price band**. — [Bajaj Broking](https://bajajbroking.in/blog/decoding-asm-and-gsm-frameworks-and-stages)
- **GSM criteria**:
  - Criterion I: net worth ≤ ₹10 cr AND net fixed assets ≤ ₹25 cr AND P/E above 2× the Nifty 500 P/E, or negative P/E.
  - Criterion II: full market cap below ₹25 cr AND P/E above 2× the Nifty 500 P/E.
  - Sources: [Bajaj Broking](https://bajajbroking.in/blog/decoding-asm-and-gsm-frameworks-and-stages); [NSE GSM page](https://nseindia.com/regulations/graded-surveillance-measure)
- NSE also runs an **Enhanced Surveillance Measure (ESM)** framework. — [NSE ESM FAQ (Oct 2024)](https://nsearchives.nseindia.com/web/sites/default/files/inline-files/FAQs%20-%20Enhanced%20Surveillance%20Measure%20(ESM)_4.10.24_NEWTEMP.pdf)

**Indian universe filters: price bands**
- NSE circuit-filter categories are **2%, 5%, 10%, 20% and none**, set daily from the previous close. — [Chittorgarh NSE circuit filters](https://www.chittorgarh.com/report/stock-nse-circuit-filters/120/2/)
- **F&O stocks have no fixed circuit.** They have a dummy dynamic operating range of 10% that "flexes" by 5% at a time when the last traded price nears the band. — [Rupeezy support](https://support.rupeezy.in/support/solutions/articles/21000004845-what-are-circuit-limits-or-price-bands) (search summary)

**Survivorship bias in Indian backtests**
- Survivor-only small-cap backtests **overstate annual returns by 4.94 pp (23.3%)**: 26.17% vs 21.23% for the true universe. They overstate Sharpe by 0.097 (9.1%).
- Index constituents turned over 82.5%: 16.1% delisted, 33.1% graduated, 33.2% demoted.
- Reconstruction from bhavcopy including delisted securities is about 85–90% accurate historically.
- Source: [arXiv 2603.19380](https://arxiv.org/abs/2603.19380) (search summary; fetch blocked)

**Regime filters**
- Daniel & Moskowitz: crashes cluster in "panic" states after market declines with high volatility. A dynamic, volatility- and mean-forecast-scaled momentum strategy **roughly doubles alpha and Sharpe**. — [NBER w20439](https://www.nber.org/papers/w20439)
- An Indian practitioner backtest (Momo India) tested a trend-overlay "cash call" to cut momentum drawdowns. A long-when-above-**200-day EMA**, else cash rule had "significantly lower maximum drawdown" than buy-and-hold. — [Momo India substack](https://momoindia.substack.com/p/backtest-reducing-momentum-drawdowns) (search summary; numbers not retrieved)

**Base rates for short-horizon retail trading (SEBI studies)**
- **71% of individual intraday traders in the equity cash segment lost money in FY23**:
  - The sample was about 7 million traders.
  - Loss-makers were 76% among traders under 30 and 80% among those with more than 500 trades a year.
  - Intraday participation rose 300% from FY19 to FY23.
  - Source: [Business Standard](https://www.business-standard.com/amp/markets/news/over-70-intra-day-traders-incur-losses-during-fy23-reveals-sebi-study-124072401110_1.html); [Moneylife](https://moneylife.in/article/7-out-of-10-individual-intraday-traders-in-equity-cash-segment-make-losses-sebi-study/74733.html)
- About 90% of active F&O traders lost money in FY22. — [Angel One summary](https://www.angelone.in/news/market-updates/sebi-highlights-intraday-trading-risk)

### Inferences
- **Recommended pipeline:**
  1. Build the universe: NSE EQ series stocks with a price floor and a median 20-day traded value above a threshold. Using the F&O list as a liquidity proxy is optional.
  2. Exclude ASM/GSM/ESM stocks, the trade-for-trade (BE) series and stocks with 2%/5% bands.
  3. Winsorize each factor, then z-score it within the universe (or within sector).
  4. Combine with a weighted average.
  5. Rank and take the top N.
  6. Apply a regime gate (index trend or volatility) that cuts position count or exposure.
- **Corporate-action errors are a material risk.** Momentum and 52-week-high features are very sensitive to unadjusted splits, bonuses and demergers (see Q4). A survivorship-free universe is essential, given the 23% overstatement found.
- **Use F&O-stock membership as a filter for intraday.** These stocks have no hard circuit, so they cannot get trapped at a limit, and they are the most liquid.

### Gaps
- No peer-reviewed study was found that quantifies a 200-DMA or breadth regime filter for Indian cross-sectional momentum or swing strategies. Only practitioner blogs exist, and their numbers were not retrieved.
- No source was found that compares z-score averaging with rank aggregation on Indian data.
- The current exact ASM short-term stages and the latest (Aug 2026) ASM FAQ details were not retrieved.

---

## Q3. Which retail screeners are popular (India and crypto), what do they offer, and is there evidence on how their scans perform?

### Takeaway
Indian retail screeners are cheap or free and fall into two groups:
- **Technical scanners:** Chartink, Streak, TradingView
- **Fundamental screeners:** Screener.in, Trendlyne, StockEdge

**No independent evidence was found on the performance of their scans.** The only hard performance evidence is SEBI's finding that most short-horizon retail traders lose money. For crypto, the CoinGecko and CoinMarketCap free tiers are adequate for daily screening.

### Cited Findings

**Indian screeners**
- **Chartink**: no-code web scanner for NSE stocks covering technical and fundamental criteria (RSI, MACD, P/E, volume, breakouts). Offers pre-built and user scans, real-time alerts and dashboards. Custom screeners are free; premium alert plans start at about **₹780/month**. — [Strike.money Chartink review 2026](https://www.strike.money/reviews/chartink)
- **Screener.in**: custom fundamental screens (D/E, P/E, ROE) plus financial statements. Free tier, and Premium at about **₹4,999/year**. — [Strike.money: Screener alternatives 2026](https://www.strike.money/reviews/screener-alternatives)
- **Trendlyne**: multi-query live screener with **1,000+ parameters**, DVM scores, delivery data, alerts and peer comparison. Paid plans (Basic, GuruQ, StratQ) start at about **₹119/month**. — [Techjockey: best stock screeners India 2026](https://www.techjockey.com/blog/technical-stock-screeners)
- **StockEdge**: ready-made scans, market-breadth data and portfolio tools, but less flexible custom logic than Chartink. Premium is about ₹250/month, Pro about ₹999/month and Club about ₹1,999/month, all billed annually. — Search summary across [Strike.money](https://www.strike.money/reviews/screener-alternatives) / [sharemarketsoftware.com](https://sharemarketsoftware.com/best-stock-screener-tools/); exact attribution uncertain.
- **Streak (Zerodha)**:
  - No-code platform covering technical scanners, strategy building, backtests and virtual or live deployment to Kite.
  - Supports NSE equity, futures, options and currency futures.
  - The scanner has been made **free for Zerodha users**.
  - Sources: [Zerodha Z-Connect: new Streak scanner free](https://zerodha.com/z-connect/zerodha/announcements/introducing-the-new-streak-scanner-now-free-for-all-our-users-and-technicals-dashboard); [Chittorgarh Streak review](https://www.chittorgarh.com/article/zerodha-streak-review-algo-trading-for-retail-investors/516/)
- **Sensibull**: options analytics and screener, option-chain analysis, paper trading and alerts, with broker integrations (Zerodha, Upstox, 5paisa). Pricing starts at about **₹800**, with a free tier after a 7-day trial. — [Strike.money Sensibull review 2026](https://www.strike.money/reviews/sensibull)
- Practitioner guidance: Chartink and TradingView suit day or swing technical scans, while Screener.in and Trendlyne suit fundamental, long-term screening. — [Strike.money](https://www.strike.money/reviews/screener-alternatives)

**Crypto market-data APIs**
- **CoinGecko**: the free Demo plan was 30 calls/min and 10,000 calls/month. On **17 May 2026** its rate limit rose from **30 to 100 requests/min**. An API key and attribution are required. — [CoinGecko API changelog](https://docs.coingecko.com/changelog.md); [Costbench free-plan summary](https://costbench.com/software/blockchain-data-api/coingecko-api/free-plan/)
- **CoinMarketCap**: the free Basic plan gives **15,000 credits/month** at **50 requests/min**. Credits usually equal 1 per 100 data points returned. — [CoinMarketCap API FAQ](https://coinmarketcap.com/api/nl/faq/); [CMC Academy on credits](https://coinmarketcap.com/academy/tr/article/how-to-understand-api-credits-rate-limits-and-pagination)

**Outcome base rates**
- **71% of intraday cash traders lost money in FY23**, and 80% of those making more than 500 trades a year. — [Business Standard](https://www.business-standard.com/amp/markets/news/over-70-intra-day-traders-incur-losses-during-fy23-reveals-sebi-study-124072401110_1.html)

### Inferences
- These products mainly offer rule-based technical filters and fundamental filters, with little cross-sectional ranking. Few expose factor z-score composites; Trendlyne's DVM score is a partial exception. A custom module can differentiate through survivorship-free backtests, volatility-adjusted momentum and 52-week-high ranking, surveillance-list exclusion, and regime gating.
- Popular scan templates such as breakouts, volume shockers and RSI cross-ups have no documented out-of-sample edge in the sources found. The weak Indian high-volume premium suggests "volume shocker" scans should not be relied on alone.

### Gaps
- **No independent study or audited track record of Chartink, Streak, Trendlyne or StockEdge scan performance was found.**
- The TradingView screener's India coverage and pricing were not researched in depth. Its fields can be pulled from Python with community libraries (for example `tradingview-screener`), but this was not verified this session.
- StockEdge pricing attribution is uncertain. Trendlyne and Chartink prices come from third-party review sites, not vendor pages.

---

## Q4. What data sources can an India-based Python developer use (NSE, brokers, yfinance, libraries, corporate actions, survivorship, crypto)?

### Takeaway
The cheapest reliable intraday and daily data comes from broker APIs:
- **Kite Connect**: ₹500/month since 2025, historical data included, minute data in 60-day chunks, 3 requests/second.
- **Fyers**: free.
- **Dhan**: ₹499/month for data.
- **Upstox**: free or ₹1,000/month, depending on the source.

For a survivorship-free daily universe, **NSE bhavcopy** archives (via jugaad-data, bhavkit, pybhav and similar) are the best free route. CMIE Prowess (used by IIMA) is the institutional route.

yfinance `.NS` data has documented corporate-action errors, for example the HDFC Bank demerger. For crypto, ccxt covers 100+ global exchanges but **not Indian exchanges (CoinDCX, WazirX)**. India also taxes crypto gains at a flat 30% plus 1% TDS.

### Cited Findings

**Zerodha Kite Connect**
- The historical data add-on became **free from 8 Feb 2025**. A base Kite Connect subscription is still required. — [Kite forum announcement](https://kite.trade/forum/discussion/comment/49021/)
- In **Mar 2025** Zerodha launched a **free "Kite Connect Personal" API**, covering orders, positions and holdings with **no market data**. It also cut paid Kite Connect, which includes live plus historical data, from **₹2,000 to ₹500/month per API key**. — [Marketcalls](https://www.marketcalls.in/fintech/zerodha-makes-trading-api-free-for-personal-use-bundles-historical-data-with-connect-api.html); [Zerodha support FAQ](https://support.zerodha.com/category/trading-and-markets/general-kite/kite-api/articles/kite-connect-api-faqs)
- Historical API limits:
  - **3 requests/second**, with no daily or monthly cap.
  - Maximum range per request: **minute 60 days**; 3-, 5- and 10-minute 100 days; 15- and 30-minute 200 days; 60-minute 400 days; daily 2,000 days.
  - Longer histories need looping.
  - Source: [Kite forum: rate limit and lookback](https://kite.trade/forum/discussion/8899/rate-limit-of-kite-connect-historical-data-api-and-lookback)
- Corporate-action-adjusted data is reliable only for the last couple of years. Data from before about 2018/2019 may be unadjusted. — [Kite forum](https://kite.trade/forum/discussion/comment/30655/) (search summary)

**Other broker APIs**
- History depth:
  - **Upstox**: daily data from 2000 and minute data from 2022.
  - **Fyers**: minute data from mid-2017, and seconds data for the last 30 trading days.
  - **Dhan**: daily data from a contract's inception and 5 years of intraday data.
  - Source: [StocksDeveloper broker API comparison](https://stocksdeveloper.in/documentation/supported-brokers/api-comparison/market-data/)
- Pricing:
  - **Fyers**: trading and data APIs free.
  - **Dhan**: trading API free; data APIs ₹499/month.
  - **Upstox**: sources conflict. Some say all APIs are free; others say the Historical API costs ₹1,000/month.
  - Sources: [Stratzy cost comparison](https://stratzy.in/blog/cost-comparison-algo-trading-apis-india/); [Chittorgarh Upstox API review](https://www.chittorgarh.com/broker/upstox/api-for-algo-trading-review/33/)
- Fyers supports intervals from 5 seconds to monthly. Upstox supports 1 minute to 1 month. — [StocksDeveloper](https://stocksdeveloper.in/documentation/supported-brokers/api-comparison/market-data/)

**yfinance and corporate actions**
- yfinance **mishandled the HDFCBANK demerger**, which produced wrong returns for about 2 years of data. Adjusted close can also be inconsistent because of dividend adjustments. — [TradingQnA thread](https://tradingqna.com/t/has-anyone-found-wrong-adjusted-prices-in-nse-historical-data-for-backtesting/192938) (search summary)
- NSE's own website charts are shown without bonus or split adjustment. Traders report finding no source with properly adjusted NSE corporate actions. — Same search summary ([Kite forum](https://kite.trade/forum/discussion/comment/37916/) / TradingQnA)

**NSE scraping libraries**
- **jugaad-data** (GitHub, 583 stars, 215 commits, 27 open issues):
  - Covers NSE stock, index and F&O history, bhavcopy, index P/E and dividend yield, and RBI rates.
  - Has a CLI and caching.
  - Explicitly supports the **new NSE website**, and warns that libraries built on the old site "might stop working". This includes nsepy, which is effectively superseded.
  - Source: [GitHub jugaad-py/jugaad-data](https://github.com/jugaad-py/jugaad-data) (fetched)
- Newer bhavcopy tools:
  - **bhavkit**: typed library and CLI that turns CM, IDX, FO and DELIV bhavcopies into a validated dataset.
  - **aynse**: history, bhavcopy and live data.
  - **pybhav**: equities, F&O, currency and SME bhavcopies.
  - **nsefin**.
  - Sources: [PyPI bhavkit](https://pypi.org/project/bhavkit/); [PyPI aynse](https://pypi.org/project/aynse/); [PyPI pybhav](https://pypi.org/project/pybhav/); [PyPI nsefin](https://pypi.org/project/nsefin/)

**Survivorship-free universes**
- Bhavcopy archives include delisted securities. Reconstructing history from them reaches about 85–90% accuracy, and survivor-only universes overstate returns by about 23%. — [arXiv 2603.19380](https://arxiv.org/abs/2603.19380) (search summary)
- CMIE Prowess (via IIMA) corrects for "vanishing companies". Survivorship-adjusted values are recommended when analysis includes 1995–2000. — [IIMA library](https://faculty.iima.ac.in/~iffm/Indian-Fama-French-Momentum/)

**Crypto data and India context**
- **ccxt** unifies about 100+ exchanges. **CoinDCX and WazirX do not appear in its supported list.** — [PyPI ccxt](https://pypi.org/project/ccxt); [ccxt docs](https://docs.ccxt.com/Manual.md) (search summary)
- **WazirX was hacked in July 2024** (about $230M, attributed to Lazarus). Trading and withdrawals were suspended, followed by a Singapore restructuring moratorium. — [Tiger Brokers news](https://www.itiger.com/news/2523968506) (search summary)
- **India crypto tax**:
  - **30% flat tax** on gains under Sec. 115BBH, plus 4% cess. Only cost of acquisition is deductible, and there is no loss set-off.
  - **1% TDS** applies above thresholds (₹50,000 a year for specified persons, ₹10,000 for others).
  - Gains are reported in Schedule VDA.
  - Source: [Cryptact India VDA tax guide 2025](https://www.cryptact.com/en/blog/vda-income-tax-india-2025-complete-guide-to-crypto-tax-rules-sections-and-filing-process)

### Inferences
- **Practical stack for a single developer:**
  1. Daily EOD universe and history from NSE bhavcopy, which includes delisted names, via jugaad-data or bhavkit. Apply your own corporate-action adjustment using NSE corporate-action files.
  2. Intraday bars from Kite Connect (₹500/month) or Fyers (free).
  3. A daily download of NSE's ASM, GSM and ESM lists and price-band files for exclusion.
  4. Crypto via ccxt on global exchanges plus the CoinGecko free tier for the universe and market caps. Indian exchanges need custom REST wrappers.
- **Use yfinance only for prototyping**, because adjustment errors for demergers, bonuses and splits directly corrupt momentum and 52-week-high features.
- **The 30% tax plus 1% TDS makes high-turnover crypto ranking strategies much less attractive** for Indian residents. Losses cannot be offset, and TDS ties up capital on every trade above the threshold.

### Gaps
- Current Upstox Historical API pricing is unresolved because sources conflict.
- Whether NSE publishes an official free corporate-actions adjustment-factor file suitable for automation was not confirmed.
- ccxt support for CoinDCX, Mudrex or Bitbns was not confirmed from the live exchange list (GitHub not fetched for this).
- Kite Connect's current price (₹500/month) is from the March 2025 change. No later change was found as of Oct 2026, but this was not explicitly confirmed.

---

## Q5. Does ML-based ranking (gradient boosting, learning-to-rank) beat linear factor scoring out of sample, especially in India and emerging markets?

### Takeaway
Yes, moderately. In emerging markets out of sample from 2002 to 2021, **linear models earned about 0.8%/month long-short, tree models about 1.0% and neural nets or ensembles about 1.2%**. In India, ML beat OLS on 35 characteristics over 1994–2019. Learning-to-rank roughly tripled the Sharpe ratio of cross-sectional momentum in one study. The gains come from nonlinear interactions among momentum, liquidity and volatility features.

### Cited Findings
- **Gu, Kelly & Xiu (RFS 2020)**: ML forecasts give large economic gains, "in some cases doubling the performance of leading regression-based strategies". Trees and neural nets are best. The dominant signals are momentum, liquidity and volatility. — [SSRN 3159577](https://papers.ssrn.com/abstract=3159577); [NBER w25398](https://nber.org/papers/w25398)
- **Hanauer & Kalsbach, "Machine Learning and the Cross-Section of Emerging Market Stock Returns"** (*Emerging Markets Review* v55, 2023):
  - Compared 9 models: OLS, elastic net, GBRT, random forest and 1- to 5-layer neural nets.
  - Out of sample **Jan 2002 to Dec 2021**, with monthly rebalancing and cap-weighted portfolios.
  - Long-short returns were about **0.8%/month for linear models, about 1.0% for RF and GBRT, and 1.2% for neural nets and the ensemble**.
  - Sources: [Robeco insight](https://robeco.com/en-uk/insights/2023/10/using-machine-learning-for-emerging-market-equity-returns); [IDEAS/RePEc](https://ideas.repec.org/a/eee/ememar/v55y2023ics1566014123000274.html)
  - Models trained only on developed-market data predict EM returns nearly as well as EM-trained models. — [Search summary of Alpha Architect / EM paper](https://alphaarchitect.com/machine-learning-and-emerging-market-stock-returns/)
- **India: Lalwani & Meshram (Applied Economics 2022)**:
  - 35 characteristics on a survivorship-free sample of Indian firms, 1994–2019.
  - Elastic net, RF, XGBoost and neural nets vs OLS show a "substantial improvement in forecast accuracy", and "investment strategies based on model forecasts provide significant returns".
  - Source: [Taylor & Francis](https://www.tandfonline.com/doi/abs/10.1080/00036846.2021.1982132)
- **Learning-to-rank (Poh, Lim, Zohren & Roberts)**: pairwise and listwise LTR methods (RankNet, LambdaMART, ListNet) applied to cross-sectional momentum **"boost Sharpe ratios approximately threefold"** over traditional heuristic or regress-then-rank approaches. — [arXiv 2012.07149](https://arxiv.org/abs/2012.07149)
- International ML study: "Alpha Go Everywhere: Machine Learning and International Stock Returns" (ABFER 2023 conference paper) exists. Its numbers were not retrieved. — [ABFER PDF](https://www.abfer.org/media/abfer-events-2023/annual-conference/papers-investment/AC23P3084-Alpha-Go-Everywhere-Machine-Learning-and-International-Stock-Returns.pdf)
- Low-quality evidence: an XGBoost model reportedly hit **73.1% accuracy predicting Indian stock price direction**. This is a non-peer-reviewed or unclear-venue paper and very likely overfit or leaky. — [Academia.edu](https://www.academia.edu/76210027/Extreme_Gradient_Boosting_for_Predicting_Stock_Price_Direction_in_Context_of_Indian_Equity_Markets)

### Inferences
- **Suggested progression:**
  1. Start with a transparent z-score composite as the baseline.
  2. Add LightGBM or XGBoost regression, or a LambdaRank objective, on the same features. Use walk-forward, purged time-series cross-validation with an embargo.
  3. Judge success by out-of-sample rank IC and top-decile minus universe returns net of costs.
- **Expect an uplift of roughly +0.2 to +0.4%/month long-short over linear models**, based on the EM evidence. Most of a retail long-only screener's edge will still come from the underlying factors.
- Headline directional-accuracy claims (70%+) in Indian ML papers should be treated as red flags for look-ahead bias or leakage.

### Gaps
- Exact out-of-sample R² and Sharpe figures from Gu–Kelly–Xiu could not be verified because the primary text was blocked. A commonly cited figure is a monthly out-of-sample R² of about 0.40% for the 3-layer net; it was not verified here.
- Lalwani & Meshram's exact long-short returns and Sharpe ratios, and India-specific gradient-boosting results at daily or weekly horizons, were not available.
- No study was found that applies learning-to-rank specifically to Indian stocks or to crypto with net-of-cost results.

---

## Q6. What SEBI rules apply if screener suggestions are shared with others, versus purely personal use?

### Takeaway
Purely personal use of a self-built screener or algo, including for immediate family, needs no SEBI registration. This holds as long as order flow stays below **10 orders/second per exchange**. Orders sent through broker APIs are still tagged as "algo" by the exchange.

**Sharing buy suggestions or rankings with others, paid or as a "black-box" algo, falls within research-analyst territory.** That requires SEBI RA registration and brings with it:
- A fee cap of ₹1.51 lakh per family per year
- A deposit of ₹1–10 lakh depending on client count
- Disclosure of AI tool use from Apr 2025
- For black-box algos, RA registration plus published strategy research reports

Since Aug 2024, unregistered "finfluencers" cannot make return claims or advise in association with SEBI-regulated entities. Education content must use price data at least 3 months old.

### Cited Findings

**RA Regulations amendments (Dec 2024)**
- The **SEBI (Research Analysts) (Third Amendment) Regulations, 2024** were issued on **16 Dec 2024**. They revise qualifications (dropping the experience requirement), introduce **client-count-based deposits**, allow dual IA+RA registration, and define "part-time research analysts" and "persons associated with research services". — [TaxGuru](https://taxguru.in/sebi/sebi-research-analysts-third-amendment-regulations-2024.html); [ComplianceCalendar](https://www.compliancecalendar.in/learn/sebi-research-analyst-third-amendment-regulations-2024)
- The RA fee cap is **₹1.51 lakh per annum per family (individual/HUF clients)**, payable in advance for no more than one quarter. — [TaxGuru](https://taxguru.in/sebi/sebi-research-analysts-third-amendment-regulations-2024.html) (search summary)
- Part-time RAs are reportedly limited to **75 clients**. — Search summary of the same results; attribution uncertain, verify.

**RA guidelines (Jan 2025)**
- Circular **SEBI/HO/MIRSD/MIRSD-PoD-1/P/CIR/2025/003, 8 Jan 2025**. RAs and IAs must:
  - **Disclose the extent of AI tool use** in their research or advice at the time terms and conditions are disclosed
  - Ensure data security and confidentiality
  - Take **full responsibility** for AI-based outputs
  - Comply by **30 Apr 2025**
  - Source: [Taxmann](https://taxmann.com/post/blog/sebi-mandates-ias-and-ras-to-disclose-use-of-ai-tools-in-providing-investment-advice-research-services-to-clients); [Outlook Money](https://www.outlookmoney.com/invest/disclosing-ai-usage-in-investment-advisory-and-ensuring-data-security-is-a-must-sebi-issues-new-rules-for-ras-and-ias)
- Deposits range from **₹1 lakh (up to 150 clients) to ₹10 lakh (more than 1,000 clients)**. — [Outlook Money](https://www.outlookmoney.com/invest/disclosing-ai-usage-in-investment-advisory-and-ensuring-data-security-is-a-must-sebi-issues-new-rules-for-ras-and-ias) (search summary)
- A further RA/IA circular, **SEBI/HO/MIRSD/MIRSD-PoD/P/CIR/2025/105 (July 2025)**, exists. Its contents were not retrieved. — [SEBI PDF](https://www.sebi.gov.in/sebi_data/faqfiles/jul-2025/1753269723942.pdf)

**Finfluencer restrictions (Aug 2024)**
- Notified in the Gazette on **29 Aug 2024**. SEBI-regulated entities and their agents must not associate, directly or indirectly, with persons who:
  - give securities advice or recommendations without SEBI registration, or
  - **make return or performance claims** without SEBI permission.
- Existing contracts had to be ended within 3 months.
- Sources: [Outlook Business](https://www.outlookbusiness.com/markets/sebi-directs-regulated-entities-to-sever-ties-with-finfluencers-in-3-months); [Nishith Desai analysis](https://nishithdesai.com/research-and-articles/hotline/technology-law-analysis/securities-market-regulators-continued-quest-against-unfiltered-financial-advice-15193)
- Educators may **not use live prices**, and must use price data **at least 3 months old**. A Jan 2025 clarification addressed this. — [Business Standard, Jan 2025](https://www.business-standard.com/markets/news/sebi-finfluencer-circular-live-stock-data-market-education-rules-125013000571_1.html); [StudyCafe clarification](https://studycafe.in/sebi-issues-clarifications-on-finfluencer-guidelines-364291.html)

**Retail algo framework (Feb 2025, phased in through Apr 2026)**
- The circular is dated **4 Feb 2025**.
- Algos are classed as **white box** (execution algos with disclosed logic) or **black box** (logic not disclosed).
- **Black-box providers must register as SEBI Research Analysts**, maintain a research report for each strategy, publish it periodically, and report changes to the exchange.
- Sources: [QuantInsti](https://blog.quantinsti.com/sebi-algo-trading-guidelines-retail-investors); [K&S Partners](https://ksandk.com/corporate/sebis-new-regulatory-framework-for-retail-algorithmic-trading/); [Moneylife](https://moneylife.in/article/sebi-introduces-new-framework-for-safer-algo-trading-for-investors/76294.html)
- **Timeline:**
  - Originally 1 Aug 2025, deferred to **1 Oct 2025**.
  - Glide-path milestones: registration applications by **31 Oct 2025**, registration completion by **30 Nov 2025**, mock session by **3 Jan 2026**.
  - Brokers missing them could not onboard new API-algo clients from **5 Jan 2026**.
  - **Full mandatory implementation from 1 Apr 2026.**
  - Sources: [Business Standard, 30 Sep 2025](https://www.business-standard.com/markets/news/sebi-extends-retail-algo-trading-framework-rollout-to-2026-125093000956_1.html); [Taxmann](https://www.taxmann.com/post/blog/sebi-extends-deadline-for-retail-algo-trading-framework); [TaxGuru](https://taxguru.in/sebi/sebi-extends-algo-trading-implementation-timeline-april-2026.html)
- **Threshold:**
  - **10 orders per second, per exchange, per client.**
  - Below it, no strategy registration is needed, but API orders are still tagged "algo" with a generic ID and monitored by the broker.
  - Above it, the strategy must be registered with the exchange through the broker.
  - **Self-developed algos may be used for self and immediate family** (spouse, dependent children, dependent parents) without separate registration below the threshold.
  - Sources: [Business Standard](https://www.business-standard.com/markets/news/sebi-extends-retail-algo-trading-framework-rollout-to-2026-125093000956_1.html); [QuantInsti 2026 guide](https://www.quantinsti.com/articles/algorithmic-trading-india/); [Angel One](https://www.angelone.in/knowledge-center/online-share-trading/sebi-algo-trading-rules) (search summary)

### Inferences
- **Personal use:** a screener that ranks candidates for the developer, with optional broker-API order placement, sits within the self-developed algo exemption. Keep order rates far below 10 orders/second. Broker API access (Kite, Fyers and others) must follow the broker's framework, such as API key registration and static IP where brokers require it. Static-IP details were not verified this session.
- **Sharing outputs:** publishing "buy candidates" from the screener to others counts as a research recommendation. That applies whether it is a Telegram channel, a paid app or an open website with live prices, and especially if return claims are made. It would likely require RA registration, AI-use disclosure and fee and deposit compliance. Offering it as an opaque algo to others falls squarely under the black-box rules (RA plus exchange empanelment via a broker).
- **Educational sharing** with no recommendations should use data that is at least 3 months old and make no return claims.
- Crypto suggestions are outside SEBI's securities remit. The VDA tax and anti-money-laundering rules still apply to the user.

### Gaps
- Primary SEBI circular texts (sebi.gov.in) were blocked. Thresholds and dates come from law-firm, news and broker summaries.
- The exact wording of when a free, public "screen output" with no specific recommendation becomes "research" under the 2024 RA definitions was not retrieved.
- The contents of the July 2025 RA/IA circular (CIR/2025/105) and any 2026 updates were not retrieved. This includes any Past Risk and Return Verification Agency (PaRRVA) framework for performance claims, which was not researched.
- No crypto-specific Indian regulatory framework for sharing crypto signals was found. FIU-IND VASP registration was not researched.
