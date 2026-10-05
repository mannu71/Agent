# Swing-Horizon (multi-day to multi-week) Stock Strategy Families: Out-of-Sample, Net-of-Cost Evidence and Indian Survival (as of Oct 2026)

Scope note for the report writer. This file deliberately does not repeat the evidence already in the repo. These three files already cover it:
- `reports/Hedge fund style strategy blueprint.md` (Sleeve A and the screener): IIMA WML, Nifty200 Momentum 30 TRI 18.8% vs 13.9%, Freefincal liquidity split, Daniel-Moskowitz, Indian 1-month reversal, Concretum stock trend, CXO on Connors, Martineau PEAD, Gupta-Dhusia NSE PEAD, and the high-volume premium.
- `reports/Price action reading strategy rules.md`: the Concretum family, Kullamägi breakouts and NSE ORB failure.
- `research_notes/Swing trading and stock screener/swing_strategies.md`: the fullest prior notes.

Where this file reuses a figure from those files, it keeps the original source link. Its new material is:
- the post-publication decay and replication literature (McLean-Pontiff, Hou-Xue-Zhang, Novy-Marx-Velikov, Sullivan-Timmermann-White);
- Barroso-Santa-Clara;
- the Indian risk-managed momentum paper (2022);
- the Indian 52-week-high paper (Raju, 2004-2023);
- an Indian PEAD study that finds significant drift (2002-2017), which conflicts with the null Indian result the blueprint relied on;
- earnings-announcement-return (EAR) drift;
- Indian technical-rule tests;
- RSI(2) decay data;
- the status of volatility-contraction (squeeze/NR7) evidence;
- volume filters.

Research-environment caveat: WebFetch was blocked by the egress proxy for every domain tried (quantpedia, sagepub, niftyindices, alphaarchitect, stockcharts, substack, businessperspectives, backtrex, jier, scirp). Every new figure below therefore comes from search-engine summaries of the cited page, not from a full-text read. Treat the exact decimals as "verify before hard-coding".

---

## Q1. Which swing rules have replicated out-of-sample and after costs, and how much did they decay after publication?

### Takeaway
Only two families replicate robustly out of sample in value-weighted, non-microcap tests and survive costs at monthly or slower turnover: **intermediate-horizon (6-12 month) price momentum and its 52-week-high variant**, plus **diversified time-series trend on futures/indices**. Their premia shrink after publication: McLean-Pontiff find anomaly returns about 26% lower out of sample and 58% lower after publication. **Short-term (1-week/1-month) reversal, classic MA/breakout rules on single indices, and Connors-style RSI(2)** largely fail one or more of three hurdles: the replication hurdle (96% of "trading-friction" anomalies, including short-term reversal, fail Hou-Xue-Zhang), the post-sample hurdle (BLL rules insignificant over 1987-1996), or the cost/benchmark hurdle (RSI(2) on SPY trails buy-and-hold). **PEAD** is contested: it is dead for US non-microcaps since about 2006 per Martineau, disputed by two 2025 papers, and the Indian evidence conflicts. **Volatility-contraction breakouts (BB squeeze, NR7)** have no peer-reviewed evidence at all.

### Cited Findings

