# Fix the Code Before Tuning Any Threshold

The SSRN-listed research, read in full from author pages and mirrors because SSRN served a bot challenge, says the agent should change very few of its unverified (U) thresholds and fix its code first. The evidence supports five real parameter changes. A1's 120-day holding cap should rise to 250 days, because in the only full-text, net-of-cost breakout study fewer than 7% of trades produced all the profit and winners lasted 370 days on average. Sleeve B's latched −7.5%/−15% stop should become a −20% halve and a −30% off, because a healthy trend sleeve with about 15% volatility has a median three-year drawdown near 19%. B's cost base should rise to 25 bps and its missing-funding default to 10% a year. D1's fixed 19-point cost should become 0.06% of notional plus slippage. The momentum-crash gate and the event blackout should be redefined. Everything else should be frozen at its published or rulebook value. That covers A1's Kullamägi filters, A2's 6% gap and 80th-percentile approval, B's nine lookbacks and D1's 0.75% stop. None of them has out-of-sample evidence to tune against, the one walk-forward reselection test found destroyed value, and every change adds a deflated-Sharpe trial. Reading the C++ source confirmed the three bugs the researchers flagged: the crypto stop ratchets before the exit test, the D1 signal spans contract rolls, and the blackout counts calendar days. It also found four more problems that matter as much. The A1 backtest runs at 2.5× smaller risk units than the paper runner. The latched sleeve stop runs inside the replication backtests. A2 cannot run the paper's "any gap" baseline at all. The gate report hard-codes nine trials. The full programme costs **27 committed trials and at most 50**. The honest expectation is that B is the only sleeve likely to pass its gates. A2 probably cannot collect enough events. D1 almost certainly cannot clear 2× its cost unless India's intraday slope is 2.5–3× the global estimate.

## Frozen published parameters beat re-optimisation, so most U thresholds stay

