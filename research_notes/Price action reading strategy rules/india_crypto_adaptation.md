# Adapting Qullamaggie / Stamatoudis momentum setups (Episodic Pivot, ORB on stocks-in-play, Breakouts, Parabolic Short) to NSE India and Crypto

> Research context and caveats (read first):
> - Run date 2026-10-05. The research proxy blocked most primary pages (qullamaggie.com, traderlion.com, all *.substack.com, tradingsim, snappchart, financialwisdomtv, emerald.com, nottingham eprints, hse.ru). Many findings below therefore rest on **search-engine extracts** of those pages, not full-page reads. They are cited to the page the extract came from. Treat any number marked "(extract)" as needing a direct re-check before it is hard-coded.
> - Items stated from general domain knowledge and NOT verified in this session are kept out of Cited Findings and put under Inferences/Gaps and labelled "UNVERIFIED".

---

## Q1. Qullamaggie's three setups (and Stamatoudis's variant): exact published criteria and any quantitative backtests

### Takeaway
The published rules are mostly qualitative with a few hard numbers: breakouts need a prior 30-100%+ move in 1-3 months, then a 2-8 week (often 1-3 week) tightening consolidation, ADR above about 4%, and price above the 10/20-day MAs. Episodic pivots need a gap of at least about 10% (Bonde/Kullamägi say 8-10%+; Stamatoudis uses 5%+) on a catalyst with huge volume. Both use the same entry and exit: buy the 1-, 5- or 60-minute opening-range high, put the stop at the low of the day with stop width at or under 1x ADR (1.5x at most), sell 1/3 to 1/2 after 3-5 days, and trail the rest on the 10- or 20-day MA. I found no rigorous, survivorship-free public backtest. The only "backtest" found is a cherry-picked case study of the top 100 winners.

