# Allocator Due Diligence, Strategy Evaluation and a Stress-Scenario Library (India + Crypto), current to Oct 2026

Method note: in this session WebFetch was blocked by the network egress proxy for almost every domain (NBER, CoinGecko, Wikipedia, Cointelegraph, NSE mirrors, HSBC MF, etc.), so the findings below come from web-search result summaries rather than full-page reads. Every figure is tied to the URL the search surfaced. Where summaries disagreed or the source looked weak (aggregators, forums, MEXC news reposts), this is flagged inline. Numbers computed by me from cited inputs are labelled "(derived)".

## Q1. Allocator evaluation metrics and thresholds, plus "too good to be true" tests

### Takeaway
Allocators don't rely on any single metric. They read Sharpe, Sortino, Calmar/MAR, max drawdown, higher moments and tail measures (VaR/CVaR) together, then discount them for estimation error, return smoothing (Lo 2002; Getmansky-Lo-Makarov 2004) and data-mining (Harvey-Liu haircuts, Deflated Sharpe, PBO). A long-run net Sharpe of about 1 is "good". Long track records above about 2 are rare. Above about 3 with almost no losing months (Madoff: Sharpe 3.4, 3% vol, only 7 small losing months in 14.5 years) is a classic fraud or smoothing red flag. Backtest Sharpe has almost no predictive power for live results (R² < 0.025 across 888 Quantopian algos), while volatility and max drawdown do.

### Cited Findings