The research covered five areas: A1, A2, B, D1, and the regime gates with the risk limits. The common finding is negative but useful. Almost no U threshold in the rulebook has an out-of-sample, net-of-cost study behind it, in India or anywhere else. The exceptions are the published specifications themselves, and the papers were finally read in full: Zarattini, Pagani and Wilcox on stock trend following, Zarattini and Stamatoudis on price action reading, Zarattini, Barbon and Aziz on stocks-in-play, and Zarattini, Pagani and Barbon on crypto trends ([ZPW PDF](https://cdn.prod.website-files.com/6666e810f1ece690fd70bf82/688d233e0fe06c2b1af5e460_Does%20Trend%20Following%20Still%20Work%20on%20Stocks_ssrn-5084316_%20(2).pdf); [PAR PDF](https://concretumgroup.com/wp-content/uploads/2026/02/The-Power-Of-Price-Action-Reading.pdf); [ZBA PDF](https://concretumgroup.com/wp-content/uploads/2026/02/A-Profitable-Day-Trading-Strategy-For-The-U.S.-Equity-Market.pdf); [CCT PDF](https://concretumgroup.com/wp-content/uploads/2026/02/Catching-Crypto-Trends.pdf)). Those texts let the agent correct specifications, but they cannot justify new values. Only one of the four Concretum papers has any out-of-sample period: ZPW treats 2005–2024 as post-publication.

The strongest guide to method comes from a replication, not a paper. An independent rebuild of Zarattini's SPY noise-area strategy ran quarterly walk-forward reselection over 27 variants. It earned a **Sharpe ratio of 0.57 against 0.92 for the fixed published configuration**, switched 14 times in 19 quarters, and found that the in-sample best of the 27 was the paper's own configuration ([giovannibrusco replication](https://github.com/giovannibrusco/zarattini-2024-momentum-spy)). Tuning a threshold on the agent's own backtest is therefore not neutral. It spends deflated-Sharpe budget, and on the best available evidence it lowers live performance. The recommendations below follow one rule: change a value only when a full text shows the current value is wrong or truncates the known source of edge. Otherwise freeze it, and count every variant actually run.

## A1 should stop truncating the right tail and leave Kullamägi's mechanics alone

**The selection layer is the best-supported part of the agent.** In India, volatility-adjusted momentum has the highest Sharpe ratio among the momentum definitions tested (December 2008 to September 2024), and stocks near their 52-week highs earn "more stable alpha than academic momentum" with weaker reversals ([Raju SSRN 4977717](https://papers.ssrn.com/sol3/papers.cfm?abstract_id=4977717); [Raju SSRN 4587697](https://papers.ssrn.com/sol3/papers.cfm?abstract_id=4587697)). Both of those are abstracts only. In US data the 52-week-high measure "dominates and improves upon" past returns and does not reverse, earning 1.23% a month outside Januaries (t = 7.06) ([George & Hwang](https://www.bauer.uh.edu/tgeorge/papers/gh4-paper.pdf)). A1's composite score, its top 20% cut and its "within 15% of the pivot" rule all point the same way, so they stay.

The liquidity floor is also consistent with the literature. ZPW required a 42-day average dollar volume above $1 million, roughly ₹8.5–8.8 crore at ₹85–88 to the dollar (author's conversion). That is close to A1's ₹10 crore 20-day floor. ZPW charged **0.50% per round trip** and still found a profit, so A1's 0.5% base and 1.0% stress costs are not generous.

**The crash risk is real, so the regime gate stays.** The IIMA Indian momentum factor earned about 10.96% a year at 17.30% volatility. It suffered drawdowns of −62.2% in 2000 and −53.1% in 2008–09, and −28.2% from March 2020 that took until January 2022 to recover ([IIMA drawdown tables](https://faculty.iima.ac.in/~iffm/Indian-Fama-French-Momentum/drawdown.php)). Those dates match the US finding that momentum crashes in rebounds after panics ([Daniel & Moskowitz](https://www.nber.org/system/files/working_papers/w20439/w20439.pdf)).

**The exits are where A1 departs from the evidence.** ZPW traded more than 66,000 all-time-high breakouts with a ratcheting stop at the all-time high minus 10×ATR(42). The win rate was 43.9% and the average trade +0.50R, but winners lasted **370 days on average** and **fewer than 7% of trades generated all the profit**. The post-2005 out-of-sample edge fell to about 0.31R a trade ([ZPW PDF](https://cdn.prod.website-files.com/6666e810f1ece690fd70bf82/688d233e0fe06c2b1af5e460_Does%20Trend%20Following%20Still%20Work%20on%20Stocks_ssrn-5084316_%20(2).pdf)). A1 sells a third on day 3, culls any trade below +1R on day 20, and force-closes everything at 120 days. Each of those rules cuts into the tail that pays for trend trading.

No study supports either the day-3 partial or the day-20 time stop. They are practitioner rules. The recommended response is graded:

| Rule | Recommendation | Trial |
|---|---|---|
| `a1.max_hold_days` | Raise from 120 to 250. This is the one change with direct evidence | 1 |
| Runner exit | Test a ratcheting high-water − 10×ATR(42) trail on the final third, using ZPW's single published multiple rather than a search over k | 1 |
| Day-3 partial | Test with the partial switched off | 1 |
| Day-20 time stop | Test with the time stop switched off | 1 |

That is four committed trials.

**The initial stop should stay at 1.0×ADR.** Han, Zhou and Zhu found that 10%, 15% and 20% stops on US momentum all produced similar results: the 10% stop lifted the monthly Sharpe from 0.165 to 0.504 and cut the worst month from −49.79% to −15.37%. However, the gain came "mainly from the losers portfolio", which a long-only sleeve does not hold ([HZZ PDF](https://smallake.kr/wp-content/uploads/2017/03/SSRN-id2407199.pdf)). That insensitivity argues for leaving the stop alone. Testing 1.5×ADR needs a second key change as well, because the risk manager rejects any stop wider than `a1.max_stop_adr_mult` (1.0).

**The Indian liquidity evidence is weaker than the rulebook implied.** The unrefereed BacktestIndia study did not split momentum stocks by absolute liquidity. It split the top 30 momentum names in the NSE top 200 by **scaled turnover (daily volume ÷ market cap)**. The low-turnover half earned 19.43% net CAGR and the high-turnover half 8.51%, against 10.41% for the Nifty 50 ([BacktestIndia](https://backtestindia.com/blog/momentum-factor-india-liquidity-premium-scaled-turnover)). That fits US evidence that high-volume winners reverse faster ([Lee & Swaminathan](https://ideas.repec.org/a/bla/jfinan/v55y2000i5p2017-2069.html)). Two responses follow:

- Log scaled turnover on every trade.
- Optionally test excluding the top tercile, as one trial.

It is not a reason to raise `a1.min_avg_traded_value`.

## A2's full text corrects four rulebook errors and supplies a volume gate

Reading SSRN 4879527 in full changes the specification, not the outlook ([PAR PDF](https://concretumgroup.com/wp-content/uploads/2026/02/The-Power-Of-Price-Action-Reading.pdf)).

**The gap universe is only three filters:**

- an open at least 6% above the prior close;
- an opening price of at least $2;
- at least 200,000 shares traded pre-market.

That produced **9,794 events** from survivorship-free CRSP data for 2016–2023. There was no catalyst filter. The paper's own text is internally inconsistent on the average gap: about 25% in the text, 28% in the Figure 1 caption.

**The "4 Targets" rule is fully specified:** sell 25% at each of **2R, 4R, 8R and 10R**.

**Five of the six mechanical rules lose money; the sixth is marginal.** "Pos OR + Trailing + 4 Targets" turns "slightly profitable from the tenth day onwards", gross of any stated cost.

**The trader saw neither price levels nor volumes.** He approved 1,721 events (17.6%) on four factors:

1. gaps after neglect;
2. multi-week or multi-month range breakouts;
3. gaps early in the momentum cycle;
4. avoiding gaps that immediately follow a gap on the previous day.

**The headline results and costs differ from what the rulebook assumed.** The micromanaged portfolio risked **0.25% per trade** and charged only $0.01 a share. Over 1,580 trades it won 18% of the time, with an average win of +10.10R, an average loss of −1.02R and an average trade of **+1.03R**. That result covers up to 50 days of discretionary management, so a coded A2 should be anchored on the filtered stage's **+0.25R peak on day 12, before costs**.

**Several A2 parameters are confirmed as faithful.**

| Parameter | Status |
|---|---|
| `a2.min_gap` = 0.06 | Exactly the paper's value |
| `a2.approve_pct` = 0.80 | Faithful; the exact equivalent of 17.6% approval would be 0.824 |
| `a2.risk_per_trade` = 0.0025 | Exactly the trader's sizing |
| `a2.max_hold_days` = 30 | Matches the mechanical rules |

None of these should be tuned. Gaps of 5%, 8% and 10% should be reported as sensitivities and counted as three trials.

**Two A2 rules have no source in either paper.** Neither paper set an entry deadline, so the replication run should keep the buy-stop live for the whole gap day (`a2.window_end` = "15:15"), with 10:15 as a single variant. The micromanaged curve peaked on day 4 and the trader managed trades for up to 50 days. Information-backed price jumps also keep drifting for one to three months ([Jiang & Zhu](https://ideas.repec.org/a/eee/jfinec/v124y2017i1p43-64.html), abstract only). So a 50-day hold is a reasonable single variant.

**The volume evidence comes from the sister paper, and it is the best case for a hard gate.** ZBA define relative volume exactly as the agent does: first-5-minute volume divided by the mean of the prior 14 days' first-5-minute volume. Net of commissions, ORB trades with RV below 100% averaged −0.02R, trades above 100% averaged +0.08R, and trades above 30× averaged +0.38R. The filtered strategy's Sharpe was 2.81 against 0.48 for the base ([ZBA PDF](https://concretumgroup.com/wp-content/uploads/2026/02/A-Profitable-Day-Trading-Strategy-For-The-U.S.-Equity-Market.pdf)). A hard gate of RV ≥ 1.0 is the closest NSE analogue to the paper's 200,000-share pre-market filter. Because the trader never saw volume, the volume-ratio and log-RV terms should leave the "trader replica" score and be reported as a separate feature. ZBA's 10%-of-ATR stop belongs to a same-day strategy and must not replace A2's first-bar-low stop.

**The academic backdrop is unfavourable for liquid names.**

- For stocks above microcap size, post-earnings drift "has been non-existent since 2006" ([Martineau](https://cfr.ivo-welch.info/published/papers/martineau2021rest.pdf)).
- A 2025 replication found the price-based earnings-drift factor insignificant once microcaps are excluded, with t = 1.43 ([UCLA Anderson Review](https://anderson-review.ucla.edu/is-post-earnings-announcement-drift-a-thing-again/)).
- In 50 mega-liquid US stocks, a surprise-signed trade on the first post-announcement print has been insignificant since 2016 once spreads are paid ([Christensen, Timmermann & Veliyev](https://arxiv.org/pdf/2601.08962)).
- All of the earnings-momentum return accrues overnight ([Lou, Polk & Skouras](https://personal.lse.ac.uk/polk/research/TugOfWar.pdf)).

These findings support three things: keeping the catalyst requirement, holding overnight, and the 15:20 weakness exit, which cuts the intraday reversal leg. They also mean A2's edge, if it exists, must come from selection rather than from drift.

## Sleeve B's stop conflict resolves at −20% halve and −30% off

**Sleeve B's signal mechanics are fully confirmed.** The crypto paper confirms every signal rule in the agent: nine closing-price Donchian lookbacks, entry on a new n-day closing high, a mid-channel trailing stop, weights of min(0.25/σ90, 2.0) averaged across lookbacks, 10 bps costs and a 20% band for volatility-only rebalancing ([CCT PDF](https://concretumgroup.com/wp-content/uploads/2026/02/Catching-Crypto-Trends.pdf)).

**The stop timing is the one place the code departs from the paper.** The paper states it exactly: "the prevailing Trailing Stop used in day t+1 is the maximum between the Trailing Stop used in day t and the middle line of the Donchian channel computed at the end of day t." The code raises the stop to today's mid and then tests today's close against it. This is a bug, listed in the bug table below.

**The paper never states the daily close time.** It also does not say whether the 20% band is relative or absolute. An absolute band of 20 percentage points on weights of about 0–0.43 would almost never trigger, which could not be described as recovering about 100 bps a year at 50 bps costs. So the agent's relative band is the plausible reading and should stay pre-committed.

**The paper's own drawdown numbers are unreliable.** The research note checked the arithmetic:

- Every single-lookback row in Table 1 satisfies MAR ≈ CAGR/MDD.
- The Combo row does not: 30/19 = 1.58 against a printed MAR of 0.88, which implies an MDD of about **34%**.
- The net BTC and ETH rows in Table 3 imply MDDs of about **26% and 28%**, against the printed 19% and 15%.
- Sortino falls from 2.03 to 1.23 while Sharpe moves only from 1.58 to 1.56, which 10 bps of costs cannot explain.

The agent's `kill.b_backtest_max_dd = 0.19` rests on a figure the paper contradicts. It should be replaced by the agent's own replicated net Combo MDD for BTC plus ETH, 2015 to March 2025. There is no post-publication test of the ensemble, so the agent's own data from April 2025 to September 2026 will be the only genuine out-of-sample evidence.

**The conflict between the blueprint and the rulebook is real, and the code makes it worse.** The blueprint gives every sleeve a halve at −7.5% and an off at −15%, while the rulebook plans for 25–30% drawdowns in B. In `src/risk.cpp`, Off is latched until a human calls `manual_reset()`. `b_kill_rule` fires only above 1.5 × 0.19 = 28.5%. So the latched 15% stop always fires first, and the kill rule is effectively unreachable.

**The drawdown statistics favour the wider stop.**

- **Analytic bound.** Rej, Seager and Bouchaud show that the 5% extreme drawdown depth is about 1.50/SR in units of annual volatility, and that investors "tend to underestimate the length and depth of drawdowns consistent with the Sharpe ratio" ([arXiv 1707.01457](https://arxiv.org/abs/1707.01457)).
- **Simulated drawdowns.** A 2026 Monte Carlo extension, which is unreviewed but reproduces Rej's analytic constant, puts the three-year maximum drawdown of a trend strategy with Sharpe 1 at 1.24 vol (median) and 1.97 vol (90th percentile) ([Landolfi, arXiv 2608.00127](https://arxiv.org/abs/2608.00127)).
- **Applied to B.** At about 15% sleeve volatility and a haircut Sharpe of 0.8, a *healthy* B has a median three-year drawdown of about 19% and a 90th percentile of about 33% (author's arithmetic, U).
- **What the current stops do.** −7.5% is crossed in most years. −15% is roughly the median two-year drawdown, so the latch would switch off a working sleeve about half the time.
- **Stop-loss theory.** Kaminski and Lo show that stop-loss rules reduce expected return unless returns are serially correlated ([MIT DSpace](https://dspace.mit.edu/handle/1721.1/114876)). B already de-risks inside each trade, through Donchian stops that cap the worst trade at −5% to −11% and through volatility targeting.

**The resolution has five parts:**

1. Set `b.drawdown_halve` = 0.20, or replace it with a linear taper that scales exposure by clamp((0.30 − DD)/0.10, 0.5, 1). The taper is a Grossman–Zhou-style cushion rule ([Grossman & Zhou](https://ideas.repec.org/a/bla/mathfi/v3y1993i3p241-276.html)) and counts as one trial.
2. Set `b.drawdown_off` = 0.30, still latched to a human review with a written re-entry rule.
3. Set `kill.b_backtest_max_dd` to the replicated MDD, provisionally 0.20, so that 1.5× lands near the same 30%.
4. Mark the blueprint's −7.5%/−15% as not applicable to Sleeve B, and cite this evidence when doing so.
5. Keep B's dollar risk small through sizing. At 4% of total capital, B is 16% of the active book, so a 30% sleeve drawdown costs about 4.8% of the active book. That is inside the book-level −10% halve.

**The regime research disagrees in one respect.** That note would keep −7.5%/−15% for every sleeve as a coarse Grossman–Zhou discretisation. Its own caveat, that crypto volatility "makes a 15% floor bind much more often", is the reason the B-specific analysis wins here. Volatility scaling, not a tighter stop, is the right lever for dollar risk. Man Group found it adds about 40 Sharpe points to bitcoin regardless of the half-life used ([Man Group](https://www.man.com/insights/crypto-too-hot-to-handle)).

**Costs and funding need conservative inputs.**

- **Costs.** The paper's 10 bps assumed global venues where BTC fees are "generally below 5 bps". The rulebook's sourced Delta Exchange India schedule is 0.05% taker plus 18% GST, about 6 bps a side before spread ([Delta docs](https://docs.delta.exchange/)). The note's 25 bps base is therefore deliberately conservative and unsourced beyond the fee. The 10 bps case should be kept as the replication run and 50 bps as the stress run.
- **Funding.** Crypto carry averages above 10% a year and predicts crashes ([BIS WP 1087](https://www.bis.org/publ/work1087.htm)), although perp mispricing has fallen almost 80% since 2022 ([He et al.](https://arxiv.org/abs/2212.06888)). The code charges zero funding on days without data. A 10% default is the safer assumption.
- **Crowding gate.** `regime.funding_red_annual` stays at 0.30 and unoptimised. A study of seven liquidation cascades from 2022 to 2025 found "no variable is event-invariant" ([Garcia Seuma](https://arxiv.org/abs/2607.27070)), so tuning the gate on a handful of events would be overfitting.

## D1 needs an Indian slope 2.5–3× the global estimate

**The signal is the right one.** D1 uses Baltussen's rest-of-day signal, and the full texts confirm that choice. In pooled equity-index futures, the move from prior close to 30 minutes before the close predicts the last half-hour with β = 4.18 (t = 7.29) and an out-of-sample R² of +2.88%. The Gao first-half-hour form has an out-of-sample R² of **−1.71%** and becomes insignificant in a horse race. The rest-of-day timing strategy earned Sharpe 1.73 before costs ([Baltussen et al.](https://www3.nd.edu/~zda/intramom.pdf); [Gao et al.](https://assets.super.so/e46b77e7-ee08-445e-b43f-4ffd88ae0a0e/files/ee7dac49-530b-4950-b5d0-e0b5eee08f2e.pdf)).

**The edge is small and conditional.**

- **Size.** About 2.6–2.7 bps a day before costs.
- **Dealer gamma.** It vanishes when dealers are long gamma: β = 0.82 (t = 1.03), against 6.63 when they are short.
- **Volatility.** In Gao's data, R² is 0.6% on low-volatility days and 3.3% on high-volatility days.
- **Recent US data.** It has been flat since 2022. One vendor measured a slope of +0.006 (t = 0.6) over 1,085 SPX sessions ([FirmTape](https://dev.to/firmtape/intraday-momentum-is-dead-in-the-0dte-era-we-measured-it-on-1085-spx-sessions-43g0)). The noise-area variant lost money in 2025–26 ([giovannibrusco](https://github.com/giovannibrusco/zarattini-2024-momentum-spy)).
- **India.** The only Indian study is an unrefereed working paper whose full text could not be read ([ResearchGate](https://www.researchgate.net/publication/383567351_Hedging_Demand_and_Intraday_Momentum_within_the_Indian_Stock_Market)). India also lacks the leveraged-ETF rebalancing flow that is one of Baltussen's two channels, though that absence was not verified.

**The costs are now pinned down.** Futures STT rose to 0.05% on sales from 1 April 2026 ([ICICI Direct](https://www.icicidirect.com/ilearn/futures-and-options/articles/stt-changes-in-budget-2026-what-f-o-traders-should-know)). With Zerodha's schedule ([Zerodha](https://zerodha.com/charges)), one 65-unit lot costs about ₹966, or **14.9 points at Nifty 25,000**, of which STT is 84%. Adding 1–2 points of slippage a leg gives 17–19 points, so the current 19-point base is right only near 25,000. Because STT is proportional, the backtest should charge 0.0006 × F plus a slippage term in points. A fixed 19 points over-charges the 2017–2020 history by more than 2×, at roughly 19 bps when Nifty was near 10,000.

**The gate is the binding constraint.** For trades above the 70th percentile of |s|, the expected gross move is β × E[|s| given |s| is in the top 30%]. Take the global slope of about 0.04 and assume the close-to-15:00 move has a volatility of 0.7–1.2%. The expected move is then **11–20 points**, against a gate of 38 points (2 × 19). That is author's arithmetic from the research note, U. Even the US negative-gamma slope clears 38 points only at the 90th percentile in a high-volatility year.

**The pre-registered plan is short.** First, estimate β on 2017–2026 Nifty futures by year, with Newey-West t-statistics. If β̂ < 0.08, archive the sleeve without running variants. Otherwise allow at most five pre-registered variants:

1. the Gao form;
2. a 0.90 percentile or a cost-aware gate (β̂·|s|·F ≥ 1.5–2 × cost);
3. a realised-volatility filter;
4. an expiry-day skip;
5. an NSE open-interest gamma proxy.

Two more requirements apply. The 2022–2026 subperiod must be positive net of base costs on its own. The expiry-skip list must follow the actual weekday: Thursday until August 2025 and Tuesday from 1 September 2025 ([Business Standard](https://www.business-standard.com/amp/markets/news/nse-bids-adieu-to-thursday-expiry-as-dates-swap-come-into-effect-explained-125082800635_1.html)).

## Regime gates need a slower crash state and trading-day blackouts

**The 200-day trend filter stays at 200 and should not be tuned.** Faber's 10-month rule lifted compounded return from 9.32% to 10.18% over 1901–2012 with nearly identical arithmetic means. The gain came from avoiding drawdowns, not from extra return ([Faber](https://mebfaber.com/wp-content/uploads/2016/05/SSRN-id962461.pdf)). Once look-ahead bias is removed, moving-average timing is "in statistical terms … indistinguishable" from buy-and-hold ([Zakamulin](https://ideas.repec.org/a/bla/irvfin/v18y2018i2p317-327.html)). Siegel's ±1% band against whipsaw is a cheap optional trial.

**The crash gate needs redefining.** The code turns red only when the 63-day return (or the 42 days to three weeks ago) is at or below −15% *and* the last 21 days have rebounded at least +10%. Daniel and Moskowitz define the dangerous state ex ante, as a negative 24-month market return combined with high 126-day market variance. They also find that the rebound months *are* the crash months: the market rose 26% from March to May 2009 while the loser decile rose 163% ([Daniel & Moskowitz](https://www.nber.org/system/files/working_papers/w20439/w20439.pdf)). The −15%/+10% values have no source. The recommended rule is red when the 504-day Nifty return is below 0 and 126-day variance is above its trailing median, with the existing fast trigger kept as an OR condition for V-shaped events such as 2020. That counts as one trial, using the published lookbacks without a grid.

**A1 should scale continuously by volatility rather than halve in a step.** In a real-time test of 103 strategies, volatility management raised Sharpe in only 53, and in combinations it lowered Sharpe in 72 of 103. Momentum was the exception: **all 9 momentum strategies improved, 5 significantly**. The leverage needed reached 864% at the 99th percentile ([Cederburg et al.](https://www.lehigh.edu/~xuy219/research/COWY.pdf)). So the A1 multiplier should be m = min(1, σ_target/σ̂_126), capped at 1. If it is adopted, the binary 80th-percentile volatility gate becomes redundant for A1 and remains only as Sleeve C's stand-down. `regime.vol_red_pct` stays at 0.80.

**The event blackout is both a bug and a design gap.** The bug is that it counts calendar days. The design gap is that elections need a longer window than ±1 day:

- On 4 June 2024 Nifty fell 5.93%, and up to 8.52% intraday ([Moneylife](https://moneylife.in/article/nifty-sensex-crash-on-election-surprise-tuesday-closing-report/74316.html)).
- In 2009 the market hit the upper circuit at +17.74% on the first session after a Saturday result ([Angel One](https://www.angelone.in/news/market-updates/election-result-day-and-market-volatility)).
- After the 2004 results the market fell about 20% to the lower circuit ([Singh 2016](https://ideas.repec.org/a/pal/assmgt/v17y2016i5d10.1057_jam.2016.23.html)).
- RBI policy surprises move equity volatility on the announcement day itself ([RBI WP 03/2024](https://website.rbi.org.in/web/rbi/-/press-releases/rbi-working-paper-no.-03/2024-equity-markets-and-monetary-policy-surprises)).

The recommended windows are set a priori and count as one trial:

| Event | Before | After |
|---|---|---|
| Election results | 5 trading days | 3 trading days |
| Union Budget | 1 trading day | 1 trading day |
| RBI policy | 0 | 1 trading day |

Blackouts should block new entries only, never force exits. Sleeve B, a daily long/flat trend, should not use the calendar.

**The remaining risk limits stay, for different reasons.** The active-book breaker (−10%/−20%), the 3% heat cap and the per-trade risk fractions have no direct academic evidence, and tuning them adds trials. The breaker, however, is a reasonable step version of Grossman–Zhou's rule of investing in proportion to the cushion above a floor. Multipliers must combine multiplicatively and each must be ≤ 1: risk_per_trade × m_vol × m_dd × gate.

## Consolidated table of recommended changes

The status column uses three labels. **V-spec** means the value was checked against a published full text. **V-code** means it was checked by reading the agent's source. **U** means it has no out-of-sample, net-of-cost confirmation. "Trials" counts deflated-Sharpe/PBO trials; a figure in brackets is optional.

| # | Config key or code rule | Current | Proposed | Evidence source | Status | Trials |
|---|---|---|---|---|---|---|
| 1 | `a1.max_hold_days` | 120 | **250** | Winners average 370 days; <7% of trades make all profit ([ZPW](https://cdn.prod.website-files.com/6666e810f1ece690fd70bf82/688d233e0fe06c2b1af5e460_Does%20Trend%20Following%20Still%20Work%20on%20Stocks_ssrn-5084316_%20(2).pdf)) | U (US, one study) | 1 |
| 2 | New `a1.runner_trail`, `a1.runner_atr_n`, `a1.runner_atr_k` | SMA10/SMA20 trail | Keep SMA as primary; variant: high-water − 10×ATR(42) ratchet on the final third | ZPW | U | 1 [+2 for k = 3, 5] |
| 3 | New `a1.partial_frac` (0 = off) | 1/3 on day 3, no off switch | Keep 1/3 as primary; variant 0 | ZPW tail evidence; no source for day 3 | U | 1 [+2 for day 5 and 1/2] |
| 4 | `a1.time_stop_day` | 20 (exit below +1R) | Keep; variant 0 (off) | None found | U | 1 |
| 5 | New `a1.mom_skip_days` | 0 (12-month return without a skip) | Keep 0; variant 21 (12-1) | IIMA and George & Hwang use a skip | U | [1] |
| 6 | New scaled-turnover log and exclusion | None | Log volume/market cap per trade; variant excludes the top tercile | [BacktestIndia](https://backtestindia.com/blog/momentum-factor-india-liquidity-premium-scaled-turnover); Lee & Swaminathan | U (unrefereed) | [1] |
| 7 | `a1.stop_adr_mult` / `a1.max_stop_adr_mult` | 1.0 / 1.0 | Keep; a 1.5 test must raise both | [HZZ](https://smallake.kr/wp-content/uploads/2017/03/SSRN-id2407199.pdf) stop-level insensitivity | U | [1] |
| 8 | `a1.min_runup`, `a1.min_adr` | 0.30, 0.04 | Keep; 0.25/0.40 and 0.03/0.05 are already in the existing 9-point grid | None (practitioner) | U | 0 (in grid of 9) |
| 9 | `a1.min_price`, `a1.min_avg_traded_value`, `a1.top_fraction`, `a1.base_len`, `a1.max_tightness`, `a1.max_below_pivot`, `a1.entry_limit_frac` | 100, 1e8, 0.20, 20, 0.5, 0.15, 0.005 | Keep all | ZPW $1M ≈ ₹8.5–8.8 crore; 52-week-high evidence (Raju, George & Hwang) | U (consistent with evidence) | 0 |
| 10 | `a1.cost_round_trip` | 0.005 (stress 0.01) | Keep | ZPW 0.50%; STT 0.1% a side plus stamp duty | U | 0 |
| 11 | `a1.risk_per_trade` | 0.004 of E_A | Keep; lower to 0.003 only if the units-corrected bootstrap 95th-percentile 12-month drawdown exceeds 15% | Blueprint sizing rule | U | 0 (scaling) |
| 12 | `a1.drawdown_halve` / `a1.drawdown_off` | 0.075 / 0.15 | Keep, after fixing the risk-unit bug (#B5) | Grossman–Zhou discretisation | U | 0 |
| 13 | New `a1.vol_target`, `a1.vol_n`, `a1.vol_scale_cap` | Binary 0.5× vol gate | m = min(1, σ*/σ̂_126); σ* = long-run median; cap 1.0 | [Cederburg et al.](https://www.lehigh.edu/~xuy219/research/COWY.pdf) 9/9 momentum; Barroso & Santa-Clara | U (US) | 1 |
| 14 | `a2.min_gap` | 0.06 | Keep; report 0.05/0.08/0.10 as sensitivities | [PAR](https://concretumgroup.com/wp-content/uploads/2026/02/The-Power-Of-Price-Action-Reading.pdf) §3 | V-spec | 3 |
| 15 | `a2.window_end` | "10:15" | "15:15" for the replication run; 10:15 as a variant | Neither paper sets a deadline | V-spec (absence) | 1 [+2 for 11:15, 15:00] |
| 16 | `a2.approve_pct`, `a2.risk_per_trade`, `a2.max_hold_days` | 0.80, 0.0025, 30 | Keep | PAR: 17.6% approval, 0.25% risk, 30-day rules | V-spec | 0 |
| 17 | `a2.max_hold_days` variant | — | 50 | Trader managed up to 50 days; Jiang & Zhu drift lasts 1–3 months | U | 1 |
| 18 | `a2.min_history_events` | 30 | Keep; report the share of days blocked by warm-up | PAR shows an 18-month event drought in 2018–19 | U | 0 |
| 19 | `a2.max_entries_per_day` | 2 | Keep; rank by score and log clipped approvals | PAR events cluster in bursts | U | [1 for 3] |
| 20 | A2 partial variant (new `a2.partial_mode`) | +1R/+2R/+3R/trail (not implemented) | 25% each at **2R/4R/8R/10R**, remainder on the SMA10 trail; day-3 1/3 stays primary | PAR pp. 10–11 | V-spec | 1 |
| 21 | New `a2.skip_after_gap_day` | Absent | Reject if day t−1 gapped ≥ `a2.min_gap` | PAR trader factor 4 | V-spec (trait) | 1 |
| 22 | New `a2.min_rv` | log RV inside the score | Hard gate RV(first 5 min, 14-day base) ≥ 1.0 | [ZBA](https://concretumgroup.com/wp-content/uploads/2026/02/A-Profitable-Day-Trading-Strategy-For-The-U.S.-Equity-Market.pdf): −0.02R below, +0.08R above | V (US intraday) | 1 |
| 23 | A2 score composition | 8 variables including vol20/vol50 and log RV | Remove the two volume terms from the approval score; report them separately | PAR: trader never saw volume | V-spec | 1 |
| 24 | New `a2.weakness_time` ("" = off) | "15:20" (struct only) | Keep; run on and off | PAR p. 11; Lou-Polk-Skouras | U | 1 |
| 25 | New `a2.require_catalyst` | Catalyst mandatory, no switch | true by default; false for the "any gap" baseline | PAR has no catalyst filter; Savor; Jiang & Zhu | V-code | 0 (reproduction check) |
| 26 | Prior-12-month top-decile exclusion | Absent | Optional | Aboody et al. (snippet only) | U | [1] |
| 27 | Crypto stop timing (code) | stop = max(stop, Mid_t), then test Close_t | Test Close_t against TS_t = max(TS_{t−1}, Mid_{t−1}); then ratchet | [CCT](https://concretumgroup.com/wp-content/uploads/2026/02/Catching-Crypto-Trends.pdf) §4.1, footnote 3 | V-spec, V-code | 0 |
| 28 | `b.vol_target`, `b.asset_cap`, lookbacks | 0.25, 1.0, nine lookbacks | Keep | CCT; 1× cap never binds ([review](https://github.com/alfred1123/Quant_Strategies/pull/61)) | V-spec | 0 |
| 29 | `b.rebalance_band` | 0.20 relative | Keep relative; absolute only as a labelled sensitivity | CCT text ambiguous | U | [1] |
| 30 | `b.cost_bps` | 10 | **25** base; 10 for replication; 50 stress | CCT; Delta fee ≈ 6 bps a side including GST | U | 0 (assumption) |
| 31 | `b.funding_annual_default` | 0 | **0.10** | [BIS WP 1087](https://www.bis.org/publ/work1087.htm) | U | 0 |
| 32 | `b.drawdown_halve` | 0.075 | **0.20**, or taper clamp((0.30 − DD)/0.10, 0.5, 1) | [Landolfi](https://arxiv.org/abs/2608.00127); [Rej et al.](https://arxiv.org/abs/1707.01457) | U | 0 [+1 for the taper] |
| 33 | `b.drawdown_off` | 0.15, latched | **0.30**, latched to human review with a written re-entry rule | Same; [Kaminski & Lo](https://dspace.mit.edu/handle/1721.1/114876) | U | 0 |
| 34 | `kill.b_backtest_max_dd` | 0.19 | Replicated net Combo MDD; provisional **0.20** | CCT MAR implies 26–34% | U | 0 |
| 35 | `regime.funding_red_annual` | 0.30 | Keep; 0.20 only as a sensitivity | BIS; [He et al.](https://arxiv.org/abs/2212.06888) (−80% since 2022) | U | [1] |
| 36 | Crypto close time | 00:00 UTC | Keep; 12:00 UTC as a robustness check | CCT silent | U | [1] |
| 37 | `ta_backtest b` replication mode | Sleeve overlay active | Add `--no-overlay` (halve/off disabled) for the reproduce-first run | Code review | V-code | 0 |
| 38 | D1 signal | r_ROD (Baltussen) | Keep; Gao form only if β̂ ≥ 0.08 | [Baltussen](https://www3.nd.edu/~zda/intramom.pdf): OOS R² +2.88% vs −1.71% | V (global) | [1] |
| 39 | D1 roll handling (code) | 15:00 price ÷ prior bar close across contracts | Same-contract prior close, or spot-based s, on roll days | Code review | V-code | 0 |
| 40 | `d1.pct_threshold` | 0.70 | Keep; one variant: 0.90 or a cost-aware gate | Linear-model arithmetic; walk-forward harm | U | [1] |
| 41 | D1 volatility filter (new) | None | 20-day RV above its 250-day median | Gao: R² 0.6% vs 3.3% | U | [1] |
| 42 | D1 expiry skip | `data.d1_skip_days` file exists | Variant with weekday-correct expiries | Settlement VWAP 15:00–15:30 | U | [1] |
| 43 | D1 gamma (NGE) proxy | None | Speculative variant | Baltussen: 6.63 vs 0.82 | U | [1] |
| 44 | `d1.stop_frac`, D1 exit time | 0.0075, 15:28 | Keep | No paper uses a stop; settlement window | U | 0 |
| 45 | `d1.cost_points` → new `d1.cost_frac` and `d1.slippage_points` | 19 points fixed | 0.0006 × F + 3 points base; + 10 points stress | [Zerodha](https://zerodha.com/charges); STT 0.05% | V (charges), U (slippage) | 0 |
| 46 | `d1.min_sleeve_equity` | 1.5e6 | **1.75e6** (≥ 65 × F) | Notional arithmetic | V (arithmetic) | 0 |
| 47 | D1 gate (`run_d1_backtest`) | Mean ≥ 2× fixed cost, t > 3, ≥ 250 trades | Add β̂ by year with Newey-West t; archive if β̂ < 0.08; require 2022–26 net > 0; proportional cost | FirmTape; replication evidence | U | 1 (baseline) |
| 48 | `regime.trend_sma` | 200 | Keep | [Faber](https://mebfaber.com/wp-content/uploads/2016/05/SSRN-id962461.pdf); Zakamulin | U | 0 |
| 49 | New `regime.trend_band` | 0 (flip on any cross) | Keep 0; variant 0.01 (Siegel) | Faber reporting Siegel | U | [1] |
| 50 | Crash gate: new `regime.crash_bear_lookback`, `regime.crash_vol_n`, `regime.crash_vol_pct`; expose `regime.crash_fall`, `regime.crash_rebound` | 63d ≤ −15% AND 21d ≥ +10% | Red if 504d return < 0 AND 126d variance > median, OR the old fast trigger | [Daniel & Moskowitz](https://www.nber.org/system/files/working_papers/w20439/w20439.pdf) | U | 1 |
| 51 | `regime.vol_red_pct`; expose `regime.vol_lookback`, `regime.realized_vol_n` | 0.80, 250, 30 (the last two are struct fields only) | Keep values; add keys | Cederburg (no tuning evidence) | U | 0 |
| 52 | Event blackout units (code) | Calendar days, tags ignored | Trading sessions of the index calendar | Code review; 2009 Saturday result | V-code | 0 |
| 53 | Per-event windows (new `regime.event_days_before.<tag>` / `regime.event_days_after.<tag>`) | ±1 for every event | Election 5/3, Budget 1/1, RBI 0/1, no crypto blackout | Angel One; Singh; RBI WP 03/2024 | U | 1 |
| 54 | Active-book breaker, heat cap | 0.10/0.20, 3% of E_A | Keep | Grossman–Zhou | U | 0 |
| 55 | Continuous drawdown multiplier | Step rules | Optional m_dd = clamp((floor − dd)/floor, 0, 1) | Grossman–Zhou | U | [1] |
| 56 | Re-entry after Off | Manual reset only | Optional: re-enable when the paper track regains its high-water mark | Kaminski & Lo (mechanism only) | U | [1] |
| 57 | Gate-1 trial count (code) | Hard-coded 9 | Read N and the PBO matrix from an append-only trial log | README rule; [Bailey & López de Prado](https://papers.ssrn.com/abstract=2460551) | V-code | 0 |
| 58 | Backtest vs paper risk units (code) | Backtest 0.4% of sleeve; paper 1.0% of sleeve | `ta_backtest` converts active-book units exactly as the paper runner does | Code review | V-code | 0 |

The account-file changes below are the immediate edits. Keys marked "new" also need code in `src/settings.cpp`, in both `apply_settings` and `default_settings()`, because `check_settings_keys` rejects unknown keys.

```ini
# A1
a1.max_hold_days = 250
a1.runner_trail = sma            # new; "atr" is the variant
a1.runner_atr_n = 42             # new
a1.runner_atr_k = 10             # new
a1.partial_frac = 0.333333       # new; 0 disables (variant)
a1.mom_skip_days = 0             # new; 21 is the 12-1 variant
a1.vol_target = <median of sigma_126 over the backtest>   # new
a1.vol_n = 126                   # new
a1.vol_scale_cap = 1.0           # new
# A2
a2.window_end = 15:15            # replication run; 10:15 is the variant
a2.require_catalyst = 1          # new; 0 for the any-gap baseline
a2.skip_after_gap_day = 1        # new
a2.min_rv = 1.0                  # new
a2.partial_mode = day3           # new; "targets" = 25% at 2R/4R/8R/10R
a2.weakness_time = 15:20         # new key for an existing field; empty disables
# B
b.cost_bps = 25
b.funding_annual_default = 0.10
b.drawdown_halve = 0.20
b.drawdown_off = 0.30
kill.b_backtest_max_dd = 0.20    # replace with the replicated net Combo MDD
# D1
d1.cost_frac = 0.0006            # new; replaces fixed d1.cost_points
d1.slippage_points = 3           # new; stress 10
d1.min_sleeve_equity = 1750000
# Regime
regime.crash_bear_lookback = 504 # new
regime.crash_vol_n = 126         # new
regime.crash_vol_pct = 0.5       # new
regime.event_day_unit = trading  # new
regime.event_days_before.election = 5
regime.event_days_after.election = 3
regime.event_days_before.rbi = 0
```

## Code bugs, each verified against the source

Every item below was checked by reading the files in `/home/user/Agent/trading-agent/src` and `include/ta`. Items 1–5 were raised by the researchers. Items 6–12 were found during this verification.

| # | Raised by | Location | What the code does | Consequence | Verdict | Fix |
|---|---|---|---|---|---|---|
| 1 | B note | `src/crypto_trend.cpp:56–57` (`update_signals`) | `sig.stop = max(sig.stop, mid)` runs before `if (s[i].close <= sig.stop)`, so today's mid enters today's exit test | Exits a day early whenever an old low drops out of the window and lifts Mid_t above Close_t; trade counts cannot be matched against the paper's Table 2 (292, 156, 78, 49, 28, 20, 15, 9, 5) | **Confirmed** | Test the close against the stop carried from yesterday, then ratchet with today's mid for tomorrow |
| 2 | D1 note | `src/d1.cpp:19–20` (`d1_signals`) | s = 15:00 price ÷ last bar close of the previous day in one continuous series; `Bar` has no contract field (`include/ta/bar.hpp`) | On the first session after a roll, s includes about a month of futures carry (roughly 0.3–0.6% of price, note author's estimate), about the size of the 70th-percentile threshold | **Confirmed** for an unadjusted continuous file (the README specifies "continuous near-month"); a back-adjusted file would avoid it, but nothing checks | Carry a contract id per bar and use the same contract's prior close, or compute s from spot |
| 3 | Regime note | `src/regime.cpp:101–107` (`EventCalendar::blackout`) | Compares `days_from_iso` differences, so windows are calendar days and tags are ignored | A Saturday result with after = 1 covers only Sunday and leaves Monday open (the 2009 case); a Sunday Budget leaves the Friday before unblocked | **Confirmed** | Count sessions on the index calendar; allow per-tag windows |
| 4 | B note | `src/risk.cpp:19` (latch), `src/monitor.cpp:156–158`, `src/paper.cpp:337, 414` | B's sleeve stop goes Off at 15% and stays off until `manual_reset()`; `b_kill_rule` needs drawdown > 1.5 × 0.19 = 28.5% | The kill rule can fire only if one day takes B from under 15% to over 28.5%, so it is effectively dead code | **Confirmed** | Rows 32–34 of the table above |
| 5 | B note | `include/ta/crypto_trend.hpp:27` | `funding_annual_default = 0` | Days without funding data cost nothing, which understates the drag of a long-only perp book | **Confirmed** (a conservative-default issue rather than a logic error) | Default 0.10 |
| 6 | D1 note (partly wrong) | `include/ta/d1.hpp`; `src/paper.cpp:241–242`; `src/main.cpp` `--skip-days` | The note said `skip_days` is "struct only"; a file loader exists (`data.d1_skip_days`, `--skip-days`). `signal_time`, `exit_time`, `lookback_days` and `min_history` really have no keys | Expiry-skip tests need only a correct date file | **Partly confirmed** | Add keys only for times that will be varied |
| 7 | Verification | `src/main.cpp` `run_b`; `src/crypto_trend.cpp:188` | `ta_backtest b` builds the engine with default `b.drawdown_halve/off` (0.075/0.15), and Off flattens and latches for the rest of the run | The reproduce-first replication halves at 7.5% and stops at 15%, so neither trade counts nor the "replicated MDD" for the kill rule can be measured | **Confirmed** | Replication mode with the overlay disabled |
| 8 | Verification | `src/main.cpp` `run_a1` vs `src/paper.cpp:189–191` | The backtest uses `a1.risk_per_trade` = 0.004 and `max_position_frac` = 0.05 of *sleeve* equity. The paper runner converts the same keys from active-book units: 0.004 × E_A / A1 capital = 1.0% of the sleeve, and 12.5% per stock | The backtest validates a strategy taking 2.5× less risk than the one paper-traded. The −7.5%/−15% stops sit at 18.75R/37.5R in the backtest but 7.5R/15R in paper | **Confirmed** | Make `ta_backtest` apply the same conversion, or label backtest keys as sleeve units |
| 9 | Verification | `src/a2.cpp:233–239` | Any symbol without a catalyst timestamp is skipped; there is no switch | The paper's catalyst-free baseline (the reproduce-the-loss check the rulebook requires) cannot be run | **Confirmed** | `a2.require_catalyst` |
| 10 | Verification | `src/main.cpp` `--gate` block | `evaluate_gate1(..., trial_sr.size() = 9, ...)`; PBO uses only the 9 grid curves | Deflated Sharpe and PBO ignore every other variant run, contrary to the README's own rule | **Confirmed** | Trial log feeding N and the PBO matrix |
| 11 | Verification | `src/swing.cpp:119–127`; `src/a1.cpp:21–23` | The partial fires when `days_held == partial_day`; entry precedes `manage_at_close`, so days_held is 0 on the entry day | Setting `a1.partial_day = 0` does not disable the partial; it moves it to the entry-day close | **Confirmed** | `a1.partial_frac` key (0 = off) |
| 12 | Verification | `src/risk.cpp` `size_units`; `include/ta/risk.hpp` | Rejects any stop wider than `max_stop_adr_mult` × ADR (default 1.0) | A 1.5×ADR stop test with only `a1.stop_adr_mult` changed rejects every trade as `stop_too_wide` | **Confirmed** (config coupling) | Change both keys together |

Item 8 matters beyond bookkeeping. The research note's reassurance that ten consecutive 0.40% losses (4% of the active book) sit "inside the sleeve's −7.5% halve threshold" is wrong in the units the paper runner uses: 4% of the active book is 10% of A1's capital.

A simple simulation, the author's own and U, shows the size of the problem. It assumes independent trades, a 40% win rate, exponentially distributed winners and an edge of +0.1R. In paper units, A1's latched −15% stop then fires in about **41% of 100-trade paths and 73% of 200-trade paths**. At backtest units it fires in only 0.5% and 4%. The drawdown conflict documented for B therefore exists for A1 too. The fix should be sizing, either a lower `a1.risk_per_trade` or the volatility multiplier, and it should be measured on the units-corrected backtest.

## The trial budget is 27 committed and 50 at most

Each sleeve's deflated Sharpe and PBO must count every variant actually run on it, including regime variants evaluated on that sleeve's backtest. Sensitivities that are "only reported" still count, because the reader sees them and can select among them. The budget below follows from the table above.

| Sleeve | Committed trials | Optional | Ceiling | What the committed set contains |
|---|---|---|---|---|
| A1 | 13 | 7 | 20 | Existing 9-point grid (min_runup × min_adr), max hold 250, ATR(42) k = 10 runner, partial off, time stop off |
| Regime/risk (scored on A1, and on A2 if run there) | 3 | 3 | 6 | Crash-gate redefinition, A1 volatility scaling, per-event trading-day windows |
| A2 | 10 | 4 | 14 | Three gap sensitivities, full-day window, 50-day hold, 2R/4R/8R/10R targets, consecutive-gap rule, RV gate, volume terms out of score, weakness exit on/off |
| B | 0 | 4 | 4 | Pre-committed replication; optional absolute band, 12:00 UTC close, 0.20 funding gate, drawdown taper |
| D1 | 1 | 5 | 6 | β estimate; the five variants run only if β̂ ≥ 0.08 |
| **Total** | **27** | **23** | **50** | |

For A1 alone, the deflated Sharpe should use **N = 16 committed, rising to 26** at the ceiling. That is well above the 9 the code now reports. Bailey et al. showed that after only seven configurations a two-year backtest Sharpe above 1 is expected even when the true Sharpe is zero ([Bailey et al.](https://carmamaths.org/jon/backtest.pdf)). The a1 note's own estimate of "10–13 additional trials on top of the 9-trial grid" double-counted the 0.03/0.05 ADR tests, which are already inside the grid. Bug fixes, cost assumptions and drawdown levels fixed in advance add no trials, provided they are not chosen by backtest result.

## Corrections to the earlier rulebook

The earlier rulebook could not read the price action PDF and relied on secondary sources. The full texts now support these corrections.

| Rulebook statement | Correction | Source |
|---|---|---|
| "The PDF itself could not be read"; cost and survivorship unconfirmed | Read in full. Survivorship-free CRSP data; unadjusted 1-minute IQFeed/Polygon bars; $0.01 a share charged only on the micromanaged curve; mechanical curves carry no stated cost | [PAR](https://concretumgroup.com/wp-content/uploads/2026/02/The-Power-Of-Price-Action-Reading.pdf) |
| "4 Targets" levels unknown; variant at +1R/+2R/+3R/trail | **25% each at 2R, 4R, 8R and 10R**; replacing the variant is a specification fix | PAR pp. 10–11 |
| Three trader preferences | **Four factors**; the fourth is "avoiding gaps following consecutive gaps", which is not coded (the "≤ 1 prior gap in 250 days" proxy is looser) | PAR p. 16 |
| Chart review removed ticker, date, price scale, news | It also removed **volumes**. The trader never saw volume, so the vol20/vol50 and RV terms are ZBA-derived additions, not replicas of his judgement | PAR pp. 14–16 |
| Mechanical rules "lost money in every variant tested" | True for five of six. "Pos OR + Trailing + 4 Targets" is slightly profitable from day 10, gross and marginal | PAR pp. 11–22 |
| Sizing convention of 1% risk, 4× cap, $0.0005/share assumed from house style | **0.25% risk per trade**, $0.01 a share; 1,580 trades, 18% win rate, +1.03R average | PAR Tables 1–2 |
| Average gap about 28% | The text says about 25%; the Figure 1 caption says 28%. The paper is internally inconsistent | PAR §3 |
| ATR length for the 1-ATR stop unknown | "Typically 14 days" (footnote); not stated explicitly for the test | PAR p. 10 |
| ZBA relative-volume edge (+0.08R) from a secondary source, U | Verified: RV < 100% gives −0.02R, > 100% gives +0.08R, > 30× gives +0.38R; the 14-day first-5-minute baseline definition matches the agent | [ZBA](https://concretumgroup.com/wp-content/uploads/2026/02/A-Profitable-Day-Trading-Strategy-For-The-U.S.-Equity-Market.pdf) |
| Crypto ensemble "BTC net Sharpe about 1.56 with a 19% drawdown" | The 19% conflicts with the paper's own MAR, which implies about 26% net and about 34% for the gross Combo. The kill rule should not rest on 0.19 | [CCT](https://concretumgroup.com/wp-content/uploads/2026/02/Catching-Crypto-Trends.pdf) |
| Stop "ratcheted as max(prior stop, Mid_n); exit when Close ≤ stop" | The order matters: day t's exit uses the stop set from Mid_{t−1} | CCT §4.1, footnote 3 |
| Rotation removal rule "below 0.5% median absolute daily move" | The paper does not say "absolute" | CCT §7.2 |
| "The liquid half of a momentum portfolio earned only 8.51%" | The split was by scaled turnover (volume ÷ market cap) inside the NSE top 200, not by absolute liquidity | [BacktestIndia](https://backtestindia.com/blog/momentum-factor-india-liquidity-premium-scaled-turnover) |
| Event blackout of ±1 day | Must be in trading sessions, and longer for election results | Bug 3; Angel One; Singh |
| Blueprint sleeve stop of −7.5%/−15% applies to B | Not applicable to B; replaced by −20%/−30% with a replicated kill threshold | Section on Sleeve B |

## What remains unverified, and which sleeves will likely fail

**Nearly every recommendation is still U.** It becomes verified only when the agent's own walk-forward, net-of-cost backtest on Indian and crypto data confirms it.

- **Not out-of-sample or not Indian.** The 250-day A1 hold rests on one US study. The ATR runner multiple was never sensitivity-tested in the text that could be read. The A2 volume gate rests on a US intraday strategy with no holdout. The B drawdown levels rest on a Monte Carlo preprint plus an assumed Sharpe of 0.8 and an assumed BTC–ETH strategy correlation of 0.6.
- **Abstract-only sources.** Raju's four Indian momentum papers, Kaminski and Lo, Barroso and Santa-Clara, the BIS carry regressions, the Indian intraday-momentum working paper and the Indian PEAD studies were seen as abstracts or summaries only.
- **Assumed inputs.** No Indian figure was found for INR-perp spreads, buy-stop slippage on NSE breakouts, or Nifty-future spreads at 15:00. Those cost inputs remain assumptions.
- **Possible revision.** The October 2025 SSRN revision of the crypto paper could not be compared with the April 2025 PDF that was read.

**The gates should be read with equal honesty.**

| Sleeve | Likely outcome |
|---|---|
| **D1** | Very likely to fail. Indian costs are about 6× the US half-spread and 2.5–3× the unconditional edge. At the global slope the 70th-percentile trade earns about 11–20 points gross against a 38-point gate. The US effect has been flat since 2022. The β < 0.08 archive rule should be expected to trigger. |
| **A2** | Unlikely to pass. Its own rulebook gate needs ≥ 300 out-of-sample events and an approved-minus-rejected uplift of ≥ 0.15R with t ≥ 2. The coded proxies imitate a trader who saw neither volume nor price levels. The paper's best filtered stage peaked at only +0.25R before costs, and costs take 0.1–0.27R. Large-cap drift has been absent since 2006. |
| **A1** | Plausible but unproven. It may well clear zero after costs, but its final test is beating a Nifty200 Momentum 30 index fund after tax, and the liquid-momentum evidence points the wrong way. |
| **B** | The only sleeve with a fully specified, cost-robust published rule. Its acceptance gate (Sharpe ≥ 0.8 net of funding, ≥ 0.5 in 2022–2026, drawdown ≤ 30%) is reachable. It can only be tested after bugs 1 and 7 are fixed and the replication matches the paper's trade counts within 5%. |

## Conclusion

The research changes the order of work more than the parameters. The highest-value next step is not a new threshold. It is to make the agent measure correctly:

- B's stop timing must match the paper;
- the replication must run without the latched overlay;
- A1's backtest must use the risk units the paper runner trades;
- A2 must be able to run the catalyst-free baseline;
- D1 must stop reading roll-day carry as momentum;
- the gate must count every trial.

Until those fixes land, any backtest number the agent produces validates a different strategy from the one it will paper-trade, so tuning on it would compound the error.

The drawdown question turned out to be broader than Sleeve B. Expressed in R, the blueprint's uniform −7.5%/−15% sleeve stop is a tight leash for any low-win-rate trend system, and A1 in paper units is exposed almost as much as B. The durable design principle from this round is to size for the drawdowns a healthy strategy is expected to produce, through volatility targets, risk per trade and capital share, and to put the latched stop beyond them. A stop that fires on ordinary variance tells the agent nothing about whether the edge has gone. One that sits at the 90th percentile of a healthy strategy's drawdown is evidence that it has.