### Cited Findings
**Breakout (continuation) setup**
- Scanner settings attributed to Qullamaggie for breakouts: ADR > 4%, price above 10-day and 20-day MA, prior move of 30-100%+ in the past 1-3 months, consolidation of 2-8 weeks (extract) — [Breakouts Happen](https://breakoutshappen.com/stock-news/how-to-trade-like-qullamaggie-setups-strategy-and-screener)
- The premise comes from studying historical winners that move in stair-steps: a 30-100% advance, then an orderly consolidation with higher lows and a tightening range over 1-3 weeks, then another advance (extract) — [Grokipedia: Qullamaggie's Breakout Entry Strategy](https://grokipedia.com/page/Qullamaggies_Breakout_Entry_Strategy) (low-quality aggregator; the same content appears in [EBC](https://www.ebc.com/forex/qullamaggie-strategy-3-trading-setups))
- Kullamägi reported a 25% win rate for 2019 and has talked about roughly 25-30% winners in his setup material (extract) — [Breakouts Happen](https://breakoutshappen.com/stock-news/how-to-trade-like-qullamaggie-setups-strategy-and-screener) / [EBC](https://www.ebc.com/forex/qullamaggie-strategy-3-trading-setups)

**Episodic Pivot (EP)**
- An EP is a 10%+ gap up on unexpected news or earnings with massive volume. Per Qullamaggie and Pradeep Bonde the gap "needs to be considerable: 8-10% or more" (extract) — [TradingSim](https://www.tradingsim.com/blog/episodic-pivot-power-earnings-gap-buyable-gap-up-explained); [Financial Wisdom TV](https://www.financialwisdomtv.com/post/the-episodic-pivot-strategy-qullamaggie-s-high-momentum-setup-explained)
- Entry: buy near the opening-range high (ORH) with the stop at the day's low. Qullamaggie waits for the first 1-minute candle to form, watches volume, and enters when the ORH breaks (extract) — [TradingSim](https://www.tradingsim.com/blog/episodic-pivot-power-earnings-gap-buyable-gap-up-explained)
- Stop is no more than 1x, at most 1.5x, the average daily range / ATR (extract) — [TradingSim](https://www.tradingsim.com/blog/episodic-pivot-power-earnings-gap-buyable-gap-up-explained)
- Exits: take 1/3 to 1/2 off at about 20% or 3-5 days into the move, then trail with the 10- or 20-day MA (extract) — [TradingSim](https://www.tradingsim.com/blog/episodic-pivot-power-earnings-gap-buyable-gap-up-explained)
- Primary source (blocked from fetching; title confirms the page exists): [Qullamaggie: How to master a setup: Episodic Pivots](https://qullamaggie.com/how-to-master-a-setup-episodic-pivots/) and [3 TIMELESS setups](https://qullamaggie.com/my-3-timeless-setups-that-have-made-me-tens-of-millions/)

**Parabolic short**
- Universe: large caps up 50-100%+ in a few days or weeks, or small caps up 300-1000%+ over the same timeframe, after rising 3-5+ days in a row (extract) — [Stonks Capital: Systemizing Kullamägi's Parabolic Short](https://stonkscapital.substack.com/p/systemizing-kullamagis-parabolic?r=5igdr)
- Entry: short the break of the opening-range low (1- or 5-minute). If the stock keeps flying at the open, wait for the first red 5-minute candle, or short the failed bounce to VWAP after it cracks. Stop at the high of the day, or at a VWAP reclaim after the failure. Targets are the 10- and 20-day MAs (extract) — [same](https://stonkscapital.substack.com/p/systemizing-kullamagis-parabolic?r=5igdr)

**Marios Stamatoudis (2023 US Investing Championship)**
- EP gap threshold is 5% or more on a catalyst (surprise earnings, drug approval, regulatory change) in a previously dormant stock. Entry is the ORH of the first 1-, 5- or 60-minute candle; the stop goes at the low of the day. The 60-minute version filters noise but enters later (extract) — [Lilys.ai notes on Marios Stamatoudis strategy](https://lilys.ai/fr/notes/us-stock-strategy-20251224/marios-stamatoudis-2023-champion-strategy)
- He enters on strength and adds as the trend confirms, using daily levels for triggers and intraday action for execution (extract) — [same](https://lilys.ai/fr/notes/us-stock-strategy-20251224/marios-stamatoudis-2023-champion-strategy)
- TraderLion reports a 291% return "with tight risk" (title only; page blocked) — [TraderLion](https://traderlion.com/investing-champions/marios-stamatoudis-usic/)

**Quantitative tests**
- Case study: from the top 100 performing stocks of the prior year, 58 "clean" Qullamaggie breakout setups were found, with average risk of about 3% and average return of 62% (more than 20R) (extract) — [Financial Wisdom TV case study](https://www.financialwisdomtv.com/post/qullamaggie-breakout-setup-case-study-what-the-top-100-winning-stocks-reveal); [Breakouts Happen](https://breakoutshappen.com/stock-news/how-to-trade-like-qullamaggie-setups-strategy-and-screener)
- Community tooling: TradingView scripts that encode the system (e.g., "Qullamaggie Trading System Pro", "High Tight Flag Table") exist but are protected or closed source — [TradingView](https://in.tradingview.com/script/iQtlXxO8-Qullamaggie-Trading-System-Pro/)

### Inferences
- The top-100-winners case study is **pure survivorship/look-ahead bias**: it chose stocks by their ex-post outcome. Its 62% average and 20R must not be used as an expectancy estimate. The realistic profile is the self-reported 25-30% win rate with a large average-win/average-loss ratio.
- Developer-ready rule skeleton (synthesised from the extracts above; parameters to optimise per market):
  - **Breakout**: `ret(close, 21..63 bars) >= +30%`, `ADR20 >= 4%` (ADR% = mean over 20 days of (High/Low − 1)·100), `close > SMA10 > SMA20`, a consolidation of 10-40 bars whose range contracts (e.g., the 5-day range is below 0.5x the 20-day range, and lows are higher), price within about 15% of the consolidation high. Entry is a break of the consolidation high on the 1/5/60-min ORH. Stop is the LOD and must be ≤ 1.0x ADR (≤ 1.5x max), otherwise skip. Sell 1/3-1/2 on day 3-5, then trail on a close below SMA10 (fast names) or SMA20.
  - **EP**: `gap = open/prevClose − 1 >= 10%` (test 5% and 8%), a catalyst present (results or announcement), `RVOL` very high (e.g., ≥ 3-5x projected, an assumption), ideally a neglected or sideways prior 3-6 months. Entry, stop and exits are the same as the breakout.
  - **Parabolic short**: large cap `ret(3..20 bars) >= 50-100%`, small cap `>= 300%`, `≥ 3` consecutive up days. Entry on the ORL break / first red 5-min / failed VWAP. Stop at HOD. Cover at SMA10/SMA20.
- The parabolic short **does not port to the Indian cash market as a swing trade** (see Q3): cash shorts must be squared off the same day. It is only possible via single-stock futures (F&O names only) or SLB. In crypto it ports to perpetual futures.

### Gaps
- Could not read qullamaggie.com directly (egress blocked), so these exact phrasings were not verified from the source: the "60-minute ORH" usage for breakouts, the "3-5 days" partial, and the risk-per-trade figures (commonly quoted as 0.25-1% of account, UNVERIFIED).
- No independent, survivorship-free systematic backtest of EP/breakout rules (US or India) was found. A Substack "deep dive on gap trading" with statistics exists ([What Works in Trading](https://whatworksintrading.substack.com/p/deep-dive-on-gap-trading-how-did)) but was blocked, so its numbers are unknown.
- Stamatoudis's risk per trade, position caps and trailing rules could not be confirmed (TraderLion/Substack interview transcripts blocked: [Retail Traders Repository](https://retailtradersrepository.substack.com/p/marios-stamatoudis-the-traderlion-983)).

---

## Q2. Evidence on earnings/news gap continuation vs fade in India (NSE), Indian PEAD, results timing

### Takeaway
Indian academic evidence supports some post-results continuation (PEAD) and an asymmetry: good news is absorbed quickly, while bad news drifts for over a week. Market response differs significantly between results released during trading hours and after hours. However, I found **no published study that directly measures "gap ≥ X% on results → continuation vs fade" in NSE stocks**. That must be backtested in-house. Indian results can come mid-session (unlike the US), so an EP in India is often an intraday breakout, not a gap.

### Cited Findings
- A study of 469 companies, Dec 2002 – Dec 2011, found significant post-event abnormal returns in 35 of 37 quarters, with strong continuation patterns in earnings (extract) — [IDEAS/RePEc, Vision (SAGE) 2015 19(1):25-36](https://ideas.repec.org/a/sae/vision/v19y2015i1p25-36.html)
- An event study on India finds relative efficiency in response to good earnings news, but evidence of **negative post-earnings drift for over a week after bad earnings news** (extract) — [Nottingham eprints 26569](https://eprints.nottingham.ac.uk/26569)
- A study of 100 NSE firms, 2014-2018 (1,130 observations), found a *negative* association between abnormal returns and earnings surprise, which contradicts the classic PEAD sign (extract) — [Trends Economics & Management (VUT Brno)](https://journals.lib.vutbr.cz/index.php/trends/article/view/541)
- Prior NSE work finds that value and glamour stocks react differently to earnings announcements, with PEAD tied to surprise-driven abnormal returns (extract) — [same](https://journals.lib.vutbr.cz/index.php/trends/article/view/541)
- **Timing**: there are statistically significant differences in market response to earnings announced during vs after trading hours, with a negative response to after-hours announcements. Firms with large surprises are more likely to announce after hours. In India companies announce both during and after trading hours, unlike the US (extract) — [Emerald, Asian Journal of Accounting Research, doi 10.1108/AJAR-04-2019-0023](https://www.emerald.com/insight/content/doi/10.1108/AJAR-04-2019-0023/full/html)
- **Regulation**: under SEBI LODR, financial results must be disclosed to the exchanges within 30 minutes of the end of the board meeting. For multi-day meetings, it is 30 minutes after that day's meeting ends — [StudyCafe FAQ on LODR amendments](https://studycafe.in/faqs-on-sebi-lodr-amendments-dated-05th-may-2021-102028.html); [Legal Wires](https://legal-wires.com/lex-o-pedia/sebi-lodr-compliance-obligations-listed-company/)
- Intraday NSE microstructure: security returns reverse direction within a few minutes of extreme price rises and falls (extract) — [International Journal of Financial Management, "Prior return effect in Indian stock market: an intra-day analysis"](https://publishingindia.com/archive/ijfm/prior-return-effect-in-indian-stock-market-an-intra-day-analysis)
- Overnight vs intraday: an Indian practitioner study and debate argues that much of Nifty's long-run return has come overnight (close→open) while intraday returns were a drag (extract; practitioner, not peer-reviewed) — [Multibagg.ai](https://www.multibagg.ai/market-pulse/articles/nifty-overnight-intraday-returns-study-cmquu8ofy9pn1nz0j1dln1abo)
- General gap lore (not India-specific, not rigorous): small gaps tend to fill, and large gaps carrying new information tend to trend; fill tendency is inversely related to gap size. One trader observed Nifty usually fills gaps the same or next day — [LuxAlgo Gap Fill](https://www.luxalgo.com/library/concept/gap-fill.md); [TradingView Nifty gap study](https://www.tradingview.com/chart/NIFTY/G7uW85qS-Nifty-Gap-Theory-Study-Observation)

### Inferences
- **Results-timing rule for the code**: tag each results event by its NSE announcement timestamp.
  - (a) Before 09:00 or after 15:30 → treat as a classic gap EP the next session (gap measured at the pre-open equilibrium price).
  - (b) 09:15-15:30 → no gap. Treat it as an "intraday EP": the trigger is a break of the pre-announcement intraday high on a surge in volume. The setup day's "opening range" is the post-announcement range (e.g., the first 5/15 minutes after the filing timestamp), not 09:15.
  - (c) Board meetings that end close to 15:30 produce partially priced moves. The next-day gap understates the total reaction, so measure "event return" from the last close before the announcement.
- The bad-news drift finding (over a week) suggests **gap-down short EPs** may be more robust than gap-up longs in India, but cash-segment shorts cannot be held overnight (see Q3), so they need F&O futures.
- The intraday-reversal-after-extremes finding and the overnight-vs-intraday finding both warn that **ORB entries on large gaps can fade intraday**. The ORH-plus-LOD-stop structure handles this through small losses, but the win rate should be expected near or below the US 25-30%.

### Gaps
- No NSE-specific statistic on P(close > open | results gap ≥ 5/10%) or on multi-day continuation was found. It needs an in-house backtest (NSE bhavcopy + corporate-announcement timestamps).
- The share of Indian results released during market hours vs after hours could not be quantified (the Emerald paper was blocked).
- Direction and size of Indian PEAD conflict across studies (positive continuation in Vision 2015 vs a negative surprise association in the VUT 2014-18 sample). This is unresolved.

---

## Q3. Indian microstructure constraints: price bands, circuits, F&O vs cash, pre-open, leverage, shorting, liquidity, ASM/GSM, costs

### Takeaway
For NSE the binding constraints are these:
- Non-F&O stocks have fixed price bands (capped at 20%; tighter for surveillance names), so a big-news gap can open locked at the upper circuit and be unbuyable.
- F&O stocks have a 10% dynamic band that flexes in 5% steps, but only after a 50-trade, 10-UCC, 3-member threshold plus a 15-minute cool-off.
- The 09:00-09:15 pre-open call auction sets the open price, so the "first 1-minute candle" starts at 09:15 on an auction-discovered price.
- Intraday leverage is at most 5x (20% margin).
- Cash shorts must be squared off the same day.
- ASM/GSM names carry 100% margin and trade-to-trade settlement (no intraday).
- STT is 0.1% on both sides of delivery and 0.025% on the intraday sell side. F&O STT rose from 1 April 2026.

### Cited Findings
**Price bands / circuits**
- Securities with F&O have no fixed band. A dynamic band of 10% of the previous close is flexed in stages in the direction of the move. Non-F&O stocks have fixed bands capped at 20% — [5paisa](https://www.5paisa.com/news/sebi-to-enforce-dynamic-price-bands-for-all-fo-stocks-equally-on-exchanges); [Rupeezy support](https://support.rupeezy.in/support/solutions/articles/21000004845-what-are-circuit-limits-or-price-bands)
- Flex mechanics: the band relaxes by 5% at a time. Since June 2024 this requires at least 50 trades with 10 different UCCs and 3 unique trading members on each side at or above 9.90% of the base price, plus a **15-minute cooling-off** before the flex (extract) — [FYERS notice](https://fyers.in/notice-board/revised-dynamic-price-bands-and-fo-segment-rules/); [5paisa](https://www.5paisa.com/news/sebi-to-enforce-dynamic-price-bands-for-all-fo-stocks-equally-on-exchanges)
- SEBI moved to require dynamic bands for F&O securities uniformly across all exchanges — [5paisa](https://www.5paisa.com/news/sebi-to-enforce-dynamic-price-bands-for-all-fo-stocks-equally-on-exchanges)
- Real-time "price band hitter" lists can be fetched (unofficial API) — [Unofficed](https://unofficed.com/?p=20211)

**Pre-open session**
- Cash-market pre-open runs 09:00-09:15: 8 minutes for order entry, modification and cancellation (closing randomly between the 7th and 8th minute), 4 minutes for matching, and 3 minutes of buffer. The equilibrium price (the price with maximum executable volume, tie-broken by minimum imbalance, then closeness to the base price) becomes the open price — [NSE special pre-open page](https://www.nseindia.com/products-services/equity-market-special-pre-open-session); [NSE circulars, e.g., CMTR57524](https://archives.nseindia.com/content/circulars/CMTR57524.pdf)
- Relisted securities and IPOs use a separate one-hour call auction. If no equilibrium is discovered, orders are cancelled and the stock stays in call auction — [NSE special pre-open](https://www.nseindia.com/products-services/equity-market-special-pre-open-session)
- **New since 8 Dec 2025**: NSE introduced a pre-open call auction (09:00-09:15, same 8/4/3 structure with random close) for **current-month index and single-stock futures**. Options, spreads and far-month contracts are excluded — [Business Standard](https://www.business-standard.com/amp/markets/capital-market-news/nse-to-commence-pre-opening-session-in-f-o-segment-from-december-08-125110400728_1.html); [Jainam](https://www.jainam.in/blog/nse-pre-open-session-fo-segment/); [5paisa](https://www.5paisa.com/index.php/news/nse-fo-pre-open-session-begins-today-aims-to-improve-price-discovery-and-reduce-volatility)

**Leverage and shorting**
- Equity intraday (MIS) margin is at least 20% of trade value (5x maximum leverage). Exchanges mandate the higher of 20% or VaR+ELM for both delivery and intraday. Under peak-margin rules F&O gets no extra intraday leverage (1x NRML) — [Zerodha support](https://support.zerodha.com/category/trading-and-markets/margins/margin-leverage-and-product-and-order-types/articles/how-much-margins-leverage-does-zerodha-provide); [Zerodha: types of margin](https://support.zerodha.com/category/trading-and-markets/margins/margin-leverage-and-product-and-order-types/articles/different-types-of-margin)

**Surveillance lists**
- GSM Stage I requires 100% margin with a price band of 5% or lower. From Stage II, trade-to-trade (T2T) settlement applies, every trade is compulsory delivery, and **intraday is not permitted**. Inclusion criteria include low market cap, high promoter concentration, low public float and weak financials — [Groww](https://groww.in/blog/graded-surveillance-measure); [Zerodha support: GSM](https://support.zerodha.com/category/trading-and-markets/trading-faqs/articles/what-does-gsm-mean); [Bajaj Broking](https://www.bajajbroking.in/blog/decoding-asm-and-gsm-frameworks-and-stages); NSE surveillance circular example [SURV56326](https://nsearchives.nseindia.com/content/circulars/SURV56326.pdf)

**Cost stack (FY2026-27)**
- From 1 April 2026: STT on futures sale rose from 0.02% to **0.05%**, on options premium (sell) from 0.10% to **0.15%**, and on options exercise from 0.125% to **0.15%**. **Equity delivery (0.1% on buy and sell) and equity intraday (0.025% on sell) are unchanged** — [ICICI Direct](https://www.icicidirect.com/ilearn/futures-and-options/articles/stt-changes-in-budget-2026-what-f-o-traders-should-know); [Groww](https://groww.in/blog/what-is-stt); [Navia](https://navia.co.in/blog/union-budget-2026-stt-hike/); [Sansa Legal](https://www.sansalegal.com/post/stt-hike-on-futures-and-options-new-rates-effective-april-1-2026)

### Inferences
- **Universe split (hard rule for the code)**:
  - **F&O stocks** (about 200+ names) can take EP longs and shorts, overnight shorts via futures, and a 10% gap that can extend intraday after a flex. Buying above +10% is possible only after the flex (and its 15-minute cool-off). So the ORH entry can be delayed or blocked near the band, and the code must check `price < band_upper − buffer`.
  - **Cash-only stocks** are long-only for swings. Reject any EP where `open >= upper_band × 0.98` (the stock opens locked or near-locked, so fills are impossible or adverse). For a 20%-band stock the maximum tradable gap is effectively under about 15-18%. For 5%/10%-band names the "EP ≥ 10% gap" rule is impossible on day 1, so use multi-day EP logic (a circuit on day 1, then an ORH entry on day 2), or lower the gap threshold to 5% (the Stamatoudis level).
- **Opening range on NSE**: the 09:15 first print is the auction equilibrium, so the first 1-minute bar reflects auction imbalance and is noisier than a US 1-minute bar. Prefer a 5-minute ORH (09:15-09:20) or a 15/60-minute ORH (09:15-10:15). Use the pre-open indicative equilibrium price and pre-open volume (09:00-09:08) as the gap and RVOL signal before the open.
- **Relative volume**: compute RVOL as cumulative volume up to time t divided by the average cumulative volume up to t over the last 20 sessions (time-of-day normalised). Day-level RVOL = volume / SMA20(volume). A suggested EP filter is day RVOL projected ≥ 3x (an assumption to test).
- **Liquidity floor (assumption, to calibrate)**: 20-day median traded value ≥ ₹10-25 crore/day for intraday ORB, and ≥ ₹5 crore/day for swing positions sized under about 1% of ADV. Exclude ASM/GSM, T2T ("BE"/"BZ"-type series) and SME stocks from intraday ORB because MIS is not allowed there.
- **Shorting**: the cash parabolic short is intraday-only (MIS square-off). An overnight parabolic short needs single-stock futures, so only F&O names qualify, and many Indian parabolic small caps are not in F&O, so the setup is largely untradable as a swing in India.
- **Cost model for backtests** (round trip, excluding brokerage, exchange, SEBI, GST and stamp duty, which add a few bps):
  - Delivery swing: STT 0.2% round trip, plus stamp duty on the buy (UNVERIFIED rate 0.015%), plus slippage.
  - Intraday: STT 0.025% on the sell, plus stamp 0.003% on the buy (UNVERIFIED).
  - Futures: STT 0.05% on the sell.
- Slippage in mid/small caps: no Indian statistic was found. As a working assumption, model 0.1-0.3% per side for ORB entries on gap days in names with ₹10-50 cr ADV, more near the bands (UNVERIFIED; needs tick-data calibration).

### Gaps
- The exact list of NSE fixed band levels (2% / 5% / 10% / 20%) and the band-revision rules for non-F&O names were not fetched from an NSE circular in this session (UNVERIFIED but standard practice). Check NSE "price band" files daily (`sec_list.csv` / price-band changes), not hard-coded values.
- Is there an ASM-specific intraday restriction distinct from GSM? Only GSM was confirmed. ASM long-term/short-term stage rules (margins 50%/100%) are UNVERIFIED.
- No quantitative slippage or impact-cost data for NSE small/mid caps on gap days was found. NSE publishes impact cost for index constituents, which could be used.
- Stamp duty and exchange transaction charges for 2026 were not verified.

---

## Q4. Data and tools needed to screen (NSE pre-open, announcements, RVOL, ADR, Chartink, TradingView, Python)

### Takeaway
All required inputs are obtainable. NSE exposes pre-open snapshots (`/api/market-data-pre-open?key=FO|ALL|NIFTY|SME|OTHERS`) and corporate announcements and board meetings through its unofficial JSON APIs, which have Python wrappers (nsepython, NseKit, nsefin) and an R package (nser). Chartink offers delayed scans free and live scans paid. TradingView covers charting and community Qullamaggie indicators. ADR% and RVOL must be computed yourself.

### Cited Findings
- NSE pre-open endpoint: `https://www.nseindia.com/api/market-data-pre-open`. nsepython's `nse_preopen()` accepts keys "NIFTY", "BANKNIFTY", "SME", "FO", "OTHERS" and "ALL", returning JSON or a DataFrame — [Unofficed forum](https://forum.unofficed.com/t/how-to-fetch-pre-open-market-data/1081)
- NseKit (PyPI) has `pre_market_info()`, `pre_market_nifty_info()` and `pre_market_all_nse_adv_dec_info()` — [PyPI NseKit](https://pypi.org/project/NseKit/)
- nsefin (PyPI) provides pre-market snapshots (All/FO/NIFTY) and corporate actions and announcements — [PyPI nsefin](https://pypi.org/project/nsefin/)
- R package `nser` has vignettes for NSE open/pre-open and F&O pre-open data — [CRAN nser nseopen](https://search.r-project.org/CRAN/packages/nser/vignettes/nseopen.html); [nseopenfo](https://cran.nics.utk.edu/cran/web/packages/nser/vignettes/nseopenfo.html)
- Chartink suits gap scanning in India. Free scans use delayed data and paid plans give live scans — [TradingQnA](https://tradingqna.com/t/how-to-find-gap-up-and-gap-down-stocks-using-scanner/7534); Chartink example scans — [Chartink articles](https://chartink.com/articles/author/akash/page/5)
- NiftyTrader publishes daily NSE gap-up/gap-down lists — [NiftyTrader](https://www.niftytrader.in/gap-ups-gap-downs)
- Stocks in play usually appear on scanners before the open because they gap in after-hours or pre-market. Traders filter gappers by market cap and average volume — [Stockbsessed Substack: Episodic Pivot](https://stockbsessed.substack.com/p/episodic-pivot-1)

### Inferences
- Pipeline:
  - (1) Daily EOD: NSE bhavcopy, then compute ADR20%, SMA10/20/50, 1/3/6-month returns, 20-day median traded value, and band/series (exclude BE/BZ/GSM/ASM).
  - (2) Event feed: NSE corporate announcements and board-meeting calendar (`/api/corporate-announcements`, `/api/event-calendar`; endpoint names UNVERIFIED, check the wrapper source). Keep the announcement timestamp.
  - (3) 09:08-09:12: pull pre-open (key=ALL and FO). Gap% = IEP/prevClose − 1. Pre-open volume ratio = pre-open qty / 20-day average of pre-open qty.
  - (4) From 09:15: 1-minute bars from a broker API (Zerodha Kite / Upstox / Dhan / Fyers) for the ORH/ORL and time-normalised RVOL.
- NSE's site rate-limits and needs a browser-like session/cookies. Production systems should prefer a broker data feed or a paid vendor (TrueData, Global Datafeeds) over scraping (UNVERIFIED vendor names, common practice).
- Chartink scan template (EP): `daily close/open gap ≥ 5-10%`, `volume ≥ 3 × SMA(volume,20)`, `close ≥ 50`, `SMA(volume×close,20) ≥ 10 cr`, an optional "results announced within last 1 day" (not natively available in Chartink, so it must be joined externally).

### Gaps
- Official, rate-limit-safe NSE APIs for announcements are not documented publicly. All Python libraries are unofficial scrapers and can break.
- I did not verify whether Chartink exposes pre-open (09:00-09:08) data.

---

## Q5. Crypto analogue: listing/news pumps, breakouts on liquid coins, continuation vs reversal after big daily moves

### Takeaway
Crypto evidence matches the "large-cap continuation, small-cap reversal" hypothesis. Across thousands of coins, yesterday's losers beat yesterday's winners (daily reversal), but the handful of largest and most liquid coins show daily and weekly momentum instead. Exchange-listing pumps (the "Binance effect") are front-loaded and largely mean-revert within about 2 weeks. Bitcoin shows intraday time-series momentum (the first high-volume session predicts the last), which supports ORB-style logic on BTC/ETH, provided the "open" is defined by a volume regime.

### Cited Findings
- Coins with a low previous-day return significantly outperform those with a high previous-day return (daily reversal), based on daily prices of more than 3,600 coins. The **handful of largest, most tradeable coins show daily momentum rather than reversal** (extract) — ["Up or down? Short-term reversal, momentum, and liquidity effects in cryptocurrency markets" (Istanbul Medeniyet Univ. repository)](https://earsiv.medeniyet.edu.tr/items/23a96c9f-3952-4bdf-aec5-abafb3aaab70)
- Weekly reversal occurs only in small, illiquid coins, while large, liquid coins show weekly momentum. Daily reversals come from the illiquidity of most traded coins (extract) — [same](https://earsiv.medeniyet.edu.tr/items/23a96c9f-3952-4bdf-aec5-abafb3aaab70)
- A separate study of 200 cryptocurrencies finds reversals are most pronounced in smaller-cap, less liquid coins (extract) — ["Cryptocurrency return reversals", Fairfield University](https://fairfield.elsevierpure.com/en/publications/cryptocurrency-return-reversals/); [RePEc, Applied Economics Letters 28(11)](https://ideas.repec.org/a/taf/apeclt/v28y2021i11p887-893.html)
- Dobrynskaya (HSE) studied about 2,000 coins with market cap above $1M over seven years. Momentum returns range from about 70% per year (2-week ranking and holding) to about −1,000% per year (1-2 week ranking with a 10-12 week holding), so crypto momentum is short-lived and reverses (extract) — [HSE news](https://www.hse.ru/en/news/research/559533243.html)
- **Binance listing effect**: average returns of +14.7% on listing day and +41% by the day after; about 73% in the first 30 days in one dataset. But "almost half" of coins had lost their gains after about 2 weeks, and buying on listing day implied an 18%+ drawdown over 6 months. Only 5 of 31 tokens listed in a 6-month window held gains (extract) — [Blockchain Research Lab](https://www.blockchainresearchlab.org/de/2019/09/10/market-reaction-to-exchange-listings-of-cryptocurrencies-2/); [Yahoo/CoinDesk "Binance effect 41%"](https://malaysia.news.yahoo.com/binance-effect-means-41-price-173526978.html); [FXStreet](https://www.fxstreet.com/cryptocurrencies/news/binance-effect-fades-less-than-20-tokens-are-profitable-six-months-after-listing-202405201006)
- For tokens already listed on 3+ major exchanges, returns are front-loaded into the pre-announcement window, and the BTC-adjusted return largely dissipates within two weeks after trading opens (extract) — [The TIE](https://www.thetie.io/insights/what-a-binance-listing-delivers-for-already-listed-tokens); [Blockworks](https://blockworks.com/news/major-exchange-listings-coinbase-binance)
- **Bitcoin intraday time-series momentum** (Shen, Urquhart & Wang): the first half hour predicts the last half hour. Because BTC trades 24/7, volume defines the trading "day". The highest-volume or highest-volatility first sessions give the greatest predictability, and gains are largest during BTC downturns — [University of Birmingham](https://research.birmingham.ac.uk/en/publications/bitcoin-intraday-time-series-momentum/); [Reading eprint PDF](https://reading-9.eprints-hosting.org/100181/3/21Sep2021Bitcoin%20Intraday%20Time-Series%20Momentum.R2.pdf). Original equity result (SPY 1993-2013): [Alpha Architect summary of Gao et al.](https://alphaarchitect.com/2014/08/attention-prop-traders-the-first-half-hour-of-trading-predicts-the-last-half-hour/)

### Inferences
- **Crypto rule adaptations**:
  - Universe: top about 20-30 by market cap / spot+perp volume (e.g., ≥ $100-200M of 24h perp volume, an assumption) for **breakout and EP longs (continuation)**. For coins outside that set, treat a big up-day as a **fade candidate**, not an EP.
  - "Gap" substitute: crypto has no session gap, so define an EP day as `daily return ≥ +10-15%` (an ADR-scaled threshold, e.g., ≥ 2.5x ADR20) with `volume ≥ 3x SMA20` on a dated catalyst (ETF/regulatory/listing on a top-tier exchange/protocol news).
  - Session "open": use 00:00 UTC (daily candle open), or better, the **13:30-14:30 UTC US cash-open window** or the highest-volume hour per Shen et al. Opening range = the first 15-60 minutes after the chosen anchor, or after the news timestamp.
  - Listing pumps: do NOT buy listing-day spikes for swings. The evidence shows reversal within about 2 weeks. If anything, the parabolic-short logic (3-5 up days, ORL/VWAP fail, stop at HOD) on perps fits these better, with funding-rate and squeeze risk.
  - Crypto ADR is structurally higher (BTC often 3-5%, alts 6-12%; UNVERIFIED ranges), so the Qullamaggie "ADR > 4%" filter is non-binding for alts. Instead use a stop ≤ 1x ADR together with a position size of risk/(stop%) and a cap on notional.
  - Costs: perp taker fees plus funding (8-hourly) matter for multi-day holds. Exact 2026 fee tiers were not verified.
- Indian residents trading crypto face a separate tax regime (30% flat tax plus 1% TDS on transfers, per Section 115BBH/194S; UNVERIFIED in this session, and check for 2026 changes), which makes high-turnover ORB strategies on Indian-regulated venues much more costly than on offshore perps.

### Gaps
- No study found that tests specifically "daily return ≥ X% on news → next 5-20 day continuation" for large-cap crypto. The momentum and reversal papers use cross-sectional sorts, not event filters.
- The exact authors and journal of "Up or down?" (the 3,600-coin study) could not be fetched. It appears to be Zaremba et al., *International Review of Financial Analysis* (2021) (UNVERIFIED).
- Post-2024 listing-effect statistics (e.g., Binance Alpha / launchpool era, Coinbase/Upbit listings) were not found beyond the 2019-2024 data cited.
- Indian crypto tax and TDS rules for 2026 were not verified.
