# ML and Composite/Ensemble Signal Approaches for Stock-Direction Prediction (daily + intraday), and How Quants Combine Weak Signals

Research note, as of October 2026. Session caveat: NBER, arXiv, SSRN, AQR, Yale, NUS, Hudson Thames, Taylor & Francis, FAU and several Substack/blog domains were blocked for full-text fetch, so most findings below come from search-result abstracts and summaries of primary papers, not from reading full papers. Numbers I remember from the papers but could not re-check this session are kept out of Cited Findings and listed under Gaps or flagged as unverified.

## Q1. What out-of-sample, net-of-cost results do ML return-prediction papers report, and how much do they degrade for large/liquid stocks and after costs?

### Takeaway
Top ML papers (Gu-Kelly-Xiu 2020; Jiang-Kelly-Xiu 2023) report large gross out-of-sample Sharpe ratios, but these come mostly from equal-weighted, small-stock, high-turnover portfolios. Value-weighted results are several times smaller: for the chart-image CNN, the Sharpe ratio falls from 2.4 equal-weighted to 0.5 value-weighted. Once you drop microcaps and charge realistic trading costs, much of the alpha goes away (Avramov-Cheng-Metzker). LLM news signals show the same pattern: they work best in small stocks and after negative news, and recent studies find that much of the reported LLM "alpha" comes from look-ahead or memorisation bias.

