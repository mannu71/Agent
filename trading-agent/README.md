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
./build/ta_tests                    # 85 unit tests, no external test framework
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
| Nifty near-month future, continuous 1- or 5-minute bars | intraday OHLCV | D1 (traded prices) |
| Nifty spot index, 1- or 5-minute bars | intraday OHLCV | D1 signal (avoids roll-day carry) |

Real data (all fetchers tested against the live sources in October 2026; run from `trading-agent/`):

```sh
# NSE cash market, 2016 onwards (~2,660 sessions, ~670 MB raw). ALL_DAYS=1 also tries
# weekends so special sessions (Budget Saturdays, Muhurat trading) are not missed.
ALL_DAYS=1 scripts/fetch_nse_bhavcopy.sh 2016-01-01 2026-10-05 data/raw/bhav
./build/ta_ingest bhavcopy data/raw/bhav data/nse_unadj --non-eq-out data/t2t.csv
# Splits, bonuses and consolidations from NSE's corporate-actions API, checked against the
# unadjusted prices (contradicted records dropped, missing clean-ratio ones inferred):
scripts/fetch_nse_actions.py 2016 2026 data/corporate_actions.csv --check data/nse_unadj --infer
./build/ta_ingest bhavcopy data/raw/bhav data/nse --actions data/corporate_actions.csv --non-eq-out data/t2t.csv
scripts/fetch_yahoo.py ^NSEI data/index/nifty50.csv              # Nifty 50 daily since 2007
scripts/fetch_yahoo.py ^INDIAVIX data/index/india_vix.csv --value
scripts/fetch_yahoo.py ^NSEI data/index/nifty50_5m.csv --interval 5m   # last ~60 days only
scripts/fetch_binance_daily.py BTCUSDT data/crypto/BTC.csv        # research history, Aug 2017+
scripts/fetch_delta_funding.py BTCUSD 2023-12-01 data/funding/BTC.csv   # Delta India, Dec 2023+
# Intraday sleeves: Nifty 5-minute bars (merged, so history grows past Yahoo's 60 days), 5-minute
# bars and results filings of every A2 gap candidate, NSE F&O list and price bands:
scripts/fetch_intraday.py 2026-07-16 2026-10-05
```

`data/` is git-ignored. NSE's bhavcopy `PREVCLOSE` is **not** adjusted for splits, so
corporate actions must come from the API above. Demergers and capital returns are not in that
feed; the residual >30% overnight gaps in liquid stocks were reviewed and adjusted out via a manual
list (`data/corporate_actions_manual.csv`, concatenated with the API file). ETFs are excluded
from stock sleeves by symbol pattern (`data/etf_symbols.txt`). Not freely available: intraday
history for NSE stocks or Nifty futures (A2 and D1 need a paid feed such as Kite Connect), and
Delta India price history before its December 2023 launch. Crypto prices are in USD; the sleeve's
rupee figures apply USD returns to INR capital and ignore the exchange rate.

## Concept Desk app (live paper trading on your computer)

`app/` is a local trading app: live charts for crypto, US and Indian stocks, strategy signals on
the chart with the memory's verdict, one-click paper orders, positions, history and accounts.
Windows: double-click `app/start.bat`. See `app/README.md`.

## Institutional-concept engine

`ta_backtest setups data/crypto_1m --tf 60` runs the concept detectors (`include/ta/concepts.hpp`)
on 1-minute bars with taker-buy volume (`scripts/fetch_binance_klines.py BTCUSDT 2020-01 2026-09
data/crypto_1m/BTC.csv`), plays every setup out minute by minute (`include/ta/casebook.hpp`),
prints each pattern's edge against a matched random entry, then runs the memory-gated portfolio.
The concept library and results are in `knowledge/`.
`scripts/live_concepts.sh` keeps a live paper record of it (hourly; see `knowledge/concepts.md`), shown on the
Concept Desk dashboard (`dashboard/concept_desk.html`).

## Backtests

```sh
./build/ta_backtest a1 data/nse --index data/nifty.csv --vix data/vix.csv --events data/events.csv \
    --exclude data/t2t.csv --start 2018-01-01 --trial-log trials.log --gate
./build/ta_backtest a2 data/nse --intraday data/nse_5m --catalysts data/filings.csv --bands data/bands.csv
./build/ta_backtest b  data/crypto --replicate          # reproduce the paper first (10 bps, no overlay)
./build/ta_backtest b  data/crypto --funding-dir data/funding
./build/ta_backtest d1 data/nifty_fut_5m.csv --spot data/nifty_spot_5m.csv --slippage-points 10
./build/ta_backtest options --legs "P:22000:-1:85,P:21800:1:40,C:24000:-1:70,C:24200:1:30" \
    --expiry 2026-10-13 --today 2026-10-06 --active-book 2500000
```

`--trial-log FILE` appends every A1 configuration you run (deduplicated by a hash of its effective
settings), and `--gate` computes the deflated Sharpe and PBO over **all** logged trials, so variants
you tried and discarded still count. Without a log the gate falls back to its 9-point grid and warns.
`--gate` also reruns A1 at 2× costs and over a 9-point parameter grid, then prints the gate-1 report:
expectancy ≥ +0.15R at 1× and > 0 at 2× costs, t > 3, deflated Sharpe ≥ 0.95, PBO < 0.20, max
drawdown ≤ 25%, positive without the top 1% of trades, no year above 40% of P&L. `--config FILE`
overrides any limit (`key = value`; unknown keys are rejected so typos cannot silently fall back
to defaults). Risk keys are in active-book units in both tools: `ta_backtest` treats `--equity` as
the sleeve's capital and converts exactly as the paper runner does (A1 0.40% of E_A = 1.0% of the
sleeve with the default allocation), so the backtest tests the risk that is paper-traded.

