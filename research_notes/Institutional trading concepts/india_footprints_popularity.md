# India-specific institutional footprints as tradable signals, and popularity / regulation of "smart money" educators (as of Oct 2026)

Research scope note: ~17 search/fetch calls. Many academic sources on India are low-tier journals (non-peer-reviewed or regional journals); almost none report results **after transaction costs**. Grading used: A = peer-reviewed evidence after costs, B = independent systematic backtests, C = practitioner claims only. Where I graded with a "+/-" it reflects peer-reviewed-but-no-costs evidence (which strictly falls short of A).

---

## Q1. NSE delivery percentage and "delivery-based accumulation"

### Takeaway
Delivery % (deliverable qty / traded qty) is freely published per stock per day by NSE, but academic evidence that it predicts returns is thin (single-stock Granger tests, mixed results) and nothing found is after costs. Grade: **C** (with weak, non-cost-adjusted academic support at best).

### Cited Findings
- Definition/practitioner reading: a rising delivery % is read as investors taking delivery, i.e. bullish conviction; but sources concede higher delivery volume "does not guarantee that the stock will perform well." — [ResearchGate: Delivery Quantity/Total Quantity vs returns study](https://www.researchgate.net/publication/377565241_A_Comparative_Study_on_Relationship_between_Delivery_Quantity_to_Total_Quantity_traded_Ratio_and_Stock_returns_in_Bajaj)
- Academic: a comparative study (2008–2018 data, two NIFTY stocks, unit-root + Granger causality) found delivery ratio Granger-causes returns for Bajaj Auto but **not** for BPCL — i.e., no robust cross-sectional result, tiny sample, no trading strategy, no costs. — [ResearchGate](https://www.researchgate.net/publication/377565241_A_Comparative_Study_on_Relationship_between_Delivery_Quantity_to_Total_Quantity_traded_Ratio_and_Stock_returns_in_Bajaj)
- Related: in a 2012–2021 NSE bulk-deal event study, "trading volume and delivery percentage both rise significantly before bulk transactions and fall drastically once it crosses the event day" — suggesting delivery spikes partly coincide with informed/large-player activity. — [Rabada & Barretto, Education and Society 2023 (NDL record)](https://rcca.ndl.gov.in/items/4fbafa21-3191-40dd-8288-8fa6019189c0/full)
- Market-level delivery share is reported as a sentiment gauge in financial press (e.g., delivery-based volumes at 1-year highs / 8-year lows). — [Business Standard 2015](https://www.business-standard.com/article/markets/delivery-based-volumes-hit-one-year-high-115033100269_1.html); [Business Standard 2018](https://www.business-standard.com/amp/article/markets/delivery-based-volumes-hit-over-eight-year-low-118050900202_1.html)
- Commercial screeners (StockEdge, Trendlyne, Nirmal Bang) offer "volume and delivery" scans (e.g., high delivery % vs average), confirming the practitioner popularity of the signal. — [StockEdge volume & delivery scans](https://web.stockedge.com/scan-category/volume-and-delivery/201); [Trendlyne delivery analysis](https://trendlyne.com/equity/delivery-analysis/755079/DATAPATTNS/data-patterns-india-ltd/); [Nirmal Bang highest/lowest delivery](https://www.nirmalbang.com/equity/highest-lowest-delivery.aspx)

### Inferences
- Practitioner rule pattern ("delivery % > 20-day average AND price up AND volume above average => accumulation") is a screener convention (C-grade). No independent backtest found.
- Mechanical caveat for the agent: delivery % is mechanically depressed by intraday (MIS) churn and inflated in low-liquidity names and on index-rebalance / block-deal days; normalize by the stock's own rolling history (z-score) rather than absolute thresholds.
- Data (from background knowledge, NOT verified this session — re-check URLs before coding): NSE daily "security-wise delivery position" files (MTO_DDMMYYYY.DAT) and the full bhavcopy with delivery columns (`sec_bhavdata_full_DDMMYYYY.csv`) in nsearchives.nseindia.com/products/content/; history of delivery data goes back to roughly the early 2000s for MTO files. Published same evening, after close (end-of-day only; not usable intraday).

### Gaps
- No peer-reviewed, cross-sectional, cost-adjusted study of a delivery-% strategy on NSE was found. This is a clear opportunity for the agent's own backtest (the project already has an NSE bhavcopy ingest).
- Exact first-available date of NSE delivery archives not confirmed.

---

## Q2. Bulk and block deals (rules, publication timing, evidence)

### Takeaway
Bulk deals (>0.5% of listed shares in a day) and block deals (separate window, now Rs 25 crore minimum since Dec 2025) are disclosed by exchanges the same day after market hours. Evidence shows strong abnormal returns *before and on* the deal day (front-running/price pressure) and ~1.3% average price rise on block-deal day, but little evidence of exploitable post-publication drift after costs. Grade: **C/B-** for trading on post-publication information.

### Cited Findings
- Bulk deal definition: any transaction(s) in a scrip where total quantity bought/sold by a client exceeds 0.5% of the company's listed equity shares; brokers disclose immediately on execution, exchanges disseminate same day after market hours (SEBI circular SEBI/MRD/SE/Cir-7/2004, 14 Jan 2004). — [NSE circular (archives)](https://archives.nseindia.com/content/circulars/cmtr4808.htm); [Ventura glossary](https://www.venturasecurities.com/share-market-glossary/bulk-deals/)
- Block deal framework revised by SEBI circular SEBI/HO/MRD/POD-III/CIR/P/2025/134 dated 8 Oct 2025: minimum order size raised from Rs 10 crore to **Rs 25 crore**; morning window 8:45–9:00 AM (reference = previous close), afternoon window 2:05–2:20 PM (reference = VWAP of cash trades 1:45–2:00 PM); price band ±3% of reference; all block deals must result in delivery (no square-off); exchanges disclose scrip, client name, quantity, price after market hours same day; effective **7 Dec 2025**. — [Taxmann](https://www.taxmann.com/post/blog/sebi-revises-block-deal-framework-minimum-order-size-rs-25-crore); [Moneylife](https://www.moneylife.in/article/block-deal-framework-revamped-sebi-introduces-rs25-crore-minimum-trade-size-and-tighter-trading-rules/78537.html); [Medianama](https://www.medianama.com/2025/10/223-sebi-revamps-block-deal-norms-hikes-25-crore/)
- Evidence (NSE 2012–2021 bulk deals): abnormal returns *before* deals are higher for single-individual buy/sell deals than multiple-party deals; volume and delivery % rise significantly pre-deal and collapse after event day; "very high cumulative returns around the trades"; interpreted as front-running by informed traders. Journal: Education and Society, Vol 47(1), 2023 (UGC-CARE type journal; no transaction costs; no post-publication strategy test). — [Rabada & Barretto (NDL)](https://rcca.ndl.gov.in/items/4fbafa21-3191-40dd-8288-8fa6019189c0/full); [PDF](https://rclibrary.rosarycollege.org/wp-content/uploads/2023/12/9JitendraHelicSandrio.pdf)
- Block deals: event study of 125 BSE block deals (2006–2012) — prices rise ~1.32% on average on block-deal day. — [IUP Applied Finance](https://www.iupindia.in/1507/Applied%20Finance/Applied_Finance.asp)
- NSE-IGIDR corporate governance working paper studies blockholder trades (purchases vs sales by pressure-resistant, pressure-sensitive, insider blockholders) and short-term returns, sample from 2005 onward. — [NSE-IGIDR WP4 2015-16](https://nsearchives.nseindia.com/research/content/NSE-IGIDR_CG_RPaper_2015-16_WP4.pdf)

### Inferences
- Because publication is after the close, the tradable piece for a paper agent is only the *post-disclosure* return (next open onward). The documented abnormal returns are concentrated pre-event/event-day, so a naive "buy the stocks with big bulk buys" rule is likely to capture little; any edge would be in filtering by buyer type (named FII/MF/promoter vs. prop/HFT arbitrage desks that buy and sell same day).
- Many NSE bulk deals are intraday round-trips by prop/HFT firms (same client both sides); filter out deals where the same client appears on both buy and sell sides.
- Bulk/block deal archives are available as CSVs on NSE ("Bulk Deals"/"Block Deals" historical reports) — URL not verified this session.

### Gaps
- No study found that measures post-publication (next-day open) drift after bulk/block disclosures with costs.
- The NSE-IGIDR paper's numerical results were not extracted.

---

## Q3. FII/FPI and DII daily flows

### Takeaway
Provisional FII/DII cash-market net figures are published by NSE ~6 PM IST each trading day; NSDL final FPI data is T+1. Indian academic literature mostly finds FIIs are return-chasers (returns Granger-cause flows), with flows affecting returns only very short-term — weak basis for a predictive signal. Grade: **C** for trading (academic evidence is mostly contemporaneous/feedback, peer-reviewed in parts but not cost-adjusted, not predictive).

### Cited Findings
- Publication timing: NSE provisional FII/DII trading activity (consolidated NSE+BSE+MSEI) is posted after ~6 PM IST each trading day; NSDL/CDSL confirmed figures follow T+1 and can differ from provisional by a few hundred to a few thousand crore. — [share.market](https://www.share.market/buzz/learn/how-to-track-fii-data-for-investing/); [onetradejournal glossary](https://onetradejournal.com/glossary/fii-dii); NSE page: [nseindia.com/reports/fii-dii](https://www.nseindia.com/reports/fii-dii)
- NSDL publishes daily FPI trends (as of previous trading day), monthly summaries and fortnightly sector-wise data with historical archives. — [onetradejournal](https://onetradejournal.com/glossary/fii-dii); [TradingQnA NSDL FPI data date](https://tradingqna.com/t/nsdl-fpi-data-date/164646)
- Academic (daily data, Granger/VAR): bidirectional causality between net FII investment and Nifty returns; FIIs "chase" returns — FII trading causes return variation only very short-term, after which returns drive FII behavior; impulse responses show flows are more return-driven (Jan 2003–Feb 2007 sample). — [Emerald (2012)](https://www.emerald.com/insight/content/doi/10.1108/17554191211274794/full/html); [MPRA paper 15793](https://mpra.ub.uni-muenchen.de/15793/1/MPRA_paper_15793.pdf); [IUP IJAF](https://www.iupindia.in/305/IJAF_Foreign_Institutional_Investment_16.html)
- Some studies find only unidirectional causality from equity returns to FII flows. — [Inderscience](https://www.inderscience.com/filter.php?aid=20430)

### Inferences
- Since flows are published after the close and are largely a function of same-day returns, using day-t flows to predict day t+1 returns is unlikely to add much beyond the day's own return; more plausible uses are regime filters (e.g., multi-week cumulative FII selling as risk-off context) and the separate NSE "participant-wise open interest" (FII index futures long/short ratio) — the latter is not verified here.
- Studies are mostly pre-2010; post-2020 DII (SIP) flows have structurally offset FII selling, so older results may not transfer.

### Gaps
- No recent (post-2015) cost-adjusted study of FII/DII daily flow trading rules found.
- Participant-wise OI (FII long/short in index futures) evidence not researched for lack of tool budget.

---

## Q4. F&O open-interest build-up classification, PCR and max pain

### Takeaway
OI build-up classification (price/OI direction quadrants) is a pure practitioner heuristic; Indian academic work mostly finds OI explains volatility less than volume and has limited/partial predictive content. One peer-reviewed India study (Jena, Tiwari & Mitra 2019, Economies) finds volume-PCR predicts Nifty returns at ~2.5-day horizons and OI-PCR at ~12-day horizons (frequency-domain causality, no costs). Max pain: no academic evidence found. Grades: build-up **C**; PCR **B-/C** (peer-reviewed predictability, no cost-adjusted strategy); max pain **C**.

### Cited Findings
- Practitioner premise tested by Indian researchers: change in stock-futures OI correlated with futures price change ("market participants believe OI has a bearing on price behaviour"). — [IUP Applied Finance: Empirical Relationship](https://www.iupindia.in/1207/Applied%20Finance/Empirical_Relationship.html)
- Nifty futures near-month data: volume has stronger impact on volatility than open interest. — [IIMB repository](https://repository.iimb.ac.in/handle/2074/18411); [Vikalpa/Sage 2008](https://journals.sagepub.com/doi/xml/10.1177/0256090920080202)
- Net open interest of stock options found to be a significant variable in determining future spot price of the underlying (Indian stock options). — [MDPI IJFS](https://mdpi-res.com/d_attachment/ijfs/ijfs-09-00007/article_deploy/ijfs-09-00007.pdf)
- Jena, Tiwari & Mitra (2019), "Put–Call Ratio Volume vs. Open Interest in Predicting Market Return: A Frequency Domain Rolling Causality Analysis," Economies 7(1):24 — "volume PCR was an efficient predictor of the market return in a short period of 2.5 days and open interest PCR in a long period of 12 days"; robust to futures-market information. — [RePEc/IDEAS](https://ideas.repec.org/a/gam/jecomi/v7y2019i1p24-d217055.html); [PDF](https://mdpi-res.com/d_attachment/economies/economies-07-00024/article_deploy/economies-07-00024.pdf)
- Practitioner thresholds: Nifty PCR > 1.4–1.5 = crowded bearish (contrarian bullish); < 0.7–0.8 = complacent; 2015–2023 analysis claims "moderate predictive power" but "far from infallible." (Practitioner site, not peer-reviewed.) — [equitiesindia glossary](https://equitiesindia.com/glossary/nifty-put-call-ratio-extremes)
- Independent quant blog: PCR predictability is "less reliable than often suggested in financial media," and weaker for index options than single-name options. — [Harbourfront Quant Substack](https://harbourfrontquant.substack.com/p/is-the-put-call-ratio-a-reliable)

### Inferences
- OI build-up quadrants (background definitions, standard practitioner usage, not sourced this session): price up + OI up = long build-up; price down + OI up = short build-up; price up + OI down = short covering; price down + OI down = long unwinding. These are descriptive of same-day positioning, not shown to predict.
- India's weekly-expiry regime changed drastically after SEBI's late-2024 F&O measures (one weekly expiry per exchange, larger lots), so pre-2024 PCR/OI relations may have shifted.
- Max pain is computed from option OI by strike; any "pinning" effect on expiry would need the agent's own test.

### Gaps
- No cost-adjusted Indian study of OI build-up rules, PCR trading rules or max pain found.
- NSE F&O bhavcopy/OI archive URL formats and history length not verified this session.

---

## Q5. Promoter buying (SAST/PIT disclosures) and mutual fund holdings changes

### Takeaway
Indian legal insider-trading studies find insider purchases earn positive and sales negative abnormal returns in the post-event window (four-factor event study), consistent with global insider literature; no cost-adjusted strategy test found. Mutual fund holdings-change evidence not found in this session. Grade: promoter buying **B-/C** (academic thesis-level evidence, no costs); MF holdings **C** (no evidence found).

### Cited Findings
- NITK doctoral study "An Empirical Analysis of Legal Insider Trading in India" (four-factor model with size, B/M, momentum; event study): insider purchase portfolios earn positive and sale portfolios negative abnormal returns; outsiders do not; both purchase and sale portfolios earn positive AR pre-event; post-event purchases positive, sales negative; insiders prefer large-cap, low B/M, high-momentum, low-P/E stocks; aggregate insider purchases/sales over a year associated with market jumps/crashes. — [NITK repository](https://idr.nitk.ac.in/handle/123456789/14150); [PDF](https://idr.l1.nitk.ac.in/jspui/bitstream/123456789/14150/1/138002HM13F05.pdf)
- IIM Bangalore repository has related insider-trading work. — [IIMB](https://repository.iimb.ac.in/handle/2074/20339)
- SAST/PIT disclosure feeds are aggregated per stock by data vendors (e.g., Trendlyne). — [Trendlyne insider/SAST page](https://trendlyne.com/equity/insider-trading-sast/all/ISTLTD/2369/ist-ltd/)

### Inferences
- (Background, not verified this session) Under SEBI PIT Regulations 2015, promoter/designated-person trades above Rs 10 lakh per quarter must be disclosed to the company within 2 trading days and by the company to exchanges within 2 trading days; SAST disclosures trigger on crossing 5% and each 2% change. NSE publishes these under "Corporate filings — Insider Trading" and "SAST." Lag of up to ~4 trading days means the agent can only trade post-disclosure.
- Mutual fund portfolios are disclosed monthly (background knowledge; typically within ~10 days of month end), so they are a swing/positional signal at best.

### Gaps
- No found study on post-disclosure promoter-buy drift with costs on NSE; no study on MF holdings-change signals in India.
- Exact PIT/SAST timelines and MF disclosure deadlines should be verified against SEBI regulations before use.

---

## Q6. Popularity of smart-money / price-action educators (global and India), and verified track records

### Takeaway
India's largest trading-education YouTubers (Pushkar Raj Thakur ~15–18M, CA Rachana Ranade ~5.3M) dwarf global SMC/ICT channels (ICT ~2.1M, TJR ~1.4M, Smart Risk ~0.95M). No independently verified track record was found for any of them. Subscriber numbers below come from aggregators with uncertain dates and should be treated as approximate.

### Cited Findings
- Pushkar Raj Thakur: surpassed 15M YouTube subscribers; creatordb lists ~18.3M followers; Guinness records for largest financial-investment lesson; teaches broad beginner stock-market/investing content. — [pocketful.in list 2026](https://www.pocketful.in/blog/trading/best-trading-youtube-channels/); [creatordb](https://creatordb.app/creatorstats/pushkar-raj-thakur/); [channel](https://www.youtube.com/channel/UCEAAzv2OBqxsSczKJ2QZyGQ)
- CA Rachana Phadke Ranade: ~5.34M subscribers (fundamental/basic stock market education, courses). — [Social Blade](https://socialblade.com/youtube/handle/carachanaranade); [Playboard](https://playboard.co/en/channel/UCe3qdG0A_gr-sEdat5y2twQ); [courses](https://www.rachanaranade.in/)
- Power of Stocks: ~2.14M subscribers; price action, option buying/selling, shares real trades/P&L. Note conflict: the aggregator attributes the channel to "Vivek Singh," whereas the channel is widely associated with Subhasish Pani — treat attribution as unverified. — [pocketful.in](https://www.pocketful.in/blog/trading/best-trading-youtube-channels/)
- The Inner Circle Trader (Michael Huddleston): ~2.1M subscribers (aggregator figure; the aggregator's "momentum and scalping" description is a poor summary of ICT — ICT is known for order blocks, fair value gaps, liquidity sweeps, kill zones [background knowledge]). — [TradingMonday YouTubers list](https://tradingmonday.com/youtubers)
- TJR Trades: ~1.4M subscribers. — [TradingMonday](https://tradingmonday.com/youtubers)
- Smart Risk: ~952K subscribers (SMC/ICT-style content). — [vidIQ](https://vidiq.com/youtube-stats/channel/UCX_LoZNPSLM4pm31TKDB6jw/); [channel](https://www.youtube.com/@smart_risk)
- Al Brooks (Brooks Trading Course), Trader Dale (volume profile/order flow), Wyckoff Analytics (Wyckoff method; free weekly streams on YouTube and X): channels located but reliable subscriber counts not obtained. — [Brooks Trading Course](https://www.youtube.com/@BrooksTradingCourse); [Trader Dale](https://www.youtube.com/c/TraderDale1); [Wyckoff Analytics channel](https://www.youtube.com/@WyckoffAnalytics); [Wyckoff Analytics on X](https://x.com/WyckoffAnalysis/status/1933606941663900062)
- Broader 2026 lists of top stock-market YouTubers. — [Feedspot 70 Stock Market YouTubers 2026](https://videos.feedspot.com/stock_market_youtube_channels/); [themoneydecoded 2026](https://themoneydecoded.com/blog/best-finance-youtube-channels)

### Inferences
- For an algorithmic agent, popularity is a sentiment/crowding consideration: widely taught SMC levels (prior-day high/low sweeps, round-number liquidity, FVGs) may be crowded, which is a hypothesis to test, not a finding.

### Gaps
- No subscriber counts verified directly from YouTube; counts for Al Brooks, Trader Dale, Wyckoff Analytics, Ali Khan, Vivek Bajaj/StockEdge, Trade with Trend not obtained.
- No audited/verified track record was found for any educator listed.

---

## Q7. SEBI finfluencer rules, enforcement, and trader-loss studies

### Takeaway
SEBI studies show ~71% of individual intraday cash traders lost money (FY23) and >91% of individual F&O traders lost money (FY25, net losses Rs 1.06 lakh crore after costs). SEBI's Jan 2025 circular bars unregistered "educators" from using market prices from the preceding three months, and in Dec 2025 SEBI impounded Rs 546 crore from finfluencer Avadhut Sathe's academy.

### Cited Findings
- SEBI intraday study (FY23, published July 2024): 7 in 10 (≈71%) individual intraday equity-cash traders made losses; average loss Rs 5,371; intraday participants up >300% vs FY19; top-10 brokers' intraday traders rose from 1.5M (FY19) to 6.9M (FY23). — [Business Standard](https://www.business-standard.com/amp/markets/news/7-in-10-intraday-traders-in-equity-cash-suffered-losses-in-fy23-sebi-study-124072400975_1.html); [ThePrint](https://theprint.in/economy/71-of-intraday-traders-incur-losses-finds-sebi-report-impact-higher-among-young-small-traders/2192598/)
- SEBI F&O study (FY25, published July 2025): >91% of individual traders lost money; net losses (after transaction costs) rose 41% to Rs 1,05,603 crore from Rs 74,812 crore in FY24; average loss ~Rs 1.1 lakh; unique traders fell from 61.4 lakh (Q1 FY25) to 42.7 lakh (Q4); traders active >100 days were 42% of traders but 94% of turnover and 87% of losses. — [Business Standard](https://www.business-standard.com/amp/markets/news/net-losses-of-traders-in-fo-widens-in-fy25-sebi-study-125070701221_1.html); [Angel One](https://www.angelone.in/news/market-updates/retail-f-o-losses-rose-to-over-1-lakh-crore-trader-participation-dropped-20-percent); [TradingQnA](https://tradingqna.com/t/win-rate-capital-experience-what-sebi-found-about-f-o-traders/197266)
- (The earlier "93% of F&O traders lost" figure refers to SEBI's FY22–FY24 study — background knowledge, not re-verified this session.)
- SEBI circular of 29 Jan 2025: persons providing stock-market education must not give advice or make performance claims unless SEBI-registered, and must not use market price data of the preceding **three months** in any form (videos, tickers, screen shares) to indicate future movements; ends "live" trading-as-education. — [Business Standard](https://www.business-standard.com/markets/news/sebi-finfluencer-circular-live-stock-data-market-education-rules-125013000571_1.html); [NewsOnAIR](https://www.newsonair.gov.in/sebi-cracks-down-on-finfluencers-selling-stock-tips-in-the-name-of-education); [Storyboard18](https://www.storyboard18.com/social-media/sebi-cracks-down-on-finfluencers-with-new-rules-on-stock-market-education-55026.htm)
- Avadhut Sathe (Dec 2025): SEBI interim order impounded Rs 546.17 crore from Avadhut Sathe and Avadhut Sathe Trading Academy Pvt Ltd, barred him, wife Gouri Sathe and the academy from the market for unregistered advisory under the guise of training (live tips, WhatsApp recommendations, misleading promotions); proposed disgorgement ~Rs 601 crore; SAT hearing on the academy's appeal set for 9 Jan 2026. — [Moneylife](https://moneylife.in/article/54617-crore-impounded-as-sebi-bans-avadhut-sathe-wife-and-academy-for-running-unregistered-advisory-network/79036.html); [Business Today](https://www.businesstoday.in/markets/stocks/story/avadhut-sathe-sebi-order-what-stock-traders-need-to-know-505149-2025-12-05); [Business Standard SAT](https://www.business-standard.com/markets/news/sat-hearing-jan-9-2026-asta-appeal-sebi-order-impounding-546-crore-125121900509_1.html)

### Inferences
- The SEBI loss statistics are the strongest available base rate: retail discretionary trading in Indian F&O/intraday is overwhelmingly loss-making after costs, which should set a high bar for any "smart money" rule the agent adopts.
- (Background, not verified this session) SEBI's Aug 2024 rules bar registered intermediaries from associating with unregistered finfluencers who give advice or make return claims; earlier actions include PR Sundar settlement (2024) and "Baap of Chart" (2023). Asmita Patel was not confirmed in my searches.

### Gaps
- Outcome of the SAT appeal in the Sathe case (post-Jan 2026) not found.
- Whether any SEBI action targeted the specific YouTubers listed in Q6 was not established (none found in searches).
