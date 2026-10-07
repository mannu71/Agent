# Concept memory: institutional / smart-money trading concepts

The agent's long-term memory of what each concept means, how it is coded, how strong the
outside evidence is, and what happened when it was tested on real data. Research behind it:
`reports/Institutional trading concepts.md` (repo root). The case memory (every setup seen and
its outcome) is built at run time by `ta_backtest setups` and the casebook (`src/casebook.cpp`).

Evidence grades: **A** peer-reviewed evidence of profit after costs · **B** independent
systematic backtests · **C** practitioner claims only. Status is per market and timeframe;
"fails" means it lost after costs or did not beat a matched random entry.

## How the memory decides

1. Every detector emits setups (entry, stop, target, context) at a bar close, using only
   completed bars; swing points are confirmed `smc.swing_n` bars late.
2. Every setup is played out on 1-minute bars (worst case when a minute touches stop and
   target; Delta India fees 0.05% taker / 0.02% maker, 0.02% slippage, funding 0.01%/8h).
   That outcome is a *case*, remembered whether or not the agent traded it.
3. **Recall rule:** a new setup is taken only if the memory holds at least 30 finished cases
   with the same pattern, timeframe, side and daily trend, their mean net R is at least
   +0.05R and the one-sided 95% lower bound is above zero. Cases still open at decision
   time are invisible (tested).
4. Every concept is compared with a **matched random entry**: same symbol, side, order type
   and entry / stop / target distances, at a random minute. A concept only matters if it
   beats that, not zero.

## Concepts as coded (parameters in `knowledge/concepts.cfg`)

| Pattern | Concept | Mechanical rule | Grade |
|---|---|---|---|
| `sweep` | ICT liquidity sweep / stop hunt, Turtle Soup | Bar wicks beyond an unswept swing high (low) of the last 50 bars and closes back inside → short (long) at next open; stop beyond the wick + 0.1 ATR; target 2R | B, null/negative |
| `sweep_absorb` | Sweep + order-flow absorption | `sweep` where the bar's taker delta points the way of the wick (aggressors trapped) | C |
| `sweep_cont` | Stop-run continuation (Osler 2005; G10 FX tests) | Same trigger, trade *with* the sweep; stop beyond the bar's other extreme; 2R | Mechanism A (stop clustering), strategy C |
| `fvg` | Fair value gap with displacement | Candle-3 low > candle-1 high (bull), gap ≥ 0.25 ATR, middle candle range ≥ 1 ATR → limit at the gap midpoint (CE), stop below candle 1 − 0.1 ATR, 2R, order lives 20 bars | B, null/negative |
| `ob` | Order block | On a close beyond the last swing high (BOS), last opposite candle since the prior swing low → limit at its proximal edge, stop beyond its far edge, 2R | B, null/negative |
| `choch` | Change of character | Trend down (LH+LL) and a close above the last swing high → long at next open, stop below the last swing low, 2R (mirror for shorts) | B, null/negative |
| `hl_pullback` | Higher low in an uptrend (lower high in a downtrend) | New swing low confirmed above the prior one while highs are rising → long at next open, stop below that low, 2R | C (trend itself is A: time-series momentum) |
| `spring` | Wyckoff spring / upthrust | 40-bar range ≤ 8 ATR; bar breaks the range low (high) on ≥ 1.5× average volume and closes back inside → trade back toward the opposite edge; stop beyond the wick | C |
| `va80` | Dalton's 80% rule | Day opens outside the prior UTC day's 70% value area, two consecutive closes back inside → target the far edge; stop beyond the day's extreme | B− (tests show ~60%, not 80%) |
| (study) | Intraday momentum (Gao et al. 2018; Baltussen et al. 2021) | Sign of prior close → 30 min before close predicts the last 30 min | A− (US/futures) |

Not coded, and why: kill zones / Silver Bullet / Power of 3 enter as the `session` context
field instead of separate rules; footprint, stacked imbalance and whale prints need tick data;
OTE, breaker and mitigation blocks are refinements of `ob`/`fvg` with no independent support;
India footprints (delivery %, bulk/block deals, FII/DII, OI build-up) are end-of-day data and
belong to the India phase; noise-area / VWAP and opening-range breakout need the US data (Alpaca)
so the published SPY/QQQ results can be reproduced first.

## Test results: crypto (Binance 1-minute bars, BTC ETH SOL XRP BNB)

Discovery 2020-01 → 2023-12, holdout 2024-01 → 2026-09 (used once). Net R per trade after
costs; "vs random" is the difference from the matched random entries (t in brackets).

| Timeframe | Best patterns in discovery | Everything else |
|---|---|---|
| 15 min | `fvg` beats random by +0.07R (t 4.6) long and +0.06R (t 3.5) short, but nets −0.10R | all negative net, −0.08 to −0.36R; sweeps *worse* than random (t −2.3) |
| 1 hour | `choch` long +0.03R net, +0.09R vs random (t 2.1) | all others negative net |
| 4 hours | `fvg` long +0.07R net (t 1.9), +0.08R vs random (t 1.3) | `sweep` long −0.15R, worse than random (t −2.2) |
| 1 day | samples of 37–222; nothing beats random at t > 1.2 | — |

With ~70 pattern × side × timeframe cells, a handful of |t| ≈ 2 results is what chance gives.

Memory-gated agent (all patterns, recall rule above):

| Run | Trades | Net R / trade | Return | Max DD |
|---|---|---|---|---|
| Discovery 1 h (trading 2021–23) | 921 | −0.041 | −19.6% | 29% |
| Discovery 4 h | 678 | −0.048 | −17.1% | 37% |
| Discovery 1 h, memory keyed by session too | 946 | −0.023 | −13.0% | 38% |
| Discovery 4 h, memory keyed by session too | 670 | −0.026 | −10.2% | 23% |
| **Holdout 1 h** (memory warmed on 2020–23) | 60 | −0.105 | −3.2% | 7% |
| **Holdout 4 h** | 393 | −0.057 | −11.9% | 19% |

Intraday momentum (last 30 minutes of the UTC day, bp per day, cost ≈ 14 bp round trip):
2020–23 BTC +6.4 (t 4.8), ETH +6.6 (t 4.0); 2024–26 BTC +0.8, ETH −2.2. Below costs and decayed.

**Status: no crypto concept passes.** Past performance of a pattern in a context did not
persist, so the memory picked contexts that had worked and then stopped working.