## Paper trading

```sh
./build/ta_paper init acct --capital 10000000 --start 2026-10-06 \
    --set data.equity_dir=data/nse --set data.crypto_dir=data/crypto --set data.nifty_fut=data/fut.csv --set data.nifty_spot=data/spot.csv
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
* Two real-data accounts are committed: `paper/live` (A1 + B from 2026-01-01) and `paper/intraday`
  (A2 + D1 from 2026-07-16, the start of Yahoo's free 5-minute window; A2 on ₹10 lakh notional).
  `scripts/daily_paper.sh` updates all data and runs both each evening after the bhavcopy is out;
  `scripts/live_check.py paper/live` shows during the session which A1 buy-stops have triggered.
  Free intraday data has limits: D1 trades Nifty **spot** bars as a stand-in for the future (same
  cost model), and the price bands are today's lists applied to the whole history.
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
| A1 | top 20% composite momentum (6/12-month vol-adjusted return, close/52-week high, 63-day return; `a1.mom_skip_days = 21` is the 12-1 variant); run-up ≥ 30%, ADR ≥ 4%, close > SMA10 > SMA20, tight base; buy-stop at pivot + 1 tick next day, limit +0.5% | fill × (1 − ADR); 1/3 at day-3 close if in profit, stop to entry (`a1.partial_frac = 0` switches it off); trail SMA10 (ADR ≥ 5%) or SMA20, or `a1.runner_trail = atr` (highest close − 10 × ATR42); day-20 time stop below +1R; **250-day cap** | 0.40% of E_A at risk, ≤ 5% of E_A per stock, × min(1, median σ126 / current σ126 of the index) when an index is given, regime-gated |
| A2 | gap ≥ 6% with a filing between prior 15:30 and 09:15 (`a2.require_catalyst = 0` runs the paper's any-gap baseline); not right after a gap day; first-5-minute relative volume ≥ 1; F&O or 10/20%-band stocks; price-only score above the 80th percentile of the prior 250 days' events; green 09:15 bar, buy-stop at its high + 1 tick until 15:15 | first-bar low − 1 tick; below entry at 15:20 → exit at close (`a2.weakness_time` empty = off); 1/3 at day 3, or `a2.partial_mode = targets` (the paper's 25% at 2R/4R/8R/10R); trail SMA10; 30-day cap | 0.25% of E_A, ≤ 2 entries a day |
| B | new n-day closing high for n ∈ {5…360} | today's close tested against yesterday's mid-channel stop, then ratcheted | min(0.25/σ90, 2) per lookback, averaged, capped at 1× per asset; rebalance on signal or 20% drift; halve on crowding; 25 bps, 10%/yr funding when data is missing; sleeve halves at −20%, off at −30% |
| C | iron condors / flies / credit spreads only | max loss defined by wings | max loss ≤ 0.75% of E_A per expiry; no entry on expiry day, in blackouts or when the vol gate is red |
| D1 | \|15:00 move\| ≥ 70th percentile of the prior 250 days; signal from the spot index when given | 0.75% stop, exit 15:28 | one lot, needs ≥ ₹17.5 lakh; cost 0.06% of price + 3 points; switch-on also needs slope β ≥ 0.08 and 2022+ trades net positive |

Regime: trend gate (index > 200-DMA); crash gate red on the fast trigger (63-day ≤ −15% then 21-day
≥ +10%) **or** the Daniel & Moskowitz state (504-day return < 0 and 126-day variance above its
median); event blackouts count **trading sessions** with per-event windows (election 5 before / 3
after, Budget ±1, RBI 0/1; set `regime.event_days_before.<tag>`, tags matched by substring).

Parameter choices and their evidence: `../reports/SSRN parameter tuning for agent.md`.

## Approximations and known limitations

* **Daily bars are treated pessimistically.** A1's initial stop is the widest the rules allow
  (fill × (1 − ADR)) because the intraday low at entry is unknown; an entry-day low through the
  stop counts as a stop-out; stops gapped through fill at the open.
* A1's 09:15 auction-bar rule and the 3-per-sector cap need intraday and sector data and are not
  modelled. A2 uses the official open as a proxy for the pre-open equilibrium price.
* Results are pre-tax except Sleeve B, which deducts 31.2% of every realised gain with no loss
  offset and no fee deduction (worst case; INR-settled perps may get business-income treatment).
* B's sleeve stop is −20% halve / −30% off, not the blueprint's −7.5%/−15%: a healthy trend sleeve
  at ~15% volatility has a median 3-year drawdown near 19%. The same tension exists for A1 at
  paper-runner risk units (the tuning report's simulation has its −15% latch firing in 41–73% of
  healthy paths); resolve it with sizing measured on the units-corrected backtest.
* D1 is very likely to fail its gate: the published slope implies 11–20 points of gross move
  against a ~38-point bar. Run `ta_backtest d1 --spot ...` first and archive the sleeve if β < 0.08.
* A2 profit targets fill on daily bars from the day after entry, not intraday on the gap day.
* Every threshold marked U (unverified) in the rulebook is a trial: change it, and the deflated
  Sharpe and PBO numbers must account for it.
* Paper fills are simulated; slippage against a real order book is not measured until a broker
  adapter exists.
