# The Power of Price Action Reading (Zarattini & Stamatoudis, SSRN 4879527): Implementable Rules, Results, Credibility

**How this was researched:** I could not open the paper itself. papers.ssrn.com, concretumgroup.com (including the full PDF at concretumgroup.com/wp-content/uploads/2026/02/The-Power-Of-Price-Action-Reading.pdf), every *.substack.com review, x.com, tradingview.com, quantpedia.com, web.archive.org and r.jina.ai were all refused by the egress proxy with a 403 policy denial. Every finding below comes from search-engine extracts of those pages. Each extract is tied to the page it came from (mostly the paper's PDF), but I could not read the full context.

Labels used:
- **[V]** = the detail appears in a search extract attributed to the paper or the authors' own posts.
- **[U]** = unverified, conflated or guessed.

Before coding anything, someone with access should read the PDF directly. It is about 25 pages and the parameters that matter sit in its tables.

---

## 1. Universe, data period and filters

### Takeaway
The dataset is 9,794 US-equity overnight gap-up events from 2016 to 2023 [V]. The gap threshold is about 6% or more [V, consistent across sources]. The average gap in the dataset is about 28% [V]. Price, volume, market-cap and earnings filters exist in some form, but I could not verify the exact numbers.

### Cited Findings
- The study covers "9,794 gap events from 2016 to 2023", which is 8 years of US stocks. [V] — [Quantpedia summary via search](https://quantpedia.com/combining-discretionary-and-algorithmic-trading/); [Paper PDF](https://concretumgroup.com/wp-content/uploads/2026/02/The-Power-Of-Price-Action-Reading.pdf)
- "The original dataset identifies 9,794 events that satisfy [the] gap criteria." [V] — [Paper PDF](https://concretumgroup.com/wp-content/uploads/2026/02/The-Power-Of-Price-Action-Reading.pdf)
- The study looks at "stocks that gap up more than 6% overnight". [V, from a search summary of the Concretum page and Substack] — [Concretum Substack](https://concretumgroup.substack.com/p/the-power-of-price-action-reading); [Concretum page](https://concretumgroup.com/the-power-of-price-action-reading/)
- "The dataset exhibits an average 28% overnight gap." [V] — [Paper PDF](https://concretumgroup.com/wp-content/uploads/2026/02/The-Power-Of-Price-Action-Reading.pdf)
- "Following the overnight gap, stock prices typically drift downward gradually." This describes the unconditional behaviour of the whole gap set. [V] — [Paper PDF](https://concretumgroup.com/wp-content/uploads/2026/02/The-Power-Of-Price-Action-Reading.pdf)
- The SSRN keywords include "overnight gaps, gap-go, PEAD, earnings gaps". This suggests earnings gaps are a central or dominant subset. [V for the keywords] — [SSRN abstract page (search extract)](https://papers.ssrn.com/sol3/papers.cfm?abstract_id=4879527)
- One search summary gives these criteria: gap of at least 6% above the prior close, opening price of at least $2, and at least 200,000 shares traded pre-market. [U: the same answer mixed in details from Zarattini's other ORB papers, so the attribution to this paper is uncertain] — [search extract citing the Paper PDF and a GitHub replication of Zarattini & Aziz 2023](https://github.com/giovannibrusco/zarattini-2023-orb-qqq)
- Stamatoudis's own conference notes give his discretionary gap criteria: gap up above about 5–6%, a "big catalyst" (about 80% of the time an earnings announcement), and big pre-market volume. These are his personal trading rules, not necessarily the paper's filters. — [Retail Traders Repository notes on Stamatoudis's TraderLion 2024 talk (search extract)](https://retailtradersrepository.substack.com/p/marios-stamatoudis-traderlionconference2024-p1)

### Inferences
- A safe baseline screener: gap ≥ 6% (open vs prior close), price ≥ $2–5, meaningful pre-market or relative volume, and a news or earnings catalyst. Only the 6% figure is reasonably verified.
- An average gap of 28% implies the set is dominated by small and mid caps and high-volatility names. Execution costs and liquidity matter a great deal (see section 5).

### Gaps
- I found no explicit market-cap, ATR, relative-volume or dollar-volume threshold attributed to this paper.
- I could not confirm whether the 9,794 events are restricted to earnings, or whether delisted stocks were included (survivorship).
- The data vendor (for example Norgate or Polygon) was not found.

---

## 2. Entry, stop, sizing, exits and holding period

### Takeaway
The mechanical baseline is multi-day, not intraday:
- **Entry:** buy-stop at the 5-minute opening-range high on the gap day, only if the first 5-minute candle is positive.
- **Stop:** 1 cent below the low of the first 5-minute candle.
- **Exit:** holding up to 30 days, with a variant that trails the stop with the 10-day SMA. The trader version adds "4 Targets", which is four 25% partial exits.

Sizing of 1% risk per trade and a 4x leverage cap is plausible but not verified for this paper.

### Cited Findings
- **Naive baselines tested** [V] — [Paper PDF](https://concretumgroup.com/wp-content/uploads/2026/02/The-Power-Of-Price-Action-Reading.pdf):
  - "Open – No Stop" (buy at the open) has a "significant negative edge".
  - "Open – Stop at 1 ATR" improves, "but still losses after 8 days".
  - The paper notes that closing quickly is preferable if weakness appears on the gap day.
- **"Pos OR"** [V] — [Paper PDF](https://concretumgroup.com/wp-content/uploads/2026/02/The-Power-Of-Price-Action-Reading.pdf):
  - Applies only when the gap-day first 5-minute candle is positive (close > open).
  - Enter on a break of the 5-minute opening-range high.
  - Stop 1 cent below the low of the first 5-minute candle.
  - Hold for 30 days or until stopped.
  - This "shows improvement… however, it continues to be unprofitable."
- **"Pos OR + Trailing"** is the same, but with a trailing stop set using the 10-day simple moving average, and a hold of 30 days or until stopped. It "enhances the results slightly, but not sufficiently to make it profitable." [V] — [Paper PDF](https://concretumgroup.com/wp-content/uploads/2026/02/The-Power-Of-Price-Action-Reading.pdf)
  - [U] Whether the trail triggers on a daily close below the 10-SMA or on an intraday touch was not visible in the extracts.
- **"Pos OR + Trailing + 4 Targets + Trader"** is the trader-filtered set with a 4-target partial-exit scheme. [V for the name] — [Paper PDF](https://concretumgroup.com/wp-content/uploads/2026/02/The-Power-Of-Price-Action-Reading.pdf)
  - The position "was divided into four parts (25% each), with partial exits." [V-ish, search summary attributed to the paper] — [search extract](https://retailtradersrepository.substack.com/p/trading-research-paper-the-power-of-price-action-reading)
  - [U] The four target levels (R multiples or price levels) were not found.
- **Sizing** [U for this paper]:
  - A search summary states 1% of current capital risked per trade (stop distance sets the share count), a maximum of 4x leverage, and $0.0005/share commission with no spread and no slippage.
  - These are the exact conventions of Zarattini's ORB papers, such as Zarattini & Aziz 2023 (SSRN 4416622). The same answer also quoted that paper's 10R target, so it probably mixed sources. Treat this as the likely Concretum house convention, not a confirmed detail of this paper. — [search extract referencing the GitHub replication](https://github.com/giovannibrusco/zarattini-2023-orb-qqq); [A Profitable Day Trading Strategy (Zarattini et al.)](https://www.wealth-lab.com/api/discussion/download/pdf/8007-ssrn-4729284-1-pdf)
  - The performance figures use a hypothetical $100,000 starting portfolio that grows to over $4M, which implies compounding with percentage-of-equity risk. [V] — [Concretum Substack (search extract)](https://concretumgroup.substack.com/p/the-power-of-price-action-reading)
- **Micromanaged stage**: Stamatoudis "micromanaged the entries, partials, and exits of each individual data point from the previous filtered dataset in an unbiased environment." This was discretionary, bar by bar, so it cannot be coded as written. [V, author's own X post] — [Stamatoudis on X](https://x.com/stamatoudism/status/1795787641490698254)

### Inferences
Coding spec for the mechanical core. It is highly confident on the entry and stop, and moderate on the trail:

| Step | Rule |
|---|---|
| Day 0, gap day | If the first 5-minute bar closes above its open, place a buy-stop at that bar's high + $0.01. |
| Stop | Low of the first 5-minute bar − $0.01. |
| Sizing | Shares = (1% × equity) / (entry − stop), capped so notional ≤ 4x equity. |
| Days 1–30 | Exit if the 10-day SMA trailing stop is hit; force-exit at day 30. |
| Trader layer | Add four 25% partial-profit targets (levels unknown). |

- **Expected baseline:** both "Pos OR" variants were unprofitable in the paper. The edge came entirely from the trader's chart selection and management. A purely mechanical implementation should expect a slightly negative edge unless the selection filter can be replicated.

### Gaps
- The four target levels for the partial exits are unknown.
- Whether the 10-SMA trail is evaluated on daily closes is unknown.
- Whether entry can happen after the first 5-minute bar on later bars (window cutoff) is unknown.
- The handling of the "1 ATR" stop period (ATR length) is unknown.
- The cost and slippage assumptions in this specific paper are not confirmed.

---

## 3. Results: automatic vs trader-filtered vs trader-micromanaged

### Takeaway
- **All gaps traded mechanically:** money-losing.
- **Trader-approved 1,721 gaps (about 18%), traded with the mechanical rules plus 4 targets:** the average trade peaks at about +0.25R around day 12.
- **Trader also managing the trades:** about +0.55R on the gap day and +0.80R by day 4. The equity curve shows a 3,968% total return, 59.1% CAGR, Sharpe 1.70 and 35% max drawdown (in 2021), with the five best trades excluded.

I found no win rate, profit factor or average-win/average-loss figures.

### Cited Findings
- "Trading all gaps on the long side consistently loses money, and every rules-based variation tested failed to produce meaningful improvements." [V] — [Concretum Substack (search extract)](https://concretumgroup.substack.com/p/the-power-of-price-action-reading)
- "After analyzing the daily charts of all 9,794 gap events… the trader approved 1,721 gaps, approximately 18% of the original dataset." [V] — [Concretum Substack (search extract)](https://concretumgroup.substack.com/p/the-power-of-price-action-reading); [Paper PDF](https://concretumgroup.com/wp-content/uploads/2026/02/The-Power-Of-Price-Action-Reading.pdf)
- "Pos OR + Trailing + 4 Targets + Trader" (trader-filtered, mechanically managed) "showed marked improvement in average profitability, reaching a peak at 0.25R, 12 days after the entry day." [V] — [Paper PDF](https://concretumgroup.com/wp-content/uploads/2026/02/The-Power-Of-Price-Action-Reading.pdf)
- Micromanaged version: "average profitability on the gap day increasing to 0.55R, reaching a local maximum of 0.80R on day 4." [V; the search extract implies this is the micromanaged stage, but the attribution to stage is my reading] — [Paper PDF](https://concretumgroup.com/wp-content/uploads/2026/02/The-Power-Of-Price-Action-Reading.pdf)
- Portfolio results: a hypothetical $100,000 grew to over $4,000,000 over 8 years, with **total return 3,968%, CAGR 59.1%, Sharpe 1.70**. [V] — [Concretum Substack (search extract)](https://concretumgroup.substack.com/p/the-power-of-price-action-reading)
  - The Substack attributes this to the stage where "Marios was allowed to also manage entries and exits bar by bar". Another extract loosely attributes the "nearly 4,000%" figure to "the discretionarily selected and traded gaps". Most likely this is the micromanaged stage, but this is a minor ambiguity.
- "The strategy encounters a maximum drawdown of 35% in 2021." [V] — [Paper PDF](https://concretumgroup.com/wp-content/uploads/2026/02/The-Power-Of-Price-Action-Reading.pdf); [Concretum Substack](https://concretumgroup.substack.com/p/the-power-of-price-action-reading)
- "We have also excluded the five best trades overall," described as making the results more conservative. [V] — [Stamatoudis on X, "The Power of Traders' Intuition #2"](https://x.com/stamatoudism/status/1795787641490698254)
- The paper was posted on SSRN on 28 June 2024 and last revised on 29 April 2025. Its findings were previewed by Stamatoudis on X in April and May 2024. — [SSRN (search extract)](https://papers.ssrn.com/sol3/papers.cfm?abstract_id=4879527); [Stamatoudis on X, April 2024](https://x.com/stamatoudism/status/1777312208290042282)

### Inferences
- Rough expectancy check, assuming about 1,721 trades over 8 years (about 215 per year), 1% risk and +0.5–0.8R average: that gives about 100–170R per year before compounding. This is consistent with a CAGR in the 50%s given overlapping positions and leverage caps. The headline numbers are internally plausible.
- A Sharpe of 1.7 with a 35% drawdown fits a concentrated small-cap momentum book.
- The value-add of selection alone (all gaps negative → about +0.25R peak) is the part an automated agent can hope to capture. The extra +0.3–0.55R came from discretionary management, which is far harder to replicate.

### Gaps
- No win rate, profit factor, average win or loss in R, or trade-count split were found for any stage.
- No CAGR, Sharpe or drawdown were found for the filter-only stage.
- The cost and slippage model of the portfolio simulation is unconfirmed (see section 2).
- It is unknown whether the 3,968% figure excludes the five best trades. The X post suggests yes for stage 2.

---

## 4. The bias-free environment and the chart features the trader favoured

### Takeaway
- **Blind review:** charts were anonymized, with no ticker, date, price scale, news or market context. Stamatoudis saw about two years of daily price history plus the gap, then approved or rejected the trade.
- **Management:** in stage 2 he replayed the trade bar by bar without seeing future bars.
- **Preferences:** he favoured gaps after a neglect period, multi-week or multi-month range breakouts, and gaps early in a momentum cycle. These can be approximated quantitatively, but the paper does not supply formulas.

### Cited Findings
- "Specialized software to anonymize charts and eliminate extraneous information to ensure an unbiased evaluation." — [Quantpedia (search extract)](https://quantpedia.com/combining-discretionary-and-algorithmic-trading/)
- He had "no information about specific tickers, dates, market conditions, prices, or news, only a two-year price history and a visual representation of the gap." — [search extract summarizing Stamatoudis's X posts and Concretum material](https://x.com/stamatoudism/status/1795787641490698254)
- Carlo Zarattini co-developed **R-Candles**, described as "the first backtester for discretionary traders". Its link to the study is likely but not confirmed in the extracts I saw. — [Concretum interview with Zarattini (search extract)](https://concretumgroup.com/exclusive-interview-with-carlo-zarattini-combining-quantitative-and-discretionary-trading/)
- Stage 2: he "micromanaged the entries, partials, and exits of each individual data point… in an unbiased environment." — [Stamatoudis on X](https://x.com/stamatoudism/status/1795787641490698254)
- Favoured features [V] — [Concretum Substack](https://concretumgroup.substack.com/p/the-power-of-price-action-reading); [Quantpedia](https://quantpedia.com/combining-discretionary-and-algorithmic-trading/):
  - Gaps following a **neglect period**, because they signal renewed interest.
  - **Multi-week or multi-month range breakouts**.
  - Gaps **early in the momentum cycle**, preferred over late-cycle gaps.
- Context on his style: 2023 US Investing Championship top-5 finisher with a verified +291% return, focused on tight risk and asymmetric payoffs (he notes that winning 30% of the time is enough). — [TraderLion USIC profile](https://traderlion.com/investing-champions/marios-stamatoudis-usic/); [TraderLion: Asymmetric Returns](https://traderlion.com/profile/marios-stamatoudis/asymmetric-returns/)

### Inferences
Possible screener proxies. These are my own guesses and are not from the paper:

| Feature | Possible proxy |
|---|---|
| Neglect period | 60–120-day pre-gap return near zero or negative; low 50-day ATR% relative to the 1-year median; volume dry-up (20-day average volume below 50-day average volume before the gap). |
| Multi-week or multi-month range breakout | Gap-day open (or close) above the highest high of the prior 20–120 days. Prior range tightness = (max high − min low) / price over N days below a threshold, such as 25–35%. |
| Early in the momentum cycle | Count of prior gaps of 6% or more, or prior 52-week-high breakouts, in the past 6–12 months is 0 or 1. Stock not already extended (for example, open less than 30–50% above its 200-day SMA, or 6-month return below some cap). |
| Combined | Rank by these features and approve the top 15–20%, matching his 18% approval rate. |

- **Lookahead caveat for my own build:** the trader saw two years of daily history up to the gap open only. Screener features must use data up to the gap-day open (pre-market volume allowed) and nothing after it.

### Gaps
- The paper reportedly analyzes the characteristics of approved vs rejected gaps (search extracts list the three preferences), but I could not access any quantitative tables comparing them.
- It is unknown whether the chart showed volume, moving averages or intraday bars at selection time.
- It is unknown whether review order was randomized.

---

## 5. Critiques, replications, limitations and follow-ups

### Takeaway
I found no independent replication and no published critique of this specific paper. The structural limitations are significant:
- The result depends on one trader (n=1).
- Discretionary management cannot be coded.
- Cost and slippage assumptions on small-cap gappers are unconfirmed.
- Recognition and hindsight leakage cannot be fully ruled out.
- The sample covers a single period (2016–2023, including the 2020–21 retail mania), with no out-of-sample test.

### Cited Findings
- The authors themselves hedge: discretion helps "at least when conducted by a skilled trader". — [SSRN abstract (search extract)](https://papers.ssrn.com/sol3/papers.cfm?abstract_id=4879527)
- Excluding the five best trades was an authors' robustness step. [V] — [Stamatoudis on X](https://x.com/stamatoudism/status/1795787641490698254)
- Related Concretum work warns that randomness can produce apparent "market wizards". This is relevant context for single-trader results, though it is not about this paper. — [Concretum: The Illusion of Exceptional Performance](https://concretumgroup.com/the-illusion-of-exceptional-performance-how-randomness-can-create-market-wizards/)
- Cost sensitivity in sibling Concretum ORB research: an independent replication of Zarattini & Aziz's 5-minute ORB on QQQ found break-even at about 2.2¢/share slippage. Another MQL5 replication on five indices found "gross reproduced, net zero". These are evidence of cost fragility in Zarattini's ORB-family results, not of this paper. — [GitHub giovannibrusco/zarattini-2023-orb-qqq](https://github.com/giovannibrusco/zarattini-2023-orb-qqq); [MQL5 blog replication](https://www.mql5.com/en/blogs/post/776235)
- Supportive commentary exists but adds no new data:
  - [Retail Traders Repository: "A research paper about price action gives me hope"](https://retailtradersrepository.substack.com/p/trading-research-paper-the-power-of-price-action-reading)
  - [Podcast discussion (Walter, Stock Trading Approach)](https://stocktradingapproach.substack.com/p/podcast-the-power-of-price-action)
  - [Wall Street Trader Substack](https://wallstreettrader.substack.com/p/how-a-traders-intuition-turned-a)
  - [TradingView write-up by Zeiierman](https://www.tradingview.com/chart/TSLA/7JBGcEeG-When-Intuition-Beats-the-Algorithm/)
  - Listed among [QuantSeeker's popular research of 2024](https://www.quantseeker.com/p/popular-investing-research-in-2024)
- Follow-ups: no later SSRN paper co-authored by Stamatoudis was found. Zarattini's other Concretum papers are separate, unrelated strategies:
  - [A Profitable Day Trading Strategy (stocks-in-play ORB)](https://www.researchgate.net/publication/379003745_A_Profitable_Day_Trading_Strategy_For_The_US_Equity_Market)
  - [The Volatility Edge (VIX ETNs)](https://papers.ssrn.com/sol3/papers.cfm?abstract_id=5316487)
  - [A Century of Profitable Industry Trends](https://papers.ssrn.com/sol3/papers.cfm?abstract_id=4857230)
  - A QuantConnect implementation exists for the stocks-in-play ORB, not for this paper: [QuantConnect](https://www.quantconnect.com/research/18444/opening-range-breakout-for-stocks-in-play/)

### Inferences
- **Recognition leakage:** a skilled trader may recognize famous 2016–2023 charts (TSLA, meme stocks, COVID names) even with no ticker or date. With a 28% average gap and well-known runners, some hindsight contamination is plausible.
- **Tail dependence:** a 35% drawdown with heavy right-tail dependence is typical of gap-momentum strategies. Removing only the top five trades is a weak robustness test for 1,721 trades.
- **Survivorship:** if the gap dataset excludes delisted names, results are biased upward. This could not be verified.
- **What an agent can code:** the mechanical rules (Pos OR + 10-SMA trail, 30-day cap) plus a quantified "neglect / range-breakout / early-cycle" filter. Expect performance between the losing mechanical baseline and the +0.25R filtered result, and net of realistic small-cap slippage, possibly near zero. Validate on post-2023 out-of-sample data before trusting it.

### Gaps
- No independent replication, peer review or critique of SSRN 4879527 was found.
- The paper's own limitations section could not be read (the PDF is blocked).
- Any podcast or interview transcript discussing implementation specifics could not be accessed because the Substack and X pages are blocked.
