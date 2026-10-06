# Real-data results (run 5 October 2026)

All numbers below come from real market data fetched with the scripts in `scripts/`
(see README "Data"). Nothing here uses synthetic data. Backtests are pre-tax except Sleeve B.

## Data

| Source | Coverage | Notes |
|---|---|---|
| NSE cash-market bhavcopy | 2016-01-01 to 2026-10-05, 2,665 sessions, 3,992 symbols | 829 split/bonus/consolidation factors (753 from NSE's API, 76 inferred clean-ratio gaps), 30 demerger/capital-return gaps adjusted; 290 ETFs excluded |
| Nifty 50, India VIX (Yahoo) | 2007-09 to 2026-10-01 | regime gates |
| Nifty 50 5-minute (Yahoo) | last 58 sessions | the only free intraday history |
| BTC, ETH daily (Binance spot) | 2017-08 to 2026-10-04 | research history; BTC-USD from Yahoo since 2014 for the paper-window replication |
| Delta Exchange India funding | 2023-12 to 2026-10-05 | 8-hour rates; 10%/yr assumed before |

## Sleeve B (crypto trend) — replicates; fails the after-tax hurdle

| Run | CAGR | Sharpe | Max DD |
|---|---|---|---|
| Paper window (BTC, 2015-01 to 2025-03), paper setup | 29.9% | 1.62 | 19.4% |
| *Catching Crypto Trends* as published | 30% | 1.56 | 19% |
| BTC+ETH 2017-08 to 2026-10, India setup (25 bps, Delta funding, 31.2% tax, −20/−30% stops) | 4.8% | 0.44 | 22.4% |
| Same at Delta's real fees (8 bps) | 5.5% | 0.49 | 21.5% |
| Same without VDA tax (if perps get business-income treatment) | 13.1% | 1.06 | 16.9% |

Entries per lookback on the paper window (306, 161, 79, 50, 29, 20, 15, 9, 5) are within 5% of the
paper's (292, 156, 78, 49, 28, 20, 15, 9, 5): the reproduce-first check passes. Under the worst-case
tax the sleeve earns less than the 91-day T-bill (~5.3%), so it fails its gate; the tax treatment of
INR-settled perpetuals decides whether it is worth running.

## Sleeve A1 (NSE momentum breakouts) — fails gate 1

2017-01 to 2026-10, risk 0.40% of the active book (1% of the sleeve), 0.5% round-trip cost:

- With the rulebook's −15% latch: 111 trades (Jan–Aug 2017), −0.30R/trade, latched off August 2017
  at −17%, where it stayed (the risk controls worked).
- Latch disabled (research): 1,939 trades, 29.7% win rate, **−0.22R/trade**, t = −5.8; every year
  negative except 2020 (+0.03R). Excluding pessimistic entry-day stop-outs still −0.01R.
- Variants (ATR runner trail, partial off, time stop off, 12-1 momentum, 1.5×ADR stop) all lose;
  the best (ATR runner) has profit factor 0.98 and a 53% drawdown.
- Gate 1 over 15 logged trials: FAIL on expectancy, t, deflated Sharpe, drawdown and robustness.

Per the blueprint, a failed A1 is replaced by a Nifty200 Momentum 30 index fund.

## The screener (A1 momentum score) — weak ranking skill, no top-20 skill

Daily scores of every eligible stock, 2017-01 to 2026-10 (~2,400 days, ~1M score rows):

| Horizon | Mean rank IC | Overlap-adjusted t | Top-20 hit rate minus base rate |
|---|---|---|---|
| 5 days | 0.014 | 2.1 | −0.6 pts |
| 20 days | 0.035 | 2.6 | +0.1 pts |
| 60 days | 0.066 | 3.1 | +0.1 pts |

The score ranks the broad universe weakly at multi-week horizons (in line with the momentum
literature), but the top picks do no better than the average stock, and IC was near zero in
2024–2025. "Buy the top few" is not supported by this evidence.

## Sleeve D1 (Nifty last half hour) — no effect in the available data

On the 57 sessions of real 5-minute Nifty data available free, the Baltussen slope is 0.004 (t = 0.09),
far below the 0.08 archive line. Too short to prove anything, but nothing argues for keeping D1;
years of futures intraday data (a paid feed) are needed for a proper test.

## Intraday sleeves on the free 5-minute window (2026-07-16 to 2026-10-06, research runs)

Yahoo serves ~60 sessions of 5-minute bars, and its NSE opening-bar volumes are mostly zero, so
these runs relax rules that cannot be met on that data. They are not gate tests.

- **A2** (relative-volume gate off, approval warm-up off, every approved event taken): 24 results
  gaps with a filing catalyst; 3 filled, all losers (−1.13R, −0.50R, −0.28R), −₹9,399 on ₹10 lakh.
  Most approved events never filled (first 5-minute bar red: 11; not triggered: 2; too close to the
  price band: 1, TRENT on 6 October; stop wider than the risk cap: 4).
- **D1** (Nifty spot as the traded series, 20-day warm-up instead of 60): 11 trades, 55% winners,
  net −₹1,629 on one lot; mean move 14.7 points in the signal direction against 16.9 points of
  cost; slope 0.016 (t = 0.34). A 40-day warm-up shows 6 trades and a profit, but that is a subset of
  the same days chosen after the fact.

## Not testable with free data

A2 (results gaps) needs historical 5-minute stock bars and filing timestamps; D1 needs years of
Nifty futures intraday bars. Both require a paid feed (e.g. Kite Connect historical data).

## Live paper account

`paper/live` runs A1 and B on real data from 2026-01-01 (state, journal, trades and scores are
committed). At 2026-10-05: A1 −11.8% (halved by its kill rule), B −1.4%, journal verified. A1 takes no new
entries while Nifty is below its 200-DMA (at 2026-10-05: 22,556 vs 24,340, gate red). Its
"next session buy-stops" are printed by `ta_paper status paper/live`; given the results above they
are **not recommendations**.
