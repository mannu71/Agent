# trading-agent

C++20 backtester and risk layer for the India/crypto trading agent described in
`../reports/Hedge fund style strategy blueprint.md` and
`../reports/Price action reading strategy rules.md`.

**No strategy here is risk-free or proven.** This code exists to test the rulebook's
claims on real data before any money is used (gates: backtest → walk-forward → paper → small live).

## What is built

| Part | File | Status |
|---|---|---|
| Deterministic risk gate (sizing, caps, cost-to-R filter, daily loss, drawdown halve/off latch) | `include/ta/risk.hpp` | done, tested |
| Sleeve A1 screener (universe filters, composite momentum score, breakout setup) | `include/ta/screener.hpp` | done, tested |
| Daily-bar backtester for A1 (entry, stops, partial, trail, time stop, costs, metrics) | `include/ta/backtest.hpp` | done, tested |
| Crypto trend ensemble (Sleeve B) | — | next |
| Broker/exchange connectivity, paper trading, live | — | later |

## Build and test

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/ta_tests
```

## Run

```sh
./build/ta_backtest <data_dir> [--equity 1000000] [--start 2019-01-01] [--end 2025-12-31] \
    [--cost-rt 0.005] [--exclude surveillance.txt] [--trades trades.csv] [--equity-out equity.csv]
```

`data_dir` holds one `<SYMBOL>.csv` per stock, split- and bonus-adjusted, with header
`date,open,high,low,close,volume` (ISO dates). Include delisted stocks — a universe of
today's survivors overstates returns. `--exclude` takes a file of symbols (one per line),
e.g. ASM/GSM/ESM surveillance names. Results are pre-tax.

## Rules implemented (Sleeve A1, from the rulebook)

- **Universe:** close ≥ ₹100, 20-day average traded value ≥ ₹10 crore, ≥ 1 year of history.
- **Score:** mean of clipped z-scores of 6-month and 12-month return ÷ annualised volatility,
  close ÷ 52-week high, and 63-day return. Only the top 20% are considered.
- **Setup:** max(21-day, 63-day) return ≥ 30%; ADR20 ≥ 4%; close > SMA10 > SMA20; 5-day range
  ≤ 0.5 × 20-day range; close within 15% below the 20-day high (the pivot).
- **Entry:** next day, buy-stop at pivot + 1 tick, limit 0.5% above; skipped if the open gaps past the limit.
- **Risk:** 0.40% of equity at risk, ≤ 5% of equity per stock, ≤ 12 positions, no leverage; skip if
  stop > 1 × ADR or round-trip cost > 0.25R. Half risk at −7.5% drawdown; off at −15% until `manual_reset()`.
  No new entries after a −1% day.
- **Exits:** sell 1/3 at the day-3 close if in profit and raise the stop to entry; exit at the next open
  after a close below SMA10 (ADR ≥ 5%) or SMA20; exit at the day-20 close if below +1R; 120-day cap.
- **Costs:** 0.25% per side by default (0.5% round trip); stress-test with `--cost-rt 0.01`.

## Daily-bar approximations (deliberately pessimistic)

- The rulebook's stop is the low of the day at entry, which daily bars cannot see. The initial stop is
  set at the maximum the rules allow: fill × (1 − ADR20).
- If the entry day's low reaches the stop, the trade is counted as stopped out that day, because the
  order of the high and low is unknown.
- A stop gapped through at the open fills at the open, not at the stop.
- The 09:15–09:20 auction-bar rule and the per-sector cap (3 per sector) need intraday and sector data
  and are not modelled yet.

On random-walk data the A1 rules lose money after costs and the drawdown latch switches the sleeve
off — the expected result when there is no edge, and a check that the kill switch works.
