# Options/Volatility Strategies, Arbitrage and Market-Making for Intraday Trading in Indian Index Options (Nifty/BankNifty/Sensex) and Crypto (BTC/ETH options, perps) — as of Oct 2026

Research note scope: mechanics, net-of-cost return evidence, tail risk, capital/margin, and suitability for a low-risk retail agent. Several primary sources (SEBI PDFs, BIS, arXiv, ECGI, Business Today) were blocked by the network proxy in this session, so some figures come from search-result summaries of those sources and are flagged as such. Items I believe to be true from background knowledge but could not verify this session are kept in the Gaps sections and labelled "unverified".

---

## 1. Short premium (short straddle/strangle, iron condor, iron fly, expiry-day/0DTE theta selling) and the variance risk premium (VRP) in India and crypto

### Takeaway
A volatility risk premium does exist in Nifty options: India VIX has averaged about 2.5 vol points above later realized volatility, and the premium was positive on about 80% of days. But the payoff is strongly negatively skewed. The worst day in that sample (Mar 2020) was about 26x the average premium. Naked short-premium positions have lost more than the posted margin (Covid, March 2020) and were whipsawed by event gaps (June 4, 2024, election results). The SEBI Jane Street order showed that expiry-day index levels can be pushed around by a large, well-capitalised participant. For a low-risk retail agent, only small, defined-risk (wing-hedged) premium selling is defensible. Naked straddles and strangles, especially 0DTE, are not.

