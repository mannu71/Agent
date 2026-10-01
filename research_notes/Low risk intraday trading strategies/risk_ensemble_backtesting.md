# Risk Management, Strategy Combination, Regime Detection and Backtesting for a Low-Risk Intraday Trading Agent (Indian equities/F&O and crypto)

> Research note scope: position sizing, risk limits, ruin/drawdown math, strategy ensembles, regime detection, backtest validity, tooling, agent architecture and LLM reliability.
> Method caveat: web search worked, but most direct page fetches (sec.gov, wikipedia.org, business-standard.com, forklog.com, dtu.dk) were blocked by the network egress proxy. Where a finding rests only on a search-result snippet, or on standard textbook material I could not re-fetch this session, it is marked **[snippet]** or **[background, not re-verified]**. Formulas marked as standard are well-established results; the report writer should keep the citations attached.

---

## 1. Position sizing (fixed-fractional, Kelly / fractional Kelly, volatility targeting, ATR sizing)

### Takeaway
Use fixed-fractional risk (0.25–1% of equity per trade) with stop distance set by ATR, then scale overall exposure toward a volatility target. Treat Kelly only as an upper bound and run at ¼–½ Kelly at most. The evidence that volatility targeting raises Sharpe is strongest for risk assets (equities, credit, momentum). Its main dependable benefit is thinner left tails, not higher Sharpe, and out-of-sample critiques show the "alpha" from vol-managed portfolios is fragile.

