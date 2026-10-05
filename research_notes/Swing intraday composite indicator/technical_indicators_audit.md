# Tested evidence for retail technical indicators and chart methods (audit, as of Oct 2026)

Method note: about 18 search/fetch calls. Several primary hosts (ideas.repec.org, diglib.natlib.lk, ijie.um.edu.my, SSRN) were blocked by the egress proxy, so some findings rest on abstracts and search snippets, not full text. Items marked "[training knowledge, not re-verified this session]" come from the researcher's background knowledge and should be checked before they are quoted as fact.

## Audit table: indicators and methods vs the best-quality test found

| Indicator / method | Best-quality test found | Market / period | Costs? | Data-snooping correction | Result |
|---|---|---|---|---|---|
| MA crossovers, filter rules, channel breakouts, S/R (BLL universe) | Sullivan, Timmermann & White (1999, JF) | DJIA daily 1897-1986 in-sample; 1987-1996 out-of-sample (OOS) | Gross mainly | White's Reality Check (RC) over ~7,846 rules | Best rule significant in-sample. **OOS 1987-96: best rule 8.63%/yr, not significant before costs** |
| Same universe, extended | Bajgrowicz & Scaillet (2012, JFE) | DJIA daily 1897-2011 | Yes, low costs | FDR (multiple testing) + persistence tests | FDR finds more outperforming rules than RC. **Even in-sample, low transaction costs wipe out performance.** Ex-ante selection of future winners impossible |
| ~21,195 rules: filters, MAs, S/R, channel breakouts, oscillators (RSI-type) | Hsu, Taylor & Wang (2016, JIE) | FX, 30 developed + emerging currencies, 45 yrs daily | Yes (performance metrics incl. excess return) | Stepwise SPA (Hansen-based, Romano-Wolf style) + OOS cross-validation | **Significant predictability and excess profit, but it varies over time and is larger in less mature (emerging) currencies** |
| Technical rules on equity indices | Hsu, Hsu & Kuan (2010, J. Empirical Finance) | Growth and emerging market indices and their ETFs | n/a in snippet | Stepwise SPA (more powerful than Romano-Wolf stepwise RC) | Significant predictive power that **weakens after ETFs were introduced** (once the market became tradable or arbitraged) |
| MA (1m vs 3-12m), momentum, **OBV-MA** as predictors | Neely, Rapach, Tu & Zhou (2014, Mgmt Sci) | US equity risk premium, monthly | Utility gains, not trade-level costs | OOS R² vs historical mean; no RC | Technical indicators show significant in-sample **and OOS** predictive power, matching or beating macro variables. Monthly horizon, used as **forecasting/regime input** |
| Candlesticks (28 patterns) | Marshall, Young & Rose (2006, J. Banking & Finance) | DJIA stocks 1992-2002, 10-day hold | n/a | Bootstrap that simulates random OHLC | **No value.** Bullish single-day signals positive under half the time; one bullish signal predicted negative returns |
| Candlesticks, Japan | Marshall, Young & Cahan (2008) | Japan | — | Bootstrap | Largely no value |
| Two-day candlesticks, Taiwan | Lu, Shiu & Chen (2012, EMFT) | Taiwan 50 stocks 2002-2009 | Considered | Bootstrap (no family-wise correction found) | Some patterns profitable (3 bullish reversals), but these were found after a pattern search, so snooping risk is high |
| Chart patterns (H&S, double bottoms, etc.) | Lo, Mamaysky & Wang (2000, JF) | US stocks 1962-1996 | No (distributional test, not a strategy) | Goodness-of-fit tests on conditional vs unconditional return distributions; no RC | Several patterns give **incremental information** (conditional return distributions differ), stronger for Nasdaq. Not shown to be profitable after costs |
| Support/resistance (published by dealers) | Osler (2000, FRBNY EPR) | Intraday FX USD/JPY, GBP, DEM, 1996-98 | n/a | Compared with randomly placed levels (bootstrap-like) | Published levels predict **intraday trend interruptions** (bounces) better than random levels. Info about *where price pauses*, not a trade trigger |
| RSI, MACD (India) | Cey. J./CBJ "across market cycles", Sensex Feb 2000-May 2018 | BSE Sensex | Yes | None (t-tests vs unconditional mean) | **RSI rule failed to deliver positive returns even before costs.** MACD sell signals beat the mean only in bear markets, before costs. Sharpe not adequate |
| MA rules (India) | Mitra (2011, Quant Finance 11(2):287-297) | 4 Indian index series | Yes | None found | Rules capture direction, significant gross returns long and short. **Not exploitable after real-world costs.** Short MAs mean more trades and more cost |
| SMA/DMA (India) | Sundhar & Kakani (practitioner/IIM working paper) | Nifty, Sensex, individual stocks 1991-2005 | Yes (claimed) | None | DMA reported profitable after costs. Weak design: no snooping correction, in-sample parameter choice |
| BB + RSI (India, intraday) | Investment Mgmt & Fin. Innovations (Business Perspectives) | 14 Nifty 50 stocks, hourly, Jan-Aug 2022 | Unclear | None | "BB + RSI best". **8-month sample with no correction: anecdotal** |
| Stochastics, ADX, Supertrend, Ichimoku, Chaikin/A-D/MFI, Fibonacci, Elliott, pivot points, cup-and-handle, flags, Elder triple screen / multi-timeframe | **No peer-reviewed test with RC/SPA/FDR correction found** in this session | — | — | — | Only vendor or blog backtests found (TradingView scripts, QuantifiedStrategies, Quantpedia BTC). Treat as **untested**. Note: oscillators like stochastics/RSI fall inside the 21k "oscillator" family in Hsu-Taylor-Wang (FX) |