### Cited Findings
**India VRP evidence**
- A study of 2,852 trading days of Nifty 50 vs India VIX (realized vol = 21-day forward std of daily log returns, annualised) found: average India VIX 16.59, average realized vol 14.12, average VRP +2.46 vol points, median +3.25, positive on 79.7% of days, 5th percentile −6.34 points, worst −64.50 points on 5 Mar 2020 (Covid). The author notes the median exceeds the mean ("small gains most days, rare very large losses") and that it "is not a trading strategy, and it ignores costs, margin and position sizing." Caveat: this is a GitHub project, not peer-reviewed. — [RajolKumar2003/volatility-risk-premium-india (GitHub)](https://github.com/RajolKumar2003/volatility-risk-premium-india)
- A 2025 academic study (Nifty monthly returns, 2015–2025) found India VIX "contains" realized monthly Nifty movements in about 75% of months. It describes India VIX as an effective monthly risk envelope and a driver of realized volatility. — [ResearchGate: Informational Content of India VIX… 2015-2025](https://www.researchgate.net/publication/404036358_Informational_Content_Of_India_Vix_Evidence_From_Volatility_Envelopes_Causality_And_Conditional_Volatility_Of_Nifty_Monthly_Returns_During_2015-2025)
- NSE's working paper on India VIX and risk management (WP/9/2013) is the exchange's original reference on the index. — [NSE Working Paper 9/2013](https://nsearchives.nseindia.com/research/content/res_WorkingPaper9.pdf)

**Crypto VRP evidence**
- Alexander & Imeraj built bitcoin implied-volatility indices with the CBOE VIX variance-swap method. They used more than 7 million Deribit option prices sampled every 15 minutes (Mar 2019–Mar 2020) and compared bitcoin's 30-day realized variance, vol index and VRP with US equities, oil, gold, EUR/USD and the 10y UST. — [University of Sussex repository: "The Bitcoin VIX and its variance risk premium"](https://sro.sussex.ac.uk/id/eprint/91094/)
- DVOL is Deribit's 30-day forward implied-volatility index for BTC and ETH, the crypto analogue of VIX. In practice, VRP is computed as DVOL minus trailing 30-day realized vol. A negative VRP is rare and usually occurs during or just after stress events. — [Amberdata DVOL docs](https://docs.amberdata.io/docs/iv-dvol); [oanor VRP explainer](https://oanor.com/tag/variance-risk-premium)

**Documented blowups / tail events (India)**
- **Covid, March 2020.** India VIX crossed 60 on 17 Mar 2020 and regained 70 on 23 Mar 2020 ([Business Standard 17 Mar 2020](https://www.business-standard.com/amp/article/news-cm/india-vix-crosses-60-mark-120031701181_1.html); [Business Standard 23 Mar 2020](https://www.business-standard.com/amp/article/news-cm/vix-regains-70-level-120032301086_1.html)). Sources disagree on the peak: one says it surpassed 86 ([IBS case study](https://ibscdc.org/Case_Studies/Finance,%20Accounting%20and%20Control/Finance,%20Accounting%20and%20Control/FAC0062.htm)); another says about 83.6 intraday on 24 Mar 2020 ([onetradejournal](https://onetradejournal.com/glossary/india-vix)). The March 2020 series was the biggest expiry-series fall since 2008 ([Equitybulls](https://equitybulls.com/category.php?id=264771)).
- In March 2020 some option sellers "lost out on all the margin deposited as the fall led the option values being more than the margin itself, and sometimes they had to pay more". Once VIX fell from 80 to 30, option writers who had survived made heavy profits. Note: this is a forum (practitioner) source. — [Zerodha TradingQnA](https://tradingqna.com/t/what-happened-with-options-trader-in-march-2020/96234)
- **Election results day, 4 June 2024.** Nifty fell 5.93% to 21,884.5, its worst fall since March 2020, and Sensex fell 5.74%. The day before (3 June), exit polls had driven Nifty up about 700 points to all-time highs. India VIX jumped about 34% to above 28. — [Angel One](https://www.angelone.in/news/share-market/election-day-chaos-indian-stock-market-crashes-to-record-lows); [Outlook Money](https://www.outlookmoney.com/equity/blood-bath-at-bourses-nifty-sensex-suffer-huge-loss-herere-tips-for-investors). Option prices swung wildly as results diverged from what the market had priced in. — [Business Standard, 4 Jun 2024](https://www.business-standard.com/amp/markets/news/lok-sabha-election-results-trends-send-options-market-in-a-tizzy-124060401523_1.html)
- Context: India VIX hit a record low as voting began in April 2024. In other words, premium was cheapest just before the largest event gap. — [Yahoo/Bloomberg](https://ca.news.yahoo.com/indias-key-equity-risk-guage-033032867.html)

**Jane Street SEBI interim order (3 July 2025): what it revealed about expiry dynamics**
- SEBI's 105-page interim order of 3 July 2025 accused Jane Street of "sharp, large and aggressive" expiry-day trades in Bank Nifty and Nifty 50 that "created a false or misleading appearance of market activity". It covered 18 expiry days between Jan 2023 and Mar 2025. — [CNBC, 4 Jul 2025](https://www.cnbc.com/2025/07/04/indian-regulator-bars-us-trading-firm-jane-street-from-accessing-securities-market.html); [Mondaq](https://www.mondaq.com/india/commoditiesderivativesstock-exchanges/1648134/sebis-interim-order-against-jane-street-allegations-of-index-manipulation-explained)
- Alleged mechanics: Jane Street bought large quantities of Bank Nifty constituent stocks and futures in the morning, which pushed the index up, while holding large short positions in index options. It then reversed the cash/futures leg later in the day so that the options book profited. — [Oxford Business Law Blog](https://blogs.law.ox.ac.uk/oblb/blog-post/2025/07/jane-street-and-expiry-day-trap-unpacking-sebis-crackdown-algorithmic); [Parth Malpani analysis](https://parthmalpani.medium.com/jane-street-india-market-manipulation-case-analysis-c8e39ea1bc34)
- Net profits of about ₹36,500 crore from Jan 2023 to Mar 2025. SEBI ordered ₹4,843.57 crore impounded into escrow and barred the firm pending proceedings. Jane Street deposited the full amount on 14 July 2025. — [CNBC](https://www.cnbc.com/2025/07/04/indian-regulator-bars-us-trading-firm-jane-street-from-accessing-securities-market.html); [Bonanza](https://bonanzawealth.com/sebi-crackdown-on-jane-street/)
- The ban was lifted after the deposit, but Jane Street reportedly had not resumed trading in India. It appealed to the Securities Appellate Tribunal (SAT) in Sept 2025. The SAT hearing was adjourned (reported 25 Feb 2026), and no final SAT order was found as of 2026. — [Business Today, 25 Feb 2026](https://www.businesstoday.in/amp/markets/story/jane-street-vs-sebi-sat-adjourns-hearing-in-market-manipulation-case-517925-2026-02-25); [Business Today, Sep 2025](https://www.businesstoday.in/amp/markets/stocks/story/jane-street-moves-sat-files-case-against-sebi-492351-2025-09-03); [Finnovate](https://www.finnovate.in/learn/blog/jane-street-sebi-sat-appeal-update)
- SEBI reportedly widened the probe in Sept 2025 and declined Jane Street's request for more data in Nov 2025. — [Business Standard, Sep 2025](https://www.business-standard.com/markets/news/sebi-ramps-up-jane-street-probe-amid-inadequate-data-ongoing-complaints-125090500792_1.html); [Business Standard, Nov 2025](https://www.business-standard.com/markets/news/sebi-to-decline-jane-street-s-request-for-more-data-in-trading-ban-case-125111700877_1.html)
- Legal commentary on the standard of proof for manipulation: [IndiaCorpLaw, 10 Oct 2025](https://indiacorplaw.in/2025/10/10/proof-of-market-manipulation-the-jane-street-case/)

**Regulatory changes affecting expiry-day selling (India)**
- SEBI measures effective 20 Nov 2024:
  - weekly options only on one benchmark index per exchange (NSE: Nifty 50 only);
  - minimum contract size raised to ₹15 lakh (from ₹5–10 lakh);
  - an additional 2% extreme-loss margin on short options on expiry day.
  — [Zerodha Z-Connect](https://zerodha.com/z-connect/business-updates/sebis-new-rules-for-index-derivatives-heres-whats-changing); [Business Standard, 3 Oct 2024](https://www.business-standard.com/amp/markets/capital-market-news/sebi-tightens-norms-for-equity-derivatives-trading-124100300354_1.html)
- The last BankNifty weekly expiry was 13 Nov 2024. Since then BankNifty has had only monthly options, so "BankNifty 0DTE" in the old sense no longer exists. — [Motilal Oswal](https://www.motilaloswal.com/learning-centre/2024/10/nse-discontinues-weekly-derivatives-on-bank-nifty-nifty-midcap-select-and-finnifty); [Groww](https://groww.in/blog/nse-to-discontinue-weekly-derivative-contracts)

### Inferences
- **Payoff shape.** On the GitHub numbers, the worst day (−64.5 pts) was about 26x the average daily VRP (+2.46). A strategy that harvests about 2.5 vol points needs years of carry to cover one Covid-style event if the position is unhedged. This is the classic "picking up pennies in front of a steamroller" profile.
- **Event gaps.** June 2024 shows that a known event (results day) with a skewed prior (exit polls) can produce a gap larger than the strangle width. India VIX at a record low before the event meant sellers were being paid the least just before the largest move. Any agent should automatically stand down from short premium on scheduled binary events (elections, RBI policy, budget, US CPI/FOMC for crypto).
- **Jane Street lesson for small sellers.** Expiry-day index settlement can be pushed by a participant with huge cash/futures capacity. A retail short-gamma position close to expiry is exposed to moves that are not "fair" random walks. The concentration of profits in large algo prop desks (Section 6) is consistent with this.
- **Defined-risk versions.** Iron condors and iron flies cap the worst case at (wing width − credit) times lot size. That converts margin-exceeding gap losses into a known maximum. In return they give up part of the VRP to the long wings, which carry the most skew premium.
- **Crypto.** Crypto trades 24/7 with no circuit breakers. Weekend and Asia-session gaps (for example 10 Oct 2025, Section 4) make short crypto options riskier than equivalent Nifty structures. Deribit and Delta Exchange offer daily expiries, which are the crypto equivalent of 0DTE.

### Gaps
- Could not fetch peer-reviewed India-specific VRP papers. India VRP numbers above come from a non-peer-reviewed GitHub study. Carr & Wu (2009, "Variance Risk Premiums", RFS) is the canonical US reference but was not fetched this session.
- Alexander & Imeraj's actual crypto VRP sign and magnitude could not be extracted. I recall (unverified) that BTC VRP was found to be large and time-varying, sometimes negative. Recent DVOL-minus-realized averages for 2024–2026 from Deribit/Glassnode/Kaiko research could not be retrieved.
- No net-of-cost backtest of Nifty short straddles/iron condors with realistic slippage and STT was found.
- Unverified background:
  - India VIX record close of about 83.6 on 24 Mar 2020.
  - Further SEBI measures from 2025: upfront collection of option premium, removal of calendar-spread margin benefit on expiry day, and intraday position-limit monitoring.
  - NSE weekly expiry moved to Tuesday and BSE to Thursday from 1 Sep 2025.
  - Nifty lot size was revised in 2025–26.
  - STT on F&O was raised on 1 Oct 2024 (options sell 0.0625%→0.1% of premium; futures 0.0125%→0.02%). Union Budget Feb 2026 reportedly raised it further (futures to 0.05%, options to 0.15%).
  All of these should be checked against NSE/SEBI circulars.
- Jane Street figures: the interim order reportedly shows about ₹43,289 crore profit in index options, offset by losses in stock futures and cash. The 17 Jan 2024 example day had about ₹735 crore options profit. Not verified this session because the ECGI page and the SEBI PDF were blocked.

---

## 2. Long volatility on events, long straddles, gamma scalping, delta-neutral hedging

### Takeaway
Long-vol positions have the opposite skew: small bleeding losses and occasional large wins (March 2020, June 2024). The VRP evidence (implied vol above realized about 80% of the time in Nifty) means a systematic long straddle loses money on average. Gamma scalping only pays when realized vol exceeds the implied vol paid plus hedging costs. Indian F&O transaction costs and STT make frequent re-hedging expensive.

### Cited Findings
- India VIX exceeded subsequently realized Nifty vol on 79.7% of days (average +2.46 pts). Systematically buying options therefore pays the premium on most days. — [GitHub VRP study](https://github.com/RajolKumar2003/volatility-risk-premium-india)
- Long-vol winners did exist in tail events. When VIX went from about 15 to 80 in March 2020, "put buyers made good money". — [TradingQnA](https://tradingqna.com/t/what-happened-with-options-trader-in-march-2020/96234). On 4 Jun 2024 the VIX rose about 34% while Nifty fell 5.93%. — [Outlook Money](https://www.outlookmoney.com/equity/blood-bath-at-bourses-nifty-sensex-suffer-huge-loss-herere-tips-for-investors)
- Before the June 2024 event, India VIX had hit a record low as voting began. Event straddles were therefore cheap going into the result. This is a rare case where buying an event straddle was favourable. — [Yahoo/Bloomberg](https://ca.news.yahoo.com/indias-key-equity-risk-guage-033032867.html)
- SEBI data shows options generated about 92% of individual traders' losses in FY26, and retail activity is concentrated in buying options. — [Angel One summary of SEBI FY26 study](https://www.angelone.in/news/market-updates/active-derivatives-trader-base-drops-to-87-5-lakh-in-fy26-sebi-study); [Finnovate](https://www.finnovate.in/learn/blog/sebi-fno-trader-losses-fy26-individual-derivatives)

### Inferences
- **Event straddles.** Implied vol usually rises into known events (IV crush follows). An event straddle is profitable only if the realized move exceeds the priced move. Election 2024 was an outlier, made possible by a market that under-priced the event. A low-risk agent can treat long straddles as a limited-loss, fully-paid-premium position. Maximum loss is the premium, which is easy to size, but expect a low win rate.
- **Gamma scalping (long options + delta hedging with futures).** Expected P&L is roughly ½·Γ·S²·(σ_realized² − σ_implied²)·dt minus hedging costs. With VRP positive about 80% of the time, the expected edge is negative before costs. Each futures re-hedge in India incurs brokerage, exchange charges, STT on the sell side, and slippage.
- **Delta-neutral hedging of short premium** reduces directional risk but not gap risk. On a 5–6% overnight or opening gap (4 Jun 2024), hedges executed after the gap do not help.

### Gaps
- No published net-of-cost backtests of gamma scalping on Nifty or BTC were found this session.
- No data was found on the distribution of event-day implied vs realized moves for RBI policy days, budget days or US CPI in crypto.

---

## 3. Defined-risk spreads (credit/debit spreads, iron condor/fly) and ruin probability

### Takeaway
Defined-risk spreads cap the per-trade worst case, which turns an unbounded, margin-exceeding gap loss into a known, pre-funded maximum. This is the main thing that makes option selling compatible with a "low-risk" mandate. But it does not change the negative expected-value problem if the strategy has no edge after costs. And repeated maximum losses can still ruin an over-sized account.

### Cited Findings
- Naked option sellers in March 2020 lost more than their posted margin and some owed brokers additional money. This is the failure mode that defined-risk wings prevent. — [TradingQnA](https://tradingqna.com/t/what-happened-with-options-trader-in-march-2020/96234)
- SEBI added a 2% extreme-loss margin on short options on expiry day (from 20 Nov 2024) and raised the minimum contract size to ₹15 lakh. Both increase the capital needed for naked selling. — [Zerodha Z-Connect](https://zerodha.com/z-connect/business-updates/sebis-new-rules-for-index-derivatives-heres-whats-changing)
- Retail-accessible crypto alternative: Delta Exchange India lists BTC/ETH options and futures. Reported details:
  - lot sizes as small as 0.001 BTC;
  - daily, weekly and monthly expiries;
  - options fees of 0.03% (maker and taker) with a cap at 3.5% of premium, plus 18% GST;
  - futures fees of 0.05% taker and 0.02% maker.
  Source caveat: these come from a broker-listing site that flags Delta as "high-risk" and from a sponsored article. — [BrokerListings](https://brokerlistings.com/scams/delta-exchange); [Outlook Business (sponsored)](https://www.outlookbusiness.com/spotlight/news-wire/how-delta-exchange-india-makes-crypto-trading-simple-and-affordable)
- One source says Delta Exchange India offers "eight times more leverage and much smaller lot sizes than SEBI's minimum". This is a red flag for retail agents, because leverage magnifies tail losses. — [Outlook Business](https://www.outlookbusiness.com/spotlight/news-wire/how-delta-exchange-india-makes-crypto-trading-simple-and-affordable)

### Inferences
- **Ruin math.** If each trade risks a fixed fraction f of equity with maximum loss L = (width − credit), then N consecutive maximum losses leave equity at (1−f)^N. With f = 2%, 10 consecutive maximum losses cost about 18%. With naked selling, a single March-2020 event can exceed 100% of the posted margin, so ruin can happen in one trade. Defined risk moves the ruin problem from "single event" to "sequence of events", which position sizing can control.
- **Win rate vs payoff.** Out-of-the-money credit spreads typically have a high win rate (about 70–85%) and a payoff ratio of credit/(width − credit) well below 1. Edge depends on whether implied vol overstates the probability of touching the strikes. Transaction costs on four legs (iron condor), including STT and exchange fees, take a meaningful share of a small credit.
- **Margin.** In India, hedged positions (spreads, iron condors) get much lower SPAN+exposure margin than naked shorts. That makes them more capital-efficient for small accounts, though the ₹15 lakh minimum notional still applies per lot.
- **Debit spreads** (long vol with capped upside) have a defined maximum loss equal to the debit paid. They suit directional event views but still pay VRP on the long leg.

### Gaps
- Current exact SPAN margin figures for a Nifty iron condor vs a naked strangle (per lot, 2026 lot size) were not retrieved. Margin calculators (Zerodha/NSE SPAN) would need to be queried directly.
- No India-specific empirical study of iron condor returns net of costs was found.

---

## 4. Arbitrage: India (cash-futures, conversion/reversal, index, ETF) and crypto (funding-rate/basis, cross-exchange, triangular)

### Takeaway
India cash-and-carry is the most "low-risk" strategy in scope. Its realistic yield is close to the short-term money-market rate: arbitrage mutual funds returned about 6.2–6.5% (1 year) and about 7.15% (3 years) as of Aug–Sep 2026, after fund expenses. A retail agent doing it directly with F&O costs and STT will struggle to beat that. Crypto funding/basis carry historically averaged more than 10% a year (sometimes over 40%). By Sept 2026, BTC funding had compressed to about 1–3% a year. The trade also carries exchange-default, auto-deleveraging (ADL) and collateral-depeg risk, all of which showed up in the 10 Oct 2025 crash. Cross-exchange and triangular arbitrage are effectively captured by HFT firms.

### Cited Findings
**India: cash-futures arbitrage and arbitrage funds**
- Arbitrage fund (Hybrid: Arbitrage) category average: 1-year return 6.18% (as of 7 Aug 2026) and 6.49% (as of 10 Sep 2026); 3-year return 7.15%. Direct-plan comparisons are available for Tata, Kotak, ICICI Pru, Invesco, Axis, DSP, HSBC and Union. — [Arthgyaan fund comparisons](https://arthgyaan.com/fund/compare/tata-arbitrage-fund-direct-growth-vs-baroda-bnp-paribas-arbitrage-fund-direct-growth-option/145724-vs-150251); [Sharpely: Union Arbitrage Fund](https://sharpely.in/mutual-funds/union-arbitrage-fund-directgrowth/38971/performance)
- Futures can trade at a steep discount to spot in a crash ("Nifty March futures trade at steep discount", 16 Mar 2020). Cash-and-carry spreads then invert, so a carry position may need to be rolled or reversed at a loss, or carried to expiry, while the mark-to-market moves against the arbitrageur. — [Business Standard, 16 Mar 2020](https://www.business-standard.com/amp/article/news-cm/nifty-march-futures-trade-at-steep-discount-120031601062_1.html)

**Crypto: funding-rate and basis carry**
- BIS Working Paper 1087, "Crypto Carry" (Schmeling, Schrimpf, Todorov; Apr 2023, revised Oct 2025; also in *Management Science*):
  - crypto carry (futures minus spot) averages above 10% a year, sometimes over 40%, with large time variation;
  - this is much larger than carry in equities, fixed income, FX and commodities;
  - the drivers are trend-chasing small investors demanding leverage and limited arbitrage capital because of regulatory and margin frictions.
  — [BIS WP 1087 summary page](https://www.bis.org/publications/working-paper-1087-crypto-carry); [Management Science](https://pubsonline.informs.org/doi/fpi/10.1287/mnsc.2024.05069)
- He, Manela, Ross & von Wachter, "Fundamentals of Perpetual Futures": perps (more than $100bn traded daily) use a funding rate paid from longs to shorts in proportion to the perp–spot gap. Funding-rate arbitrage "is not risk-free even disregarding margin requirements and trading costs" because there is no expiry date at which the trade is guaranteed to converge. — [arXiv 2212.06888](https://arxiv.org/pdf/2212.06888v5) (summary via search; PDF blocked); [CFTC comment letter](https://comments.cftc.gov/PublicComments/ViewComment.aspx?id=74863)
- Sept 2026 snapshots: BTC perpetual funding averaged about +1.7% a year across 6 exchanges, with readings from +0.8% to +2.9% a year. Individual venues diverged, for example Coinbase International about +5–6% and Kraken about −5.5% to −6.6% annualised. — [DeFiRate perp snapshot, 25 Sep 2026](https://defirate.com/perp/snapshot/2026-09-25T1854/)
- **Run-up to 10 Oct 2025.** BTC/ETH perp funding rose from about 10% annualised to nearly 30% by 6 Oct 2025, alongside elevated open interest. On 10 Oct 2025, more than $19bn of crypto leverage was liquidated in about a day, triggered by a 100% China tariff threat. — [FTI Consulting](https://www.fticonsulting.com/insights/articles/crypto-crash-october-2025-leverage-met-liquidity)
- **Ethena USDe during the crash.** USDe is a tokenised funding-arb trade: stETH collateral plus short perps. It depegged to $0.65 on Binance on 10–11 Oct 2025 while holding near peg on DEXs, which points to exchange-specific pricing and oracle problems. It then had about $8bn of outflows. — [Netcoins](https://www.netcoins.com/blog/ethenas-usde-depeg-an-overview-and-its-relation-to-the-ena-token); [99Bitcoins](https://99bitcoins.com/news/altcoins/ethena-usde-8b-outflows/); [Coin Metrics](https://coinmetrics.substack.com/p/state-of-the-network-issue-335)
- The NY Fed (June 2026) analysed synthetic stablecoins as a financial-stability concern. It describes a reinforcing loop: funding losses lead to collateral outflows and redemptions, which push prices and funding lower. — [Liberty Street Economics, Jun 2026](https://libertystreeteconomics.newyorkfed.org/2026/06/synthetic-stablecoins-and-financial-stability/)

### Inferences
- **India cash-and-carry (buy stock or basket, sell futures).** The yield equals the futures premium, which tracks the cost of carry (roughly the repo/T-bill rate). Arbitrage funds achieve about 6.2–7.2% after expense ratios, and they get equity taxation as long as they stay 65% or more hedged in equity. A retail trader doing it directly pays:
  - STT on both the delivery leg and the futures leg;
  - brokerage, exchange fees and stamp duty;
  - slab-rate business-income tax (F&O income is non-speculative business income).
  So direct execution is likely to net less than an arbitrage fund. Practical conclusion: for a "low-risk" retail goal, an arbitrage fund (or liquid fund) is the benchmark, and an agent doing cash-futures arb should be measured against about 6.5% a year.
- **Residual risks (India):**
  - futures discount or inversion in crashes (March 2020);
  - mark-to-market margin calls on the short futures leg in a rally, which need cash buffers;
  - corporate actions and dividends changing fair basis;
  - execution slippage when legging into baskets (index arbitrage);
  - possible ETF tracking error or iNAV divergence.
- **Put-call parity / conversion-reversal on Nifty.** Nifty options are European and cash-settled, so a conversion (long future + long put + short call at the same strike) should earn about the risk-free rate. Observable mispricings are usually smaller than the STT, exchange fees and bid-ask on three legs, and prop algos capture them within milliseconds. Expect near-zero retail edge.
- **ETF arbitrage** (creation/redemption vs iNAV) needs authorised-participant status for unit creation. Retail can only trade secondary-market premium/discount mean reversion, which is thin and illiquid.
- **Crypto funding arb (long spot + short perp).** The gross yield equals the funding rate, which ranged from about 1.7% a year (Sep 2026) to about 30% (Oct 2025). Net of:
  - taker fees on entry and exit (for example 0.05% per perp side on Delta);
  - spot spread;
  - capital split between spot and margin;
  the 2026 compressed level is below Indian arbitrage-fund returns. For Indian residents, add the 1% TDS on VDA transfers and 30% flat tax on crypto gains (unverified this session; see Gaps).
- **Residual risks (crypto):**
  1. Funding flips negative, so the short pays.
  2. Liquidation of the short perp in a spike if margin is held on a different venue from the spot.
  3. ADL. On 10 Oct 2025, profitable shorts on several venues were reportedly auto-deleveraged, which strands the spot leg unhedged (background; not verified this session).
  4. Exchange insolvency (FTX, Nov 2022 — background).
  5. Collateral depeg (USDe at $0.65 on Binance).
  6. Stablecoin and on/off-ramp risk.
- **Cross-exchange and triangular arbitrage.** Opportunities last milliseconds to seconds, and the bottlenecks are latency, colocation and fee tiers. Retail has to pre-fund both venues, which adds counterparty risk. Withdrawal delays turn a "risk-free" spread into a directional position. The SEBI FY26 data (Section 6) shows 99% of prop/FPI gains are algorithmic, consistent with these spreads being captured by fast firms.

### Gaps
- The BIS paper's Sharpe ratios, drawdown statistics and the exact sample average of BTC/ETH carry could not be extracted (bis.org blocked).
- He–Manela et al.'s reported Sharpe ratios for the perp-spot arb strategy could not be extracted (arXiv blocked). I recall (unverified) that they report very high Sharpe ratios before costs that shrink substantially after costs.
- Not retrieved this session:
  - typical Nifty futures annualised premium (cash-carry spread) for 2025–26;
  - expense ratios of arbitrage funds;
  - current India tax treatment of arbitrage funds after Budget 2024 (STCG 20%, LTCG 12.5%, unverified);
  - crypto TDS/30% tax rules (unverified).
- Glassnode/Kaiko research on funding-rate distributions and ADL counts on 10 Oct 2025 was not retrieved.
- Deribit vs Delta Exchange regulatory status for Indian residents (FIU-registration, enforcement) was not verified.

---

## 5. Market-making for retail: feasibility, Avellaneda–Stoikov basics, inventory risk

### Takeaway
Retail market-making in Nifty options or major crypto books is not feasible in practice. Profits from liquidity provision go to colocated, low-fee, algorithmic prop firms: the top 10 prop desks earned about 75% of prop gross profits in FY26, and about 99% of prop/FPI gains were algorithmic. Retail cannot compete on latency or fee tier, and is adversely selected on every news move. Inventory risk dominates on gap events.

### Cited Findings
- SEBI FY26 study: proprietary traders' gross profits were ₹44,483 crore and FPIs' ₹13,896 crore. Algorithmic entities accounted for about 99% of these gains, and the top 10 prop desks earned nearly 75% of prop gross profits. — [Finnovate summary, Sep 2026](https://www.finnovate.in/learn/blog/sebi-fno-trader-losses-fy26-individual-derivatives); [Angel One](https://www.angelone.in/news/market-updates/active-derivatives-trader-base-drops-to-87-5-lakh-in-fy26-sebi-study)
- SEBI FY24 study: 97% of FPI profits and 96% of prop-trader profits came from algorithmic trading. — [Business Standard](https://www.business-standard.com/amp/markets/news/net-losses-of-traders-in-fo-widens-in-fy25-sebi-study-125070701221_1.html); [Wright Research](https://www.wrightresearch.in/blog/sebi-futures-and-option-report-individual-traders-in-fandos-incur-rs-18-lakh-crore-loss-over-3-years/)
- Delta Exchange maker fee on futures is 0.02% (no rebate), and options fees are 0.03% for both maker and taker. A retail maker therefore pays fees rather than earning rebates. — [BrokerListings](https://brokerlistings.com/scams/delta-exchange)

### Inferences
- **Avellaneda–Stoikov (2008, *Quantitative Finance*, "High-frequency trading in a limit order book") basics.** The model is standard theory; the paper was not fetched this session.
  - The market maker quotes around a reservation price r = s − q·γ·σ²·(T−t), where s is the mid, q is inventory, γ is risk aversion, and σ is volatility.
  - The optimal total spread is δ = γσ²(T−t) + (2/γ)·ln(1 + γ/κ), where κ is order-arrival intensity decay.
  - Practical implications: skew quotes away from inventory, widen with volatility, and widen when fill intensity is low.
- **Why retail fails:**
  1. Latency. Colocated firms update quotes in microseconds. Retail API round-trips are tens to hundreds of milliseconds, so stale quotes are picked off (adverse selection).
  2. Fees. With no maker rebates for small accounts, the captured half-spread on liquid Nifty or BTC books (often 1 tick) is smaller than fees plus STT.
  3. Inventory risk. On gap events (4 Jun 2024, 10 Oct 2025) inventory cannot be offloaded and quotes get filled in the wrong direction.
  4. In Indian options specifically, the Jane Street case shows large players can move the underlying on expiry, which makes inventory in short-dated options especially toxic.
- A feasible retail substitute is passive limit-order execution (posting limit orders inside the spread to reduce entry costs on otherwise-planned trades), not continuous two-sided quoting.

### Gaps
- No data was found on retail market-making profitability on Delta Exchange or Deribit, or on availability of maker-rebate tiers for small accounts.
- No NSE data on the share of options market-making by prop firms vs others was retrieved.

---

## 6. SEBI evidence: most retail F&O traders lose; who profits

### Takeaway
SEBI's studies are consistent across years: about 88–93% of individual F&O traders lose money, with aggregate net losses of about ₹75k–1.06 lakh crore a year. Options account for about 92% of individual losses. Meanwhile, proprietary traders and FPIs earn tens of thousands of crores, almost entirely through algorithms and highly concentrated in a few firms.

### Cited Findings
- **FY22–FY24 (study released Sept 2024):** individual traders' cumulative losses were about ₹1.8 lakh crore over three years. — [Wright Research](https://www.wrightresearch.in/blog/sebi-futures-and-option-report-individual-traders-in-fandos-incur-rs-18-lakh-crore-loss-over-3-years/); [Business Standard, 24 Sep 2024](https://www.business-standard.com/markets/capital-market-news/sebi-study-exposes-massive-losses-for-individual-f-o-traders-in-india-124092400948_1.html); [News On Air](https://www.newsonair.gov.in/sebi-study-reveals-individual-traders-face-severe-losses-in-equity-fo-segment)
- **FY24 profits elsewhere:** prop traders' gross trading profit was ₹33,000 crore and FPIs' ₹28,000 crore, before costs. 96% (prop) and 97% (FPI) came from algo trading. — [Business Standard, 7 Jul 2025](https://www.business-standard.com/amp/markets/news/net-losses-of-traders-in-fo-widens-in-fy25-sebi-study-125070701221_1.html)
- **FY25 (study released 7 Jul 2025):** 91% of individual traders lost money. Net losses were ₹1,05,603 crore, up 41% from ₹74,812 crore in FY24. — [Business Standard, 7 Jul 2025](https://www.business-standard.com/amp/markets/news/net-losses-of-traders-in-fo-widens-in-fy25-sebi-study-125070701221_1.html)
- **FY26 (study released around 21 Aug 2026):**
  - 87.7% of individual equity-derivatives traders made net losses;
  - aggregate net loss ₹91,685 crore;
  - average loss about ₹1.17 lakh;
  - options generated about 92% of individual losses;
  - transaction costs took about ₹25,000 crore;
  - prop gross profit ₹44,483 crore and FPI ₹13,896 crore, about 99% algorithmic;
  - top 10 prop desks earned about 75% of prop profits.
  — [Finnovate](https://www.finnovate.in/learn/blog/sebi-fno-trader-losses-fy26-individual-derivatives); [Angel One](https://www.angelone.in/news/market-updates/active-derivatives-trader-base-drops-to-87-5-lakh-in-fy26-sebi-study); [Outlook Money](https://www.outlookmoney.com/invest/nearly-88-of-retail-derivatives-traders-incur-losses-even-as-participation-cools-says-sebi-study)
- **Conflict on FY26 participation figures:** Angel One reports active individual traders fell 18% to 87.5 lakh. Another summary reports 98.1 lakh (FY25) falling to 78.6 lakh (FY26), about −20%, and headlines it as "9 in 10" losing. — [Angel One](https://www.angelone.in/news/market-updates/active-derivatives-trader-base-drops-to-87-5-lakh-in-fy26-sebi-study) vs [Open Magazine](https://openthemagazine.com/business/sebi-fo-loss-study-explained-why-9-in-10-retail-traders-lost-91685-crore-in-fy26). The original SEBI PDF should be checked.
- A separate SEBI study found 7 in 10 intraday traders in the equity cash segment lost money in FY23. — [Business Standard, 24 Jul 2024](https://www.business-standard.com/amp/markets/news/7-in-10-intraday-traders-in-equity-cash-suffered-losses-in-fy23-sebi-study-124072400975_1.html)
- SEBI's Jane Street order alleged that the firm's schemes moved about ₹35 of every ₹100 of option profits away from retail. Note: this phrasing comes from a secondary summary. — [Oxford Business Law Blog / search summary](https://blogs.law.ox.ac.uk/oblb/blog-post/2025/07/jane-street-and-expiry-day-trap-unpacking-sebis-crackdown-algorithmic)

### Inferences
- The losing population is dominated by retail **option buyers**: options account for about 92% of losses and retail mostly buys cheap out-of-the-money weeklies. The winners are latency-advantaged algo prop and FPI desks, often on the selling or market-making side. This supports the VRP evidence: on average, sellers are paid. It does not mean a retail seller captures that premium safely, because of tail events and expiry-day manipulation risk.
- Transaction costs (about ₹25,000 crore in FY26) alone are about 27% of the ₹91,685 crore aggregate loss. High-turnover intraday strategies face a large structural cost drag.
- The falling loss count and participant numbers from FY25 to FY26 coincide with the Nov 2024 SEBI measures (one weekly expiry, ₹15 lakh contract size, expiry-day ELM). This suggests the regulatory tightening reduced retail participation, though causality is not established in the sources.

### Gaps
- Not retrieved: the original SEBI study PDFs (FY24, FY25, FY26), the exact percentages for FY22–FY24 (widely reported as 93% of about 1.13 crore traders, average loss about ₹2 lakh, unverified), and the split of losses between option buyers and sellers.
- No SEBI data was found on profitability of retail traders using spreads or hedged strategies specifically.

---

## 7. Suitability for a low-risk retail agent (synthesis)

### Takeaway
Ranked from most to least suitable for a low-risk retail agent:
1. India arbitrage via arbitrage funds, about 6.2–7.2% a year; direct cash-and-carry only if costs beat the fund.
2. Small, defined-risk premium selling: iron condors or iron flies with strict event blackouts and position sizing.
3. Crypto funding arbitrage, only in high-funding regimes on a single, reputable venue with spot and perp margined together. Currently about 1–3% a year for BTC, which does not compensate for exchange risk.
4. Long event straddles, only as tiny fully-paid bets.
Unsuitable: naked straddles/strangles (especially 0DTE), PCP or cross-exchange or triangular arbitrage (no retail edge), and retail market-making.

### Cited Findings
- Arbitrage fund category returns: 6.18–6.49% (1 year), 7.15% (3 years) as of Aug–Sep 2026. — [Arthgyaan](https://arthgyaan.com/fund/compare/tata-arbitrage-fund-direct-growth-vs-baroda-bnp-paribas-arbitrage-fund-direct-growth-option/145724-vs-150251)
- Nifty VRP of about +2.46 vol points on average, with worst −64.5 points (Mar 2020). — [GitHub VRP study](https://github.com/RajolKumar2003/volatility-risk-premium-india)
- BTC funding about +1.7% a year (Sep 2026) vs about 30% (early Oct 2025), followed by a $19bn liquidation day. — [DeFiRate](https://defirate.com/perp/snapshot/2026-09-25T1854/); [FTI Consulting](https://www.fticonsulting.com/insights/articles/crypto-crash-october-2025-leverage-met-liquidity)
- About 88–91% of individual F&O traders lose (FY25–FY26), and 99% of prop/FPI gains are algorithmic. — [Business Standard](https://www.business-standard.com/amp/markets/news/net-losses-of-traders-in-fo-widens-in-fy25-sebi-study-125070701221_1.html); [Finnovate](https://www.finnovate.in/learn/blog/sebi-fno-trader-losses-fy26-individual-derivatives)

### Inferences
| Strategy | Expected net return (retail) | Tail profile | Worst case | Capital/margin | Low-risk suitability |
|---|---|---|---|---|---|
| Naked short straddle/strangle (Nifty, incl. 0DTE) | VRP positive about 80% of days, but net of costs, slippage and gaps uncertain | Strong negative skew | Can exceed posted margin (Mar 2020); 5–6% gap days (4 Jun 2024) | High: ₹15 lakh min notional per lot plus SPAN, plus 2% expiry-day ELM | **Unsuitable** |
| Iron condor / iron fly | Small; part of the VRP is given up to the wings | Capped negative skew | (Width − credit) × lot | Lower hedged margin | **Conditionally suitable** (small size, event blackout, avoid expiry-day manipulation window) |
| Long event straddle | Negative on average (positive VRP) | Positive skew | Premium paid | Premium only | Suitable only as tiny, capped bets |
| Gamma scalping | Negative before costs unless realized vol exceeds implied | Mixed | Premium plus hedge costs | Moderate | Low |
| Credit/debit vertical spreads | Strategy-dependent | Capped | Width − credit / debit | Low | Suitable with sizing |
| India cash-and-carry (direct) | About risk-free rate minus costs, at or below the ~6.5% fund benchmark | Low; futures-discount and MTM risk | Small, liquidity-driven | Large (full cash leg plus futures margin) | Suitable, but the fund is usually better |
| Arbitrage mutual fund | About 6.2–7.2% | Very low | Brief negative months in dislocations | Any amount | **Most suitable** |
| PCP conversion/reversal, index/ETF arb | About 0 after costs for retail | Execution risk | Leg risk | High | Not worthwhile |
| Crypto funding arb (spot + short perp) | Funding 1–30%/yr, regime-dependent; about 1.7%/yr in Sep 2026 | Exchange default, ADL, depeg, funding flip | Loss of exchange balance; stranded leg | Spot plus perp margin (about 2x capital) | Conditional (high-funding regimes only, single venue, low leverage) |
| Cross-exchange / triangular arb | About 0 for retail (latency) | Transfer and withdrawal risk | Counterparty loss | Pre-funded on multiple venues | Unsuitable |
| Retail market-making | Negative (adverse selection, fees) | Inventory gap risk | Large on gaps | Moderate | Unsuitable |

### Gaps
- No single source quantified net-of-cost returns of retail-implementable defined-risk Nifty strategies over 2020–2026. A report writer should present those as model-based rather than empirical.
- Need verification of the 2026 cost stack (STT after Budget 2026, exchange transaction charges, GST, stamp duty) and of the current Nifty/Sensex lot sizes and expiry weekdays before computing break-even credits.
