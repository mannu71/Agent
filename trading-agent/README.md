# trading-agent

C++20 implementation of the trading agent designed in
`../reports/Hedge fund style strategy blueprint.md` and
`../reports/Price action reading strategy rules.md`.

**Nothing here is risk-free or proven.** The code exists to test the rulebook on real data and to
run a forward paper account before any money is used. Every strategy must pass the blueprint's
gates in order: backtest → walk-forward/holdout → paper trading → small live → scale. On the
synthetic random-walk data in the smoke test every sleeve loses, which is the correct result
when there is no edge.

## What is built

| Part | Header | Notes |
|---|---|---|
| Data: NSE bhavcopy ingest (legacy, UDiFF, sec_bhavdata_full), corporate actions, dated exclusion lists, intraday bars | `data.hpp` | `ta_ingest` |
| Deterministic risk gate: sizing, position/leverage/cash caps, cost-to-R filter, daily loss, drawdown halve/off latch | `risk.hpp` | no ML anywhere near it |
| Regime gates: trend (200-DMA), volatility percentile (India VIX or realised), momentum-crash state, crowding (funding + OI), event blackouts | `regime.hpp` | thresholds are unverified (U) |
| Portfolio: allocation, active-book breaker (halve −10%, stop −20%), A1+A2 shared 3% heat cap and 12-position limit | `portfolio.hpp` | |
| **A1** momentum-leader breakouts (NSE) | `a1.hpp`, `screener.hpp`, `swing.hpp` | rulebook: live at small size after gates |
| **A2** episodic-pivot gaps (NSE) with proxy scoring and approved-vs-rejected shadow study | `a2.hpp` | rulebook: paper only |
| **B** crypto Donchian ensemble (BTC/ETH), 1× cap, funding, worst-case VDA tax | `crypto_trend.hpp` | rulebook: live at small size after gates |
| **C** defined-risk options validator and sizer | `options.hpp` | validates structures; no backtest (no chain data) |
| **D1** Nifty last-half-hour momentum | `d1.hpp` | zero capital until it clears 2× costs |
| Monitoring: rank IC, precision@k vs base rate, CUSUM, PSI, per-sleeve kill rules | `monitor.hpp` | |
| Validation: t-stat, deflated Sharpe, PBO (CSCV), top-1% removal, year concentration, bootstrap drawdown, gate-1 report | `validate.hpp` | |
| Paper trading account with hash-chained journal | `paper.hpp` | `ta_paper` |

Not built: broker/exchange order routing (Kite Connect, Delta Exchange). This environment cannot
reach those APIs, so nothing could be tested against them; the paper account simulates fills
against bars instead. Gate 3 in the blueprint requires paper trading through the real broker API,
so that adapter is the next piece of work once it can be developed against the live endpoints.

## Build and test

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release   # needs OpenSSL (libssl-dev)
cmake --build build
./build/ta_tests                    # 70 unit tests, no external test framework
scripts/smoke_test.sh build         # end-to-end run of every tool on synthetic data
```

## Data

All inputs are plain CSV files; the agent never fetches anything itself.

| Input | Format | Used by |
|---|---|---|
| NSE daily, one file per symbol | `date,open,high,low,close,volume` (split/bonus adjusted, delisted names included) | A1, A2 |
| Index (e.g. Nifty 50), India VIX | same OHLCV / `date,value` | regime gates |
| Event calendar | `date,tag` (Budget, RBI policy, elections, FOMC, CPI) | blackouts |
| Exclusions | `SYMBOL` or `SYMBOL,YYYY-MM-DD` per line (ASM/GSM/ESM, trade-for-trade days) | A1, A2 |
| 5-minute bars, one file per symbol | `YYYY-MM-DD HH:MM,open,high,low,close,volume` (bar start time) | A2 |
| Catalysts | `SYMBOL,YYYY-MM-DD HH:MM` filing timestamps | A2 |
| Price bands | `SYMBOL,FO` or `SYMBOL,20` | A2 |
| Crypto daily, one file per asset, 00:00 UTC close | OHLCV | B |
| Funding / open interest per asset | `date,value` (day's total funding rate as a fraction) | B |
| Nifty near-month future, continuous 1- or 5-minute bars | intraday OHLCV | D1 |

Helpers (untested here because the network is blocked; check one request by hand first):

```sh
scripts/fetch_nse_bhavcopy.sh 2017-01-01 2026-10-01 raw/bhav     # then:
./build/ta_ingest bhavcopy raw/bhav data/nse --actions corporate_actions.csv --non-eq-out data/t2t.csv
scripts/fetch_delta_candles.py BTCUSD 2019-01-01 2026-10-01 data/crypto/BTC.csv
```

Bhavcopy includes every stock that traded each day, so the ingested universe is free of
survivorship bias. Corporate actions are `symbol,ex_date,factor` (1:1 bonus = 0.5). Symbol
renames are not merged automatically.

## Backtests

```sh
./build/ta_backtest a1 data/nse --index data/nifty.csv --vix data/vix.csv --events data/events.csv \
    --exclude data/t2t.csv --start 2018-01-01 --gate
./build/ta_backtest a2 data/nse --intraday data/nse_5m --catalysts data/filings.csv --bands data/bands.csv
./build/ta_backtest b  data/crypto --funding-dir data/funding --cost-bps 25
./build/ta_backtest d1 data/nifty_fut_5m.csv --cost-points 38
./build/ta_backtest options --legs "P:22000:-1:85,P:21800:1:40,C:24000:-1:70,C:24200:1:30" \
    --expiry 2026-10-13 --today 2026-10-06 --active-book 2500000