**Metric definitions**
- **Sharpe annualization error:** annualizing a monthly Sharpe by multiplying by √12 is correct only under special conditions. The right multiplier depends on serial correlation. Ignoring positive serial correlation can overstate annual Sharpe by more than 65%, understate it when correlation is negative, and produce inconsistent rankings across styles — [Lo 2002, FAJ (CFA Institute)](https://rpc.cfainstitute.org/research/financial-analysts-journal/2002/the-statistics-of-sharpe-ratios); [IDEAS record](https://ideas.repec.org/a/taf/ufajxx/v58y2002i4p36-52.html); [MetricGate note on the Lo-adjusted Sharpe as the field standard for smoothed series](https://metricgate.com/docs/lo-autocorrelation-sharpe/)
- **Sharpe's limits:** Sharpe uses only the first two moments, so it can misdiagnose hedge-fund (option-like, skewed) returns. It is also open to manipulation and estimation error — [The Hedge Fund Journal](https://thehedgefundjournal.com/the-investment-performance-of-sharpe-ratios/)
- **Sortino:** return above a Minimum Acceptable Return (MAR), divided by downside deviation below that MAR. Only downside volatility is penalized — [CFA Institute/GIPS paper "The Sortino Ratio"](https://rpc.cfainstitute.org/-/media/documents/code/gips/the-sortino-ratio.pdf); [Wall Street Prep](https://www.wallstreetprep.com/knowledge/sortino-ratio)
- **Calmar and MAR ratios:**
  - Calmar = CAGR / |max drawdown|, typically over a 36-month window. It is used to evaluate CTAs and hedge funds — [Corporate Finance Institute](https://corporatefinanceinstitute.com/resources/uncategorized/calmar-ratio/)
  - The MAR ratio uses CAGR and max drawdown since inception, not a 36-month window — [Business Professor](https://thebusinessprofessor.helpjuice.com/mar-ratio-definition); [Gate39 tear-sheet ratio definitions](https://www.gate39media.com/blog/hedge-fund-performance-3-popular-financial-tear-sheet-ratio-definitions)
- **VaR and CVaR/ES:** CVaR (Expected Shortfall) is the average loss given that the loss exceeds the VaR threshold. It is subadditive, so it is a coherent risk measure, which VaR is not. These are standard CAIA, FRM and CFA L3 tools for hedge-fund risk — [Ryan O'Connell Finance](https://ryanoconnellfinance.com/course-articles/expected-shortfall-cvar/); [CAIA L1 study guide: hedge fund risk metrics](https://open-exam-prep.com/study-guides/caia-level1/hedge-funds-relative-value-risk/hf-risk-metrics); [EDHEC comparison of downside-risk approaches for hedge funds](https://climateinstitute.edhec.edu/publications/comparison-alternative-approaches-determining-downside-risk-hedge-fund-strategies)
- **Minimum Track Record Length (MinTRL):** the smallest number of observations needed for a measured Sharpe to be statistically significant at a chosen confidence level. For typical hedge-fund return distributions, MinTRL "can easily exceed several years of monthly data" — [TradingStrategy.ai glossary (PSR/MinTRL)](https://tradingstrategy.ai/glossary/probabilistic-sharpe-ratio)
  - The source framework is Bailey & López de Prado, "The Sharpe Ratio Efficient Frontier", Journal of Risk — [Risk.net](https://www.risk.net/journal-risk/2223785/sharpe-ratio-efficient-frontier); [IDEAS](https://ideas.repec.org/a/rsk/journ4/2223785.html); [PerformanceAnalytics MinTrackRecord](https://www.quantargo.com/help/r/latest/packages/PerformanceAnalytics/NEWS/MinTrackRecord)
  - Formula (per-period SR, skew γ3, kurtosis γ4, benchmark SR*): MinTRL = 1 + [1 − γ3·SR + (γ4−1)/4·SR²]·(Z_α/(SR − SR*))²
- **Deflated Sharpe Ratio (DSR):** corrects for selection bias under multiple testing (many trials, best one reported) and for non-normal returns — [Bailey & López de Prado, SSRN 2460551](https://papers.ssrn.com/abstract=2460551)
- **Harvey & Liu backtest haircuts:**
  - Practitioners commonly haircut backtested Sharpe ratios by 50% "as a rule of thumb". Harvey & Liu turn the Sharpe into a t-ratio and adjust the p-value for multiple testing.
  - The correct haircut is nonlinear. The highest Sharpes are penalized only moderately, while marginal Sharpes are penalized heavily. A t-ratio of 3.0 that is significant in a single test may not be significant once multiple tests are counted.
  - Sources: [Harvey & Liu "Backtesting" (Duke PDF)](https://people.duke.edu/~charvey/Research/Published_Papers/P120_Backtesting.PDF); [Man Group summary](https://www.man.com/insights/backtesting); [CME-hosted copy](https://www.cmegroup.com/content/dam/cmegroup/education/files/backtesting.pdf)
- **Probability of Backtest Overfitting (PBO):** Bailey, Borwein, López de Prado & Zhu (J. Computational Finance, 2015) introduce PBO, estimated by Combinatorially Symmetric Cross-Validation (CSCV). They argue that standard hold-out methods are unreliable for investment backtests — [SSRN 2326253](https://papers.ssrn.com/abstract=2326253); [Risk.net](https://www.risk.net/journal-of-computational-finance/2471206/the-probability-of-backtest-overfitting); [R `pbo` package](https://packages.oit.ncsu.edu/cran/web/packages/pbo/readme/README.html)
- **Backtest vs. live (Quantopian):**
  - In 888 Quantopian algorithms, each with at least 6 months of out-of-sample (OOS) trading, backtest Sharpe had almost no predictive value for OOS performance (R² < 0.025).
  - Volatility, max drawdown and portfolio-construction features such as hedging did predict OOS results.
  - Non-linear ML classifiers on backtest features reached R² = 0.17.
  - Sources: [Quantpedia summary of Wiecki et al. 2016](https://quantpedia.com/quantopians-academic-paper-about-in-vs-out-of-sample-performance-of-trading-alg/); [CXO Advisory](https://www.cxoadvisory.com/big-ideas/in-sample-vs-out-of-sample-performance-of-888-trading-strategies)
- **Published anomalies decay (McLean & Pontiff, 97 predictors):** returns are 26% lower out-of-sample and 58% lower after publication, implying about 32% from publication-informed trading. The decline is bigger for predictors with higher in-sample returns — [McLean & Pontiff 2016 (PDF)](https://Www.Gwern.net/doc/economics/2016-mclean.pdf); [Counterpoint Funds copy](https://counterpointfunds.com/wp-content/uploads/2017/07/PredictabilityMcleanPontiff.pdf)
- **Capacity (square-root impact law):** expected impact ≈ σ_daily × √(Q/V), where Q is order size and V is daily volume. Doubling size raises impact by about 41% (√2), not 100%. Impact depends mostly on total volume traded and the participation rate — [Bouchaud, "The Square-Root Law of Market Impact"](https://bouchaud.substack.com/p/the-square-root-law-of-market-impact); [arXiv 2311.18283 "two square root laws"](https://arxiv.org/pdf/2311.18283); [practical formula, fffinstill](https://fffinstill.com/learning/concepts/slippage-market-impact)

**Thresholds and credibility levels**
- **Rules of thumb for Sharpe:**
  - Sharpe above 1 is "pretty good".
  - Hedge funds reportedly ask aspiring PMs or quants for track records with Sharpe above 2, while practitioners question "where all the funds with long-term Sharpe > 2 are".
  - Source quality: a practitioner forum plus a search synthesis, so lower quality — [Wilmott forum](https://forum.wilmott.com/viewtopic.php?p=791864); [The Hedge Fund Journal "Myth of persistent Sharpe ratio"](https://thehedgefundjournal.com/the-myth-of-persistent-sharpe-ratio/)
- **Industry Sharpe benchmarks (Preqin-type data, as of 2015, monthly reported returns):**
  - Five-year Sharpe: 2.51 for "new" emerging hedge funds, 1.56 for "small" ones, 1.54 for the wider industry.
  - Three-year Sharpe: 2.84 for funds above $1bn.
  - These come from self-reported monthly data that is prone to the smoothing problem below — [Hedge Fund Alpha](https://hedgefundalpha.com/news/is-bigger-always-best-a-closer-look-at-effect-of-size-on-hedge-funds/)
- **Emerging-manager definition and investor flexibility (AIMA/Preqin survey):**
  - An "emerging" manager has ≤ $300m AUM, or is a first-time fund with ≤ 3-year track record.
  - Half of investors would consider a manager with a track record under 1 year.
  - About three quarters set a minimum AUM below $500m.
  - Two thirds are open to managers below $100m.
  - Sources: [Hedge Fund Alpha / Opalesque](https://www.opalesque.com/industry-updates/4974/emerging-hedge-funds-outperform-established-peers.html); [AIMA press release](https://www.aima.org/article/press-release-how-are-emerging-hedge-fund-managers-attracting-capital-and-keeping-their-edge.html)
- **Madoff's numbers (the canonical "too smooth" case):**
  - Fairfield Sentry, July 1989–Feb 2001: Sharpe 3.4 with 3.0% standard deviation, the best risk-adjusted fund among 41 in the Zurich database.
  - Only 7 small losing months in about 14.5 years. Losses never exceeded 55 bp, and occurred in just 4 of 139 consecutive months.
  - Gross returns slightly above 1.5% a month; net about 15% a year.
  - Markopolos noted a beta of only about 6% to the S&P 100, inconsistent with the claimed split-strike conversion strategy.
  - Sources: [SEC OIG Madoff report, Exhibit 0221](https://sec.gov/news/studies/2009/oig-509/exhibit-0221.pdf); [Institutional Investor](https://www.institutionalinvestor.com/article/b150q7h1dm0mw2/madoff-scheme-had-early-skeptics); [NBC News](https://www.nbcnews.com/news/amp/wbna28199834); [Cochrane/RiskData note on Madoff](https://www.johnhcochrane.com/s/2009_02_RiskData_Madoff.pdf)
- **Getmansky, Lo & Makarov (JFE 74, 2004, pp. 529–609):**
  - Hedge-fund returns are often highly serially correlated, unlike long-only funds. The most likely cause is illiquidity exposure.
  - Smoothed reported returns understate volatility and inflate Sharpe.
  - The paper proposes an econometric smoothing model, estimators of the smoothing profile and a smoothing-adjusted Sharpe, and applies them to 908 TASS funds. Smoothing coefficients vary widely by style and work as a proxy for illiquidity.
  - Sources: [MIT (Lo) paper page](https://web.mit.edu/~alo/www/Papers/serialhf.html); [NBER w9571](https://www.nber.org/papers/w9571)
- **GLM model specification:**
  - Observed return R°_t = θ0·R_t + θ1·R_{t−1} + … + θk·R_{t−k}, with θj ≥ 0 and Σθj = 1.
  - Smoothing index ξ = Σθj². Lower ξ means more smoothing.
  - It is implemented in R PerformanceAnalytics `SmoothingIndex`, which uses an MA(2) fit — [rdocumentation](https://www.rdocumentation.org/packages/PerformanceAnalytics/versions/1.5.3/topics/SmoothingIndex)
- **Bollen & Pool:**
  - The pooled distribution of monthly hedge-fund returns shows a discontinuity at zero: far more small gains than small losses. It disappears with bimonthly returns, which suggests the gains are later reversed. It is also absent in the 3 months before an audit.
  - Their "performance flags" fire more often for funds later charged with misappropriation, overvaluation, misrepresentation or Ponzi schemes.
  - Sources: [Bollen & Pool, JF 2009 (IDEAS)](https://ideas.repec.org/a/bla/jfinan/v64y2009i5p2257-2288.html); [Bollen & Pool, RFS 2012 "Suspicious Patterns…" (IDEAS)](https://ideas.repec.org/a/oup/rfinst/v25y2012i9p2673-2702.html); [Vanderbilt News](https://news.vanderbilt.edu/2012/02/28/do-not-ignore-signs-of-hedge-fund-fraud/)

### Inferences
- **Worked MinTRL numbers (derived from the cited formula; 95% one-sided, SR* = 0):**

  | Annual Sharpe | Monthly data, normal | Monthly data, skew −1 / kurtosis 6 | Daily data, normal |
  |---|---|---|---|
  | 0.5 | ≈ 132 months (11 yrs) | ≈ 12.7 yrs | ≈ 10.8 yrs |
  | 1.0 | ≈ 35 months (2.9 yrs) | ≈ 3.9 yrs | ≈ 2.7 yrs (684 trading days) |
  | 1.5 | ≈ 17 months (1.4 yrs) | ≈ 2.1 yrs | — |
  | 2.0 | ≈ 10.5 months | ≈ 17 months | ≈ 173 days |
  | 3.0 | ≈ 6 months | — | ≈ 78 days |

  - **Implication:** a live record of a few months can only "prove" a Sharpe of 2–3 or more. A realistic Sharpe of 0.5–1.0 needs about 3–11 years of evidence, which is why allocators scale in gradually (see Q3).
- **Practical acceptance screen for a trading agent (synthesis, not an industry standard):**
  - Net-of-cost live Sharpe ≥ 1 (≥ 0.7 is acceptable if correlation to Nifty/BTC is low).
  - Backtest Sharpe haircut by at least 50% unless DSR/PBO tests are run.
  - Calmar ≥ 1 over 36 months.
  - Max drawdown ≤ 15–20% (multi-manager platforms enforce 5–7.5%, see Q3).
  - Skew not strongly negative, and CVaR(95%) daily loss inside the risk budget.
  - Lag-1 autocorrelation of daily/monthly P&L not significantly positive (Ljung-Box).
  - No "missing small losses" kink at zero.
- **Red-flag combination:** Sharpe > 3, fewer than 5% losing months, near-zero beta to a strategy's natural benchmark, and positive autocorrelation. Any agent output with this profile should be treated as a bug (look-ahead, stale prices, mark-to-model) until proven otherwise.

### Gaps
- No published, authoritative "acceptance threshold table" was found from CAIA, AIMA or CFA. The AIMA DDQ is a questionnaire with no numeric cut-offs, and the thresholds above are a synthesis.
- The GLM 2004 tables (style-level θ0 and smoothing indices, e.g. for convertible arbitrage and emerging markets) could not be retrieved because the fetch was blocked.
- I found no reliable source for typical allocator turnover or capacity limits (e.g. % of ADV). Only the square-root impact law is sourced.

## Q2. Operational due diligence red flags and model-risk checks (paper vs. live, slippage, kill switch, documentation)

### Takeaway
Operational due diligence (ODD) centres on independent verification:
- an independent administrator calculating NAV;
- a reputable auditor and custody/prime broker that the investor can contact directly;
- a documented valuation policy, cash controls and compliance.

A single red flag such as self-administered NAV can end an allocation regardless of returns. For a systematic or agentic strategy, the matching checks are:
- live-vs-paper reconciliation and slippage tracking;
- pre-trade risk limits and a tested kill switch (Knight Capital lost about $440m in about 45 minutes without one);
- awareness of venue-specific failure modes such as exchange outages, ADL and order-book-priced collateral.

### Cited Findings
- **ODD scope and red flags:**
  - ODD covers the administrator, auditor, custody, valuation, cash controls, compliance, regulatory filings and service-provider verification.
  - One operational red flag, such as an unclear valuation process or a self-administered fund (the GP computing its own NAV, or an unknown administrator), "can end a manager's chances no matter how good the returns look".
  - Unwillingness to let investors speak directly with the administrator, auditor or prime broker is itself a red flag.
  - Sources: [Thomas Murray](https://thomasmurray.com/insights/common-operational-red-flags-investors-miss-due-diligence); [Angel Investors Network ODD checklist](https://angelinvestorsnetwork.com/regulatory-compliance/operational-due-diligence-emerging-manager-checklist)
- **AIMA DDQ:** the AIMA due-diligence questionnaire is the standard starting point, but the "nuts and bolts" of ODD are learned on-site — [Operational due diligence (Wikipedia)](https://en.wikipedia.org/wiki/Operational_due_diligence_(alternative_investments)); [CFA L3 hedge funds DD/operational risk](https://pastpaperhero.com/resources/cfa-level3-hedge-funds-and-liquid-alternatives-due-diligence-and-operational-risk)
- **Knight Capital (1 Aug 2012):**
  - Defective deployment of "Power Peg" code at the NYSE open turned 212 parent orders into millions of child orders: about 4 million executions in 154 stocks, more than 397 million shares, in about 45 minutes.
  - Realized pre-tax loss of about $440m. The firm had no kill switch for live positions and was near bankruptcy within hours.
  - Sources: [Knight 8-K (SEC)](https://www.sec.gov/Archives/edgar/data/0001060749/000119312512332176/d391111dex991.htm); [Knight 10-Q](https://www.sec.gov/Archives/edgar/data/0001060749/000119312512346917/d361681d10q.htm); [bad.technology case write-up](https://bad.technology/articles/knight-capital-loses-440-million-in-45-minutes-from-misdeployed-trading-algorithm.html)
- **SEBI retail algo framework (India):**
  - Exchanges run an SOP for algo testing, surveil algo orders, and can trigger a kill switch on a specific algo ID.
  - Brokers must distinguish algo from non-algo orders. API access needs 2FA and static-IP restrictions, and algo providers must be empanelled.
  - The effective date moved from 1 Aug 2025 to 1 Oct 2025.
  - Sources: [Angel One](https://www.angelone.in/news/market-updates/sebi-introduces-new-regulations-for-retail-algo-trading-to-enhance-market-safety); [Angel One on deadline](https://oga-prod.angelone.in/news/market-updates/sebi-pushes-algo-trading-deadline-to-october-1-2025); [Groww](https://groww.in/blog/sebi-regulations-on-algorithmic-trading-in-india); [Marketcalls compliance checklist](https://www.marketcalls.in/market-regulations/sebis-2025-regulations-for-retail-algo-trading-the-ultimate-compliance-checklist.html)
- **Paper/backtest vs. live divergence:** backtest Sharpe R² < 0.025 for OOS performance (Wiecki et al.); anomaly returns −26% OOS and −58% post-publication (McLean & Pontiff). See Q1 for sources — [Quantpedia](https://quantpedia.com/quantopians-academic-paper-about-in-vs-out-of-sample-performance-of-trading-alg/); [McLean & Pontiff](https://Www.Gwern.net/doc/economics/2016-mclean.pdf)
- **Venue failure modes in crypto:**
  - **BitMEX outage (12 Mar 2020):** the matching engine went dark for about 25 minutes. Inverse contracts with BTC collateral meant collateral value fell together with price, and the insurance fund drained. The outage acted as an accidental circuit breaker. Source quality: a Binance Square repost, so weaker — [Binance Square](https://www.binance.com/ar/square/post/350135015472097)
  - **BitMEX outflows:** BTC held on BitMEX fell from about 315k on 13 Mar to about 244k by 29 Mar 2020 (CoinMetrics) — [Cointelegraph](https://cointelegraph.com/news/btc-held-by-bitmex-fell-25-after-mass-liquidations)
  - **USDe on Binance (10–11 Oct 2025):** the depeg was confined to Binance's order books. USDe held near $1 on-chain (Curve) and in Ethena mint/redeem. This shows that exchange-internal pricing of collateral is a model risk in its own right — [21Shares](https://www.21shares.com//research/why-did-ethenas-stablecoin-remain-stable-onchain-but-depegged-on-binance)
  - **Hyperliquid ADL (10–11 Oct 2025):** Hyperliquid triggered cross-margin auto-deleveraging for the first time in more than 2 years. About 35,000 positions across 20,000 traders were mechanically unwound. More than 1,000 wallets were "completely wiped out", and losses exceeded $1.2bn across 6,300+ wallets. Profitable hedges can be force-closed — [The Defiant 10/10 autopsy](https://thedefiant.io/newsletter/defi-daily/the-ultimate-10-10-crash-autopsy); [Blockworks Empire](https://empire-blockworks.beehiiv.com/p/hyperliquidated)
  - **MakerDAO "Black Thursday" (12 Mar 2020):** with the Ethereum mempool clogged, 1,462 of 3,994 liquidation auctions (36.6%) were won by zero bids. $8.32m of collateral was lost in about 12 hours, $4.5m of DAI was left unbacked, and Maker recorded losses of 6.65m DAI — [Glassnode](https://insights.glassnode.com/what-really-happened-to-makerdao)

### Inferences
- **Model-risk checklist for an agent (synthesis):**
  1. A written strategy spec covering hypothesis, universe, signals, sizing, exits and known failure regimes, under version control.
  2. Daily paper-vs-live reconciliation: fills, slippage vs. modelled cost, missed and rejected orders. Flag when realized cost exceeds about 1.5× the model, or when live Sharpe falls more than 1 standard error below backtest over a MinTRL-sized window.
  3. Pre-trade hard limits on order count, notional, position, daily loss and messages per second.
  4. A kill switch that is tested regularly (fire drill) and flattens positions or cancels orders. The broker/exchange kill switch is a backstop only.
  5. A deployment checklist with staged rollout, since Knight's failure was a deployment error.
  6. Venue-risk rules: no collateral in exotic or synthetic stablecoins; assume an exchange can go down for 25–90 minutes at the worst moment; assume ADL can close your hedge.
- **India-specific:** SEBI's framework means the broker/exchange can halt the agent's algo ID. The agent's own risk logic must assume it can be cut off mid-position.

### Gaps
- The SEC's 2013 Knight order (Market Access Rule 15c3-5 findings) was not retrieved.
- The full AIMA DDQ section list was not retrieved because the fetch was blocked.
- I found no quantified industry benchmark for "acceptable" live-vs-backtest slippage divergence.

## Q3. Sizing a new strategy (start small, scale with live record) and core-satellite/barbell construction for an individual

### Takeaway
- **Allocators start small and scale.** The average first ticket to a new manager is $16m, against $37.7m for established managers. For successful managers the core position grows to roughly 3.6–5× the initial ticket.
- **Multi-manager platforms cut risk mechanically:** capital is halved at a 5% drawdown and the pod is shut at 7.5%.
- **For an individual:** Taleb's barbell puts 85–90% in near-riskless assets and 10–15% in risk-seeking bets. In India in 2026 the "safe" end yields about 5.3% (91-day T-bill; repo 5.25%), with arbitrage funds at about 6% trailing 1-year and equity taxation.
- **Retail derivatives evidence:** SEBI found 91% of F&O traders lost money in FY25 (₹1.06 lakh crore net). That argues for capping the active-trading sleeve.

### Cited Findings
- **Ticket sizes for new vs. established managers:**
  - The average ticket is $16m for new managers and $37.7m for established ones.
  - When managers succeed, the core position averages almost 5× the initial ticket in the US and EMEA, and 3.6× in APAC.
  - Attribution among the surfaced results was ambiguous (Altss glossary vs. AIMA press release); verify before quoting — [Altss "ticket size"](https://altss.com/glossary/ticket-size); [AIMA press release on emerging managers](https://acc.aima.org/article/press-release-emerging-managers-secure-earlier-investor-backing-by-raising-their-institutional-game.html)
- **Allocation capacity:** allocators assess strategy scalability, deployment pacing and concentration limits — [Altss "allocation capacity"](https://altss.com/taxonomy/allocation-capacity)
- **Millennium drawdown rule:**
  - A PM at a 5% drawdown has capital cut in half. At 7.5% the pod is terminated, and the whole team leaves the same day.
  - Millennium hired about 160 PMs in a year, with 15–20% annual PM turnover.
  - Source quality: a secondary Substack — [Young and Calculated](https://youngandcalculated.substack.com/p/what-senior-pms-at-multi-managers)
- **Taleb barbell:**
  - 85–90% in ultra-safe assets such as cash, and 10–15% in many small speculative bets.
  - The worst case is a 10–15% haircut on total wealth. The design avoids the "mediocre middle" with its hidden tail risk.
  - Sources: [RBC Direct Investing](https://www.rbcdirectinvesting.com/learn/en/di/hubs/ideas-and-motivation/article/barbell-and-other-investing-strategies/mrqeu0ww); [SingSaver "90-10"](https://singsaver.com.sg/blog/barbell-strategy-the-90-10-rule-to-make-you-rich)
- **SEBI F&O study (FY25):**
  - 91% of individual equity-derivative traders lost money.
  - Net losses rose 41% year on year to ₹1,05,603 crore, from ₹74,812 crore. The average loss was about ₹1.1 lakh.
  - FY22–FY24: 93% lost, with aggregate losses above ₹1.8 lakh crore.
  - Loss rates by tenure: 94.4% of traders active for 2 years were in loss, 96% after 3 years, and 95.3% of those active for 5 consecutive years.
  - Sources: [Moneylife](https://moneylife.in/article/91-percentage-of-retail-traders-lost-money-in-derivatives-losses-in-fo-surged-41-percentage-to-rs105-lakh-crore-in-fy2425-sebi-study/77613.html); [Business Standard](https://www.business-standard.com/amp/markets/news/net-losses-of-traders-in-fo-widens-in-fy25-sebi-study-125070701221_1.html); [Value Research](https://www.valueresearchonline.com/stories/225398/average-trader-lost-rs-1-1-lakh-fo-fy25/)
- **Indian risk-free proxies (2026):**
  - RBI policy repo rate was 5.25% as of July 2026.
  - 91-day T-bill yield: 5.5586% (early June 2026), 5.2998% (mid-June), 5.3324% (late July), 5.2640% (late August 2026).
  - Source: the RBI "Current Rates" page via a search summary; verify on rbi.org.in — [RBI](https://www.rbi.org.in/)
- **Arbitrage funds:**
  - They hold ≥ 65% equity (cash-futures hedged), so they are taxed as equity funds.
  - As of July 2026 the category's 1-year return averaged about 6%, on ₹3.41 lakh crore AUM; 2026 calendar YTD return was 3.61%.
  - Net inflows were about ₹15,702 crore in May 2025 as rate cuts lowered short-duration debt yields.
  - Sources: [INDmoney](https://www.indmoney.com/mutual-funds/hybrid/arbitrage-funds); [PersonalFN](https://www.personalfn.com/dwl/Mutual-Funds/best-arbitrage-funds-your-alternative-to-investing-money-in-bank-fds); [Business Standard (Jul 2025)](https://www.business-standard.com/finance/investment/arbitrage-funds-low-risk-tax-efficient-6-month-horizon-125070800818_1.html)
- **Kelly sizing:**
  - Full Kelly has roughly a 1-in-2 chance of the account eventually halving.
  - Half Kelly (target 4×, ruin defined at 25%) gives about a 98.5% chance of reaching the target vs. 80% at full Kelly, and about 1.5% vs. 20% risk of ruin.
  - Practitioners use ½ or ¼ Kelly because over-betting costs much more growth than under-betting.
  - Sources: [LuxAlgo Kelly](https://www.luxalgo.com/library/concept/kelly-criterion/); [NexusFi thread](https://nexusfi.com/showthread.php?p=199000); [FlashAlpha](https://flashalpha.com/concepts/kelly-criterion)

### Inferences
- **Suggested individual blueprint (synthesis, not advice):**
  - **Core: about 70–90% of investable capital** in T-bills, liquid/overnight funds and arbitrage funds (with an emergency fund held separately). At about 5.3–6% it covers most inflation and funds the trading sleeve's drawdowns.
  - **Satellite: about 10–30%** as active trading capital. Within it, a new strategy goes live at about 10–25% of its intended size.
  - **Scaling rule:** scale up stepwise (e.g. 2× steps, toward the 3.6–5× scale-up seen at institutions) only after live P&L passes a MinTRL-style significance test and live slippage matches the model.
  - **Drawdown rules modelled on multi-manager practice:** halve risk at −5% to −7.5% from the high-water mark, stop the strategy at −10% to −15%, and re-paper-trade before restarting.
- **Arbitrage funds are not risk-free.** Spreads compress in calm markets (2026 YTD 3.61% vs. 6% trailing 1-year), but they rarely lose money over 3+ months, so they suit the "safe" leg for equity-tax reasons.

### Gaps
- No primary source was found for a standard institutional "% of capital to a new strategy" rule (e.g. 1–2% of portfolio).
- The Millennium rule comes from a secondary blog; no primary confirmation was found.
- Exact 2026 liquid-fund category yields were not retrieved.

## Q4. Historical stress scenarios: Indian markets (Nifty, Bank Nifty, India VIX, gaps, circuit breakers)

### Takeaway
Indian index stress is dominated by three things:
1. Single-day crashes of −10% to −13% with 45-minute market-wide halts (Jan 2008, Oct 2008, 13 & 23 Mar 2020).
2. Event-driven intraday collapses that open almost flat. On 4 June 2024 the Nifty opened −0.4% but hit −8.5% intraday; Bank Nifty closed about −7.95%.
3. Macro gap-downs: −4.9% Sensex open on 9 Nov 2016, about −5% open on 7 Apr 2025, and the US–Iran war drawdown of 2026.

Short-side traders face equally violent gap-ups: +5%+ on 20 Sept 2019, +3.8% on 12 May 2025, and +3% on 8 Apr 2026. India VIX extremes are 92.53 intraday (Nov 2008) and 86.63 (24 Mar 2020), and it can jump 40–60% in a day.

### Cited Findings

**Circuit-breaker mechanics (NSE/BSE, index-based, market-wide)**
- **Trigger levels:** 10%, 15% and 20% moves in either direction of the Nifty 50 or Sensex, whichever breaches first. A halt covers all equity and equity-derivative markets nationwide.
- **10% trigger:** before 1:00 pm, a 45-minute halt plus a 15-minute pre-open call auction. From 1:00 to 2:30 pm, a 15-minute halt plus pre-open. At or after 2:30 pm, no halt.
- **15% trigger:** before 1 pm, a 1 h 45 min halt plus pre-open. From 1 to 2 pm, 45 minutes plus pre-open. At or after 2 pm, the rest of the day.
- **20% trigger:** halt for the rest of the day.
- Sources: [NSE circuit breakers](https://www.nseindia.com/static/products-services/equity-market-circuit-breakers); [SEBI 2001 circular](https://www.sebi.gov.in/legal/circulars/jun-2001/index-based-market-wide-circuit-breaker-in-compulsory-rolling-settlement_17986.html); [Tradejini](https://www.tradejini.com/support/knowledge-base/trading-markets/trading-qs/circuit-breakers/what-are-market-wide-circuit-breakers)

**2008 GFC**
- **Peak-to-trough:** Nifty peaked at 6,357 on 8 Jan 2008 and bottomed at 2,252 on 27 Oct 2008, a fall of 65% — [Business Standard](https://www.business-standard.com/article/markets/bullish-signals-for-2010-110010400065_1.html); [Marketcalls 2008 report](https://www.marketcalls.in/nifty-technicals/nifty-overall-performance-report-for-the-year-2008.html)
- **January 2008 lower circuit:** the Sensex opened −9.75% (−1,716.41 to 15,888.94) and the Nifty −12.10% (−630.45 to 4,578.35). Both hit the lower circuit and trading was halted for one hour.
  - The blog post is dated Monday 21 Jan 2008 (US time). The Indian session was very likely 22 Jan 2008 IST (my inference).
  - Source: [Calculated Risk](https://calculatedriskblog.com/2008/01/india-sensex-and-nifty-hit-10-circuit.html)
- **24 Oct 2008:**
  - Nifty fell from 2,943.15 to 2,584.00 (−359.15), about −12.2% (derived).
  - Sensex fell 1,070.63 (−10.96%) to 8,701.07, then described as the worst day in Indian market history, after a disappointing RBI policy review.
  - Sources: [Oneindia](https://www.oneindia.com/2008/10/24/sensex-ends-below-9k-plunges-by-107063-1224849967.html); [Business Standard circuit-breaker explainer](https://www.business-standard.com/amp/article/markets/us-crash-what-is-a-circuit-breaker-in-a-stock-market-how-does-it-work-120030901196_1.html)
- **India VIX:** all-time intraday high 92.53 in Nov 2008, with the 2008 closing peak at 85.13 — [Business Standard](https://www.business-standard.com/amp/article/markets/covid-19-impact-india-vix-touches-86-63-level-breaches-2008-peaks-120032400788_1.html)

**Gap-up risk for shorts**
- **18 May 2009:** Sensex +2,110.79 points after the UPA election win, an upper-circuit day — [ETV Bharat](https://etvbharat.com/english/business/economy/5-biggest-intraday-gain-of-sensex/na20190920232742700)
- **20 Sept 2019:** the corporate tax cut to 22% sent the Sensex up 1,921.15 points. Sensex and Nifty each rose more than 5%, the biggest one-day gain since May 2009 — [Business Standard](https://www.business-standard.com/amp/article/markets/market-celebrates-corporate-tax-cut-with-biggest-rally-in-10-years-119092100074_1.html)
- **12 May 2025:** after the India–Pakistan ceasefire and US–China tariff truce, Nifty +916.70 (+3.82%) to 24,924.70 and Sensex +2,975.43 (+3.74%) to 82,429.90. It was the Nifty's best day in 4 years; Nifty IT rose 6.7% — [Samco](https://www.samco.in/knowledge-center/articles/sensex-news-and-nifty-news-markets-soar-on-india-pakistan-ceasefire-and-us-china-tariff-truce/); [Finology](https://ticker.finology.in/discover/market-update/indian-stock-market-today-12-may-2025)
- **8 Apr 2026:** Indian markets rallied more than 3% on the US–Israel–Iran ceasefire (headline only) — [News On AIR](https://www.newsonair.gov.in/indian-markets-rally-over-3-on-us-israel-iran-ceasefire)

**2016 demonetisation (announced evening of 8 Nov 2016; coincided with the US election result)**
- **9 Nov 2016 open and range:** the Sensex opened −1,339.76 points (−4.86%) at 26,251.38 against a 27,591.14 prior close, then traded between 27,397.38 and 25,902.45 (about 1,689 points intraday).
- **9 Nov close:** Sensex −338.61 (−1.23%) at 27,252.53; Nifty −111.55 (−1.31%) at 8,432.00, with an intraday range of 8,476.20–8,002.25. That implies an intraday Nifty low about −6.3% below the prior close (derived).
- Source for both: [Business Standard/PTI](https://www.business-standard.com/amp/article/pti-stories/mkt-crashes-on-trump-win-black-money-curb-ends-339-pts-down-116110901119_1.html)
- **Following weeks:** the Nifty lost 7.5% (650 points) to 7,965.60 by end-November, then hit a seven-month low of 7,908 on 26 Dec 2016, about −7% from above 8,500 — [Business Standard (one year on)](https://www.business-standard.com/amp/article/economy-policy/one-year-of-demonetisation-short-lived-scare-for-stock-markets-117110700022_1.html); [Business Standard (Nov 24, 2016)](https://www.business-standard.com/article/markets/sensex-nifty-post-biggest-loss-in-a-series-since-august-2013-116112400623_1.html)

**March 2020 Covid crash**
- **Peak-to-trough:** Nifty peaked at 12,430 intraday on 20 Jan 2020 (close 12,224) and hit 7,511 intraday on 24 Mar 2020, about −40%. The closing low was 7,610.25 on 23 Mar. March 2020 alone was −23.2% — [BloombergQuint](https://www.bloombergquint.com/markets/from-12168-to-7511-back-mapping-the-niftys-journey-in-2020); [Stable Investor](https://stableinvestor.com/2021/02/markets-100percent-march-2020.html)
- **12 Mar 2020:** the Sensex fell 2,919 points, then its biggest one-day point fall, and the Nifty hit a 33-month low — [Business Standard](https://www.business-standard.com/article/markets/market-live-markets-sensex-nifty-bse-nse-coronavirus-dow-jones-oil-120031200172_1.html)
- **13 Mar 2020:**
  - At 9:20 am the Nifty hit the 10% lower circuit, down 966 points (−10.07%) to 8,625.
  - Trading halted for 45 minutes and resumed with a pre-open at 10:05.
  - It was the first halt in 12 years and the first lower circuit since May 2009 (a reference to the May 2009 upper-circuit halt).
  - Sources: [Business Standard](https://www.business-standard.com/amp/article/markets/nifty-hits-lower-circuit-amid-global-sell-off-trading-halted-for-45-mins-120031300188_1.html); [Business Today](https://www.businesstoday.in/markets/stocks/sensex-nifty-crash-circuit-breaker-rules/story/398162.html); [Zerodha bulletin](https://zerodha.com/marketintel/bulletin/249418/trading-halted-at-the-exchange)
- **18 Mar 2020:** Bank Nifty fell 7% to 20,511, its lowest since 3 Mar 2017. On 13 Mar it had fallen more than 2,500 points intraday — [Business Standard](https://www.business-standard.com/amp/article/markets/bank-nifty-crashes-7-to-3-year-low-over-rising-coronavirus-pandemic-120031801892_1.html)
- **23 Mar 2020 (lockdown):**
  - The Sensex hit the 10% lower circuit (about −3,000 to 26,924) and the Nifty was down 842 points (−9.63%) to 7,903, triggering a 45-minute halt, the second in 10 days.
  - Close: Nifty −1,135.20 (−12.98%) to 7,610.25, its record one-day fall. Sensex −13.15% to 25,981.24.
  - Nifty Private Bank fell 17.88%; HDFC Bank was down as much as 13.4%. About ₹14 trillion of market cap was erased.
  - Sources: [Business Standard (halt)](https://www.business-standard.com/amp/article/markets/sensex-hits-10-lower-circuit-trading-halted-for-45-minutes-120032300237_1.html); [Business Standard ("biggest crash ever")](https://www.business-standard.com/article/markets/biggest-crash-ever-makes-india-worst-performing-market-in-the-world-120032301809_1.html); [Khaleej Times/Reuters](https://www.khaleejtimes.com/business/markets/banking-auto-indian-shares-sink-as-investors-go-on-selling-frenzy?amp=1)
- **Bank Nifty, week of 23 Mar 2020:** −4,848.85 points (−19.27%) to 20,317.60, with a weekly range of 24,074.15–18,675.65. Low confidence: attribution among the search results was unclear — [EquityPandit](https://www.equitypandit.com/?p=77782)
- **24 Mar 2020:** India VIX hit 86.63, above the 2008 closing peak of 85.13 — [Business Standard](https://www.business-standard.com/amp/article/markets/covid-19-impact-india-vix-touches-86-63-level-breaches-2008-peaks-120032400788_1.html)

**4 June 2024 (Lok Sabha results day)**
- **Nifty:** opened 23,179.50 against a 23,263.90 prior close, a gap of only −0.36% (derived). It fell as much as 8.5% intraday to 21,281.45, then closed −1,379 points (−5.93%) at 21,884.50, its worst day since early 2020 — [OnePercentClub](https://news.onepercentclub.io/stock-market/indian-stock-market-dips-as-election-results-diverge-from-poll-predictions/13737/); [Business Standard](https://www.business-standard.com/amp/markets/news/sensex-plunges-over-4-000-points-intraday-check-factors-behind-the-fall-124060400591_1.html); [Angel One](https://www.angelone.in/news/share-market/election-day-chaos-indian-stock-market-crashes-to-record-lows)
- **Bank Nifty:** opened 312 points lower at 50,667 and closed at 46,928, about −7.95% from the implied prior close of about 50,979 (derived) — [5paisa](https://5paisa.com/news/election-results-2024-sensex-nifty-50-cracks-investors-loose); [Liquide](https://blog.liquide.life/post-market-summary-4th-june-2024/)
- **India VIX, sources conflict:**
  - +24%, or about +40% to above 29 — [OnePercentClub](https://news.onepercentclub.io/stock-market/indian-stock-market-dips-as-election-results-diverge-from-poll-predictions/13737/); [5paisa](https://5paisa.com/news/india-vix-jumps-39)
  - +34% to above 28, or up to about 31 intraday — [Angel One](https://www.angelone.in/news/market-updates/india-vix-comparing-this-volatility-with-previous-major-events)
  - The day before (exit-poll rally), VIX fell more than 20% — [Angel One](https://www.angelone.in/news/share-market/india-vix-plummets-20-percent-following-exit-poll-results)

**5 Aug 2024 (global yen-carry unwind)**
- Nifty −662 points (−2.68%) to 24,055.6; Sensex −2,222.5 points (−2.74%).
- India VIX closed at 20.37, up 42.23%, after rising 61% intraday to 23.15, its largest rise since 2015.
- Sources: [Business Standard live blog](https://business-standard.com/markets/news/stock-market-live-updates-on-august-5-gift-nifty-sensex-nifty-nikkei-kospi-nasdaq-sbi-divis-labs-124080500059_1.html); [Liquide](https://blog.liquide.life/post-market-summary-5th-august-2024/); [Angel One pre-market 6 Aug](https://angelone.in/blog/pre-market-updates-06-august-2024)

**Sept 2024–Apr 2025 correction**
- **Peak and trough:** Nifty peaked near 26,277 on 27 Sept 2024 (record close 26,216.05 on 26 Sept 2024) and fell about 17.3% to about 21,743 — [Elliott Wave Forecast](https://elliottwave-forecast.com/news/nifty-elliott-wave-forecast-impulsive-sequence-approaching-end/); [Business Today](https://businesstoday.in/markets/trending-stocks/story/sensex-nifty-lose-from-record-highs-within-a-year-analysts-turn-cautious-558827-2026-09-30)
- **Timing conflict:** the source dates the 21,743 low to "early March 2025". My recollection is that 21,743.65 was the 7 Apr 2025 intraday low. Treat the trough date as uncertain.
- **Losing streak:** on 4 Mar 2025 the Nifty logged its longest-ever losing streak and closed at its lowest since June 2024 — [Business Standard](https://www.business-standard.com/markets/news/nifty-50-logs-longest-ever-losing-streak-closes-at-lowest-since-june-2024-125030400805_1.html)

**7 Apr 2025 (US "reciprocal tariff" shock)**
- **Open:** Sensex more than 5% lower at 71,449.94; Nifty about 5% lower at 21,758.4 (reported as −5.07%).
- **Early-trade sector moves:** Nifty Metal −7.03%, IT −5.34%, Smallcap 100 −5.64%, Midcap 100 −4.49%.
- **India VIX:** up more than 59% to 21.94.
- Sources: [Outlook Money](https://www.outlookmoney.com/amp/story/invest/big-stock-market-crash-nifty-50-sensex-nosedive-amid-global-selloff-triggered-by-trump-tariff-jitters); [Rachana Ranade](https://www.rachanaranade.com/blog/stock-market-crash-understanding-the-nifty-s-fall-on-april-7-2025); [HSBC MF Monday flash](https://www.assetmanagement.hsbc.co.in/assets/documents/mutual-funds/en/ab3c035e-446f-4ee5-a159-d34870674405/monday-market-flash-april-7-2025.pdf)

**2026: US/Israel–Iran war shock (conflict began 28 Feb 2026)**
- **Start and early losses:** the war began on 28 Feb 2026, with cumulative Sensex/Nifty losses of about 10.6% thereafter (summary attribution of the date was ambiguous). A crash on 2 Mar 2026 is reported (headline) — [HDFC Sky](https://hdfcsky.com/news/bse-nse-sensex-july-22-2026-close-report-sensex-nifty-fall-as-oil-and-iran-war-escalate); [Navbharat Live](https://navbharatlive.com/business/share-market-crash-march-2-2026-sensex-nifty-iran-israel-war-impact-1590024.html)
- **Pre-war peak:** Nifty peaked at 26,373 on 5 Jan 2026 and stood at 23,114 on 20 Mar 2026, a drawdown of about 14.09% — [Finnovate "India VIX in 2026"](https://www.finnovate.in/learn/blog/india-vix-2026-what-fear-index-tells-investors); [Wright Research drawdown analysis](https://www.wrightresearch.in/blog/what-history-tells-us-about-indian-stock-market-corrections/)
- **19 Mar 2026:**
  - Nifty −775.65 (−3.26%) to 23,002.15; Sensex −2,496.89 (−3.26%) to 74,207.24.
  - BSE MidCap 150 −3.04%, SmallCap 250 −2.58%.
  - India VIX +21.79% to 22.80.
  - Brent (May contract) +$7.51 (+6.99%) to $114.89 after attacks on Middle East energy infrastructure, plus a hawkish Fed. About ₹12 lakh crore of market value was wiped out.
  - Sources: [Arihant Capital](https://www.arihantcapital.com/News/News-Details/1682376/investor-attention); [Swarajya](https://swarajyamag.com/amp/story/economy/sensex-crashes-nearly-2500-points-rs-12-lakh-crore-wiped-out-as-oil-surges-past-110-amid-west-asia-tensions)
- **23 Mar 2026:** Nifty −601.85 (−2.60%) to 22,512.65; Sensex −1,836.57 (−2.46%) to 72,696.39. India VIX hit 27.17 intraday, up from 13.70 in under a month and its highest since June 2024 — [The Week](https://www.theweek.in/news/biz-tech/2026/03/23/stock-market-crash-sensex-nifty-plunge.html); [Multibagg](https://www.multibagg.ai/market-pulse/articles/sensex-falls-nifty-22500-vix-cmov3fw4ph27opl0jciduio23); [Finnovate](https://www.finnovate.in/learn/blog/india-vix-2026-what-fear-index-tells-investors)
- **30 Mar 2026:**
  - Sensex opened −1,018 (−1.38%) at 72,565.22; Nifty −269.95 (−1.18%) at 22,549.65. Both closed more than 2% lower on the last trading day of FY26.
  - FPIs sold about ₹21,000 crore in 4 days.
  - Nifty YTD was about −11%, about 7 points of it in March.
  - Sources: [Outlook Money](https://www.outlookmoney.com/invest/sensex-nifty-fall-at-open-nikkei-kospi-crash-up-to-5-percent-as-us-israel-iran-war-continues); [Kotak Neo](https://www.kotakneo.com/news/market-news/post-market-30-march-2026-sensex-nifty-fall-2-percent/)
- **8 Jul 2026 (headline figures conflict):**
  - "Worst single-day crash since March": Sensex −1,923 points, Nifty −594 points, about ₹9 lakh crore wiped out — [Goodreturns](https://www.goodreturns.in/news/sensex-nifty-worst-single-day-crash-since-march-stock-market-crash-8-7-2026-rs-9-lakh-crore-wiped-1520827.html)
  - Nifty −2.4% with VIX +26% (another summary said +28%) after US strikes on Iran — [Bajaj Broking](https://www.bajajbroking.in/share-market-news/nifty-drops-2-4percent-on-us-iran-conflict-vix-surges-26-percent)
  - In one escalation Brent jumped about 21% to $112 in early Asian trade, and the rupee fell 50 paise to 91.47, its sharpest one-day drop since late January (date not pinned down) — [HDFC Sky](https://hdfcsky.com/news/bse-nse-sensex-july-22-2026-close-report-sensex-nifty-fall-as-oil-and-iran-war-escalate)
- **Late Sept 2026:**
  - On 29 Sept 2026 India VIX rose 6.6% to 14.54 as the Nifty fell below 22,700 — [HDFC Sky](https://hdfcsky.com/news/india-vix-rises-6-6percent-to-14-54-as-oil-us-yields-and-nifty-selling-lift-volatility-september-29-2026)
  - On 30 Sept 2026 Business Today reported Sensex/Nifty down as much as 16% from record highs set within the past year (Nifty about 22,125, Sensex about 73,198) — [Business Today](https://businesstoday.in/markets/trending-stocks/story/sensex-nifty-lose-from-record-highs-within-a-year-analysts-turn-cautious-558827-2026-09-30)
  - "Nifty down 13% in 2026 even as EPS rises" (headline) — [Multibagg](https://www.multibagg.ai/market-pulse/articles/nifty-down-eps-rising-cmunptox200232zt8ibk0ubnv)

**Overnight and single-stock gaps (swing-trader risk)**
- **Largest index gaps, low confidence and conflicting definitions:** one data summary puts the largest Nifty gap-down at −7.96% (1999) and the largest Bank Nifty gap-down at −6.37% (2008). It also counts only eight cases of two consecutive >1% Nifty gap-downs (European debt crisis 2011, March 2020, the 2022 Russia–Ukraine war) — [TradingQnA](https://tradingqna.com/t/trading-the-gap-how-does-one-trade-nifty-bank-nifty-during-gap-down-and-gap-up-days/78838); [Samco](https://www.samco.in/knowledge-center/articles/two-consecutive-gap-downs-in-nifty-50-what-history-suggests-about-market-direction/)
  - This conflicts with Jan 2008, when the Nifty traded −12.1% at the open and hit the circuit. The figures likely reflect different open-print definitions.
- **IndusInd Bank, 11 Mar 2025:** −27% in one day (about ₹20,000 crore of market cap) after disclosing derivative-accounting discrepancies worth about 2.35% of net worth (about ₹1,529 crore). It was down 31% for March — [Moneylife](https://www.moneylife.in/article/indusind-bank-crashes-27-percentage-on-severe-discrepancies-in-derivatives-portfolio/76611.html); [Angel One](https://www.angelone.in/news/share-market/indusind-bank-share-price-recovers-but-still-down-31-percent-in-march)
- **Adani Enterprises, 21 Nov 2024:** −23% (Adani Green −19%) after the US bribery/fraud indictment. The group lost about $27bn of market value in a day — [Business Today](https://businesstoday.in/markets/stocks/story/adani-enterprises-shares-crack-23-today-heres-what-analysts-say-454490-2024-11-21); [AP via ClickOnDetroit](https://www.clickondetroit.com/news/world/2024/11/21/shares-in-indias-adani-group-plunge-20-after-us-bribery-fraud-indictments/)

### Inferences
- **Minimum India stress set for an agent (synthesis of the above):**
  - (a) A −13% index day with a 45-minute halt mid-session, so stops can't execute. Use 23 Mar 2020 (Nifty −12.98%, Bank Nifty/Private Bank about −18%).
  - (b) A −10% open with no fill until the halt ends: Jan 2008 and 13 Mar 2020.
  - (c) An election-style intraday −8.5% from a near-flat open, with India VIX +30–50% intraday (option premiums explode): 4 June 2024.
  - (d) A −5% macro gap open: 9 Nov 2016 (Sensex −4.9%) and 7 Apr 2025.
  - (e) A +4% to +5% short-squeeze gap: 20 Sept 2019 and 12 May 2025.
  - (f) A single-stock −20% to −27% overnight gap: Adani Nov 2024, IndusInd Mar 2025.
  - (g) A slow grind: −16% to −17% over 6 months (2024–25), −14% to −16% in 2026, and −65% over 10 months (2008).
  - (h) A VIX regime shift from about 13–14 to 27–29 within weeks (2026, 2024), up to 86–92 in true crises.
- **Implication:** stop-losses don't bound overnight or halt risk for swing traders. Position size must assume the stop executes 5–10% beyond its level on an index and 20–27% beyond it on a single stock.

### Gaps
- The exact Bank Nifty all-time worst single-day % (likely 23 Mar 2020) and its March 2020 peak-to-trough % were not confirmed. One search asserted 32,613 → 16,116 (about −50.6%) but the trough was unverified.
- The 7 Apr 2025 closing figures (believed to be about −3.2%) and 13 Mar 2020 close-of-day figures were not confirmed.
- Reliable lists of the largest Nifty overnight gaps are inconsistent across sources.
- The exact date of India VIX's 2026 high (one source says a 52-week high of 28.91) is unconfirmed.

## Q5. Historical stress scenarios: crypto (crashes, liquidation cascades, stablecoin depegs), 2020–2026

### Takeaway
- **Single-day price shocks:** BTC has had 24-hour drops of about −37% to −50% (12 Mar 2020) and about −30% intraday (19 May 2021). The modern ETF-era BTC still had −13% to −15% days in Oct 2025 and Feb 2026.
- **Altcoins** can drop 60–90% in minutes: Oct 10–11 2025 saw a mean daily drop above −60% across the top 1,500 coins.
- **Liquidation cascades** have grown in scale: about $1bn (Mar 2020), more than $8bn (May 2021), more than $2.2bn (Feb 2025), more than $19bn across 1.6m accounts (Oct 2025).
- **Stablecoin depeg history:** USDC to $0.87–0.88 (Mar 2023, 3 days), USDe to $0.65 on Binance only (Oct 2025, about 90 minutes), xUSD to $0.30 (Nov 2025), Resolv USR to about $0.27 (Mar 2026), and UST/LUNA's total collapse (May 2022).
- **2026 cycle:** BTC's drawdown from its about $126k ATH (6 Oct 2025) reached about 52% by mid-2026.

### Cited Findings
- **12 Mar 2020 ("Black Thursday"):**
  - BTC fell from about $7,900–8,000 to about $3,600 within 24 hours. Different measures give a day change of −37.5% to −40%, the worst day since 2013. About $1bn of longs was liquidated.
  - ETH fell 43% ($194 → $111), its largest single-day loss (Glassnode); other reports say about −60%. BNB fell about 65%. The S&P 500 fell 9.5% that day.
  - Sources: [Decrypt](https://decrypt.co/26710/major-cryptos-recover-covid-crash); [Cointelegraph Magazine](https://cointelegraph.com/magazine/black-thursday-anniversary-can-crypto-markets-see-another-huge-crash); [Bloomberg](https://www.bloomberg.com/news/articles/2020-04-28/bitcoin-s-most-volatile-day-prompts-exchanges-to-make-changes); [Glassnode](https://insights.glassnode.com/what-really-happened-to-makerdao); [Binance Square (BitMEX 25-min outage)](https://www.binance.com/ar/square/post/350135015472097)
- **18–19 May 2021 (China ban headlines, Tesla stops accepting BTC):**
  - BTC flash-crashed to about $38k, bounced to about $41k, then plunged to about $30k, about −33% "overnight".
  - More than $8bn of positions was liquidated across about 700k–900k traders.
  - BTC recovered from $30k to $40,442 in under 4 hours.
  - Sources: [CryptoSlate](https://cryptoslate.com/bitcoin-dumps-to-38500-2-billion-in-crypto-liquidations-as-market-drops/); [GetBlock](https://getblock.io/blog/bitcoin-btc-is-under-30k-long-live-bitcoin/)
- **Terra/LUNA, 9–12 May 2022:**
  - The collapse ran over 9–11 May. UST had first slipped to about $0.98 on 8 May after Anchor's deposit rate was cut below about 18%.
  - Luna Foundation Guard's (LFG) BTC reserves fell from about 80,000 BTC to 313 BTC.
  - LUNA fell more than 99%. UST's low is reported as $0.078 (Forklog) or $0.0225 (The Paypers), a conflict.
  - On 12 May BTC fell to about $26,700, or $26,297 on CoinGecko (lowest since 28 Dec 2020). BTC lost 13.5% in 24 hours and ETH more than 21%, testing $1,800.
  - Futures liquidations were $1.23bn in a day and DeFi lending liquidations $170.84m. Total crypto market cap fell about $570bn from 7 to 12 May. USDT briefly slipped below $0.99.
  - Sources: [Forklog](https://forklog.com/en/terras-death-spiral-how-and-why-luna-and-ust-collapsed/); [The Paypers](https://thepaypers.com/cryptocurrencies/luna-drops-to-new-market-lows-on-12-may-2022--1256303); [Forklog price](https://forklog.com/en/bitcoin-price-slips-below-27000-ethereum-tests-1800/); [Cointelegraph](https://cointelegraph.com/markets/bitcoin-falls-below-27k-to-december-2020-lows-as-tether-stablecoin-peg-slips-under-99-cents); [CoinDesk timeline](https://coindesk.com/learn/the-fall-of-terra-a-timeline-of-the-meteoric-rise-and-crash-of-ust-and-luna)
- **FTX, Nov 2022:**
  - Timeline: CoinDesk's Alameda balance-sheet story on 2 Nov; CZ announced Binance would sell its FTT on 6 Nov; FTX filed for bankruptcy on 11 Nov with an $8bn hole.
  - FTT went from $24.01 (6 Nov) to $2.10 (13 Nov). Sources say −90.4%; −91.3% by my calculation.
  - BTC hit $15,480 on 22 Nov, its lowest since Nov 2020, and Bloomberg cites about $15.6k. The crypto market lost more than $260bn from 6 Nov.
  - Sources: [The Block timeline](https://www.theblock.co/post/186132/ftx-collapse-timeline-six-days-that-rocked-the-crypto-industry); [Scorechain](https://scorechain.com/blog/the-collapse-of-ftx); [CNBC](https://www.cnbc.com/2022/11/21/bitcoin-btc-ether-eth-fall-as-ftx-collapse-ripples-through-market.html); [Bloomberg](https://www.bloomberg.com/news/articles/2022-11-22/bitcoin-s-slide-pauses-in-wait-for-next-domino-to-fall-after-ftx); [Forkast](https://forkast.news/bitcoin-price-falls-to-more-than-two-year-low-amid-growing-concerns-about-ftx-fallout/amp/)
- **USDC/SVB, 10–13 Mar 2023:**
  - Circle disclosed $3.3bn, about 8% of about $40bn reserves, held at the failed Silicon Valley Bank.
  - USDC fell to about $0.87–0.88 overnight on 11 Mar, and DAI also traded at about $0.90. USDC's market cap fell below $40bn.
  - The peg was restored in about 3 days after Circle pledged to cover any shortfall.
  - Sources: [The Block](https://www.theblock.co/post/218993/usdc-and-dai-remain-at-about-0-90-following-circles-disclosure-of-funds-at-svb); [21Shares](https://21shares.com/research/newsletter-issue-193); [Forklog](https://forklog.com/en/usdc-loses-peg-to-the-dollar-amid-svb-collapse/)
- **5 Aug 2024 (yen-carry unwind):**
  - After a BoJ hike, USD/JPY moved from about 153 to about 145 from 31 July.
  - The Nikkei 225 fell 12.4% and the Topix had its worst day since 2011.
  - BTC fell to about $49,000 (lowest since mid-February). BTC and ETH dropped as much as 18% and 26% respectively within hours; BTC was down about 15% against JPY.
  - Crypto liquidations were about $950m to more than $1bn.
  - Sources: [CoinDesk](https://coindesk.com/markets/2024/08/05/bitcoin-drops-15-against-japanese-yen-outpacing-declines-versus-usd-as-yen-carry-trades-unwind); [BIS Bulletin 90](https://www.bis.org/publ/bisbull90.pdf); [FXStreet](https://www.fxstreet.com/amp/cryptocurrencies/news/how-the-bank-of-japan-wrecked-the-yen-carry-trade-and-crypto-markets-202408060601); [CryptoSlate](https://cryptoslate.com/insights/leveraged-short-positions-close-to-1-billion-liquidation-as-bitcoin-climbs/)
- **3 Feb 2025 (US tariffs on Canada, Mexico and China):**
  - More than $2.2bn was liquidated: $1.87bn of longs and $345m of shorts (CoinGlass), then called the largest single-day liquidation ever.
  - ETH fell to about $2,500 (about −20% in 24 hours); BTC dropped to $91,200 (−6.5% on the day).
  - Feb 2025 also saw about $500m of ETH DeFi liquidations, the second-largest in DeFi history.
  - Sources: [crypto.news](https://crypto.news/crypto-crash-bitcoin-ethereum-liquidation/); [The Block](https://www.theblock.co/post/344276/market-crash-triggers-500-million-in-eth-liquidations-second-largest-in-defi-history)
- **10–11 Oct 2025 (liquidation cascade after the 100% China tariff announcement):**
  - More than $19bn of leveraged positions was liquidated in under 24 hours across more than 1.6m accounts (CoinGlass), about 1.6m within 6 hours. That is 9× the Feb 2025 event and 19× the Mar 2020 and FTX events.
  - BTC fell from $122,574.46 to $104,782.88 (more than −14%); ETH fell 12.2% to $3,436.29. Another source puts the single-day moves at BTC −13% and ETH more than −16%.
  - ETH and SOL fell about 20%. Small tokens fell 80–90%, and the mean daily drop across the top 1,500 coins exceeded −60%.
  - On Binance, USDe fell to about $0.65–0.66, and wBETH and BNSOL fell further. About 75% of liquidations happened before USDe deviated. USDe held its peg on Curve and in mint/redeem.
  - The USDe dislocation lasted about 90 minutes. Ethena processed about $2bn of redemptions on 10–11 Oct, and USDe's market cap fell from $14.8bn to $12.6bn by 12 Oct.
  - Hyperliquid ADL figures are in Q2.
  - Sources: [CoinGecko explainer](https://www.coingecko.com/learn/october-10-crypto-crash-explained); [CoinShares](https://coinshares.com/at/insights/knowledge/billions-in-liquidations-what-happened/); [FTI Consulting](https://www.fticonsulting.com/zh-cn/china/insights/articles/crypto-crash-october-2025-leverage-met-liquidity); [Sentora](https://sentora.com/research/articles/structural-shifts-in-crypto-the-black-friday-drawdown-and-hyperliquid-s-bid-to-challenge-cexs); [21Shares](https://www.21shares.com//research/why-did-ethenas-stablecoin-remain-stable-onchain-but-depegged-on-binance); [Bloomberg Gov](https://news.bgov.com/crypto/third-largest-stablecoin-briefly-loses-dollar-peg-in-crypto-rout)
- **Stream Finance xUSD, 3–4 Nov 2025:** an external fund manager disclosed a $93m loss and withdrawals and deposits were suspended. xUSD fell about 77% to a record low of $0.30 on 4 Nov, and Stream's TVL dropped from about $204m to about $98m — [Decrypt](https://decrypt.co/347285/stream-finance-stablecoin-plunges-77-protocol-fund-manager-loses-93-million); [Forklog](https://forklog.com/en/news/stream-finance-halts-operations-following-93-million-loss)
- **2026 events:**
  - **Cycle peak:** BTC's ATH was about $126,080 (also cited as $126,198) on 6 Oct 2025 — [Newhedge drawdown](https://ws.newhedge.io/bitcoin/price-drawdown); [Datawallet](https://www.datawallet.com/crypto/bitcoin-statistics)
  - **30 Jan 2026:** BTC fell below $83,000 with $1.68bn liquidated: Hyperliquid $598m, Bybit $339m, Binance $181m — [Cointelegraph](https://cointelegraph.com/news/bitcoins-crash-to-65k-triggers-18b-in-crypto-liquidations)
  - **5 Feb 2026, source figures differ:**
    - Described as BTC's worst single day since FTX: about −13%, from about $70k to a session low of about $62.4k, with some reports saying about $60k.
    - Realized losses hit a record $3.2bn, above the Terra episode.
    - Total crypto market cap fell from $2.65tn to $2.49tn (−6.4%), with more than $800m liquidated across about 165k traders.
    - The week's fall was −19.3% to $59,100 with $1.75bn liquidated. On 6 Feb BTC hit a multi-year low below $60,000.
    - Sources: [MEXC news (aggregator; lower quality)](https://www.mexc.com/news/695664); [Gigazine](https://gigazine.net/gsc_news/en/20260206-bitcoin-70k-usd); [Cointelegraph](https://cointelegraph.com/news/bitcoins-crash-to-65k-triggers-18b-in-crypto-liquidations)
  - **Resolv USR, 22 Mar 2026:** fell 73% in 24 hours to about $0.27, after unbacked mints in an "overcollateralized, ETH-backed" design. 2026 added six stablecoin collapses, taking the total to 36 collapses with measurable loss since 2022 and $2.5bn destroyed — [Webacy](https://www.webacy.com/blog/stablecoin-depeg-failure-mechanisms)
  - **June 2026 (US–Iran war escalation):** BTC fell 8% from $71,300 to a nine-week low of $65,360. $1.58bn of longs and $1.83bn in total were liquidated, the largest since 6 Feb — [Cointelegraph](https://cointelegraph.com/news/bitcoins-crash-to-65k-triggers-18b-in-crypto-liquidations)
  - **Cycle drawdown:** a 52% peak-to-trough drawdown "bottomed near $58,500 in June", the shallowest BTC bear market (earlier ones were −77% and −84%). BTC was 47.3% below ATH on 23 July 2026 and about $85,300 in late Sept 2026 (about −32%) — [Newhedge](https://ws.newhedge.io/bitcoin/price-drawdown); [Cointelegraph "Bitcoin price in 2026"](https://cointelegraph.com/news/bitcoin-price-in-2026-predictions-vs-charts-and-reality)
    - Conflict: Feb lows of about $59.1k and a June bottom of about $58.5k are both reported. $58.5k against a $126,080 ATH is −53.6% (derived).
  - **2 Aug 2026:** stablecoin supply fell by about $15bn (USDT about $189bn → $183.2bn; USDC about $80bn → $72.1bn), attributed to the GENIUS Act's ban on yield payments. Both stayed at about $1. Attribution is uncertain and the source is a MEXC repost — [MEXC news](https://www.mexc.com/news/973170)

### Inferences
- **Minimum crypto stress set for an agent (synthesis):**
  - (a) BTC −40% to −50% in 24 hours, ETH −43% to −60%, with the main derivatives venue offline for 25 minutes and on-chain gas so congested that collateral top-ups fail (Mar 2020).
  - (b) BTC −30% intraday with a V-shaped 4-hour recovery that whipsaws stops (May 2021).
  - (c) BTC −14% and alts −60% to −90% in minutes, with USD-pegged collateral marked at $0.65 on your venue and ADL closing your hedges (Oct 2025).
  - (d) A slow venue failure: an exchange token −90% in a week and withdrawals frozen (FTX). Any capital on that venue is a total loss.
  - (e) A "safe" stablecoin at $0.87 for 3 days (USDC), and synthetic or yield stablecoins going to $0.27–0.30 permanently (USR, xUSD).
  - (f) A macro/geopolitical cross-asset shock: BTC −15% to −20% alongside the Nikkei −12% (Aug 2024), or −8% to −13% on war or tariff news (2025–26).
- **Implication:** the scale of liquidation events has kept growing (about $1bn → $19bn) even as BTC's cycle drawdowns have become shallower. Venue and collateral risk now matter as much as price risk.

### Gaps
- An authoritative BTC intraday low for 10–11 Oct 2025 by venue was not found (Binance reportedly traded lower than the composite figure).
- Precise ETH figures for 5 Aug 2024 were not confirmed.
- Sources conflict on BTC's exact 2026 trough (Feb vs. June, $58.5k–$60k).
- I found no primary-source data from Kaiko or The Block on order-book depth collapse in Oct 2025 (fetch blocked).

## Q6. Monte Carlo / bootstrap stress-testing and probability-of-ruin estimation

### Takeaway
Allocators and quants stress an equity curve in several ways:
- **Resample trades or returns.** Use a simple reshuffle for independent trades, or a stationary/block bootstrap (Politis-Romano) when returns are autocorrelated. This yields distributions, not point estimates, of max drawdown, time under water and ruin probability. Size off the 90th–95th percentile drawdown.
- **Correct for selection bias** with Deflated Sharpe, PBO/CSCV and MinTRL.
- **Overlay historical shock days** (Q4/Q5).

Fractional Kelly (½ or ¼) cuts ruin probability sharply at modest cost to growth.

### Cited Findings
- **Trade-sequence Monte Carlo:**
  - Reshuffle or resample the backtest's trade results thousands of times to get the distribution of equity paths. The main output is the max-drawdown distribution at the 50th, 80th, 90th and 95th percentiles, and position sizing should come from these tail percentiles.
  - "Path risk" means identical trade sets can produce very different drawdowns depending on order.
  - Probability of ruin = the share of simulated paths that hit the ruin threshold under a given stopping rule.
  - Sources: [NexusFi Monte Carlo](https://nexusfi.com/a/risk-management/monte-carlo-simulation); [NexusFi risk of ruin](https://nexusfi.com/a/risk-management/risk-of-ruin); [Backtrex](https://backtrex.com/en/blog/probability-of-ruin-trading-monte-carlo); [MQL5 blog](https://www.mql5.com/en/blogs/post/774072)
- **Stationary bootstrap (Politis & Romano):**
  - Resamples circular, overlapping blocks of random, geometrically distributed length with mean 1/p. This preserves short-range dependence such as autocorrelation and volatility clustering, which i.i.d. bootstraps destroy. It is the default for financial returns.
  - Automatic block-length selection is given by Politis-White and its Patton-Politis-White (2009) correction.
  - Sources: [Portfolio Optimizer](https://portfoliooptimizer.io/blog/bootstrap-simulation-with-portfolio-optimizer-usage-for-financial-planning/); [Univ. of Cyprus record](https://gnosis.library.ucy.ac.cy/handle/7/57533); [Patton, Politis & White 2009](https://public.econ.duke.edu/~ap172/Patton_Politis_White_2009.pdf); [MathWorks implementation](https://www.mathworks.com/matlabcentral/fileexchange/101929-stationary-bootstrap/)
- **Overfitting controls:** PBO via CSCV gives the probability of backtest overfit, performance degradation and probability of loss. DSR and MinTRL are covered in Q1 — [SSRN 2326253](https://papers.ssrn.com/abstract=2326253); [R pbo](https://packages.oit.ncsu.edu/cran/web/packages/pbo/readme/README.html); [SSRN 2460551](https://papers.ssrn.com/abstract=2460551)
- **Kelly and ruin:**
  - Kelly f* = W − (1−W)/R, or (b·p − q)/b for discrete bets; f* = (μ − r_f)/σ² for continuous returns.
  - Full-Kelly expected max drawdown is roughly 50–80%. Half Kelly cuts ruin probability from about 20% to about 1.5% in a 4×-target / 25%-ruin set-up.
  - With Kelly-scaled fixed-fractional betting, ruin depends only on the ruin fraction, the target multiple and the Kelly fraction, not on expectancy.
  - Sources: [LuxAlgo](https://www.luxalgo.com/library/concept/kelly-criterion/); [NexusFi thread](https://nexusfi.com/showthread.php?p=199000); [FlashAlpha](https://flashalpha.com/concepts/kelly-criterion)
- **Transaction-cost stress:** square-root impact σ√(Q/V) for capacity and slippage scenarios (see Q1) — [Bouchaud](https://bouchaud.substack.com/p/the-square-root-law-of-market-impact)

### Inferences
- **Recommended stress-test protocol for the trading agent (synthesis):**
  1. **Bootstrap:** use a stationary block bootstrap of daily strategy returns (mean block about 5–20 days) for at least 10,000 paths over a 1–3-year horizon. Report the median and 95th-percentile max drawdown, longest time under water, and the probability of touching −10%, −20% and the ruin level (e.g. −30%).
  2. **Trade shuffle:** reshuffle trades for intraday strategies, with i.i.d. resampling as a check. Also resample with the win rate cut 5–10 points and costs doubled.
  3. **Scenario overlay:** splice the Q4/Q5 shock days into random points of each path. Execution rules:
     - halts mean no exit for 45 minutes;
     - gaps mean the stop fills at the open;
     - Oct 2025 means alt collateral at −60% and USDe at $0.65;
     - IV spikes mean option-short P&L moves on VIX +40–60%.
  4. **Selection-bias controls:** compute DSR and PBO given the number of variants tried, and MinTRL for the live record.
  5. **Acceptance:** for example, P(ruin) < 1% and P(−20%) < 5% over 12 months at intended size; otherwise scale size down linearly with the 95th-percentile drawdown.
- **Closed-form check (standard continuous-time result, not retrieved from a source this session; verify):** under growth-optimal (full) Kelly, the probability of ever falling to fraction x of current wealth is about x. Under fractional Kelly c it is about x^(2/c − 1), so for halving:
  - c = ½: about 12.5%
  - c = ¼: about 0.8%

  This is consistent with the cited "roughly ½ chance of halving at full Kelly".

### Gaps
- I found no peer-reviewed source giving a standard institutional "acceptable probability of ruin" threshold. The 1–5% figures above are judgment.
- López de Prado's combinatorial purged cross-validation (CPCV) and the synthetic-data backtesting literature were not retrieved this session.
- No source was found comparing bootstrap-estimated vs. realized live drawdowns for retail Indian or crypto strategies.