### Cited Findings
**Gu, Kelly and Xiu (2020, RFS), "Empirical Asset Pricing via Machine Learning"**
- Trees and neural networks perform best. The gains come from nonlinear interactions between predictors. ML forecasts "in some cases" double the performance of leading regression-based strategies. — [NBER w25398 listing / search abstract](https://www.nber.org/papers/w25398.pdf); [SSRN abstract](https://papers.ssrn.com/abstract=3159577)
- Portfolio strategies built on ML forecasts earn annualised Sharpe ratios above 2.0. — [Mirova research summary](https://www.research-center.mirova.com/en/research-library/Empirical-Asset-Pricing-via-Machine-Learning); [SFI press release](https://www.sfi.ch/resources/public/dtc/media/press-release_english_opa_0.pdf)
- A penalised or dimension-reduced linear model with 900+ predictors recovers only a modest monthly out-of-sample R² of about 0.26%. Even the best ML R² values are well under 1% per month. — [Mirova summary](https://www.research-center.mirova.com/en/research-library/Empirical-Asset-Pricing-via-Machine-Learning)

**Jiang, Kelly and Xiu (2023, JF), "(Re-)Imag(in)ing Price Trends"**
- This is a CNN trained on images of OHLC and volume charts. Its out-of-sample accuracy at predicting up vs. down one-month returns is just over 53%. — [Paper PDF (search excerpt)](https://www.aidf.nus.edu.sg/wp-content/uploads/2022/02/Xiu-Re-Imagining-Price-Trends.pdf)
- Monthly-horizon decile long-short Sharpe ratios reach 2.4 equal-weighted but only 0.5 value-weighted. For the I5/R20, I20/R20 and I60/R20 models, the high-minus-low Sharpe ratios are 2.4, 2.2 and 1.3. — [Paper PDF (search excerpt)](https://www.aidf.nus.edu.sg/wp-content/uploads/2022/02/Xiu-Re-Imagining-Price-Trends.pdf); [kuntara mirror](https://www.kuntara.net/uploads/1/1/4/9/114945401/ssrn-id3756587-3.pdf)
- Weekly strategies earn gross Sharpe ratios as high as 7.2 equal-weighted and 1.7 value-weighted. At quarterly horizons, Sharpe ratios are 1.3 equal-weighted and 0.5 value-weighted. — [Paper PDF (search excerpt)](https://www.aidf.nus.edu.sg/wp-content/uploads/2022/02/Xiu-Re-Imagining-Price-Trends.pdf)
- The patterns are distinct from standard trend and momentum signals. Patterns learned at short scales also work at longer scales, and patterns learned on US stocks "predict equally well in international markets" (transfer learning). — [Yale Economics summary](https://economics.yale.edu/research/re-imagining-price-trends); [SUFE academic newsletter](https://academicnewsletter.sufe.edu.cn/info/358989)
- Independent replication and extension code exists, including transfer to Chinese stocks. — [GitHub replication](https://github.com/George-hardworking/reimaging-price-trends-replication-and-extension)

**Avramov, Cheng and Metzker (Management Science; 2021 WP), "Machine Learning vs. Economic Restrictions"**
- The value-weighted ML long-short earns 0.95%–2.18% per month gross, or 0.62%–1.87% after FF6 adjustment. — [Quantpedia / AP summary](https://www.advisorperspectives.com/articles/2023/03/06/the-promise-of-machine-learning)
- Performance drops a lot when microcaps, distressed and non-rated firms are excluded. It falls further under reasonable trading costs because of high turnover and extreme positions. An average investor "would struggle to achieve alpha after taking transaction costs into account." — [Advisor Perspectives](https://www.advisorperspectives.com/articles/2023/03/06/the-promise-of-machine-learning); [CUHK Business School summary](https://cbk.bschool.cuhk.edu.hk/the-limitations-of-using-ai-to-pick-stocks/); [HUJI record](https://cris.huji.ac.il/en/publications/machine-learning-vs-economic-restrictions-evidence-from-stock-ret/)

**Leippold, Wang and Zhou (2022, JFE), "Machine Learning in the Chinese Stock Market"** (a useful analogue for India: retail-heavy market with short-sale limits)
- Retail dominance boosts short-term predictability, especially for small stocks. Large stocks and SOEs are unusually predictable at longer horizons. Liquidity is the most important predictor, and out-of-sample performance stays economically significant after transaction costs. — [SFI publication page](https://www.sfi.ch/en/publications/machine-learning-in-the-chinese-stock-market); [ZIBS summary](https://zibs.zju.edu.cn/enzibs/2024/0729/c82471a2972355/page.htm)

**Krauss, Do and Huck (2017, EJOR)**
- Deep neural networks, gradient-boosted trees, random forests and their ensembles were trained on lagged returns of survivorship-bias-free S&P 500 constituents for daily statistical arbitrage. — [IDEAS/RePEc](https://ideas.repec.org/a/eee/ejores/v259y2017i2p689-702.html)
- I could not verify the paper's specific gross and net numbers this session (see Gaps).

**LLM / news-sentiment signals**
- Lopez-Lira and Tang ("Can ChatGPT Forecast Stock Price Movements?"):
  - ChatGPT's headline scores predict next-day returns out of sample and subsume traditional sentiment measures.
  - Predictability is stronger in smaller stocks and after negative news, which suggests the market underreacts.
  - Older models (GPT-1, GPT-2, BERT) fail. GPT-4 long-short gives the highest Sharpe ratio.
  - The latest version (Oct 2025) adds analysis of headlines published after the models' knowledge cutoff.
  - — [arXiv 2304.07619](https://arxiv.org/abs/2304.07619v5); [UCLA Anderson slides](https://www.anderson.ucla.edu/sites/default/files/document/2024-04/4.19.24%20Alejandro%20Lopez%20Lira%20ChatGPT_V3.pdf)
- Look-Ahead-Bench (2026) tested LLM stock selection in-sample (Apr–Sep 2021) vs. after the training cutoff (Jul–Dec 2024):
  - DeepSeek 3.2 went from +20.73% annualised alpha to −1.04%; Llama 3.1 8B went from +13.81% to −3.42%.
  - Larger models showed more decay, consistent with memorisation.
  - — [arXiv 2601.13770](https://arxiv.org/pdf/2601.13770); [Hedge Fund Alpha summary](https://hedgefundalpha.com/education/your-llms-alpha-might-be-mere-memorization/)
- "The Alpha Illusion": gross returns of LLM trading agents often become net losses, or underperform buy-and-hold, once real costs are applied. The authors blame temporal contamination, unmodelled frictions and short evaluation windows. — [search summary; see also arXiv 2608.27734 "What survives honest evaluation?"](https://arxiv.org/pdf/2608.27734)
- Practitioners argue that LLMs shorten the shelf life of copyable alpha, because many participants can industrialise the same indicators at once. — [IBKR Quant News](https://www.interactivebrokers.com/campus/ibkr-quant-news/llms-and-the-shortening-shelf-life-of-copyable-alpha/)

### Inferences
- **What a ~53% hit rate means.** For a large-cap swing or intraday trader, the honest reading of the flagship CNN result is roughly "53% directional accuracy and a value-weighted Sharpe around 0.5 before costs" at monthly horizons. The headline equal-weighted Sharpe of 2–7 is mostly a small-stock, gross-of-cost number.
- **Short horizons are hit hardest.** Weekly and daily horizons show the biggest gross Sharpe ratios but also the highest turnover, so they lose the most to costs. The weekly value-weighted Sharpe of 1.7 has no reported net-of-cost counterpart in the excerpts I saw.
- **Use LLM signals only point-in-time.** Any LLM/news component should be backtested only on data after the model's training cutoff, or it should be treated as contaminated.
- **What may transfer to India.** The Chinese result (retail-driven short-term predictability, liquidity as the top predictor, profits surviving costs) suggests that Indian mid/small caps may show more short-horizon predictability than the US. However, the most predictable names are also the least liquid, so costs and impact matter a lot.

### Gaps
- I could not fetch full GKX tables, so these figures from memory are unverified this session: NN3 monthly OOS R² of about 0.40%, sharp R² drop for top-1000 stocks, value-weighted NN Sharpe of about 1.35, and turnover.
- I could not verify the Krauss-Do-Huck ensemble daily return before and after a 5 bps half-turn cost, or the reported post-2010 decay. My recollection: about 0.45%/day pre-cost, around 0.25%/day net, with sharp decay in later years. This is unverified.
- No net-of-cost figure for the JKX weekly CNN strategy was confirmed.

## Q2. Are there rigorous Indian (NSE) studies of ML/deep learning for stock prediction? Flag low-quality ones.

### Takeaway
I found only one clearly rigorous, finance-journal-grade Indian ML cross-section study: Lalwani and Meshram (2022, Applied Economics). It uses a survivorship-free sample from 1994–2019, rolling one-month-ahead out-of-sample forecasts, and 35 characteristics. Most other NSE "ML/LSTM prediction" papers are engineering or conference papers. They predict price levels, use short samples, ignore costs, and report RMSE or accuracy rather than tradable net returns. Treat them as low quality for trading decisions.

### Cited Findings
- **Lalwani and Meshram (2022)**, "The cross-section of Indian stock returns: evidence using machine learning", *Applied Economics* 54(16): 1814–1828:
  - Survivorship-bias-free sample of all firms on the major Indian exchanges, 1994–2019, with 35 characteristics.
  - 14 predictors have t-stats above 3 in Fama-MacBeth regressions.
  - Rolling one-month-ahead out-of-sample forecasts using elastic net, random forests and XGBoost.
  - "Substantial improvement in forecast accuracy" over OLS, and strategies "provide significant returns".
  - — [IDEAS/RePEc](https://ideas.repec.org/a/taf/applec/v54y2022i16p1814-1828.html); [Taylor & Francis](https://www.tandfonline.com/doi/full/10.1080/00036846.2021.1982132); [ResearchGate](https://www.researchgate.net/publication/354819735_The_cross-section_of_Indian_stock_returns_evidence_using_machine_learning)
- A related IIMB repository item and an SSRN paper on cross-sectional return predictability in India exist. — [IIMB repository](https://repository.iimb.ac.in/handle/2074/19146); [SSRN 3838938](https://dx.doi.org/10.2139/ssrn.3838938)
- **Lower-quality / engineering-style NSE studies:**
  - "NSE Stock Market Prediction Using Deep-Learning Models" (Procedia CS 2018) compares MLP, RNN, LSTM and CNN on NSE prices. It uses price-level prediction with no trading or cost evaluation. — [ScienceDirect](https://www.sciencedirect.com/science/article/pii/S1877050918307828); [Amrita](https://www.amrita.edu/publication/nse-stock-market-prediction-using-deep-learning-models/)
  - Sen/Mehtab-style LSTM regression papers on NIFTY use walk-forward validation (e.g. Dec 2018–Jul 2020) but forecast prices and report error metrics, not net P&L. — [arXiv 2009.10819](https://arxiv.org/pdf/2009.10819); [arXiv 2208.07166](https://arxiv.org/pdf/2208.07166)
  - Others include "Stock Market Prediction of NIFTY 50 Index Applying Machine Learning Techniques" (Applied AI, 2022) and "Intraday Stock Prediction Based on Deep Neural Network" (NSE data 2008–2018, five-layer DNN on candlesticks and technical indicators). They report classification accuracy; I found no evidence of cost-adjusted, walk-forward trading tests. — [T&F](https://www.tandfonline.com/doi/full/10.1080/08839514.2022.2111134); [ResearchGate](https://www.researchgate.net/publication/337994109_Intraday_Stock_Prediction_Based_on_Deep_Neural_Network)
  - A Springer IJSAEM paper (2023) "Analysis and prediction of Indian stock market: a machine-learning approach" falls in the same category. — [Springer](https://link.springer.com/article/10.1007/s13198-023-01934-z)

### Inferences
Red flags for Indian ML papers:
- They predict price levels, so high R² is trivial because price is non-stationary.
- They use a single train/test split or a short window (2–3 years).
- They don't account for STT, brokerage, exchange fees, impact or slippage.
- They have survivorship bias, for example by using current NIFTY 50 constituents.
- Accuracy is reported without any P&L.
- They are published in conference, IJACSA or Procedia-type outlets.

By these criteria, Lalwani-Meshram is the benchmark. It is a monthly cross-sectional study, not daily or intraday.

### Gaps
- I found no rigorous, peer-reviewed NSE study of daily or intraday ML direction prediction that uses walk-forward testing plus realistic Indian costs (STT, stamp duty, GST, brokerage) and impact.
- I could not extract Lalwani-Meshram's exact out-of-sample R², long-short spreads, or whether they net out costs.

## Q3. Best practice for building a composite indicator from several evidence-backed components across two timeframes, without overfitting

### Takeaway
Combine standardised components (cross-sectional or time-series z-scores, or ranks, with winsorising) using equal or lightly shrunk weights. Equal weights are notoriously hard to beat out of sample (the "forecast combination puzzle"). The fundamental law, IR ≈ IC × √breadth, tells you a composite of weakly correlated signals improves IC only modestly, and the transfer coefficient shrinks it further. Control overfitting with walk-forward or purged cross-validation and the Deflated Sharpe Ratio. Use meta-labeling (a secondary model that decides whether to act on, and how big to size, a primary signal) rather than fitting one big model.

### Cited Findings
**Equal weights vs. estimated weights**
- Combinations with estimated "optimal" weights often do worse than equal weights (Stock and Watson 2004, the "forecast combination puzzle"). Estimated weights add bias and variance, so the "optimal" combination is not guaranteed to beat equal weights. — [VU Amsterdam / Claeskens et al.](https://research.vu.nl/en/publications/the-forecast-combination-puzzle-a-simple-theoretical-explanation-2/); [Tinbergen WP](https://econpapers.repec.org/RePEc:dgr:uvatin:20140127)
- Practitioner tests of alpha combinations find naive equal weighting "surprisingly hard to beat". Fitted methods improve in-sample but give some back out of sample, especially with short estimation samples. — [Delphic Alpha, "Alpha Combinations Part 2"](https://delphicalpha.substack.com/p/alpha-combinations-part-2-from-theory)
- JOIM discusses efficiently combining multiple alpha sources. — [JOIM](https://joim.com/efficiently-combining-multiple-sources-of-alpha)
- MSCI/Barra's "Converting Scores into Alphas" uses the Grinold rule: alpha = volatility × IC × score, where the score is a standardised z-score. — [MSCI PDF](https://www.msci.com/documents/10199/1645561/PI_Converting_Scores_Into_Alphas.pdf/7adf1f42-10aa-40eb-9e8c-ecc11eeba2d4)

**Fundamental law and IC**
- Fundamental law: IR = IC × √Breadth, where breadth is the number of independent bets per year. IC and breadth are not independent in practice: loosening thresholds to get more breadth adds false positives and lowers IC. The theoretical IR overstates what is achievable, and the "full law" adds a transfer coefficient for constrained portfolios. — [CFI](https://corporatefinanceinstitute.com/resources/capital-markets/fundamental-law-of-active-management/); [AnalystPrep CFA L2](https://analystprep.com/study-notes/cfa-level-2/state-and-interpret-the-fundamental-law-of-active-portfolio-management-including-its-component-terms-transfer-coefficient-information-coefficient-breadth-and-active-risk-aggressiveness/); [Quant 4.0, arXiv 2301.04020](https://arxiv.org/pdf/2301.04020)
- Practitioner benchmarks: IC 0.05 is "good" and 0.10 "very good". In liquid large caps, an IC around 0.03 is genuinely good. Consistency of IC matters as much as size. — [FE Training](https://www.fe.training/?p=10077911); [R-bloggers factor evaluation](https://r-bloggers.com/2015/03/factor-evaluation-in-quantitative-portfolio-management)

**Overfitting control**
- The Deflated Sharpe Ratio (Bailey and López de Prado 2014, JPM 40(5)) corrects for selection bias from multiple trials and for non-normal returns. Without controlling for the number of trials, expectations are over-optimistic. — [SSRN 2460551](https://papers.ssrn.com/abstract=2460551); [Bailey overfit tools](https://www.davidhbailey.com/dhbpapers/overfit-tools-at.pdf); [CXO Advisory](https://www.cxoadvisory.com/big-ideas/backtest-overfitting-the-movies/)

**Triple-barrier labeling and meta-labeling (López de Prado, *Advances in Financial ML*, 2018)**
- Triple-barrier labeling uses a profit-take barrier, a stop-loss barrier (both scaled to volatility) and a time barrier. It fixes the problem that fixed-horizon labels ignore heteroskedasticity and stop or target exits. — [Hudson Thames](https://hudsonthames.org/does-meta-labeling-add-to-signal-efficacy/)
- Meta-labeling uses a secondary classifier to decide whether to take or skip each primary signal, and how big the bet should be. Hudson Thames reports that event-based sampling plus triple-barrier plus meta-labeling improved strategy performance in their tests. This is vendor or practitioner evidence, not peer-reviewed out-of-sample evidence. — [Hudson Thames research update](https://hudsonthames.org/?p=3448)

### Inferences
Suggested recipe for a daily-trend + intraday-timing composite (my synthesis, not taken from a single source):
1. **Daily layer.** Standardise each evidence-backed daily component (for example 12-1 momentum, trend or MA slope, and an earnings/news-drift flag). Use cross-sectional or rolling time-series z-scores, winsorise at ±3, and average with equal weights. Check pairwise correlations and collapse near-duplicates first, so one idea is not counted twice.
2. **Use the daily score as the direction gate.** Only trade in its sign, and only when it clears a threshold.
3. **Intraday layer for timing and sizing only.** Use things like opening-range behaviour, VWAP deviation or volume surges, ideally as a meta-labeling filter, not as an independent alpha with its own weight. This keeps the number of fitted parameters small.
4. **Validate the whole thing with walk-forward or purged/embargoed cross-validation.** Report the Deflated Sharpe Ratio and count every variant you tried.
5. **Expect the composite IC to be only modestly above the best single component.** With roughly 3–5 correlated weak signals, the gain is far less than √N.

**Signal decay and half-life:** estimate the IC of each component at horizons 1, 2, 5, 10 and 20 days. Holding period and smoothing (EWMA half-life) should match the horizon where IC peaks. Short-horizon signals decay within days and need cheap execution.

### Gaps
- I did not obtain a primary-source quantification of the IC gain from combining N signals with correlation ρ, or of typical half-lives for Indian daily and intraday signals.
- I found no peer-reviewed out-of-sample evidence that meta-labeling improves net returns on equities. The evidence is practitioner-only (Hudson Thames; Joubert 2022 in JFDS, not verified this session).
- Regime-switching combination (e.g. HMM-weighted signals) was not covered by any source I could access.

## Q4. What position-sizing and exit rules (volatility targeting, ATR stops, time stops, triple-barrier) are supported by evidence?

### Takeaway
- **Volatility scaling:** the best-supported rule. For risk assets like equities it reduces tail risk and modestly raises Sharpe ratios. The famous Moreira-Muir alphas are fragile out of sample and after costs.
- **Stop-losses:** add value only when returns have momentum or serial correlation. Under a random walk they always lower expected return. On momentum portfolios they sharply cut crash losses.
- **ATR stops and time stops:** I found no rigorous standalone academic evidence. They are reasonable as vol-scaled versions of the barriers in the triple-barrier framework.

### Cited Findings
**Volatility targeting**
- Harvey, Hoyle, Korgaonkar, Rattray, Sargaison and Van Hemert (Man Group), "The Impact of Volatility Targeting":
  - Vol targeting raises Sharpe ratios for risk assets (equity, credit) via the leverage effect.
  - Its effect is negligible for bonds, FX and commodities.
  - It reduces the likelihood of extreme returns in all asset classes.
  - — [Quantpedia](https://quantpedia.com/the-impact-of-volatility-targeting-on-equities-bonds-commodities-and-currencies); [Alpha Architect](https://alphaarchitect.com/volatility-targeting-improves-risk-adjusted-returns/); [CXO Advisory](https://www.cxoadvisory.com/equity-premium/benefits-of-volatility-targeting-across-asset-classes)
- Moreira and Muir: volatility-managed factor portfolios produce large alphas and higher Sharpe ratios. However:
  - Cederburg et al. find the gains fail out of sample.
  - Barroso and Detzel find they don't survive transaction costs.
  - Implementation is especially hard in large caps.
  - — [Advisor Perspectives](https://www.advisorperspectives.com/articles/2018/12/17/dont-ride-out-the-storm-why-you-can-and-should-time-volatility); [Nazarbayev Univ. thesis summary](https://nur.nu.edu.kz/items/cde07ded-0481-4157-ba8f-65e69365d0d3); [LBS multifactor perspective](https://lbsresearch.london.edu/id/eprint/3716)

**Stop-losses**
- Kaminski and Lo (2014, J. Financial Markets), "When Do Stop-Loss Rules Stop Losses?":
  - Under the random walk hypothesis, simple 0/1 stop-loss rules always lower expected return. With momentum they can add value.
  - In US equities from 1950–2004, certain stop rules added 50–100 bps per month during stop-out periods.
  - — [MIT DSpace](https://dspace.mit.edu/handle/1721.1/114876); [PDF](https://dspace.mit.edu/bitstream/handle/1721.1/114876/Lo_When%20Do%20Stop-Loss.pdf)
- Han, Zhou and Zhu, "Taming Momentum Crashes: A Simple Stop-Loss Strategy":
  - A 10% stop-loss on individual momentum positions cut the worst monthly loss from −49.79% to −11.34% equal-weighted, and from −65.34% to −23.69% value-weighted.
  - Average returns and Sharpe ratios more than doubled.
  - — [Alpha Architect](https://alphaarchitect.com/2016/08/taming-the-momentum-roller-coaster-fact-or-fiction/); [Crossing Wall Street](https://www.crossingwallstreet.com/?p=28384)
  - Alpha Architect's review questions how robust this is (the title asks "fact or fiction"). In particular, daily monitoring and intramonth execution assumptions matter. Treat the doubling of Sharpe ratio with caution.

**Triple-barrier**
- Triple-barrier labeling (vol-scaled profit-take and stop plus a time barrier) is a labeling and backtest construct from López de Prado. Its support is methodological and practitioner, not peer-reviewed evidence of improved net returns. — [Hudson Thames](https://hudsonthames.org/does-meta-labeling-add-to-signal-efficacy/)

### Inferences
Evidence-consistent rules for a swing/intraday composite:
- **Sizing:** size positions inversely to recent volatility (ATR or EWMA vol) and target constant risk per trade. This is supported mainly for reducing tail risk.
- **Stops:** use volatility-scaled stops (e.g. a multiple of ATR) only for a momentum or trend component, where the theory says stops can add value. Avoid tight stops on mean-reversion entries.
- **Time stop:** a time stop that matches the horizon where the signal's IC peaks follows directly from signal decay. It is the "vertical barrier" in the triple-barrier framework.

### Gaps
- I found no rigorous study of ATR-multiple stops specifically, and no evidence on stops or vol targeting for Indian equities or intraday NSE trading.
- I could not verify the Cederburg et al. and Barroso-Detzel details from primary sources (only secondary summaries).

## Q5 (cross-cutting). Realistic accuracy and IC expectations

### Takeaway
Realistic, honest targets for a stock-direction composite:
- **Directional hit rate:** about 51–54%. The best published deep-learning chart model is just over 53% at the one-month horizon.
- **Cross-sectional IC:** about 0.02–0.05, where 0.03 counts as good for liquid large caps.
- **Out-of-sample R²:** below 1% per month.

Anything much above these in a backtest, such as 60%+ daily accuracy, is a strong signal of leakage, look-ahead or overfitting.

### Cited Findings
- JKX CNN out-of-sample up/down accuracy is just over 53% for one-month returns. — [Paper PDF excerpt](https://www.aidf.nus.edu.sg/wp-content/uploads/2022/02/Xiu-Re-Imagining-Price-Trends.pdf)
- GKX penalised linear models reach about 0.26% monthly out-of-sample R². — [Mirova summary](https://www.research-center.mirova.com/en/research-library/Empirical-Asset-Pricing-via-Machine-Learning)
- IC benchmarks: 0.05 good, 0.10 very good; about 0.03 good in liquid large caps. — [FE Training](https://www.fe.training/?p=10077911)
- LLM alpha that looks strong in-sample turns negative after the training cutoff. — [Look-Ahead-Bench](https://arxiv.org/pdf/2601.13770)

### Inferences
**Why small edges can still be valuable:**
- Under the fundamental law, an IC of 0.03 across about 100 independent bets per year gives an IR of about 0.3. Across 1,000 independent bets it gives about 0.95.
- For a single-stock or small-basket trader, breadth is low, so even a genuine edge yields a modest IR, and costs (STT, impact) can wipe it out.
- An accuracy of 52–53% only pays if average wins are at least as large as average losses and the cost per round trip is small relative to the average move.

### Gaps
- I found no authoritative source for a "51–53% daily hit-rate" benchmark specific to Indian stocks.