```

`--gate` reruns A1 at 2× costs and over a 9-point parameter grid, then prints the gate-1 report:
expectancy ≥ +0.15R at 1× and > 0 at 2× costs, t > 3, deflated Sharpe ≥ 0.95, PBO < 0.20, max
drawdown ≤ 25%, positive without the top 1% of trades, no year above 40% of P&L. `--config FILE`
overrides any limit (`key = value`; unknown keys are rejected so typos cannot silently fall back
to defaults). In `ta_backtest` the whole backtest equity is the sleeve, so risk fractions are of
that equity.

## Paper trading

```sh
./build/ta_paper init acct --capital 10000000 --start 2026-10-06 \
    --set data.equity_dir=data/nse --set data.crypto_dir=data/crypto --set data.nifty_fut=data/fut.csv
# after each day's data update (e.g. from cron at 18:30 IST):
./build/ta_paper run acct          # catch up, print positions and tomorrow's buy-stops
./build/ta_paper status acct
./build/ta_paper scorecard acct    # screener rank IC, precision@k, auto-disable rule
./build/ta_paper verify acct       # hash chain intact?
./build/ta_paper kill acct --reason "..."            # flatten everything, latch off
./build/ta_paper reset acct --sleeve a1 --confirm    # only after a human review
```

* The account directory holds `account.cfg` (all limits, with defaults written out), engine state,
  `journal.log`, `trades.csv`, `equity.csv` and `scores.csv`.
* In `account.cfg`, `a1.`/`a2.` risk fractions are **fractions of the active book** (rulebook
  units: 0.40% and 0.25% per trade, 5% per stock, 1% daily loss); the runner converts them to each
  sleeve's capital. Capital comes from the allocation: core 75%, buffer 5%, A1 10%, B 4%, C 3%,
  reserve 3%, A2 and D 0%.
* A2 and D1 run as paper-only shadows (A2 on the reserve's notional capital, D1 on ₹20 lakh
  notional) and do not count toward the active book.
* Kill rules are evaluated before every trading day, so a catch-up run gives exactly the same
  result as running every evening (tested): A1 halves on a negative 50-trade expectancy and stops
  on a negative 100-trade one; A2 stops on −0.1R over 40 events or no approved-vs-rejected uplift
  over 100; B stops above 1.5× its backtest drawdown (`kill.b_backtest_max_dd`); D1 stops on a
  negative 60-day expectancy. A manual reset makes rules count only trades closed afterwards.
* The journal is append-only and SHA-256 hash-chained: every signal, fill, reject, regime state,
  kill decision and run is recorded with a UTC timestamp before outcomes are known.
* State files are replaced atomically, so a crash never leaves a half-written state.

## Rules implemented

| Sleeve | Entry | Stop and exits | Size |
|---|---|---|---|
| A1 | top 20% composite momentum (6/12-month vol-adjusted return, close/52-week high, 63-day return); run-up ≥ 30%, ADR ≥ 4%, close > SMA10 > SMA20, tight base; buy-stop at pivot + 1 tick next day, limit +0.5% | fill × (1 − ADR); 1/3 at day-3 close if in profit, stop to entry; trail SMA10 (ADR ≥ 5%) or SMA20; day-20 time stop below +1R; 120-day cap | 0.40% of E_A at risk, ≤ 5% of E_A per stock, regime-gated |
| A2 | gap ≥ 6% with filing between prior 15:30 and 09:15; F&O or 10/20%-band stocks; score above 80th percentile of the prior 250 days' events; green 09:15 bar, buy-stop at its high + 1 tick until 10:15 | first-bar low − 1 tick; below entry at 15:20 → exit at close; 1/3 at day 3; trail SMA10; 30-day cap | 0.25% of E_A, ≤ 2 entries a day |
| B | new n-day closing high for n ∈ {5…360} | ratcheting mid-channel stop per lookback | min(0.25/σ90, 2) per lookback, averaged, capped at 1× per asset; rebalance on signal or 20% drift; halve on crowding |
| C | iron condors / flies / credit spreads only | max loss defined by wings | max loss ≤ 0.75% of E_A per expiry; no entry on expiry day, in blackouts or when the vol gate is red |
| D1 | \|15:00 move\| ≥ 70th percentile of the prior 250 days | 0.75% stop, exit 15:28 | one lot, needs ≥ ₹15 lakh |

## Approximations and known limitations

* **Daily bars are treated pessimistically.** A1's initial stop is the widest the rules allow
  (fill × (1 − ADR)) because the intraday low at entry is unknown; an entry-day low through the
  stop counts as a stop-out; stops gapped through fill at the open.
* A1's 09:15 auction-bar rule and the 3-per-sector cap need intraday and sector data and are not
  modelled. A2 uses the official open as a proxy for the pre-open equilibrium price.
* Results are pre-tax except Sleeve B, which deducts 31.2% of every realised gain with no loss
  offset and no fee deduction (worst case; INR-settled perps may get business-income treatment).
* The blueprint's sleeve stop (halve −7.5%, off −15%) also applies to B, although the rulebook
  plans for 25–30% drawdowns in B; with defaults B will often be switched off. This is a real
  conflict between the two documents and needs a decision (`b.drawdown_halve`, `b.drawdown_off`).
* Every threshold marked U (unverified) in the rulebook is a trial: change it, and the deflated
  Sharpe and PBO numbers must account for it.
* Paper fills are simulated; slippage against a real order book is not measured until a broker
  adapter exists.