## What do large-scale tests find after correcting for data snooping?

### Takeaway
Once the full universe of rules is accounted for (RC, SPA, stepwise, FDR) and costs are included, simple technical rules on developed equity indices show no reliable post-1987 profitability. The apparent profits reported for early periods (pre-1987 DJIA) and for immature markets (emerging FX, emerging indices before ETFs) shrink as markets mature. Candlesticks fail rigorous bootstraps. Chart patterns and S/R levels carry some *distributional/conditional information*, but this has not been shown to produce cost-adjusted trading profit.

### Cited Findings
- STW (1999) used White's Reality Check bootstrap to adjust for snooping across the full universe of rules, extending Brock-Lakonishok-LeBaron's 26 rules over 100 years of DJIA data — [FMG/LSE DP303](https://www.fmg.ac.uk/sites/default/files/2020-11/dp303.pdf); [LSE Research Online](https://researchonline.lse.ac.uk/id/eprint/119144)
- On 1987-1996 DJIA OOS data, the best BLL-type rule earned a statistically insignificant 8.63%/yr before costs — [Park & Irwin review, ageconsearch](https://ageconsearch.umn.edu/record/19039/files/cp05pa01.pdf) (cited via search snippet)
- Bajgrowicz & Scaillet (2012, JFE 106(3):473-491), DJIA 1897-2011: the FDR approach selects more outperforming rules than RC, but persistence tests show an investor could never have picked the future best rules ex ante. "Even in-sample, the performance is completely offset by the introduction of low transaction costs" — [RePEc](https://ideas.repec.org:443/a/eee/jfinec/v106y2012i3p473-491.html); [CFA Digest](https://rpc.cfainstitute.org/research/cfa-digest/2013/02/technical-trading-revisited-false-discoveries-persistence-tests-and-transaction-costs-digest); [EFMA working paper](https://www.efmaefm.org/0EFMAMEETINGS/EFMA%20ANNUAL%20MEETINGS/2008-Athens/papers/Bajgrowicz.pdf)
- Hsu, Taylor & Wang (2016, JIE): 21,195 rules (filters, MAs, S/R, channel breakouts, oscillators) on 30 currencies over 45 years, stepwise test against snooping, OOS cross-validation. Found substantial predictability and excess profitability, with time-series variation and larger effects in less mature markets ("temporarily not-fully-rational behavior and market immaturity") — [WUSTL profile](https://profiles.wustl.edu/en/publications/technical-trading-is-it-still-beating-the-foreign-exchange-market/); [CEPR DP10018](https://cepr.org/publications/DP10018)
- Hsu, Hsu & Kuan (2010, J. Empirical Finance): the stepwise SPA test (more powerful than Romano-Wolf stepwise RC) finds significant predictive power of technical rules for growth/emerging indices, but "such evidence weakens after the ETFs are introduced" — [HKU repository](https://repository.hku.hk/handle/10722/141768)
- Park & Irwin (2007, J. Econ. Surveys 21(4)): of ~92-95 "modern" studies, about 56-58 report positive results, 20-24 negative, 10-19 mixed. Most positive studies have problems such as snooping, ex-post rule selection and cost/risk estimation — [RePEc](https://ideas.repec.org/a/bla/jecsur/v21y2007i4p786-826.html); [AgMAS report](https://ageconsearch.umn.edu/record/37487/files/AgMAS04_04.pdf). Note that the counts differ between versions (58/92 vs 56/95).
- Marshall, Young & Rose: 28 candlestick signals on DJIA stocks 1992-2002, with a bootstrap that generates random OHLC. "Candlestick technical analysis does not have value." Bullish single-day signals were positive less than half the time — [Massey repository](https://mro.massey.ac.nz/handle/10179/1604); [CXO summary](https://www.cxoadvisory.com/technical-trading/candlesticks-fiddlesticks/)
- Taiwan 2002-2009: some two-day patterns are profitable, including 3 bullish reversals and 2 "new" patterns — [NCCU](https://nccur.lib.nccu.edu.tw/ir/handle/140.119/66916)
- Lo, Mamaysky & Wang (2000, JF 55:1705-1765): kernel-regression pattern recognition on US stocks 1962-96. "Several technical indicators do provide incremental information and may have some practical value", more so on Nasdaq — [MIT page](https://web.mit.edu/~alo/www/Papers/techanal.html)
- Osler (2000): S/R levels published by 6 FX firms (1996-98) predicted intraday trend interruptions better than random levels. Levels stayed useful for about 5 days — [FRBNY EPR PDF](https://resources.newyorkfed.org/medialibrary/media/research/epr/00v06n2/0007osle.pdf)
- Neely et al. (2014, Mgmt Sci 60(7)): MA(1 vs 3-12m), momentum and OBV-MA signals give significant in-sample and OOS forecasts of the equity premium and complement macro variables — [WUSTL](https://profiles.wustl.edu/en/publications/forecasting-the-equity-risk-premium-the-role-of-technical-indicat/); [St Louis Fed WP 2010-008](https://files.stlouisfed.org/files/htdocs/wp/2010/2010-008.pdf)

### Inferences
- **Indicators that carry real (if small) information:** slow trend measures (long MA relative position, time-series momentum) and volume-trend (OBV-MA) as *monthly regime/forecast inputs* (Neely et al.). S/R levels as *places where price tends to pause* (Osler). Some price-path shapes contain distributional information (LMW). In all these cases the information works best as a **filter, sizing or regime input**, not as a stand-alone short-horizon trigger, because trigger-level use multiplies turnover and costs (Bajgrowicz-Scaillet, Mitra).
- **Likely noise:** candlestick patterns as stand-alone triggers, RSI overbought/oversold mean reversion on Indian indices (negative even gross), and any parameter-optimised short-MA crossover after costs.
- **Untested, so assume noise until shown otherwise:** Fibonacci, Elliott waves, pivot points, Supertrend, Ichimoku, ADX thresholds, stochastics, Chaikin/MFI/A-D, cup-and-handle and flags. No peer-reviewed snooping-corrected test was found.
- **Why "a new indicator nobody uses" is not an edge:** RSI, stochastics, MACD, BB, Supertrend, ADX and Ichimoku are all deterministic transforms of the same OHLC(V) series. Most reduce to (a) the distance of price from a lagged average or range, scaled by volatility (trend/momentum), or (b) the reverse of that (mean reversion). A new composite is one more draw from that same rule universe, so it adds to the multiple-testing count rather than escaping it. STW/BS/HTW show that the *best of a large universe* tends to be explained by luck once the universe is accounted for. Novelty does not create information. Only a genuinely new *input* (different data, such as order flow or positioning) or a structural reason (a risk premium or behavioral friction) can.
- Profitability decays as markets mature and become arbitrageable (ETF introduction in Hsu-Hsu-Kuan; time variation in HTW; pre-1987 vs post-1987 DJIA). This matters for India, where liquidity, algo share and F&O participation have grown sharply since the 2000s, so older Indian findings likely overstate current edges.

### Gaps
- No peer-reviewed RC/SPA/FDR test was found specifically for Supertrend, Ichimoku, ADX, Fibonacci, Elliott, pivot points, MFI/Chaikin, cup-and-handle or flags.
- Exact cost levels (bps) used in Bajgrowicz-Scaillet and the size of HTW's net profits could not be extracted, because full texts were blocked.
- [Training knowledge, not re-verified this session] Savin, Weller & Zvingelis (2007, J. Financial Econometrics) found that H&S patterns have some predictive power for US stocks. Brock-Lakonishok-LeBaron (1992) is the in-sample precursor. Han, Yang & Zhou (2013, JFQA) find MA timing works on high-volatility portfolios. These should be checked before use.

## What do Indian studies find for technical rules on NSE/BSE, and do they include costs?

### Takeaway
Indian studies generally find that simple MA/MACD rules on Nifty/Sensex capture direction and earn significant *gross* returns, but net returns largely vanish after realistic costs. RSI rules fail even gross. Studies claiming net profits (DMA, BB+RSI) use no data-snooping correction and often short or in-sample windows. No Indian study applying White's RC, Hansen's SPA, Romano-Wolf or FDR to Nifty/NSE rules was found.

### Cited Findings
- Mitra (2011, Quantitative Finance 11(2):287-297), "How rewarding is technical analysis in the Indian stock market?": MA rules on 4 Indian index series give significant positive long and short returns, but "these returns cannot be exploited fully due to real world transaction costs". Short MAs generate many trades and high costs — [RePEc listing](https://ideas.repec.org/a/taf/quantf/v11y2011i2p287-297.html) (snippet)
- Sensex Feb 2000-May 2018, split into bull/bear phases: "RSI trading rule failed to deliver positive returns even before deducting transaction costs". MACD sell signals beat the unconditional mean only in bear markets and only before costs. Returns were not adequate for the risk (Sharpe) — [Colombo Business Journal](https://mgmt.cmb.ac.lk/cbj/index.php/testing-the-profitability-of-technical-trading-rules-across-market-cycles-evidence-from-india/); [CBJ SLJOL](https://cbj.sljol.info/articles/56)
- Sundhar & Kakani: SMA and Displaced MA on Nifty, Sensex and stocks, 1991-2005. DMA was reported to generate profitable signals even after costs — [PDF copy](https://c.mql5.com/forextsd/forum/214/Profiting%20from%20Technical%20Analysis%20in%20Indian%20Equity%20Market%20-%20Using%20Moving%20Averages.pdf) (snippet; no snooping correction)
- An NSE 500 study of SMA/EMA rules shows how trading frequency (window length) erodes net returns after costs — [IJIE, Univ. Malaya](https://adum.um.edu.my/index.php/ijie/article/view/51136) (snippet only)
- Nifty 50: 14 large stocks, hourly data, Jan-Aug 2022. The BB + RSI strategy "performed the best" — [Business Perspectives IMFI](https://businessperspectives.org/journals/investment-management-and-financial-innovations/issue-430/the-effectiveness-of-technical-trading-strategies-evidence-from-indian-equity-markets)
- Technical analysis has also been tested on beta-sorted Indian portfolios — [Business Perspectives IMFI](https://businessperspectives.org/journals/investment-management-and-financial-innovations/issue-501/can-technical-analysis-create-returns-for-beta-based-portfolios-in-the-indian-market) (findings not extracted)
- A Nottingham thesis tests weak-form efficiency with simple rules on Indian data — [Nottingham eprints](https://eprints.nottingham.ac.uk/20281) (findings not extracted)

### Inferences
- The Indian evidence matches the global pattern: gross direction-capture is common, net profit is fragile, and RSI-type mean reversion is weakest. Indian costs for intraday and short swing trades are material: STT, exchange fees, GST, stamp duty, slippage and brokerage. Any Indian composite should be tested net of a full cost stack at the actual turnover.
- Positive Indian claims have not been tested against the universe of alternative rules. Given STW/BS, they should be treated as in-sample, unadjusted results.

### Gaps
- No Indian study using RC/SPA/Romano-Wolf/FDR on NSE/BSE rules was located. Vikalpa and IIMB Management Review searches did not surface a directly relevant snooping-corrected paper.
- No Indian tests of Supertrend, Ichimoku, candlesticks or pivot points with costs and correction were found.
- Full texts (cost assumptions in bps, exact returns) could not be fetched for Mitra or the IJIE paper.

## Is there evidence that multi-timeframe confirmation improves results?

### Takeaway
No peer-reviewed, snooping-corrected evidence was found that multi-timeframe alignment (e.g., Elder triple screen) improves cost-adjusted results. The support is practitioner backtests only. The closest academic analogue is that slow trend signals carry information as regime inputs (Neely et al. 2014), which is consistent with using a higher-timeframe trend as a *filter*, but this is not a direct test.

### Cited Findings
- Practitioner claim: an HTF filter cuts counter-trend losers in trending markets, but it skips early reversal entries and does little in ranges, so the net effect depends on the regime mix — [LuxAlgo concept page](https://www.luxalgo.com/library/concept/higher-timeframe-trend-filter/)
- Practitioner backtest: raw MACD signals "lack selectivity" and improve with HTF trend confirmation (single strategy, no snooping correction) — [QuantifiedStrategies](https://quantifiedstrategies.substack.com/p/multi-timeframe-analysis-and-strategy)
- A Quantpedia example of a multi-timeframe trend strategy on Bitcoin (single asset, illustrative) — [Quantpedia](https://quantpedia.com/how-to-design-a-simple-multi-timeframe-trend-strategy-on-bitcoin/)
- Slow technical indicators (MA 1m vs 12m, OBV-MA) carry OOS information about monthly equity returns — [Neely et al. WP](https://files.stlouisfed.org/files/htdocs/wp/2010/2010-008.pdf)

### Inferences
- A defensible design is: a higher timeframe sets the regime or direction (filter or position sizing), and a lower timeframe sets timing. However, every added timeframe and threshold is another parameter, which raises the trial count used in DSR/PBO. Each extra confirmation layer must be counted as additional trials.

### Gaps
- No academic test of the Elder triple screen was found.

## How should a new composite indicator be validated?

### Takeaway
Treat the composite as one member of a large search space. Pre-register the rule family. Evaluate on walk-forward / purged combinatorial CV net of full costs. Report the number of trials. Apply a multiple-testing correction across all variants tried (RC/SPA/Romano-Wolf/FDR). Compute the Probability of Backtest Overfitting (CSCV) and the Deflated Sharpe Ratio. Finally, demand persistence on untouched data, ideally in other markets and regimes.

### Cited Findings
- Bailey, Borwein, López de Prado & Zhu (2015/2017, J. Computational Finance) proposed estimating PBO via combinatorially symmetric cross-validation (CSCV). Standard hold-out is "unreliable and inaccurate" for investment backtests. The more configurations tried, the higher the PBO, and most analysts do not report the number of trials — [Risk.net JCF](https://www.risk.net/journal-of-computational-finance/2471206/the-probability-of-backtest-overfitting); [eScholarship PDF](https://escholarship.org/content/qt4w1110bb/qt4w1110bb.pdf); R package [pbo](https://archive.linux.duke.edu/cran/web/packages/pbo/readme/README.html)
- "Pseudo-mathematics and financial charlatanism" (Bailey et al., Notices AMS 2014) covers how backtest overfitting degrades out-of-sample performance — [WMU ScholarWorks](https://scholarworks.wmich.edu/math_pubs/40/); [carmamaths PDF](https://carmamaths.org/jon/backtest2.pdf)
- Persistence testing (BS 2012) shows that picking the in-sample best rule did not forecast OOS winners — [CFA Digest](https://rpc.cfainstitute.org/research/cfa-digest/2013/02/technical-trading-revisited-false-discoveries-persistence-tests-and-transaction-costs-digest)
- The stepwise SPA test identifies *which* rules beat the benchmark while controlling family-wise error, and it has more power than Romano-Wolf stepwise RC — [HKU repository](https://repository.hku.hk/handle/10722/141768)

### Inferences (validation protocol for a composite swing/intraday indicator)
1. Specify the hypothesis and an economic rationale before testing, and freeze the rule family and parameter grid. Log every variant tried, because N trials is needed for the DSR.
2. Use realistic Indian costs per trade (brokerage, STT, exchange charges, GST, stamp duty, SEBI fee, slippage by liquidity bucket) and test at the actual turnover. Bajgrowicz-Scaillet show that low costs alone erase in-sample gains.
3. Benchmark against simple baselines built from the same series: buy-and-hold, a 200-day MA filter, and 12-1 momentum. A composite has to beat these, not just zero.
4. Use walk-forward (anchored and rolling) plus combinatorial purged CV with an embargo. This avoids label leakage from overlapping holding periods and yields a distribution of OOS paths.
5. Apply a multiple-testing correction across all variants: White RC / Hansen SPA (family-wise), Romano-Wolf stepwise, or Bajgrowicz-Scaillet FDR when several survivors are acceptable.
6. Compute PBO via CSCV and the Deflated Sharpe Ratio. [Training knowledge, not re-verified this session: Bailey & López de Prado 2014, J. Portfolio Management, adjusts SR for the number of trials, skewness, kurtosis and track length.] Reject the composite if PBO is high or DSR is not significant.
7. Run robustness checks: parameter-neighborhood stability (no knife-edge optimum), subperiods (bull/bear, pre/post-2020 retail surge), cross-sectional breadth (many NSE stocks, not a few), and a placebo test on shuffled or synthetic data.
8. Hold back a final untouched period and then paper-trade. Expect decay: Hsu-Hsu-Kuan show edges fade once markets become more tradable.

### Gaps
- The Deflated Sharpe Ratio paper and CPCV (López de Prado 2018, *Advances in Financial Machine Learning*) were not fetched this session. Their content is from training knowledge.
- Romano-Wolf (2005, Econometrica) and Hansen (2005, JBES) primary sources were not fetched; they are cited here via Hsu-Hsu-Kuan.