### Cited Findings
- **Volatility-managed portfolios (Moreira & Muir 2017, Journal of Finance 72(4):1611–1644):** "Managed portfolios that take less risk when volatility is high produce large alphas, increase Sharpe ratios, and produce large utility gains for mean-variance investors." Documented for the market, value, momentum, profitability, ROE, investment and betting-against-beta factors and the currency carry trade. Mechanism: "changes in volatility are not offset by proportional changes in expected returns." — [NBER w22208](https://www.nber.org/papers/22208); [Hedge Fund Alpha summary](https://hedgefundalpha.com/strategies/volatility-managed-portfolios/)
  - Standard construction **[background, not re-verified]**: f_t = (c / σ̂²_{t-1}) · f_t^unmanaged, where σ̂² is the previous month's realized variance from daily returns and c scales the managed and unmanaged portfolios to the same unconditional volatility. Note that it scales by **inverse variance**, not inverse volatility. — [NBER w22208](https://www.nber.org/papers/22208)
- **Critique (Cederburg, O'Doherty, Wang & Yan, JFE Oct 2020):** across 103 equity strategies, "volatility-managed portfolios do not systematically outperform their corresponding unmanaged portfolios". The spanning-regression strategies "are not implementable in real time", and realistic out-of-sample versions "generally earn lower certainty equivalent returns and Sharpe ratios than simple investments in the original, unmanaged portfolios", mainly because of structural instability. Vol-management did help **momentum, profitability and BAB**. — [Cederburg et al. PDF (Lehigh)](https://www.lehigh.edu/~xuy219/research/COWY.pdf); [Alpha Architect summary](https://alphaarchitect.com/the-performance-of-volatility-managed-portfolios/)
- DeMiguel et al. (Journal of Finance 2024) revisit vol-managed portfolios from a multifactor angle. — [LBS repository PDF](https://lbsresearch.london.edu/id/eprint/3716/1/The%20Journal%20of%20Finance%20-%202024%20-%20DeMIGUEL%20-%20A%20Multifactor%20Perspective%20on%20Volatility%E2%80%90Managed%20Portfolios.pdf) (only the title and existence were confirmed; the findings were not fetched)
- **Harvey, Hoyle, Korgaonkar, Rattray, Sargaison & Van Hemert (2018), "The Impact of Volatility Targeting", JPM 45(1):14–33:** 60 assets, daily data from as early as 1926 to 2017, **10% annualized vol target**, volatility estimated from the standard deviation of daily returns. Vol targeting **raises Sharpe for risk assets (equities, credit)**, which they link to the leverage effect, and has a **negligible Sharpe effect for bonds, currencies and commodities**. Across **all** asset classes it **reduces the likelihood of extreme returns**: left-tail events are less severe because they tend to occur when volatility is already elevated. — [Quantpedia summary](https://quantpedia.com/the-impact-of-volatility-targeting-on-equities-bonds-commodities-and-currencies); [Alpha Architect](https://alphaarchitect.com/volatility-targeting-improves-risk-adjusted-returns/); [Duke Scholars record](https://scholars.duke.edu/publication/1370354)
- **Kelly criterion [background, not re-verified]:** for a bet with win probability p, loss probability q = 1−p and win/loss payoff ratio b, f* = p − q/b. For a continuous strategy with excess return μ and variance σ², f* ≈ μ/σ² (leverage), and the maximal growth rate is g* ≈ SR²/2. Full Kelly has a high chance of deep drawdowns: under the continuous-time approximation, the probability of ever falling to fraction x of peak wealth is x^(2/k−1) for fractional-Kelly multiple k, so at full Kelly (k=1) P(ever halving) = 50%. **Half Kelly keeps about 75% of the growth rate with about 50% of the volatility**, and P(ever halving) falls to 12.5%. Estimation error in μ is the main practical reason to under-bet, because overestimating the edge pushes you past Kelly, where growth falls quickly and turns negative at 2× Kelly. Classic references: Kelly (1956); Thorp, "The Kelly Criterion in Blackjack, Sports Betting and the Stock Market"; MacLean, Thorp & Ziemba (2011), *The Kelly Capital Growth Investment Criterion*. (No URL was fetched this session, so the report writer should cite these as textbook results.)
- A 2026 arXiv preprint proposes using conformal prediction intervals to set the fractional-Kelly scale ("Conformal Kelly"), which shows the idea of scaling Kelly by forecast uncertainty is current. — [arXiv 2608.01494](https://arxiv.org/pdf/2608.01494) [snippet; title only]
- **Meta-labeling for sizing:** Joubert's "Meta-Labeling: Calibration and Position Sizing" (Journal of Financial Data Science) tests six position-sizing algorithms, including a new "sigmoid optimal position sizing" (SOPS), driven by the secondary model's probability of a profitable trade. **Probability calibration significantly improves fixed position-sizing methods**, while methods that fit their sizing function on training data gain little from calibration. Strategies were evaluated on Sharpe and maximum drawdown. — [search summary via Hudson & Thames / JFDS](https://hudsonthames.org/tag/research) [snippet; exact numbers not obtained]

### Inferences
- A concrete default recipe for the agent:
  1. **Per-trade risk budget:** R = 0.25–0.5% of equity while live-testing, with 1% as a hard maximum.
  2. **Stop distance:** d = k·ATR(n), with n = 14 bars on the trading timeframe and k = 1.5–3. An intraday ATR is usually computed on 5- or 15-minute bars.
  3. **Quantity:** Q = floor( R·Equity / (d · point_value) ). For F&O, round down to the lot size. If one lot already exceeds R, **skip the trade** rather than oversize.
  4. **Portfolio vol overlay:** multiply all position sizes by min(cap, σ_target/σ̂). Use a short-window EWMA for σ̂ (for example, a 20-day half-life on daily returns, or intraday realized vol). Set the leverage cap at about 1–2× for crypto and within SEBI/broker margin rules for F&O.
  5. **Kelly cap:** if the strategy's estimated edge implies f* below the fixed-fractional size, use f*/4. Kelly is a ceiling, not a target.
- Vol targeting probably fits intraday systems because it mainly cuts exposure when volatility spikes (crypto liquidation cascades, India VIX spikes, event days). That tail-reduction benefit appeared in every asset class Harvey et al. studied, while the Sharpe improvement did not.
- Given Cederburg et al., the agent should not claim "alpha from vol management". It should use vol scaling as a **risk control**.

### Gaps
- I could not fetch Moreira–Muir's exact alpha and Sharpe numbers (the commonly quoted figure is about 4.9% annualized alpha on the market factor, which I could not verify here).
- I found no peer-reviewed study on intraday ATR multipliers specific to Indian F&O or crypto. The ATR k and n values above are practitioner conventions, not evidence-based optima.

---

## 2. Risk limits, kill switches, and lessons from algo disasters

### Takeaway
Most algo failures that destroy a firm are control failures, not strategy failures. Knight Capital lost over $440–460M in 45 minutes because there were no automated pre-trade limits and no kill switch. Regulators (SEC Rule 15c3-5 in the US; SEBI's 2025 retail-algo framework in India) now require broker- and exchange-level pre-trade checks and kill switches. A retail agent should replicate these controls locally and not rely only on the broker's.

### Cited Findings
- **Knight Capital (1 Aug 2012):** in the first 45 minutes of trading, a router running discontinued, incorrect code sent **over 4 million orders** while trying to fill **212 small retail orders**. Knight traded **over 397 million shares**, took on **several billion dollars in unwanted positions**, and suffered a **trading loss of over $460 million**. Press reports commonly cite about $440M. The SEC found Knight "did not have adequate safeguards in place to limit the risks posed by its access to the markets." This was the **first enforcement action under the Market Access Rule (Rule 15c3-5, adopted 2010)**, and Knight paid a **$12 million** settlement (16 Oct 2013). — [SEC press release 2013-222](https://www.sec.gov/news/press-release/2013-222) [snippet; page fetch blocked]; [WilmerHale client alert](https://www.wilmerhale.com/en/insights/client-alerts/knight-capital-settles-rule-15c3-5-violations-with-sec-agrees-to-pay-12-million); [SEC Order (Justia mirror)](https://contracts.justia.com/companies/kcg-holdings-inc-18985/contract/555902)
  - Detail from the SEC order **[background, not re-verified]**: a deployment left old "Power Peg" code active on **1 of 8** SMARS servers. A reused feature flag activated it. Knight had no automated control comparing orders sent against orders filled, and no pre-set capital thresholds that would halt trading. Automated emails (about 97) warning of the error before market open were not acted on. — [SEC Order (Justia mirror)](https://contracts.justia.com/companies/kcg-holdings-inc-18985/contract/555902)
- Regulators later said some US brokers still needed better buffers against trading errors. — [Business Insurance](https://www.businessinsurance.com/some-us-brokers-still-need-buffers-against-trading-errors-regulators/) [snippet]
- **SEBI retail algo framework (circular 4 Feb 2025; effective date moved from 1 Aug 2025 to 1 Oct 2025):**
  - Exchanges keep a **kill switch** to halt orders from a particular **algo ID**.
  - Exchanges set an SOP for algo testing and run surveillance and behavioural monitoring.
  - The **threshold is 10 orders per second (OPS) per exchange**, counting placements, modifications and cancellations in any rolling one-second window. Below it, orders carry a generic algo ID with no strategy approval needed, but a **static IP and daily authentication are still required**. Above it, exchange approval and a unique strategy ID are required, and **brokers must block unapproved strategies that cross the limit**.
  — [QuantInsti blog](https://blog.quantinsti.com/sebi-algo-trading-guidelines-retail-investors); [Moneylife](https://moneylife.in/article/sebi-introduces-new-framework-for-safer-algo-trading-for-investors/76294.html); [Angel One deadline note](https://oga-prod.angelone.in/news/market-updates/sebi-pushes-algo-trading-deadline-to-october-1-2025); [Fyers](https://fyers.in/blog/sebi-algo-trading-rules-and-regulations-in-india/)
- SEBI later relaxed algo rules for the commodity derivatives segment. — [Markets Media](https://www.marketsmedia.com/sebi-relaxes-commodity-exchange-algo-rules) [snippet]

### Inferences
Below is a **deterministic risk-limit stack** for the agent, enforced in code outside any ML or LLM component. The numbers are proposed defaults for a "low-risk" mandate, not evidence-derived optima.

| Layer | Control | Suggested default |
|---|---|---|
| Pre-trade (per order) | max qty / notional / lots; price collar vs LTP (reject if > x% away); instrument whitelist; no naked short options | collar 0.5–1% (equity), 1–2% (crypto) |
| Per trade | max risk R | 0.25–0.5% equity |
| Per strategy | max gross exposure; max concurrent positions; strategy daily loss limit | e.g. 1% equity/day → strategy disabled for the day |
| Portfolio | **daily loss limit** (hard stop → flatten + halt) | 1.5–2% equity |
| Portfolio | **max drawdown circuit breaker** from high-water mark | 5% → halve size; 8–10% → halt and require human review |
| Portfolio | gross leverage cap | ≤1× equity (cash equity), F&O within margin with ≥50% margin headroom; crypto ≤2× |
| Correlation | cap on sum of same-direction beta-weighted exposure; treat ρ>0.7 positions as one | e.g. net beta to NIFTY/BTC ≤ 0.5 |
| Rate / sanity | orders per second < 10 (SEBI threshold); orders-sent vs fills reconciliation; max orders/day; duplicate-order detection | **the Knight lesson** |
| Kill switch | one command/flag that cancels all open orders and flattens, callable manually, by heartbeat loss, by stale data, or by any limit breach | must be tested regularly |
| Ops | static IP, daily auth (SEBI), feature-flag hygiene, staged deployments, end-of-day square-off for intraday | — |

- The Knight case shows that the **orders-sent vs fills/position reconciliation** check and a **capital-threshold auto-halt** are the two controls whose absence turned a bug into a loss that ended the firm.

### Gaps
- Exchange-specific (NSE/BSE) pre-trade risk parameters for retail API orders (for example exact price bands, MPP rules or market-order restrictions under the 2025 framework) were not retrieved.
- I could not obtain crypto-exchange-specific risk controls (Binance/Bybit/Hyperliquid self-trade prevention, auto-deleveraging rules) this session.
- Other disasters (2010 Flash Crash, 2013 Everbright, 2018 "Volmageddon" XIV) were not researched with sources here.

---

## 3. Probability of ruin, drawdown asymmetry, and why "no-risk" trading is impossible

### Takeaway
Losses compound asymmetrically, since recovering from a loss of L needs a gain of L/(1−L). Ruin probability rises steeply with bet size. In efficient markets, any expected excess return is compensation for bearing some risk or a capacity-limited inefficiency, so the goal is "controlled, bounded risk", not "no risk".

### Cited Findings
- **Drawdown asymmetry [background, standard arithmetic]:** required recovery gain = L/(1−L). A 10% loss needs +11.1%, a 20% loss needs +25%, a 50% loss needs +100%, and a 90% loss needs +900%. (Arithmetic identity; no citation needed beyond the formula.)
- **Kelly drawdown math [background, not re-verified]:** under continuous-time Kelly betting with fraction k of full Kelly, P(wealth ever falls to fraction x of its starting value) = x^(2/k − 1). At full Kelly, P(ever losing 50%) = 0.5. At half Kelly it is 0.125. At quarter Kelly it is 0.5^7 ≈ 0.8%. — MacLean, Thorp & Ziemba (2011), *The Kelly Capital Growth Investment Criterion* (textbook; not fetched)
- **Gambler's-ruin form [background]:** for a fixed-fraction bettor with per-trade edge, risk of ruin ≈ ((1−E)/(1+E))^U, where E is the edge (expectancy / average loss) and U is the number of risk units in the account. Doubling risk per trade halves U, which **squares** the ruin probability ratio. (Classic gambler's-ruin result; see Vince, *The Mathematics of Money Management*. Not fetched.)
- Every published strategy evaluated in the meta-research has non-trivial left-tail risk, which even vol targeting only reduces, not removes. — [Quantpedia on Harvey et al. 2018](https://quantpedia.com/the-impact-of-volatility-targeting-on-equities-bonds-commodities-and-currencies)
- Real-world frontier LLM agents lost 30–63% in two weeks of live crypto trading (Section 8), a reminder that capital loss is the default outcome when risk is not tightly bounded. — [iWeaver summary of Alpha Arena S1](https://iweaver.ai/blog/alpha-arena-ai-trading-season-1-results)

### Inferences
- **No-arbitrage logic [background]:** under the fundamental theorem of asset pricing, a riskless positive excess return (an arbitrage) cannot persist in a frictionless market. Any remaining "edge" comes from (a) bearing a priced risk (risk premia such as momentum crash risk, carry crash risk, or short-volatility tail risk), (b) providing liquidity or immediacy, or (c) a capacity-constrained or transient inefficiency. All three can lose money. Intraday retail strategies must also beat costs (STT, exchange fees, GST, stamp duty and slippage in India; maker/taker fees and funding in crypto). So "low risk" should be defined operationally: a bounded daily loss, a bounded max drawdown and a bounded loss per trade, with a probability of ruin near zero by construction.
- SEBI's own studies of F&O retail traders (outside this scope; another researcher should cover them) are relevant evidence that the base rate for retail intraday derivatives is negative.

### Gaps
- I did not fetch a primary source for the risk-of-ruin formula or the SEBI F&O loss study in this session.

---

## 4. Combining strategies (ensembles, risk parity, meta-labeling, bandits, momentum + mean reversion + carry)

### Takeaway
Combining low-correlation return streams is the most reliable "free lunch". For N strategies with equal Sharpe S and average pairwise correlation ρ, the combined Sharpe is S·√(N / (1 + (N−1)ρ)). Allocate across strategies by risk (inverse-vol, risk parity or HRP), not by capital. Gate entries with a meta-labeling filter, and use bandit or online-learning reallocation only with heavy shrinkage, because noise dominates short performance windows.

### Cited Findings
- **Diversification arithmetic [background, standard]:** with N equal-Sharpe, equal-vol strategies with average pairwise correlation ρ, the portfolio Sharpe is S_p = S·√N / √(1 + (N−1)ρ). At ρ = 0 and N = 4, Sharpe doubles. At ρ = 0.5 and N = 4, it rises only about 1.26×. (Standard portfolio algebra, as used in Grinold & Kahn's "fundamental law" and in López de Prado, *Advances in Financial Machine Learning* (AFML), 2018.)
- **Volatility management helps momentum specifically**, one of the factors where Cederburg et al. found a robust out-of-sample gain. That supports vol-scaling the trend/momentum sleeve of an ensemble. — [Cederburg et al.](https://www.lehigh.edu/~xuy219/research/COWY.pdf)
- **Moreira–Muir found vol-timing gains across value, momentum and carry factors**, which supports applying a common risk-scaling overlay to a multi-style ensemble. — [NBER w22208](https://www.nber.org/papers/22208)
- **Meta-labeling (López de Prado, AFML 2018; Joubert et al., JFDS 2022–23):** a primary model decides the *side*. A secondary ML classifier, trained on triple-barrier labels (profit-take, stop-loss and time barriers), predicts *whether to act* and *how much*, via a calibrated probability fed to a sizing function. Calibration significantly improves fixed sizing methods. — [Hudson & Thames research index](https://hudsonthames.org/tag/research); [Hudson & Thames toy example](https://hudsonthames.org/meta-labeling-a-toy-example/); [Hudson & Thames: Does meta-labeling add to signal efficacy?](https://hudsonthames.org/does-meta-labeling-add-to-signal-efficacy-triple-barrier-method/embed/); [Jesse docs on meta-labeling](https://docs.jesse.trade/docs/research/ml/meta-labeling)
- **Hierarchical Risk Parity (López de Prado 2016, JPM)** clusters assets or strategies by their correlation tree and allocates by inverse variance within clusters. It avoids inverting an unstable covariance matrix and reports better out-of-sample variance than critical-line mean–variance. **[background, not re-verified this session; cite AFML ch. 16]**

### Inferences
- Proposed ensemble design:
  - **Sleeves:** (1) intraday trend/breakout (opening-range breakout, VWAP trend), (2) intraday mean reversion (VWAP or Bollinger reversion in range regimes, pairs/stat-arb), (3) crypto carry/basis (perpetual funding-rate capture, delta-neutral), which is low-correlation but carries exchange/counterparty risk. Trend and mean reversion tend to be negatively correlated, which is the classic reason to combine them.
  - **Allocation:** equal risk budget per sleeve using inverse-vol or HRP on rolling 60–120-day sleeve returns. Each sleeve has its own loss limit.
  - **Bandit/online reallocation (EXP3, Thompson sampling or multiplicative weights):** only bounded tilts, for example each sleeve's weight kept within ±50% of its risk-parity weight, and updated slowly (weekly), because intraday P&L noise swamps the signal over short windows.
  - **Meta-labeling gate:** secondary classifier on regime and microstructure features, outputting a calibrated P(win), with size = f(P) via SOPS, a sigmoid, or a step function with a minimum threshold (for example, trade only if P > 0.55).
- **Caveat:** correlations between strategies rise in crises, so diversification benefit shrinks exactly when it is needed. The portfolio-level loss limits in Section 2 remain mandatory.

### Gaps
- I did not obtain primary empirical numbers on Sharpe and drawdown improvement from combining trend and mean-reversion at intraday frequency, or on bandit-based strategy allocation in live trading. A dedicated search would be needed (e.g., AQR "A Century of Evidence on Trend-Following"; Asness, Moskowitz & Pedersen 2013 "Value and Momentum Everywhere", which shows value and momentum are negatively correlated, ≈ −0.5 **[background]**).
- No quantitative meta-labeling Sharpe/F1 gains were retrievable (the JFDS articles are paywalled).

---

## 5. Regime detection (HMM, volatility regimes, trend vs range filters)

### Takeaway
A simple two-state (low-vol/high-vol) Gaussian HMM, re-estimated adaptively, has published out-of-sample evidence of beating static rebalancing after transaction costs and with realistic signal delays. Volatility regimes are the most robust thing to detect. Trend-versus-range filters (ADX, Hurst exponent, variance ratio) are cheap, interpretable gates, but their intraday out-of-sample value has little rigorous evidence behind it.

### Cited Findings
- **Nystrup, Madsen et al. (DTU):** strategies based on an **adaptively estimated two-state Gaussian HMM outperform a rebalancing strategy out of sample after transaction costs**, assuming no knowledge of future returns and with a realistic delay between detecting a regime change and adjusting the portfolio. — [DTU pubdb: "Regime-Based Asset Allocation – Do Profitable Strategies Exist?"](https://www2.imm.dtu.dk/pubdb/pubs/6808-full.html) [snippet; fetch blocked]
- Nystrup et al. (2015) built a regime-based allocation on a **2-state (high- and low-volatility) HMM**. Nystrup, Madsen & Lindström (2017, Journal of Forecasting) model HMMs with time-varying parameters to capture long memory. — [search summary; DTU / KAIST / NCCU theses](https://dspace.kaist.ac.kr/handle/10203/221194)
- Other theses report that both in-sample and out-of-sample portfolio performance improves with regime models, which flag crisis periods adequately. — [KAIST thesis](https://koasas.kaist.ac.kr/handle/10203/221194); [NCCU thesis](https://ah.lib.nccu.edu.tw/handle/140.119/141066); [MPRA 121552](https://mpra.ub.uni-muenchen.de/121552/1/MPRA_paper_121552.pdf) [snippets; these are lower-quality student or working-paper sources]
- Vol regimes are where vol targeting cuts left tails (Section 1). — [Quantpedia on Harvey et al.](https://quantpedia.com/the-impact-of-volatility-targeting-on-equities-bonds-commodities-and-currencies)

### Inferences
- Concrete regime-filter defaults (practitioner conventions **[background, not evidence-validated]**):
  - **Volatility regime:** India VIX roughly below 13 is low, 13–20 normal, above 20–25 high/stress. For crypto, use 30-day realized BTC vol percentiles (e.g., top 20% counts as stress). In stress regimes, cut size by 50% or disable mean-reversion sleeves. These cut-offs should be set from rolling percentiles of the series' own history, not fixed numbers.
  - **Trend vs range:**
    - ADX(14) above 25 means trending (enable breakout/trend); below 20 means ranging (enable mean reversion).
    - Hurst exponent H above 0.5 means persistent/trending, below 0.5 mean-reverting. Estimate it on 100+ bars and note it is noisy on short windows.
    - Lo–MacKinlay variance ratio VR(q) = Var(r_q)/(q·Var(r_1)): VR > 1 indicates momentum, VR < 1 mean reversion, and its z-statistic is a test.
  - **HMM:** two or three states on (returns, realized vol), fit with `hmmlearn`. Re-fit on a rolling window using only past data, and act with a one-bar delay. Never use the smoothed (forward-backward) state probabilities in a backtest, because that is lookahead. Use filtered (forward-only) probabilities.
- Regime models add parameters and so increase backtest overfitting risk (Section 6). Count every regime threshold tried as a "trial" for the deflated Sharpe ratio.

### Gaps
- I found no rigorous out-of-sample study of ADX/Hurst/VR filters on Indian intraday data or crypto intraday data. The evidence base is mostly blogs.
- Exact Nystrup out-of-sample Sharpe and drawdown figures were not retrievable (fetch blocked).

---

## 6. Backtesting pitfalls and validation methodology

### Takeaway
With enough trials, any backtest can look good. Account for the number of strategy variants tried (deflated Sharpe ratio, PBO via CSCV), prevent leakage (purging and embargo, point-in-time data), model costs conservatively, then require walk-forward and paper-trading confirmation before going live with small capital.

### Cited Findings
- **Deflated Sharpe Ratio (Bailey & López de Prado 2014, JPM):** corrects for **selection bias under multiple testing** and for **non-normal returns**. It gives the probability that the selected strategy's true Sharpe exceeds a benchmark, conditional on the number of trials, the variance of Sharpe ratios across trials, and the return skewness and kurtosis. — [SSRN 2460551](https://papers.ssrn.com/abstract=2460551); [Trading Strategy docs](https://tradingstrategy.ai/docs/learn/backtesting.html)
  - Formula **[background, from the paper; not re-fetched]**:
    - PSR(SR*) = Φ( (SR̂ − SR*)·√(T−1) / √(1 − γ₃·SR̂ + ((γ₄−1)/4)·SR̂²) ), where T is the number of return observations, γ₃ the skewness and γ₄ the (non-excess) kurtosis.
    - DSR sets the benchmark to the expected maximum Sharpe ratio under the null across N independent trials: SR* = √V[SR_n] · ( (1−γ)·Φ⁻¹(1 − 1/N) + γ·Φ⁻¹(1 − 1/(N·e)) ), where γ ≈ 0.5772 is the Euler–Mascheroni constant.
    - Rule of thumb: require DSR ≥ 0.95.
- **Probability of Backtest Overfitting (Bailey, Borwein, López de Prado & Zhu):** PBO is estimated with **combinatorially symmetric cross-validation (CSCV)**. Backtest overfitting, meaning too many strategy variations tried relative to the data available, "is now thought to be a primary reason why quantitative investment models and strategies that look good on paper often disappoint in practice." — [SSRN 2326253](https://papers.ssrn.com/abstract=2326253); [eScholarship PDF](https://escholarship.org/content/qt2329p290/qt2329p290.pdf); [Bailey et al. overfit tools](https://www.davidhbailey.com/dhbpapers/overfit-tools-at.pdf)
  - CSCV procedure **[background]**:
    1. Split the T×N matrix of trial returns into S (even, e.g. 16) blocks.
    2. For each of the C(S, S/2) combinations, pick the best in-sample trial and find its out-of-sample rank ω̄.
    3. Compute the logit λ = ln(ω̄/(1−ω̄)).
    4. PBO = fraction of combinations with λ ≤ 0, i.e., the in-sample best ranks below the out-of-sample median.
- Related: "Pseudo-Mathematics and Financial Charlatanism" (Bailey, Borwein, López de Prado & Zhu, Notices of the AMS 2014) shows that with about 5 years of daily data, trying only about 45 independent configurations is enough to expect an in-sample Sharpe of about 1 from strategies with zero true skill. — [CXO Advisory discussion](https://www.cxoadvisory.com/big-ideas/backtest-overfitting-the-movies/) **[background figure, not re-verified this session]**
- **Combinatorial Purged Cross-Validation (CPCV), López de Prado AFML ch. 7 and 12 [background]:**
  - **Purging:** drop training observations whose label horizons overlap the test set.
  - **Embargo:** drop a further h ≈ 1% of observations after each test fold.
  - **Combinations:** the C(N, k) train/test splits produce multiple backtest paths, giving a *distribution* of Sharpe ratios instead of a single walk-forward path.
  - Cite AFML (Wiley 2018).

### Inferences
- **Validation checklist for the agent's research pipeline:**
  1. **Data:** point-in-time, survivorship-free universes. Delisted NSE stocks and dead crypto coins must be included. Use corporate-action-adjusted prices and F&O contract roll handling. Exclude the *current* index membership from the historical universe.
  2. **Lookahead:**
     - Signals are computed on the close of bar t and executed at the open of bar t+1 or later.
     - No smoothed HMM states, no full-sample normalization, no future-adjusted fundamentals.
     - Use intraday timestamps in IST and treat exchange session boundaries explicitly.
  3. **Costs (India intraday):**
     - Model brokerage, STT (equity intraday is charged on the sell side; F&O STT rates rose in Oct 2024), exchange transaction charges, SEBI fee, stamp duty, GST, plus slippage. Slippage should be at least the half-spread plus an impact term, e.g. k·σ·√(Q/ADV).
     - Crypto: maker/taker fees, funding payments, withdrawal costs.
     - Stress-test at 2× the assumed costs. If the edge disappears, reject the strategy.
     (Specific rate tables are outside this note; another researcher should source them.)
  4. **Trial accounting:** log every parameter combination tried. Compute DSR with N = number of trials and require PBO < 0.1–0.2.
  5. **Walk-forward:** rolling or anchored fit and test windows, e.g. 12 months in-sample and 1 month out-of-sample, re-optimized monthly. Report only the concatenated out-of-sample results.
  6. **CPCV** for ML components such as the meta-labeling classifier.
  7. **Paper trading:** at least 4–8 weeks, or at least 100 trades, on live data through the real broker API (Kite or ccxt testnet). Compare paper fills with backtest fills to measure the implementation shortfall.
  8. **Go-live:** start at 10–25% of target size and scale up only if live Sharpe and drawdown fall within the backtest's confidence band.
  9. **Live monitoring:** kill or review a strategy when live performance falls outside, e.g., the 5th percentile of its backtest-simulated distribution.

### Gaps
- I could not fetch the full DSR and PBO papers to quote exact numerical examples. The formulas above come from memory of the papers and should be checked against SSRN 2460551 and 2326253.
- Current (2026) Indian transaction-cost and STT rate table: not covered here.

---

## 7. Open-source tooling for a Python agent

### Takeaway
Use vectorbt for fast vectorized research and parameter sweeps (watch the trial count), and an event-driven engine for realistic simulation and live trading with the same code: NautilusTrader for a production-grade, Rust-core system; freqtrade or hummingbot for crypto. Connect to brokers through kiteconnect (Zerodha) and ccxt (crypto). Avoid starting new systems on backtrader.

### Cited Findings
- **Event-driven vs vectorized:** event-driven frameworks (backtrader, NautilusTrader) process bar by bar, using only information available up to that point. VectorBT processes signals as whole NumPy arrays. VectorBT uses NumPy and Numba to test thousands of parameter combinations, e.g. 10 years of daily data × 1,000 combinations in seconds to tens of seconds. — [AlgoLab 2026 comparison](https://algolab.co.kr/blog/python-backtesting-library-comparison-2026); [aiindigo vectorbt review 2026](https://aiindigo.com/blog/vectorbt-review-2026-the-powerhouse-of-vectorized-backtesting) [secondary blog sources]
- **Backtrader** "has been effectively unmaintained for several years and fails to install cleanly on Python 3.10+ without patching". It is useful for learning but "not a production candidate for new systems". — [AlgoLab 2026](https://algolab.co.kr/blog/python-backtesting-library-comparison-2026) [secondary]
- **NautilusTrader 1.222.0 (Jan 2026)** added Cap'n Proto serialization for zero-copy tick-data replay. It is described as production-ready, with a Rust core and Python API, and the same strategy code runs in backtest and live. — [Groundy article](https://groundy.com/articles/nautilustrader-building-production-ready-algorithmic/) [secondary]
- Framework comparisons (Backtrader, Lumibot, QuantConnect, NautilusTrader). — [ChatSlide comparison deck](https://www.chatslide.ai/shared/lm-1-slide-so-snh-quant-trading-framework-b-xmtrhz) [low-quality source]
- **[background, not re-verified this session; official repos]:**
  - **QuantConnect Lean** (C# engine with a Python API; cloud and local; includes Indian-market data and brokerage integrations such as Zerodha and Samco): https://github.com/QuantConnect/Lean
  - **freqtrade** (crypto bot with built-in backtesting, hyperopt, dry-run, FreqAI and protections such as StoplossGuard and MaxDrawdown): https://github.com/freqtrade/freqtrade
  - **hummingbot** (market-making and arbitrage connectors): https://github.com/hummingbot/hummingbot
  - **pykiteconnect** (Zerodha Kite Connect REST and WebSocket; paid API subscription): https://github.com/zerodha/pykiteconnect
  - **ccxt** (unified API over 100 exchanges; sandbox/testnet support): https://github.com/ccxt/ccxt

### Inferences

| Tool | Best for | Pros | Cons |
|---|---|---|---|
| vectorbt | research sweeps | very fast; good analytics | easy to overfit; simplified fills; not live-capable (open-source version) |
| NautilusTrader | backtest→live parity, tick data | Rust core, event-driven, realistic order book and fills, risk engine | steeper learning curve; Indian broker adapters may need to be written |
| Lean/QuantConnect | multi-asset, India support | mature; built-in brokerages; cloud data | C#-centric; heavier |
| freqtrade | crypto spot/futures | dry-run, protections, hyperopt, community | crypto only; hyperopt invites overfitting |
| hummingbot | crypto MM/arbitrage | many connectors | market-making focus |
| backtrader | learning | simple | unmaintained |
| kiteconnect / ccxt | broker connectivity | official / de-facto standard | Kite: SEBI static-IP and daily-auth requirements |

- Recommended stack: research in vectorbt with trial logging and DSR/PBO, then port the finalists to NautilusTrader (or Lean) for event-driven validation and live trading. The broker adapter wraps kiteconnect or ccxt behind the same interface, and the deterministic risk engine sits between strategy and adapter.

### Gaps
- I did not verify which brokers NautilusTrader officially supports (e.g., whether a Zerodha adapter exists), the current vectorbt PRO licensing, or freqtrade's exact protection parameter names. Check the official docs.

---

## 8. Agent architecture patterns and LLM reliability

### Takeaway
Separate the **signal**, **portfolio/risk** and **execution** layers in an event-driven design. Make the risk layer deterministic, auditable and able to veto any upstream decision. Evidence from live LLM trading competitions in 2025–2026 shows frontier LLMs trading autonomously mostly lost money, often heavily. At most, LLMs should be used for bounded, non-critical inputs (news or sentiment features, research assistance, explanations), never for order sizing or limit enforcement.

### Cited Findings
- **Alpha Arena Season 1 (Nof1; real-money crypto perpetuals; ended 3 Nov 2025; $10k per model):**
  - **Qwen3 Max** won with **+22.3%** (30.2% win rate, 43 trades).
  - **Claude Sonnet 4.5 −30.8%**, **Grok 4 −45.3%**, **Gemini 2.5 Pro −56.7%**, **GPT-5 −62.7%**.
  - More than half of the models ended in the red.
  — [iWeaver summary](https://iweaver.ai/blog/alpha-arena-ai-trading-season-1-results); [Decrypt](https://decrypt.co/345006/ai-crypto-trading-showdown-deepseek-grok-winning-gemini-implodes); [Forklog](https://forklog.com/en/four-out-of-six-ai-models-suffer-losses-in-trading-tournament/amp) [snippets]
- **Later Alpha Arena season on US tech stocks (reported by Business Standard / Bloomberg, May 2026):**
  - Eight frontier models, including Claude, Gemini, ChatGPT and Grok, each got **$10,000 per contest across four competitions**, trading US tech stocks for two weeks.
  - The combined portfolio **lost about one third of its capital**, and **only 6 of 32 model-contest results finished in profit**.
  — [Business Standard, "AI bots auditioning for Wall Street trading are mostly losing money"](https://www.business-standard.com/markets/news/ai-bots-auditioning-for-wall-street-trading-are-mostly-losing-money-126050701793_1.html) [snippet; fetch blocked]. A Forklog report says Grok 4.2 won one later tournament. — [Forklog](https://forklog.com/en/ai-model-grok-4-2-triumphs-in-trading-tournament/amp) [snippet]
- **Knight Capital:** a deterministic system failed for lack of deterministic controls, with over 4M erroneous orders in 45 minutes. An LLM-driven order path adds stochastic failure modes on top of that. — [SEC 2013-222](https://www.sec.gov/news/press-release/2013-222)
- **SEBI** requires every algo order to carry an algo ID and lets exchanges kill a specific algo ID. The architecture therefore needs traceable, tagged order flow. — [QuantInsti](https://blog.quantinsti.com/sebi-algo-trading-guidelines-retail-investors)

### Inferences
- **Reference architecture (event-driven, message bus):**
  1. **Data layer:** market-data feed (Kite WebSocket, ccxt/websockets), bar builder, feature store, staleness detection.
  2. **Regime layer:** vol regime, trend/range classification and HMM state. Publishes regime events.
  3. **Signal layer:** independent strategy sleeves emit *intents* (side, confidence, stop, target). They never send orders directly. The meta-labeling gate turns these into sized intents.
  4. **Portfolio and allocation layer:** risk-parity/HRP sleeve budgets, a bounded bandit tilt, and netting of intents.
  5. **Deterministic risk engine (veto layer):** pre-trade checks; per-trade, per-strategy and portfolio limits; correlation and leverage caps; daily loss and drawdown breakers; OPS throttle (<10/s); kill switch. Pure functions plus a state machine, unit-tested, with limits in version-controlled config. **No ML or LLM sits on this path.**
  6. **Execution layer:** order management, broker adapter, idempotent client order IDs, reconciliation of positions and orders against the broker every N seconds, end-of-day square-off.
  7. **Monitoring and ops:** heartbeats, alerting, audit log of every decision and veto, human approval to resume after a breaker trips.
- **Why the risk layer must be deterministic:** (a) LLM outputs are stochastic and can hallucinate quantities, symbols or sides. (b) Regulators and brokers need explainable, reproducible controls (SEC 15c3-5, SEBI algo-ID, kill switch). (c) Live LLM trading records show large losses. (d) Limits must hold under adversarial inputs such as prompt injection via news text.
- **Acceptable LLM roles:** summarizing news or filings into structured features with bounded influence (for example, a sentiment feature that can only reduce size or veto a trade); explaining trades in post-trade reports; assisting code and research work, with outputs passing through the same DSR/PBO gates.

### Gaps
- I did not retrieve academic LLM-trading benchmarks (e.g., StockBench, InvestorBench, FinBen trading tasks, or the "LLMs can't beat buy-and-hold" style 2025 papers). Only the Alpha Arena live competitions were sourced. Alpha Arena is non-academic, short-horizon (about 2 weeks) and small-sample, so its results are suggestive, not conclusive.
- Nof1's own methodology (prompts, leverage allowed, fees) was not verified from the primary source.