**Cross-cutting decay and replication evidence**
- McLean & Pontiff (JF 2016), 97 cross-sectional predictors: portfolio returns are **26% lower out-of-sample and 58% lower post-publication**. The 26% is an upper bound on data mining, which implies about a 32% decline attributable to publication-informed trading. Post-publication declines are larger for predictors with higher in-sample returns. Returns are higher in high-idiosyncratic-risk, low-liquidity stocks. The largest degradation comes **3-4 years after publication**. — [Gwern-hosted PDF of McLean & Pontiff 2016](https://Www.Gwern.net/doc/economics/2016-mclean.pdf); [SUFE abstract](https://academicnewsletter.sufe.edu.cn/info/359722); [CXO Advisory](https://www.cxoadvisory.com/big-ideas/effects-of-market-adaptation)
- Hou, Xue & Zhang, "Replicating Anomalies" (RFS 2020), 452 anomalies:
  - 65% fail a single-test hurdle and 82% fail a multiple-testing threshold.
  - Microcaps are 3.2% of market cap but 60.7% of stocks; equal-weighted sorts overweight them.
  - In the **trading-frictions category, 102 of 106 anomalies (96%) fail, including short-term reversal**, share turnover and dollar-volume variation.
  - **Momentum, value, investment and profitability replicate well.**
  - [IDEAS/RePEc](https://ideas.repec.org/a/oup/rfinst/v33y2020i5p2019-2133..html); [NBER w23394](https://www.nber.org/papers/w23394.pdf); [Alpha Architect summary](https://alphaarchitect.com/replicating-anomalies/)
- Novy-Marx & Velikov, "A Taxonomy of Anomalies and Their Trading Costs" (RFS 2016), 23 anomalies:
  - Most anomalies with **one-sided monthly turnover below 50%** keep significant net spreads when designed to mitigate costs; few higher-turnover strategies do.
  - A **buy/hold spread** (keep holding names you would not actively buy) is the single most effective cost mitigation.
  - Momentum is a high-turnover strategy, more cost-sensitive than value, profitability or investment.
  - [NBER w20721](https://nber.org/papers/w20721); [Rochester PDF](https://mysimon.rochester.edu/novy-marx/research/ToAatTC.pdf); [Alpha Architect](https://alphaarchitect.com/trading-costs-destroy-factor-investing/)
- Lesmond, Schill & Zhou (JFE 2004): momentum round-trip costs rarely fall below about 1.5% (1980-1998), and costs "eclipse in large part" momentum profits. — [Lesmond et al. PDF](https://www.bauer.uh.edu/rsusmel/phd/Lesmond_et%20al%20_2004_JFE.pdf) (via prior notes)

**Family-by-family evidence table**

| Family | Exact rule (canonical) | Sample / period | Costs assumed | OOS / post-publication result | India specifically |
|---|---|---|---|---|---|
| Cross-sectional momentum (Jegadeesh-Titman) | Rank on past 3-12 m return; long top decile, short bottom; hold 3-12 m | US 1965-89; OOS 1990s (JT 2001) | Gross; Lesmond et al. put costs at ≥1.5% round trip | About 1%/month; **continued out of sample in the 1990s, "not data snooping"** ([RePEc JT 2001](https://ideas.repec.org/a/bla/jfinan/v56y2001i2p699-720.html)); passes HXZ replication ([RePEc](https://ideas.repec.org/a/oup/rfinst/v33y2020i5p2019-2133..html)); turnover makes it cost-sensitive ([NBER](https://nber.org/papers/w20721)) | Survives. IIMA WML about 21.9%/yr 1994-2014, −62% DD in 2000 ([IIMA](https://web.iima.ac.in/~iffm/Indian-Fama-French-Momentum/drawdown.php)); NSE 1997-2013 momentum at 6-12 m ([NSE](https://nsearchives.nseindia.com/content/research/res_paperfinal223.pdf)); 6-month lookback with quarterly rebalancing best risk-adjusted in a long-only IIT-B study ([Nigam & Pandey 2023](https://journals.sagepub.com/doi/abs/10.3233/AF-220399)); net-of-cost alpha concentrated in illiquid half (liquid half 8.51% vs Nifty 50 10.41% net CAGR, Dec 2006-Jun 2025) ([Freefincal](https://freefincal.com/is-indian-momentum-investing-just-a-liquidity-illusion/)) |
| Volatility-adjusted momentum index (NSE) | Top 30 of Nifty 200 by Normalised Momentum Score (6 m and 12 m return ÷ daily vol); weight = FF mcap × score, cap min(5%, 5× mcap wt); semi-annual rebalance | Base Apr 2005; **launched 25 Aug 2020**, so pre-2020 is back-calculated | Index (no costs); index-fund turnover about 124%/yr ([Business Standard](https://www.business-standard.com/markets/news/nifty200-momentum30-a-flip-of-index-switch-jolts-16k-cr-across-the-grid-125121400388_1.html)) | TRI 18.8% vs 13.9% (Nifty 200 TRI) Apr 2005-Jun 2026 ([HDFC MF](https://files.hdfcfund.com/s3fs-public/Others/2026-07/HDFC%20NIFTY200%20Momentum%2030%20Index%20Fund_PPT%20%28July%202026%29.pdf)); calendar 2025 −12.09% (third-party tracker, unverified) ([search summary of Angel One/Kotak index pages](https://www.angelone.in/indices/nifty200-momentum-30)); −31.79% DD Sep 2024-Apr 2025 ([HDFC MF](https://files.hdfcfund.com/s3fs-public/Others/2026-07/HDFC%20NIFTY200%20Momentum%2030%20Index%20Fund_PPT%20%28July%202026%29.pdf)) | This *is* the Indian evidence. The live period since Aug 2020 is about 6 years. The search did not yield a clean live-only CAGR split (gap) |
| 52-week-high proximity (George-Hwang) | Price ÷ 52-week high; long nearest, short furthest; hold 6 m | US 1963-2001 | Gross; some later tests net of costs | About 0.45%/month; dominates JT and industry momentum in Fama-MacBeth; **no long-run reversal** ([CXO](https://www.cxoadvisory.com/1284/technical-trading/the-52-week-high-as-a-momentum-indicator-for-individual-stocks/); [SUFE](https://academicnewsletter.sufe.edu.cn/info/361244)) | **Survives (in-sample, gross).** Raju, "The 52-Week High Effect and Momentum Investing: Evidence from India", Oct 2004-Aug 2023: "distinct and robust", **more stable alpha than academic momentum, weaker long-term reversals**, higher returns and Sharpe after controlling for size. Quantpedia lists indicative performance of **20.39%/yr at 13.68% vol** (strategy #949) ([Quantpedia update](https://vvv.quantpedia.com/?p=31439); [Quantpedia](https://quantpedia.com/an-analysis-of-52-weeks-high-effect-on-indian-stocks/)). Cost treatment not retrieved |
| 52-week-high "HTP" crash-avoiding variant | High-to-price momentum per "Decomposing Momentum: Eliminating its Crash Component" | Stockviz backtest on Indian stocks | Not stated | Avoided crashes with "decent" returns but sat **about 70% in cash** and missed bear-market rallies; some picks fell 30% in a month; "jury still out" ([Stockviz](https://stockviz.substack.com/p/momentum-without-the-crash)) | Practitioner-only |
| Time-series momentum (MOP 2012) | Sign of own 12 m excess return; vol-scaled position; 1 m hold | 58 futures 1985-2009; 1880-2016 (Hurst-Ooi-Pedersen) | HOP: 2/20 fee assumption | Positive in every decade net of assumed fees ([AQR](https://www.aqr.com/Insights/Research/Journal-Article/A-Century-of-Evidence-on-Trend-Following-Investing?aqrPDF=1)); "Trends Everywhere" gross SR 1.17 traditional, 1.60 combined ([Quantpedia](https://quantpedia.com/time-series-momentum-works-everywhere/)) | **Untested on single Indian stocks.** Nifty trend rules 2005-2012 (no costs) paid mainly in sharp declines ([Indian Journal of Finance](https://www.indianjournalofentrepreneurship.com/index.php/IJF/article/view/71980)) |
| MA crossover / trading-range breakout (BLL 1992) | 26 rules: 1-50, 1-150, 5-150, 1-200, 2-200 day MA (with/without 1% band); 50/150/200-day TRB | DJIA 1897-1986 | None | **Sullivan-Timmermann-White (1999)**: BLL results not snooped in-sample, but on 1987-1996 the best rule earned an **insignificant 8.63%/yr before costs**, and snooping-adjusted profitability disappears ([SUFE](https://academicnewsletter.sufe.edu.cn/info/361689); [LSE](https://researchonline.lse.ac.uk/id/eprint/119144); [FMG DP303](https://www.fmg.ac.uk/sites/default/files/2020-11/dp303.pdf)); Bajgrowicz-Scaillet (2012): with FDR control, performance "fully disappears" net of costs ([RePEc](https://ideas.repec.org/a/eee/jfinec/v106y2012i3p473-491.html)) | Mixed and weak. Ten Asian indices 1990-2012: MA rules positive *after costs*, larger in less-developed markets ([search summary of Virtus Interpress paper](https://virtusinterpress.org/IMG/pdf/10-22495cocv11i2c5p6.pdf)); South Asia 1990-2000 VMA/FMA: **India the weakest** of four ([Newcastle PDF](https://www.staff.ncl.ac.uk/i.m.dobbs/Files/Jordan%20n.pdf)); BSE Sensex 2000-2018: RSI rule negative *even before costs* ([Colombo Business Journal](https://mgmt.cmb.ac.lk/cbj/index.php/testing-the-profitability-of-technical-trading-rules-across-market-cycles-evidence-from-india/)) |
| Donchian / new-high breakout on stocks | Buy new all-time high; 10×ATR(42) trail; inverse-vol sizing | US 1950-2024, survivorship-free | Gross | 15.19% CAGR, author OOS 2005-2024 holds; <7% of trades make the profit; 305-day average hold ([Concretum](https://concretumgroup.com/does-trend-following-still-work-on-stocks/)) | **Untested in India.** It is a position trade, not a swing trade |
| Short-term reversal (1-week / 1-month) | Long last week's/month's losers, short winners; weekly or monthly rebalance | US | Gross vs net | **Fails HXZ replication** (trading-friction category) ([RePEc](https://ideas.repec.org/a/oup/rfinst/v33y2020i5p2019-2133..html)); about 30-50 bps/week net only in large caps or residual/industry-neutral form ([EFMA 2011](https://efmaefm.org/0EFMAMEETINGS/EFMA%20ANNUAL%20MEETINGS/2011-Braga/papers/0259_update.pdf)); Robeco: neutralising industry/factor momentum about doubles risk-adjusted return ([Robeco](https://www.robeco.com/en-uk/insights/2023/10/reversing-the-trend-of-short-term-reversal)) | Present gross. 1 m/1 m reversals across all winner-loser combinations, NSE 1995-2008 ([Publishing India](https://www.publishingindia.com/archive/ijfm/a-study-of-contrarian-and-momentum-profits-in-indian-stock-market)); buying **low-volume one-day losers** and selling one-day winners earned significant short-horizon profits ([search summary, Emerald/IDEAS](https://ideas.repec.org/a/eme/rbfpps/rbf-06-2018-0058.html)); reversals for up to 6 months after large monthly moves ([CEEOL](https://ceeol.com/search/article-detail?id=1054639)). **No net-of-cost Indian weekly test** |
| RSI(2) (Connors) | Close > 200-DMA; buy RSI(2) < 5-10; exit close > 5-DMA (rule from book, unverified here) | US 1995-2007 (in-sample) | None in book | CXO: no data-mining or OOS controls ([CXO](https://cxoadvisory.com/technical-trading/a-few-notes-on-short-term-trading-strategies-that-work)); live Connors product 2008-11 with 0.67% friction lagged SPY, win rate 62%→55% ([CXO](https://www.cxoadvisory.com/individual-gurus/review-of-larry-connors-daily-battle-plan/)); 10-yr retests: S&P 500 **71% win rate, +1.22% avg win vs −2.10% avg loss, about 1.7%/yr vs 13.5% buy-and-hold**; Nasdaq-100 75% win rate but +2.4%/yr vs 20.2% ([Backtrex S&P](https://backtrex.com/en/backtests/connors-rsi-2-sp-500); [Backtrex NDX](https://backtrex.com/en/backtests/connors-rsi-2-nasdaq-100)) | **No rigorous Indian test.** RSI rule on Sensex 2000-2018 lost before costs ([CBJ](https://mgmt.cmb.ac.lk/cbj/index.php/testing-the-profitability-of-technical-trading-rules-across-market-cycles-evidence-from-india/)) |
| Bollinger/RSI combos | SMA, EMA-RSI, BB-RSI on hourly bars | 14 Nifty 50 stocks, Jan-Aug 2022 | Not clear | BB-RSI net-profitable on 11/14 and beat buy-and-hold on 10/14 ([Tadas 2023, IMFI](https://businessperspectives.org/journals/investment-management-and-financial-innovations/issue-430/the-effectiveness-of-technical-trading-strategies-evidence-from-indian-equity-markets)) | An 8-month in-sample window on 14 stocks: **anecdote, not evidence** |
| PEAD (SUE) | Long top-SUE decile, short bottom; hold about 60 days | US 1970s-2010s | Varies | Martineau (CFR 2022): gone for non-microcaps since about 2006 ([CFR PDF](https://cfr.ivo-welch.info/published/papers/martineau2021rest.pdf)); two 2025 papers say "alive"; Subrahmanyam (2025) finds it **vanishes once microcaps (about 3% of market value) are removed** ([UCLA Anderson Review](https://anderson-review.ucla.edu/is-post-earnings-announcement-drift-a-thing-again/)) | **Conflicting.** 2002-2017 Fama-MacBeth study: **significant PEAD, robust across sub-periods and after controlling for beta, size, P/B, illiquidity and idiosyncratic vol** ([search summary of SCIRP Theoretical Economics Letters paper](https://file.scirp.org/Html/20-1501629_88060.htm)) vs 100 NSE firms 2014-2018: no exploitable drift ([Trends E&M](https://journals.vutbr.cz/index.php/trends/article/view/541)); older SUE study: drift into month 3 ([IIMB](https://repository.iimb.ac.in/handle/2074/20501)); drift after bad news only ([Nottingham](https://eprints.nottingham.ac.uk/26569)). No study is net of costs |
| Earnings gap / EAR drift | Sort on 3-day abnormal return around the announcement (EAR) rather than SUE | US | Gross | Brandt, Kishore & Santa-Clara (2008): EAR and SUE strategies about **12.5%/yr abnormal**; EAR significantly related to subsequent drift ([search summary of Ivey PDF](https://www.ivey.uwo.ca/media/3775558/when_two_anomalies_meet.pdf); [Quantpedia tag](https://quantpedia.com/strategy-tags/earnings-announcement/)) | **Untested in India.** Indian results can land mid-session within 30 min of the board meeting ([StudyCafe](https://studycafe.in/faqs-on-sebi-lodr-amendments-dated-05th-may-2021-102028.html)) |
| Volatility contraction → expansion (BB squeeze, NR4/NR7, VCP) | BB width at an N-day low or narrowest range in 7 bars; trade the breakout direction | Blogs, TradingView | Usually none | **No peer-reviewed test found.** One blog test of 399 squeezes on 50 S&P 500 stocks: **49.6% directional accuracy unfiltered** (coin-flip); 71.4% for a "graded" subset (an in-sample filter, so likely overfit) ([search summary, LuxAlgo/TradingView](https://www.luxalgo.com/library/concept/squeeze-release-direction.md)); Arthur Hill S&P 1500 squeeze test exists but was not retrievable ([StockCharts](https://articles.stockcharts.com/article/articles-arthurhill-2017-03-systemtrader---testing-a-bollinger-band-squeeze-system-for-stocks-in-the-sp-1500-/)) | **Untested in India** |

### Inferences
- Applying McLean-Pontiff's haircut to any published swing edge before using it is the evidence-based default: roughly −25% for out-of-sample data mining and −50 to −60% if the effect has been public for 3+ years. Momentum (published 1993) and 52-week-high (2004) are long past that window, so today's Indian numbers already embed whatever decay has happened. Back-calculated index history, such as Nifty200 Momentum 30 before Aug 2020, does not.
- The cost hurdle is mainly a turnover hurdle (Novy-Marx-Velikov's 50%/month line). A 3-10 day swing signal rebalanced weekly has turnover far above that line, so on its own it is unlikely to clear Indian delivery costs of about 0.24-0.3% round trip plus slippage (blueprint figures). It can survive only as a *timing overlay* on a slow selection signal, combined with a buy/hold band.
- BLL's own rules were not data-snooped in-sample but died post-sample. That pattern is consistent with arbitrage or regime change, not fabrication. Single-index MA rules should be treated as a regime/exposure filter, not an alpha source.
- The canonical Connors and Minervini rules quoted in the table are from general knowledge of the books and were not verified from a fetched source.

### Gaps
- No full-text read was possible: decimals for Raju (India 52-week high), the 2002-2017 Indian PEAD paper (authors not retrieved), and Brandt et al. are from search summaries.
- No study was found that measures *post-publication decay specifically in India* (a McLean-Pontiff-style test on Indian anomalies).
- No peer-reviewed test of volatility-contraction breakouts (squeeze, NR7, VCP) in any market was found.
- No net-of-cost Indian test exists for: weekly reversal, RSI(2), Donchian on single stocks, PEAD/EAR, or earnings-gap continuation.
- Arthur Hill's StockCharts squeeze test and the Backtrex pages (periods, costs) could not be read in full.

---

## Q2. What is the Indian evidence (IIM/ISB/NSE/IIT papers, Indian momentum/PEAD papers, Nifty momentum indices)?

### Takeaway
India has **stronger and better-replicated evidence for 6-12 month momentum and 52-week-high proximity** than for anything at swing horizons. The supporting work spans IIMA's survivorship-corrected factor library, NSE research papers, an IIT-Bombay long-only study, the Raju 2004-2023 paper, a 2022 risk-managed momentum paper and NSE's own index. The catches are large crashes, back-calculated index history and the illiquidity of the net-of-cost alpha. At 1-day to 1-month horizons, Indian stocks show **reversal**, especially low-volume one-day losers, not continuation. Indian PEAD evidence is split: significant over 2002-2017, absent over 2014-2018 in a 100-firm NSE sample. Indian technical-rule evidence (MA/RSI on indices) is weak or negative.

### Cited Findings
- **IIMA factor library** (Agarwalla, Jacob, Varma): survivorship-corrected, illiquid firms excluded, from CMIE Prowess Oct 1993. WML about 21.9%/yr 1994-2014; worst drawdown −62% (Mar-Dec 2000, 5.1-year recovery); −52.8% Dec 2008-Sep 2009. — [IIMA drawdown page](https://web.iima.ac.in/~iffm/Indian-Fama-French-Momentum/drawdown.php); [IIMA library](https://faculty.iima.ac.in/~iffm/Indian-Fama-French-Momentum/); [IIMA WP on RePEc](https://ideas.repec.org/p/iim/iimawp/12130.html)
- **Nigam & Pandey (IIT Bombay SJMSOM), Algorithmic Finance 2023**: a long-only Indian momentum strategy delivers superior risk-adjusted performance; **lagged 6-month compounded returns with quarterly rebalancing** gave the best risk-adjusted result among the variants compared. Sharpe, CAGR and cost figures were not retrievable. — [Sage abstract](https://journals.sagepub.com/doi/abs/10.3233/AF-220399); [Crossref](https://api.crossref.org/works/10.3233%2FAF-220399)
- **"Timing the Tide" (2024)**: shorter rebalancing periods capture academic momentum more effectively in Indian equities. Cost treatment unknown. — [search summary listing, Emerald IJOEM](https://www.emerald.com/insight/content/doi/10.1108/IJOEM-09-2023-1518/full/html)
- **Raju (2004-2023)**: the Indian 52-week-high effect is a distinct and robust anomaly. It gives more stable alpha than academic momentum with weaker long-term reversals, and stocks near their 52-week high earn higher returns and Sharpe after controlling for size. Quantpedia indicative performance is 20.39%/yr at 13.68% volatility. — [Quantpedia premium update Dec 2023](https://vvv.quantpedia.com/?p=31439); [Quantpedia blog](https://quantpedia.com/an-analysis-of-52-weeks-high-effect-on-indian-stocks/)
- **NSE research paper (1997-2013)**: significant momentum at 6-12 months and contrarian returns at 3 years. Another study finds significant reversals up to 6 months after large monthly price changes. — [NSE res_paperfinal223](https://nsearchives.nseindia.com/content/research/res_paperfinal223.pdf); [CEEOL](https://ceeol.com/search/article-detail?id=1054639)
- **Short-horizon India**: a contrarian strategy buying low-volume one-day losers and selling one-day winners produced significant short-horizon economic profits. — [IDEAS (Review of Behavioral Finance 2018)](https://ideas.repec.org/a/eme/rbfpps/rbf-06-2018-0058.html) (search summary; attribution of this exact finding to this paper should be verified)
- **Technical rules in India**:
  - BSE Sensex, Feb 2000-May 2018: the RSI rule failed to deliver positive returns even before costs. — [Colombo Business Journal](https://mgmt.cmb.ac.lk/cbj/index.php/testing-the-profitability-of-technical-trading-rules-across-market-cycles-evidence-from-india/)
  - South Asian indices, 1990-2000: India showed the lowest MA-rule performance of the four. — [Newcastle PDF](https://www.staff.ncl.ac.uk/i.m.dobbs/Files/Jordan%20n.pdf)
  - Ten Asian indices, 1990-2012: MA rules earned positive excess returns after costs. — [Virtus Interpress](https://virtusinterpress.org/IMG/pdf/10-22495cocv11i2c5p6.pdf)
  - These are index-level, not stock-level, tests.
- **Indian PEAD**:
  - 2002-2017: significant PEAD, robust to sub-periods and to controls for beta, size, P/B, illiquidity and idiosyncratic volatility (Fama-MacBeth). — [SCIRP Theoretical Economics Letters](https://file.scirp.org/Html/20-1501629_88060.htm)
  - Gupta & Dhusia, 2014-2018, 100 NSE firms: no exploitable drift. — [Trends E&M](https://journals.vutbr.cz/index.php/trends/article/view/541)
  - Absolute-SUE bucketing explains 2.5-month drift with R² 0.86. — [IIMB repository](https://repository.iimb.ac.in/handle/2074/20501)
- **Nifty200 Momentum 30 index**:
  - Launched 25 Aug 2020, base 1 Apr 2005.
  - TRI 18.8% CAGR vs 13.9% for the Nifty 200 TRI to 30 Jun 2026.
  - Drawdowns: −67.9% in 2008; −31.79% Sep 2024-Apr 2025.
  - [HDFC MF PPT Jul 2026](https://files.hdfcfund.com/s3fs-public/Others/2026-07/HDFC%20NIFTY200%20Momentum%2030%20Index%20Fund_PPT%20%28July%202026%29.pdf); [NSE whitepaper Sep 2020](https://www.niftyindices.com/docs/default-source/indices/nifty200-momentum-30/nifty200_momentum_30_index_whitepaper_sep_20.pdf); [Kotak Securities index page](https://www.kotaksecurities.com/indices/indian-indices/nifty200-momentum-30)
- **Freefincal / BacktestIndia (Mar 2026)**: top-30 Nifty 200 by 12-month return, Dec 2006-Jun 2025, annual rebalance, net of brokerage, slippage and tax. Results: base 14.60% net CAGR (−70.61% max DD); liquid half 8.51% (−75.09%); illiquid half 19.43%; Nifty 50 10.41%. — [Freefincal](https://freefincal.com/is-indian-momentum-investing-just-a-liquidity-illusion/)

### Inferences
- New evidence that changes the blueprint's framing:
  - The blueprint called results season "a risk event, not an edge", citing only the 2014-2018 null study. The 2002-2017 Indian Fama-MacBeth evidence of significant PEAD weakens that to "**contested**". Together with the US-microcap pattern (Subrahmanyam), it suggests any surviving Indian drift is likely concentrated in small, illiquid names, which is the same place the momentum alpha sits.
  - The policy of cutting size before results stays sensible because of gap risk. But a post-results EAR/SUE signal for *entry after* the announcement is a legitimate untested candidate, not folklore.
- Raju's finding that 52-week-high proximity is "more stable than momentum" supports making proximity the primary selection input and raw 12-1 return secondary. This reverses neither report but strengthens the choice already in the Price-action rulebook.
- Nigam-Pandey and "Timing the Tide" both favour shorter formation (6 m) and more frequent rebalancing in India. That conflicts with the Freefincal annual-rebalance setup and with NSE's semi-annual cadence. Whether the gain survives costs is untested in what was retrievable.

### Gaps
- Full Indian results (Sharpe, costs) for Nigam-Pandey, "Timing the Tide", Raju and the 2002-2017 PEAD paper were not retrievable; authors of the PEAD paper were not identified.
- No ISB or NSE working paper on swing-horizon (1-6 week) Indian strategies net of post-2024 costs was found.
- No live-only (Aug 2020-2026) CAGR for Nifty200 Momentum 30 versus its back-calculated history was found.
- The NSE 2026 momentum whitepaper (niftyindices.com) was blocked.

---

## Q3. Momentum crash risk and regime filters (Daniel-Moskowitz, Barroso-Santa-Clara): do they hold in India?

### Takeaway
In the US, scaling momentum by its own lagged volatility, or forecasting crashes in "panic states", roughly doubles Sharpe, mainly by avoiding crashes. A 2022 Indian paper on 450 BSE stocks reports the same: **risk-managed momentum doubled the adjusted Sharpe ratio** and cut downside risk. Indian momentum crashes (−62% in 2000, −52.8% in 2008-09, −31.8% in 2024-25) match the panic-state/rebound pattern. Practitioner crash-avoidance variants in India (Stockviz HTP) reduce drawdowns at the cost of long cash spells. Evidence for the direction is good; evidence for net-of-cost magnitude in India is thin.

### Cited Findings
- **Barroso & Santa-Clara, "Managing the Risk of Momentum" (JFE 2015)**: scaling a long-short decile momentum portfolio by inverse 6-month realised volatility (12% target) raised average annual return from 14.5% to **16.5%** over Jul 1926-Dec 2011. The gain comes mostly from crash avoidance. — [CXO Advisory](https://www.cxoadvisory.com/momentum-investing/avoiding-momentum-strategy-crashes/); [ETF.com Swedroe](https://www.etf.com/node/94292.md); [Alpha Architect](https://alphaarchitect.com/avoiding-momentum-crashes/)
- **Daniel & Moskowitz, "Momentum Crashes"**: crashes occur in panic states, after market declines with high volatility and coinciding with rebounds. Examples: Jul-Aug 1932, when losers returned +232% vs winners +32%; Mar-May 2009, losers +163% vs winners +8%. A dynamic, volatility-scaled strategy roughly doubles alpha and Sharpe. — [NBER w20439](https://www.nber.org/system/files/working_papers/w20439/w20439.pdf)
- **India: Singh, Walia, Panda & Gupta, "Risk-Managed Momentum: An Evidence from Indian Stock Market"** (FIIB Business Review 11(3), Sep 2022), 450 BSE stocks: relative momentum yields substantial profits but is negatively skewed and prone to occasional severe losses. The proposed risk-managed momentum **doubled the adjusted Sharpe ratio and significantly improved downside risk measures**. — [Sage](https://journals.sagepub.com/doi/10.1177/23197145211023001)
- India: no long-run (2-5 year) momentum reversal was observed in one Indian study, which differs from US findings. — [Universidade Católica thesis (search summary)](https://repositorio.ucp.pt/bitstreams/be7fa383-f5c7-406b-b753-4ea8e49d5d23/download) (attribution uncertain); this contrasts with the 3-year contrarian returns in [NSE res_paperfinal223](https://nsearchives.nseindia.com/content/research/res_paperfinal223.pdf)
- **Stockviz HTP ("Decomposing Momentum") backtest on Indian stocks**: avoided crashes with decent returns versus other large-cap momentum strategies, but stayed about 70% in cash and missed bear-market rallies. Single picks fell 30% in a month, which forced a more dynamic and costlier version. — [Stockviz](https://stockviz.substack.com/p/momentum-without-the-crash)
- Capitalmind: naive monthly Indian momentum has suffered drawdowns of more than 50% "every 3-4 years". — [Capitalmind](https://www.capitalmind.in/insights/momentum-investing-basics-india)
- Momentum-index episode: Nifty200 Momentum 30 fell −31.79% vs −15.35% for the Nifty 50 (Sep 2024-Apr 2025). — [HDFC MF](https://files.hdfcfund.com/s3fs-public/Others/2026-07/HDFC%20NIFTY200%20Momentum%2030%20Index%20Fund_PPT%20%28July%202026%29.pdf); [DSIJ](https://insights.dsij.in/dsijarticledetail/active-momentum-funds-timing-the-market-or-tapping-the-trend-51070)

### Inferences
- Volatility scaling of the momentum *exposure* is the best-supported crash control and has Indian peer-reviewed support (Singh et al. 2022). For a composite indicator, this argues for a **regime/vol multiplier** on the momentum component, using lagged realised volatility of a momentum basket or India VIX. A binary on/off switch is less supported.
- The 2024-25 Indian drawdown happened without a market crash. It was a rotation in which small and mid caps fell 18-22% (per the existing notes). Panic-state filters keyed on index declines may not catch rotation crashes; scaling by realised momentum volatility is more general.
- The cost of vol-scaling in India (extra turnover at each rescaling) is unmeasured. Rescaling with a band, per Novy-Marx-Velikov's buy/hold spread, is the obvious mitigation.

### Gaps
- Singh et al. (2022) full numbers (Sharpe before/after, period, costs) were not retrievable.
- No Indian test of Daniel-Moskowitz's *dynamic* (bear-market-plus-variance forecast) weighting was found.
- No Indian test was found that compares a 200-DMA index filter against vol-scaling for momentum, net of costs.

---

## Q4. What filters (volume, trend, volatility) materially improve a swing signal in tests? Which components belong in a swing+intraday composite, and which are folklore?

### Takeaway
Tested improvements that hold up:
1. **Volatility adjustment** of momentum: NSE's own score and Barroso/Santa-Clara/Daniel-Moskowitz/Singh et al.
2. **52-week-high proximity** in place of, or alongside, raw returns.
3. **Industry/factor neutralisation** for short-term reversal (Robeco).
4. **Low volume** as a conditioner. Low-volume winners persist longer and low-volume one-day losers reverse in India, while the high-volume premium is weak in India.
5. **Cost-aware construction** (buy/hold bands).

Trend filters such as close above the 200-DMA are widely used in RSI(2)-style systems but have no out-of-sample test beyond practitioner retests. Volatility-contraction (squeeze/NR7/VCP) and delivery-% filters are untested and should be treated as folklore until proven.

### Cited Findings
- **Volume**: Lee & Swaminathan (JF 2000): high (low) past-turnover firms have glamour (value) traits, earn lower (higher) future returns, and have more negative (positive) earnings surprises for eight quarters. **High-volume winners reverse faster.** — [RePEc](https://ideas.repec.org/a/bla/jfinan/v55y2000i5p2017-2069.html); [Cochrane-hosted PDF](https://www.johnhcochrane.com/s/lee_swaminathan_returns_volume_JF.pdf)
- India volume: the high-volume return premium is weak, significant only at 10% when value-weighted. — [Singh, Wang & Hua 2025, PBFJ](https://ideas.repec.org/a/eee/pacfin/v91y2025ics0927538x2500126x.html) (via blueprint). Low-volume one-day losers reverse profitably. — [IDEAS](https://ideas.repec.org/a/eme/rbfpps/rbf-06-2018-0058.html)
- **Liquidity in India**: momentum's net alpha sits in the low-turnover (illiquid) half. — [Freefincal](https://freefincal.com/is-indian-momentum-investing-just-a-liquidity-illusion/)
- **Volatility adjustment**: NSE's Normalised Momentum Score divides 6 m and 12 m returns by daily volatility. — [HDFC MF](https://files.hdfcfund.com/s3fs-public/Others/2026-07/HDFC%20NIFTY200%20Momentum%2030%20Index%20Fund_PPT%20%28July%202026%29.pdf). Portfolio-level vol scaling also helps. — [CXO](https://www.cxoadvisory.com/momentum-investing/avoiding-momentum-strategy-crashes/); [Sage, Singh et al. 2022](https://journals.sagepub.com/doi/10.1177/23197145211023001)
- **Reversal construction**: neutralising short-term reversal against industry and factor momentum about doubles risk-adjusted returns and keeps it effective over time. Residual reversal stays significant net of costs in large caps post-1990. — [Robeco](https://www.robeco.com/en-uk/insights/2023/10/reversing-the-trend-of-short-term-reversal); [CXO](https://www.cxoadvisory.com/?p=15847)
- **Cost mitigation**: a buy/hold spread is the most effective simple tool, and anomalies under 50% monthly one-sided turnover survive. — [NBER w20721](https://nber.org/papers/w20721)
- **Earnings-reaction filter**: the announcement-window abnormal return (EAR) predicts drift. — [Ivey PDF](https://www.ivey.uwo.ca/media/3775558/when_two_anomalies_meet.pdf)
- **Volatility-contraction filters**: unfiltered BB-squeeze direction accuracy was 49.6% across 399 events; the "graded" 71.4% subset is an in-sample selection. — [LuxAlgo](https://www.luxalgo.com/library/concept/squeeze-release-direction.md). No academic test was found.
- **Trend filter on mean reversion**: RSI(2) with a 200-DMA filter still trails buy-and-hold on the S&P 500 and Nasdaq-100 over 10 years, despite 71-75% win rates. — [Backtrex](https://backtrex.com/en/backtests/connors-rsi-2-sp-500)
- **Delivery %**: no peer-reviewed predictive evidence. — [ResearchGate (Bajaj group only)](https://www.researchgate.net/publication/377565241_A_Comparative_Study_on_Relationship_between_Delivery_Quantity_to_Total_Quantity_traded_Ratio_and_Stock_returns_in_Bajaj) (via prior notes)

### Inferences

**Composite-indicator component grading (author synthesis from the findings above)**

| Component | Grade | Role in a swing+intraday composite | Basis |
|---|---|---|---|
| Vol-adjusted 6-12 m momentum (NSE-style score) | **A: core input** | Slow selection layer (weekly/monthly update, with a buy/hold band) | JT OOS, HXZ replicates, IIMA, NSE index, Nigam-Pandey; decay and cost caveats |
| 52-week-high proximity (close ÷ 252-day high) | **A: core input** | Selection; possibly the primary momentum proxy in India | George-Hwang, Raju India 2004-2023 |
| Momentum-volatility / regime multiplier | **A−: risk input** | Scales the composite's exposure, not the stock ranking | Barroso-SC, Daniel-Moskowitz, Singh et al. India 2022 |
| Industry/factor-neutral 1-5 day reversal | **B: timing input only** | Entry timing inside the selected universe ("pullback in leader") | Real gross in India; fails HXZ standalone; net only in large-cap/residual form |
| Low relative volume on the pullback / avoid high-volume extension | **B−: conditioner** | Modifier on reversal and momentum | Lee-Swaminathan; Indian low-volume loser reversal; weak high-volume premium in India |
| Post-results EAR/SUE (entry after results) | **C: candidate, test first** | Event flag | US decayed except microcaps; Indian evidence split |
| Index 200-DMA / trend-state filter | **C: exposure filter** | Market regime gate, not alpha | BLL decayed post-1986; India index rules weakest; useful mainly in sharp falls |
| Donchian/new-high breakout on single stocks | **C: needs long holds** | Exit/trailing logic, not a short swing signal | Concretum: edge in <7% of trades held about 305 days |
| RSI(2)/Bollinger mean reversion as a standalone | **D: folklore as alpha** | At most a timing tiebreaker | No OOS test; trails buy-and-hold; Sensex RSI negative pre-cost |
| BB squeeze / NR7 / VCP contraction | **D: folklore (untested)** | Research-only feature | Coin-flip direction unfiltered; no peer-reviewed test |
| Delivery % | **D: folklore (untested)** | Logging only | No evidence |

- Overall design implication: build a *slow* selection score (momentum, 52-week high, vol-adjusted) with a *fast* timing overlay (residual short-term reversal on low volume) and a *risk* multiplier (momentum-vol or India VIX). Each leg has separate evidence; **no published test of the combined rule net of Indian costs exists**, so the composite itself is untested.
- Expected live edge after McLean-Pontiff-style decay is materially below the back-tested Indian numbers (about 18-22%/yr gross factor returns). The blueprint's planning ranges (+0 to +0.2R per trade after costs) remain the appropriate prior.

### Gaps
- No test was found in any market of a combined "momentum or 52-week-high selection + short-term-reversal entry" rule net of costs, or of the intraday component of such a composite.
- No peer-reviewed evidence was found that volatility contraction (squeeze/NR7/ATR percentile) improves breakout success.
- No Indian test of volume filters at swing horizons net of costs was found.
- The improvement from a 200-DMA filter on Indian stock-level swing signals is untested.
