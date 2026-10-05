# Existing repo context for a swing + intraday composite indicator (/home/user/Agent)

Scope: a read-only study of the two strategy reports, the related research notes and the C++ `trading-agent` project, as of the repo HEAD `f007450` ("Add D1, options validator, monitoring, validation and paper trading runner"). This is not web research. Every "source" below is a local file with line numbers. Abbreviations: **BP** = `reports/Hedge fund style strategy blueprint.md`; **PA** = `reports/Price action reading strategy rules.md`; **TA** = `trading-agent/`. Line numbers refer to the files as they stand at that commit.

## Q1. Which indicators and signals are already implemented in C++, and which sleeves and regime filters exist?

### Takeaway
The C++ engine has only a small, daily-bar indicator library: SMA, ADR, ATR%, annualised volatility, n-day return, traded value, volume SMA, highest-high and lowest-low. All of these are look-ahead-safe functions of `(Series, i, n)`. There is no RSI, EMA, MACD, Bollinger, VWAP or Supertrend, and no intraday indicator module. Five sleeves are implemented: A1, A2, B, C (validator only) and D1. Each builds its "signals" inline from those primitives. Composite scoring exists twice, both times as a z-score average clipped at ±3: once cross-sectionally in A1's screener and once over the trailing event history in A2's proxy score. The regime layer has trend (200-DMA), volatility percentile (India VIX or realised), momentum-crash, crowding and event-blackout gates.

### Cited Findings
**Indicator primitives** (all return NaN when history is short and read only bars `[0, i]`):
- `sma_close(s,i,n)`, `adr_frac(s,i,n)` (mean of high/low − 1), `avg_traded_value(s,i,n)` (mean close×volume), `ret(s,i,n)`, `ann_vol(s,i,n)` (log-return stdev × √252), `atr_frac(s,i,n)` (mean true range ÷ close), `sma_volume(s,i,n)`, `highest_high(s,i,n)`, `lowest_low(s,i,n)`. Declarations: [TA include/ta/indicators.hpp L12–34](/home/user/Agent/trading-agent/include/ta/indicators.hpp). Implementations: [TA src/indicators.cpp L17–86](/home/user/Agent/trading-agent/src/indicators.cpp). The no-look-ahead contract is stated in the header comment ([indicators.hpp L9–10](/home/user/Agent/trading-agent/include/ta/indicators.hpp)) and tested by `indicators_never_look_ahead` ([tests/test_a1.cpp L48–53](/home/user/Agent/trading-agent/tests/test_a1.cpp)).
- A search for vwap, rsi, ema, macd, bollinger and supertrend over `src/`, `include/` and `tests/` found none of them implemented (this session's grep).
- `zscore_clipped(x, clip=3)` is the cross-sectional z-score helper: [TA include/ta/screener.hpp L40](/home/user/Agent/trading-agent/include/ta/screener.hpp); [src/screener.cpp L16–28](/home/user/Agent/trading-agent/src/screener.cpp). `percentile_rank(history, x)` is declared in [include/ta/regime.hpp L34](/home/user/Agent/trading-agent/include/ta/regime.hpp) and implemented in [src/regime.cpp L36](/home/user/Agent/trading-agent/src/regime.cpp).

**Sleeve A1, momentum-leader breakouts (NSE daily)**:
- Composite score in `score_universe`: the average of four clipped z-scores. These are the 126-day return ÷ 252-day vol, the 252-day return ÷ vol, close ÷ 252-day high, and the 63-day return. Universe: price ≥ ₹100, 20-day traded value ≥ ₹10 crore, and at least 252 bars. Source: [src/screener.cpp L30–35, L59–84](/home/user/Agent/trading-agent/src/screener.cpp).
- Setup filter in `setup_pivot`: max(21-day, 63-day) return ≥ 30%; ADR20 ≥ 4%; close > SMA10 > SMA20; a 20-bar base whose 5-day range is at most 0.5 × the base range; close within 15% below the pivot. Source: [src/screener.cpp L37–57](/home/user/Agent/trading-agent/src/screener.cpp).
- `ScreenConfig` defaults are in [include/ta/screener.hpp L13–22](/home/user/Agent/trading-agent/include/ta/screener.hpp). `screen()` keeps the top 20% and then applies the setup filter ([src/screener.cpp L86–98](/home/user/Agent/trading-agent/src/screener.cpp)).
- Engine: `A1Engine::step` runs open exits, then entries, then close management, then `screen_for_tomorrow` ([src/a1.cpp L18–30](/home/user/Agent/trading-agent/src/a1.cpp)). The buy-stop sits at pivot + 1 tick with a 0.5% limit, and the stop is fill × (1 − ADR) ([src/a1.cpp L50–54](/home/user/Agent/trading-agent/src/a1.cpp)). Scores are stored in `last_scores_` for the scorecard ([src/a1.cpp L92](/home/user/Agent/trading-agent/src/a1.cpp)).
- Shared exits in `ExitRules`/`SwingBook`: day-3 partial of 1/3, day-20 time stop below +1R, 120-day cap, and an SMA10 trail when ADR ≥ 5% (SMA20 otherwise). Source: [include/ta/swing.hpp L15–24, L41–82](/home/user/Agent/trading-agent/include/ta/swing.hpp).

**Sleeve A2, episodic-pivot gaps (daily plus 5-minute bars, paper only)**:
- `detect()` requires a gap of at least 6% at the official open, a catalyst filed between the prior 15:30 and 09:15, ADV20 ≥ ₹10 crore, price ≥ ₹50, and an F&O stock or a 10%/20%-band cash stock ([src/a2.cpp L210–250](/home/user/Agent/trading-agent/src/a2.cpp)).
- It then computes eight oriented proxy variables ([src/a2.cpp L251–286](/home/user/Agent/trading-agent/src/a2.cpp)):
  - the 120-day pre-gap return;
  - ATR%(50) ÷ its 250-day median;
  - SMA20(volume) ÷ SMA50(volume);
  - open ÷ the 60-day high;
  - the 60-day range ÷ close;
  - the count of prior ≥6% gaps in 250 days;
  - open ÷ SMA200 − 1;
  - **log first-5-minute relative volume against the prior 14 days' first bars**, computed from intraday bars.
- `score()` z-scores each variable against the prior 250 trading days' events only, clips at ±3 and averages. It approves events above the 80th percentile of the prior event scores, with at least 30 prior events required ([src/a2.cpp L291–334](/home/user/Agent/trading-agent/src/a2.cpp)).
- Entry simulation `simulate_entry`: the 09:15 bar must be green; buy-stop at its high + 1 tick; window until 10:15; band-lock and upper-circuit skips ([src/a2.cpp L150–198](/home/user/Agent/trading-agent/src/a2.cpp)). Day-0 management includes the 15:20 weakness exit ([src/a2.cpp L37–59](/home/user/Agent/trading-agent/src/a2.cpp)). Config: [include/ta/a2.hpp L19–49](/home/user/Agent/trading-agent/include/ta/a2.hpp).

**Sleeve B, crypto Donchian ensemble (daily, 00:00 UTC)**:
- Lookbacks {5, 10, 20, 30, 60, 90, 150, 250, 360}.
- Per-lookback weight min(0.25/σ90, 2), averaged and capped at 1× per asset.
- 20% rebalance band, 10 bps per side, 31.2% worst-case tax and funding.
- Source: [include/ta/crypto_trend.hpp L17–30](/home/user/Agent/trading-agent/include/ta/crypto_trend.hpp).

**Sleeve C, defined-risk options**: a validator and sizer only, with no backtest ([README L24](/home/user/Agent/trading-agent/README.md)).

**Sleeve D1, Nifty last-half-hour momentum (intraday futures bars)**:
- Signal: `d1_signals` computes s = (close of the last bar starting before 15:00) ÷ prior day's last close − 1 ([src/d1.cpp L10–23](/home/user/Agent/trading-agent/src/d1.cpp)).
- Trade rule: trade if |s| is at or above the 70th percentile of the prior 250 days (minimum 60 days of history); 0.75% stop; exit at 15:28; one lot ([src/d1.cpp L35–80](/home/user/Agent/trading-agent/src/d1.cpp); config [include/ta/d1.hpp L17–30](/home/user/Agent/trading-agent/include/ta/d1.hpp)).
- Switch-on gate: mean move ≥ 2× cost, t > 3 and at least 250 trades ([src/d1.cpp L153](/home/user/Agent/trading-agent/src/d1.cpp)).

**Regime gates** ([include/ta/regime.hpp L18–80](/home/user/Agent/trading-agent/include/ta/regime.hpp); [src/regime.cpp L42–127](/home/user/Agent/trading-agent/src/regime.cpp)):
- `trend_gate`: index close > SMA200.
- `realized_vol_gate`: 30-day realised vol, red at or above the 80th percentile of its trailing 250 values.
- `vix_gate`: the same test applied to India VIX, using only dated values on or before the date.
- `crash_gate`: 63-day fall ≤ −15% and 21-day rebound ≥ +10%.
- `crowding_gate`: funding > 30% a year with rising open interest.
- `EventCalendar::blackout`: ±1 calendar day.
- `EquityRegime::allow_new_entries()` blocks entries on a red trend gate, a red crash gate or an event blackout. `risk_multiplier()` returns 0.5 when the vol gate is red ([regime.hpp L59–70](/home/user/Agent/trading-agent/include/ta/regime.hpp)).

**Risk and portfolio**:
- `RiskConfig` sets 0.40% risk per trade (1% ceiling), a 5% position cap, 12 positions, stop ≤ 1.0×ADR, cost ≤ 0.25R, −1% daily loss, and drawdown rules of halve at −7.5% and off at −15% ([include/ta/risk.hpp L11–22](/home/user/Agent/trading-agent/include/ta/risk.hpp)).
- `Allocation`: core 75%, buffer 5%, A1 10%, A2 0%, B 4%, C 3%, D 0%, reserve 3%. A1 and A2 share a 3% heat cap ([include/ta/portfolio.hpp L10–39](/home/user/Agent/trading-agent/include/ta/portfolio.hpp)).

### Inferences
- A new composite would be the first module that computes indicators *across* timeframes. Today, daily features live in `indicators.cpp`, while intraday features are written inline inside A2's `detect()` and D1's `d1_signals()`.
- The house style for combining features is settled. Orient each feature so that higher is better, winsorise or clip the z-scores at ±3, and average them. The z-scores are taken either cross-sectionally (A1) or against a trailing history (A2). A new indicator that follows this style would be easiest to review and to monitor.
- Classic oscillators (RSI, MACD, Bollinger, VWAP, Supertrend) would be new code. The research notes also class intraday versions of these as cost-eaten (see Q2), so adding them needs a stated economic mechanism to pass "gate 0".

### Gaps
- The full bodies of `swing.cpp`, `crypto_trend.cpp`, `options.cpp` and `risk.cpp` sizing were skimmed through their headers only. The README and headers describe their behaviour, but individual edge cases were not audited.
- There is no sector data in the engine, so the 3-per-sector cap is not modelled ([README L138](/home/user/Agent/trading-agent/README.md)).

## Q2. Which thresholds are marked unverified (U), and what did prior research conclude about intraday trading in India?

### Takeaway
Almost every numeric threshold in the rulebook is marked U (unverified) or P (practitioner, untested) and must be counted as a backtest trial. Prior research is close to unanimous that high-turnover intraday technical systems in India lose after costs. The April 2026 STT rise made this worse: futures STT went from 0.02% to 0.05%, so a Nifty futures round trip is about ₹970 (about 15 points, 0.06%), and intraday equity costs about 0.08% before slippage. The only intraday idea kept is D1 (last-half-hour index-futures momentum, which has a dealer-gamma mechanism), and it gets zero capital until it clears 2× costs out of sample. Opening-range breakout, the noise area, VWAP, MA crosses, Supertrend, intraday pairs, order flow and market-making were all refused.

### Cited Findings
**Status conventions.** V means the detail is quoted in a source; U means unverified or inferred ([PA L9](/home/user/Agent/reports/Price%20action%20reading%20strategy%20rules.md)). P means a practitioner rule ([PA L97–102](/home/user/Agent/reports/Price%20action%20reading%20strategy%20rules.md)). The C++ README restates that "Every threshold marked U … is a trial" ([TA README L145–146](/home/user/Agent/trading-agent/README.md)), and regime.hpp says every regime threshold is U ([regime.hpp L12–14](/home/user/Agent/trading-agent/include/ta/regime.hpp)).

**U thresholds that matter for a composite indicator** (PA tables, L85–162):
- Regime multiplier m: Nifty above its 200-DMA and India VIX below its 1-year 80th percentile give m = 1; one failing gives 0.5; both failing gives 0. The thresholds are U and count as trials (PA L87).
- Liquidity cap of 1% of ADV20 (PA L87). Cost gate: skip if modelled cost exceeds 0.25R; this is a key rule but derived (PA L87).
- A1 universe floors (price ≥ ₹100, ADV20 ≥ ₹10 crore) are U. The composite factors are V but their **weights are U** (PA L95–96).
- A1 entry times (09:15–09:20 bar, window 09:20–15:00) are U. Risk 0.40% is U. The day-20 time stop and 120-day cap are U (PA L98–103).
- A2 proxies: the traits are V, but **every formula is U**. That covers the 120-day return ≤ 15%, ATR ratio < 1, volume ratio < 1, the 60-day high breakout, range ≤ 35%, at most one prior gap, and extension above SMA200 ≤ 50%. The 80th-percentile approval rule and the 09:20–10:15 window are also U (PA L115–129).
- B: the close time, the 1× cap, the √365 annualisation and the crowding threshold are U (PA L138–144).
- D1: every parameter except the published signal definitions is U, including the 70th-percentile trigger, the 0.75% stop and slippage of 2 points a side (PA L148–162).
- A1+A2 heat cap of 3% and the 12-position and 3-per-sector limits are U (PA L212).
- The BP ledger lists VIX bands, sleeve stops, PSI levels, auto-disable rules and ruin acceptance as "practitioner convention and judgment … count each as a backtest trial" ([BP L387](/home/user/Agent/reports/Hedge%20fund%20style%20strategy%20blueprint.md)).

**India intraday cost and tax conclusions:**
- Futures STT rose from 0.02% to 0.05%, option STT from 0.1% to 0.15% and exercise STT from 0.125% to 0.15%, all from 1 April 2026. Equity intraday STT stays at 0.025% on the sell side ([research_notes/Low risk intraday trading strategies/india_market_regulation_costs.md L114–122](/home/user/Agent/research_notes/Low%20risk%20intraday%20trading%20strategies/india_market_regulation_costs.md)).
- Worked round trips from the same note (L126–141):
  - intraday equity ₹1 lakh: about ₹82 (0.082%), with a realistic hurdle of 0.15–0.2% once spread and slippage are added;
  - Nifty future, one lot: about ₹970, 0.06% or about 15 points (versus 6–7 points before April 2026);
  - Nifty option, one lot: about ₹62.5.
- The stamp-duty and NSE-charge figures conflict between sources and are flagged as unverified (same note, L122–124 and L143–147).
- In the PA cost table, an intraday MIS trade is modelled at 0.18% base and 0.36% stress, and a Nifty future at about 19 points base and 38 stress. Intraday equity is taxed as speculative income ([PA L70–75](/home/user/Agent/reports/Price%20action%20reading%20strategy%20rules.md)).
- Tax: intraday cash equity is speculative income (offset only against speculative income, carried forward 4 years). F&O is non-speculative (carried forward 8 years). Brokers auto-square-off around 15:20–15:25 ([BP L167, L336–340](/home/user/Agent/reports/Hedge%20fund%20style%20strategy%20blueprint.md)).
- SEBI base rates: 71% of intraday cash traders lost money in FY23, with traders making more than 500 trades a year doing worst. 87.7% of F&O traders lost money in FY26, with costs turning about 4.4 lakh gross winners into net losers ([BP L20](/home/user/Agent/reports/Hedge%20fund%20style%20strategy%20blueprint.md)).

**Rejected intraday strategies:**
- The BP Sleeve D table and refusal list cover opening-range breakout (QQQ breaks even at 2.2¢ of slippage; BankNifty "not profitable"), Nifty Supertrend (₹57,892 gross became ₹1,652 net), 5-minute NSE pairs (+1.3% gross became −3.7% net), short-horizon reversal, order-flow imbalance (explains moves rather than predicting them), LSTM signals, VWAP and MA crosses ("costs eat 90%+ of the gross edge"), market-making and LLM order choice ([BP L153–183](/home/user/Agent/reports/Hedge%20fund%20style%20strategy%20blueprint.md)).
- The PA D table runs the candidates at Indian costs ([PA L148–162](/home/user/Agent/reports/Price%20action%20reading%20strategy%20rules.md)):
  - D2 Nifty noise-area: **negative** (6–7.5 bps of cost against an edge of about 2.6 bps).
  - D3 NSE stocks-in-play opening-range breakout: **strongly negative** (about 0.9R of cost against +0.08R). A public NSE opening-range test lost 21.8% and lost in every out-of-sample year (PA L77).
  - Only D1 remains a candidate.
- The directional-strategies note gives the reasons:
  - Opening-range breakout edges are within the spread.
  - Intraday momentum (Gao et al., R² 1.6%; Baltussen et al., more than 60 futures markets, gamma hedging) is real but small. It is strongest on volatile, high-volume, news days and **reverses over the following days**, so positions must be flat by the close.
  - VWAP, MA and Supertrend evidence is thin, and costs eat it.
  - Short-horizon reversal is a microstructure effect.
  - Source: [directional_strategies.md L8, L45–69, L79, L105](/home/user/Agent/research_notes/Low%20risk%20intraday%20trading%20strategies/directional_strategies.md).

**Swing conclusions relevant to the daily half:**
- Momentum is a 3–12-month effect. At 1–4 week horizons Indian stocks show **reversal**, so short-term weakness should only *time* entries into 6–12-month leaders ([BP L118](/home/user/Agent/reports/Hedge%20fund%20style%20strategy%20blueprint.md); [swing_strategies.md L10](/home/user/Agent/research_notes/Swing%20trading%20and%20stock%20screener/swing_strategies.md)).
- 52-week-high proximity is a more stable alpha in India ([BP L116](/home/user/Agent/reports/Hedge%20fund%20style%20strategy%20blueprint.md)). The high-volume return premium is weak in India, so "volume surges confirm but never select" ([BP L194](/home/user/Agent/reports/Hedge%20fund%20style%20strategy%20blueprint.md)). Delivery percentage has no peer-reviewed evidence ([BP L120](/home/user/Agent/reports/Hedge%20fund%20style%20strategy%20blueprint.md)).
- Liquid-half Indian momentum earned only 8.51% net CAGR against 10.41% for the Nifty 50 ([BP L109–116](/home/user/Agent/reports/Hedge%20fund%20style%20strategy%20blueprint.md)). The realistic A1 edge is 0 to +0.2R per trade ([PA L107](/home/user/Agent/reports/Price%20action%20reading%20strategy%20rules.md)).
- Honest ceilings: about 51–56% directional accuracy and rank IC of about 0.02–0.06. Claims above 60–70% indicate leakage ([BP L202–206](/home/user/Agent/reports/Hedge%20fund%20style%20strategy%20blueprint.md)).
- Extreme intraday moves in Indian stocks tend to reverse within minutes ([PA L66](/home/user/Agent/reports/Price%20action%20reading%20strategy%20rules.md)).

**Intended use of intraday data in swing sleeves:**
- The BP plans an "intraday watchlist of F&O stocks … kept in research-only logging until validated because no peer-reviewed Indian intraday ranking evidence exists" ([BP L198](/home/user/Agent/reports/Hedge%20fund%20style%20strategy%20blueprint.md)).
- A1's rulebook entry uses intraday data only for *timing*: arm the buy-stop after the 09:15–09:20 bar completes, and skip if that bar ran more than 1×ADR past the pivot ([PA L98](/home/user/Agent/reports/Price%20action%20reading%20strategy%20rules.md)). This rule is **not modelled in C++** ([TA README L138](/home/user/Agent/trading-agent/README.md)).

### Inferences
- A composite indicator whose intraday component drives *intraday round trips* in cash equity runs straight into the documented cost wall: about 0.18% base, which is about 0.9R on tight stops. The design most consistent with the repo uses intraday bars only to (a) time or veto entries into daily-selected swing candidates, as A1's 09:15-bar rule and A2's first-5-minute relative volume already do, or (b) feed the single D1-style index-futures trade.
- Any new threshold (lookbacks, percentile cut-offs, weights) inherits U status and must be logged as a trial for the deflated Sharpe and PBO.
- Relative volume can enter a composite only as a confirming or ranking feature, not as a selector, given the weak Indian high-volume premium.

### Gaps
- The research found no peer-reviewed, net-of-cost Indian evidence on intraday *ranking* signals or on multi-timeframe composites ([BP L198](/home/user/Agent/reports/Hedge%20fund%20style%20strategy%20blueprint.md)). The India intraday-momentum paper is unrefereed ([directional_strategies.md L50](/home/user/Agent/research_notes/Low%20risk%20intraday%20trading%20strategies/directional_strategies.md)).
- Exact 2026 stamp-duty and NSE transaction-charge rates remain unconfirmed ([india_market_regulation_costs.md L143–147](/home/user/Agent/research_notes/Low%20risk%20intraday%20trading%20strategies/india_market_regulation_costs.md)).

## Q3. What data does the engine support, and could it compute a composite that mixes daily and intraday bars?

### Takeaway
Yes, and A2 already does it. Daily and intraday bars share one `Bar` struct. Intraday bars are distinguished only by a `"YYYY-MM-DD HH:MM"` (bar start, exchange-local) date string and are grouped per day with `index_by_day`. Any bar interval works because there is no hard-coded interval; A2 assumes 5-minute bars and D1 accepts 1- or 5-minute bars. The engine never fetches data: inputs are local CSVs. Limitations: the `IntradayData` container lives in `a2.hpp` rather than a shared header; there is no timezone or `available_at` field; corporate-action adjustment exists only for daily universes; and nothing aggregates intraday bars into other timeframes beyond `daily_closes`.

### Cited Findings
**Data structures:**
- `struct Bar {date, open, high, low, close, volume}`, `Series = vector<Bar>`, `Universe = map<symbol, Series>` ([include/ta/bar.hpp L10–22](/home/user/Agent/trading-agent/include/ta/bar.hpp)).
- Intraday helpers: `day_of`, `time_of`, `DayIndex`, `index_by_day`, `daily_closes` and `load_dated_values` ([include/ta/data.hpp L73–84](/home/user/Agent/trading-agent/include/ta/data.hpp); [src/data.cpp L213–251](/home/user/Agent/trading-agent/src/data.cpp)).
- `IntradayData { map<symbol, Series> bars; map<symbol, DayIndex> index; finalize(); day(sym, date, &n) }` is declared in [include/ta/a2.hpp L51–57](/home/user/Agent/trading-agent/include/ta/a2.hpp) and implemented in [src/a2.cpp L97–112](/home/user/Agent/trading-agent/src/a2.cpp).
- `MarketData` gives O(1) (symbol, date) → bar lookup and the sorted union of dates ([include/ta/market.hpp L12–31](/home/user/Agent/trading-agent/include/ta/market.hpp)).

**Loaders:**
- `load_series` reads `date,open,high,low,close,volume` with a header, at least 6 columns, sorted by the date string. `load_universe(dir)` reads one CSV per symbol ([src/csv.cpp L31–64](/home/user/Agent/trading-agent/src/csv.cpp)). The same loader serves daily and intraday files.
- Bhavcopy ingest supports the legacy, UDiFF and sec_bhavdata_full formats, plus corporate actions (`symbol,ex_date,factor`, applied to daily universes) ([include/ta/data.hpp L22–56](/home/user/Agent/trading-agent/include/ta/data.hpp)).

**Supported inputs** ([TA README L46–59](/home/user/Agent/trading-agent/README.md)):
- NSE daily per symbol (split/bonus adjusted, delisted names included);
- index and India VIX;
- event calendar;
- exclusions;
- **5-minute bars per symbol** (`YYYY-MM-DD HH:MM,...`, bar start time), used by A2;
- catalysts and price bands;
- crypto daily at the 00:00 UTC close, plus funding and open interest;
- **Nifty near-month continuous 1- or 5-minute bars**, used by D1.

Paper-runner data keys are `data.equity_dir`, `data.exclusions`, `data.index`, `data.vix`, `data.events`, `data.intraday_dir`, `data.catalysts`, `data.bands`, `data.crypto_dir`, `data.crypto_funding_dir`, `data.crypto_oi_dir`, `data.nifty_fut` and `data.d1_skip_days` ([src/paper.cpp L320–322](/home/user/Agent/trading-agent/src/paper.cpp)).

**Mixed-timeframe precedent in A2:**
- `A2Engine::detect` reads daily bars from `MarketData` (ATR, SMA, volume ratios, 60-day high). It then reads the same day's and the prior 14 days' first 5-minute bars from `IntradayData` to build log relative volume as one of the eight z-scored variables ([src/a2.cpp L260–283](/home/user/Agent/trading-agent/src/a2.cpp)).
- In paper mode the intraday directory is loaded only when A2 has capital: `if (!intra_dir.empty() && a2_cap > 0)` ([src/paper.cpp L198–213](/home/user/Agent/trading-agent/src/paper.cpp)).

**Intraday-only precedent in D1:**
- D1 works from one continuous futures series and derives prior closes itself ([src/d1.cpp L10–23](/home/user/Agent/trading-agent/src/d1.cpp)).

**Data sources named in research:**
- Kite Connect serves minute candles 60 days per request at 3 requests a second. Fyers has minute data from mid-2017, and Dhan has five years of intraday history ([BP L200](/home/user/Agent/reports/Hedge%20fund%20style%20strategy%20blueprint.md)).
- The A1 test plan calls for 1-minute bars from mid-2017 (Fyers) for intraday entry and stop fills ([PA L192](/home/user/Agent/reports/Price%20action%20reading%20strategy%20rules.md)).
- Fetch helpers exist but are untested because the network is blocked ([TA README L61–67](/home/user/Agent/trading-agent/README.md); `scripts/fetch_nse_bhavcopy.sh`, `scripts/fetch_delta_candles.py`).

**Point-in-time requirement:**
- The BP requires every value to carry a UTC `available_at` timestamp ([BP L226](/home/user/Agent/reports/Hedge%20fund%20style%20strategy%20blueprint.md)), and the PA requires inputs to carry `available_at` ([PA L83](/home/user/Agent/reports/Price%20action%20reading%20strategy%20rules.md)). The C++ code has no `available_at` field (this session's grep). Look-ahead safety relies instead on index discipline (`[0, i]`) and on bar *start* timestamps.
- One example of a reasoning rule in code: D1 treats "the price at 15:00" as the close of the last bar *starting before* 15:00 ([d1.hpp L32–34](/home/user/Agent/trading-agent/include/ta/d1.hpp)).

### Inferences
- Combining daily and intraday features is feasible with the existing types and needs no new data format. The natural pattern is a function taking `(const MarketData& daily, const IntradayData& intraday, symbol, date, cutoff_time)`. Its daily features would use only index `i − 1`, and its intraday features only bars whose *start* time + interval ≤ `cutoff_time`. A2's `detect()` is the template.
- To share `IntradayData` beyond A2, move it from `a2.hpp` into `data.hpp`. This is a mechanical refactor; A2's tests would cover the move.
- Intraday bars are not corporate-action adjusted, because `apply_corporate_actions` operates on a `Universe` of daily series. Any intraday feature that compares across days (relative volume, intraday ATR) would break on split or bonus days unless it is adjusted or normalised per day.
- Because the bar interval is implicit, a composite should either take the interval as a config value or infer it from timestamps, and should test 1- and 5-minute inputs the way D1 already allows.

### Gaps
- No intraday data ships in the repo, and no test uses real NSE intraday files; tests generate synthetic bars (`tests/harness.hpp`, `scripts/smoke_test.sh`). Behaviour on real feeds (missing bars, pre-open prints, half-days) is untested.
- How the README's timezone convention ("exchange local") interacts with crypto UTC bars was not checked in code.

## Q4. What validation infrastructure must a new indicator pass through?

### Takeaway
The repo implements the *statistics* of gate 1:
- t-statistic of R;
- probabilistic and deflated Sharpe with trial counting;
- PBO by CSCV;
- removal of the top 1% of trades;
- single-year P&L concentration;
- stationary-block-bootstrap drawdown;
- trades needed for t = 3;
- a gate-1 report wired to A1 with a 2×-cost rerun and a 9-point grid.

It also implements live monitoring: rank IC, precision@k against the base rate, CUSUM, PSI, per-sleeve kill rules and a hash-chained journal. It does **not** implement walk-forward, purged or embargoed CV, or the single-use holdout. Those exist only as documented requirements and a printed reminder.

### Cited Findings
**`validate.hpp`/`validate.cpp`:**
- `deflated_sharpe(returns, n_trials, var_trial_sharpe)` uses `probabilistic_sharpe` and `expected_max_sharpe` (Bailey and López de Prado).
- `pbo_cscv(perf[t][j], splits=16)`.
- `expectancy_without_top`, `max_year_share`, `bootstrap_drawdown_p95` and `trades_needed`.
- Declarations: [include/ta/validate.hpp L22–41](/home/user/Agent/trading-agent/include/ta/validate.hpp). Implementations: [src/validate.cpp L104–220](/home/user/Agent/trading-agent/src/validate.cpp).
- `GateThresholds`: expectancy ≥ 0.15R at 1× costs and > 0 at 2×; DSR ≥ 0.95; PBO < 0.20; t > 3; max drawdown ≤ 25%; no year above 40% of P&L ([validate.hpp L50–58](/home/user/Agent/trading-agent/include/ta/validate.hpp)). `evaluate_gate1` is at [src/validate.cpp L236–262](/home/user/Agent/trading-agent/src/validate.cpp).
- Tests: `deflated_sharpe_penalises_many_trials`, `pbo_noise_vs_real_edge`, `robustness_helpers` and `gate1_report` ([tests/test_monitor_validate.cpp L83–160](/home/user/Agent/trading-agent/tests/test_monitor_validate.cpp)).

**Trial counting in practice:**
- `ta_backtest a1 --gate` reruns at 2× cost and sweeps a 3×3 grid (`min_runup` ∈ {0.25, 0.30, 0.40} × `min_adr` ∈ {0.03, 0.04, 0.05}). It collects per-period returns into `perf` and the trial Sharpes into `var_sr`, then calls `pbo_cscv(perf, 16)` and `evaluate_gate1` ([src/main.cpp L92–124](/home/user/Agent/trading-agent/src/main.cpp)).
- The comment "Every grid point counts as a trial for the deflated Sharpe ratio and PBO" is at [src/main.cpp L97](/home/user/Agent/trading-agent/src/main.cpp).
- `--gate` exists for A1 only. A2 prints the approved-minus-rejected uplift and its t-statistic, with a pass bar of ≥ 0.15R, t ≥ 2 and at least 300 out-of-sample events ([src/main.cpp L128–150](/home/user/Agent/trading-agent/src/main.cpp); [src/a2.cpp L436–505](/home/user/Agent/trading-agent/src/a2.cpp)). D1 has its own switch-on gate ([src/d1.cpp L153](/home/user/Agent/trading-agent/src/d1.cpp)).

**Walk-forward and holdout:**
- These appear only as text: "Also required before capital: walk-forward/holdout, paper trading…" ([src/main.cpp L122](/home/user/Agent/trading-agent/src/main.cpp)). A grep for walk-forward, holdout, purge or embargo found nothing else in `src/`, `include/` or `tests/`.
- The plan is purged walk-forward with train 2017–2022 and test 2023–2026, plus a single-use 12-month holdout ([PA L192](/home/user/Agent/reports/Price%20action%20reading%20strategy%20rules.md)). Gate 2 calls for "Only concatenated out-of-sample results … Purged, embargoed folds … Single-use 12–24-month holdout" ([BP L309](/home/user/Agent/reports/Hedge%20fund%20style%20strategy%20blueprint.md)).
- The researchers' notes back purged and embargoed CV, CPCV, deflated Sharpe and PBO ([risk_ensemble_backtesting.md L157](/home/user/Agent/research_notes/Low%20risk%20intraday%20trading%20strategies/risk_ensemble_backtesting.md); [screener_accuracy.md L177–179](/home/user/Agent/research_notes/Swing%20trading%20and%20stock%20screener/screener_accuracy.md)).

**The full gate ladder** ([BP L301–314](/home/user/Agent/reports/Hedge%20fund%20style%20strategy%20blueprint.md)):
- Gate 0: a written specification with an economic mechanism.
- Gate 1: backtest at 2× costs, DSR ≥ 0.95, PBO < 0.1–0.2, t > 3, leakage smoke tests.
- Gate 2: walk-forward and holdout.
- Gate 3: paper trading for 4–8 weeks and at least 100 trades.
- Gate 4: small live at 10–25% of size for 6–12 months.
- Gate 5: scale.
- PA adds **reproduce first**: reproduce a published number and the negative baseline, freeze defaults and count every grid value as a trial ([PA L184](/home/user/Agent/reports/Price%20action%20reading%20strategy%20rules.md)).
- Leakage smoke tests: shift features forward one bar, shuffle labels within each date, run on a random walk ([BP L226](/home/user/Agent/reports/Hedge%20fund%20style%20strategy%20blueprint.md)). The repo has `a1_random_walk_has_no_edge_after_costs` ([tests/test_a1.cpp L301](/home/user/Agent/trading-agent/tests/test_a1.cpp)), and the README notes that every sleeve loses on random-walk data ([README L10–11](/home/user/Agent/trading-agent/README.md)).

**Live monitoring of a score** (`monitor.hpp`):
- `spearman`; `forward_return` (next open to the close `horizon` sessions later, NaN if the future is not yet known); `score_outcomes` (daily rank IC, precision@k, base rate); `rolling_mean_ic`; `Cusum`; and `psi` (alert above 0.1, act at 0.25) ([include/ta/monitor.hpp L15–53](/home/user/Agent/trading-agent/include/ta/monitor.hpp); [src/monitor.cpp L50–120](/home/user/Agent/trading-agent/src/monitor.cpp)).
- `screener_rule` turns a signal family off when the 60-day IC is below 0 together with a CUSUM alarm, or when the 120-day expectancy is below 0 ([src/monitor.cpp L171–175](/home/user/Agent/trading-agent/src/monitor.cpp)). Per-sleeve kill rules follow (L139–169).
- The paper runner logs every eligible A1 score to `scores.csv` each day ([src/paper.cpp L470–471](/home/user/Agent/trading-agent/src/paper.cpp)). `PaperAccount::scorecard(horizon, top_k)` computes IC at 20, 60 and 120 days, precision@k, CUSUM (`kill.ic_target = 0.03`) and the screener rule ([src/paper.cpp L594–638](/home/user/Agent/trading-agent/src/paper.cpp)). The journal is SHA-256 hash-chained ([README L119–120](/home/user/Agent/trading-agent/README.md)).
- Test count: 70 unit tests across 8 files, using the in-house `TEST()` harness ([tests/harness.hpp](/home/user/Agent/trading-agent/tests/harness.hpp); README L40).

### Inferences
- A new indicator can reuse the scoring and monitoring path almost for free. If it emits `(date, symbol, score)` rows, `score_outcomes`, `rolling_mean_ic`, `Cusum`, `psi` and `screener_rule` already compute the BP scorecard: rank IC, precision@k against the base rate and the auto-disable rule. The scorecard is currently hard-wired to `scores.csv` and A1 trades, so a second score file or a family column would be needed.
- To meet gate 1, the indicator needs a sleeve or backtest wrapper that produces `Trade`s and an `EquityCurve`, so that `evaluate_gate1` can run. It also needs an explicit grid of every U parameter, so `pbo_cscv` and `deflated_sharpe` see the true trial count.
- Walk-forward, purged CV and the holdout are a real gap. A composite designed now would be the first component that needs them implemented rather than merely documented.

### Gaps
- The README's `--gate` wording ("9-point parameter grid") counts only the 9 grid trials. Trials spent before that grid (earlier experiments) are not logged anywhere in code; there is no trial-log file.
- No leakage smoke test (feature shift or label shuffle) exists beyond the random-walk test.

## Q5. Where would a new indicator plug in? (exact paths and lines)

### Takeaway
A new composite needs edits in about six places:
1. Daily primitives in `indicators.hpp/.cpp`.
2. A new header and source pair for the multi-timeframe composite (for example `include/ta/composite.hpp` and `src/composite.cpp`). These are picked up automatically by the `GLOB` in CMake.
3. A config struct, plus keys in `settings.cpp` and `default_settings()`.
4. A consumer: either A1's ranking and entry path, or a new sleeve or `ta_backtest` subcommand.
5. Logging of scores for the scorecard in `paper.cpp`.
6. Tests in `tests/`.

### Cited Findings
- **Build:** `file(GLOB TA_SOURCES CONFIGURE_DEPENDS src/*.cpp)` and `file(GLOB TA_TESTS … tests/*.cpp)` pick up new `.cpp` files without CMake edits ([trading-agent/CMakeLists.txt L16–19, L31–33](/home/user/Agent/trading-agent/CMakeLists.txt)).
- **Daily primitives:** add declarations after [include/ta/indicators.hpp L34](/home/user/Agent/trading-agent/include/ta/indicators.hpp) and implementations after [src/indicators.cpp L86](/home/user/Agent/trading-agent/src/indicators.cpp), using the `(const Series&, size_t i, size_t n)` NaN-on-short-history convention (`has_window`, L12–14).
- **Intraday access:** `IntradayData` is at [include/ta/a2.hpp L51–57](/home/user/Agent/trading-agent/include/ta/a2.hpp) (candidate to move to `data.hpp`, after L84). The day-slicing helpers are at [data.hpp L73–81](/home/user/Agent/trading-agent/include/ta/data.hpp). A worked mixed-timeframe feature is at [src/a2.cpp L264–283](/home/user/Agent/trading-agent/src/a2.cpp).
- **Cross-sectional combination:** `zscore_clipped` ([src/screener.cpp L16](/home/user/Agent/trading-agent/src/screener.cpp)). For a historical (per-event) z-score, the pattern is `A2Engine::score` ([src/a2.cpp L291–319](/home/user/Agent/trading-agent/src/a2.cpp)).
- **A1 ranking hook:** `score_universe` builds the four factors at [src/screener.cpp L61–80](/home/user/Agent/trading-agent/src/screener.cpp). A new daily factor could be added as a fifth z-score there, with the config field in `ScreenConfig` ([include/ta/screener.hpp L13–22](/home/user/Agent/trading-agent/include/ta/screener.hpp)).
- **A1 entry-timing hook:** `A1Engine::enter` decides fills from the daily bar only ([src/a1.cpp L43–78](/home/user/Agent/trading-agent/src/a1.cpp)). This is where an intraday confirmation or veto (for example the rulebook's unmodelled 09:15-bar rule) would go. It would need `IntradayData` passed through `A1Engine`'s constructor ([include/ta/a1.hpp L44–45](/home/user/Agent/trading-agent/include/ta/a1.hpp)) and into `A1Config` ([a1.hpp L20–29](/home/user/Agent/trading-agent/include/ta/a1.hpp)).
- **Signal and score logging:** `screen_for_tomorrow` ([src/a1.cpp L81–115](/home/user/Agent/trading-agent/src/a1.cpp)) emits `signal` and `regime` journal records. The paper runner writes `scores.csv` at [src/paper.cpp L470–471](/home/user/Agent/trading-agent/src/paper.cpp).
- **Config keys:** add an `apply_settings(const Config&, NewConfig&)` overload and its declaration ([include/ta/settings.hpp L18–23](/home/user/Agent/trading-agent/include/ta/settings.hpp)). Add the keys inside `apply_settings` (pattern at [src/settings.cpp L41–61](/home/user/Agent/trading-agent/src/settings.cpp)) **and** in `default_settings()` ([src/settings.cpp L99–166](/home/user/Agent/trading-agent/src/settings.cpp)). `check_settings_keys` rejects any key not listed in `default_settings()`, except the `data.`, `account.`, `paper.` and `kill.` prefixes ([src/settings.cpp L168–179](/home/user/Agent/trading-agent/src/settings.cpp)).
- **Existing config keys:**
  - `alloc.*`;
  - `a1.*` (risk keys plus `cost_round_trip`, `min_price`, `min_avg_traded_value`, `top_fraction`, `min_runup`, `min_adr`, `base_len`, `max_tightness`, `max_below_pivot`, `entry_limit_frac`, `stop_adr_mult`, `partial_day`, `time_stop_day`, `max_hold_days`);
  - `regime.trend_sma`, `regime.vol_red_pct`, `regime.event_days_before`, `regime.event_days_after`, `regime.funding_red_annual`;
  - `a2.*` (risk keys plus `cost_round_trip`, `min_gap`, `window_end`, `approve_pct`, `min_history_events`, `max_entries_per_day`, `max_hold_days`);
  - `b.*` (risk keys plus `vol_target`, `asset_cap`, `rebalance_band`, `cost_bps`, `tax_rate`, `funding_annual_default`);
  - `d1.*` (risk keys plus `pct_threshold`, `stop_frac`, `lot_size`, `cost_points`, `min_sleeve_equity`);
  - `c.budget_frac`, `c.cost_per_leg_lot`.
  - The risk keys for each prefix are `risk_per_trade`, `max_position_frac`, `max_positions`, `max_stop_adr_mult`, `max_cost_to_r`, `daily_loss_limit`, `drawdown_halve`, `drawdown_off` and `max_gross_leverage` ([src/settings.cpp L10–20, L30–97](/home/user/Agent/trading-agent/src/settings.cpp)).
  - Paper-only keys: `kill.b_backtest_max_dd = 0.19`, `kill.ic_target = 0.03`, `paper.a2_capital`, `paper.d1_capital` ([src/paper.cpp L174–178, L337](/home/user/Agent/trading-agent/src/paper.cpp)).
  - A new data input (for example `data.composite_intraday_dir`) would also need adding to the `init` list at [src/paper.cpp L320–322](/home/user/Agent/trading-agent/src/paper.cpp) and loading near L183–215.
- **Backtest CLI:** add a subcommand next to `run_a1`/`run_a2`/`run_d1` and to the dispatcher ([src/main.cpp L24–42 usage, L214–233 dispatch](/home/user/Agent/trading-agent/src/main.cpp)). Reuse the `--gate` block (L92–124) for the 2× cost run, grid trials, DSR and PBO.
- **Monitoring:** `score_outcomes` and `screener_rule` ([include/ta/monitor.hpp L32–33, L81](/home/user/Agent/trading-agent/include/ta/monitor.hpp)). If the composite drives a new sleeve, add a kill rule alongside L68–78.
- **Regime inputs:** reuse `evaluate_equity_regime` ([include/ta/regime.hpp L79–80](/home/user/Agent/trading-agent/include/ta/regime.hpp)) rather than embedding a trend or volatility filter inside the indicator, so regime thresholds stay in one place and are counted once.
- **Tests:** the new tests should copy the existing patterns: `indicators_never_look_ahead` ([tests/test_a1.cpp L48](/home/user/Agent/trading-agent/tests/test_a1.cpp)), the random-walk no-edge test (L301), and the save/load equivalence tests (L246). The intraday fixture pattern is in `intraday_day_index` ([tests/test_infra.cpp L125](/home/user/Agent/trading-agent/tests/test_infra.cpp)) and the a2 tests ([tests/test_a2.cpp L76–158](/home/user/Agent/trading-agent/tests/test_a2.cpp)).

### Inferences
- The lowest-friction design that fits both the evidence and the code is a **daily-selected, intraday-timed** composite:
  - The daily half reuses A1-style factors (6/12-month vol-adjusted momentum, 52-week-high proximity) plus perhaps ATR and volume regime.
  - The intraday half is computed only up to a fixed cutoff (for example the 09:15–09:20 bar or 15:00). It uses A2-style relative volume at the same time of day and an opening-range or last-half-hour signal.
  - The two are combined by clipped z-scores and logged daily for rank-IC monitoring.
  - Trades are held overnight as swing positions, so costs stay near the 0.5% delivery round trip rather than repeated intraday round trips.
- Every lookback, weight and cutoff time must be enumerated in a grid fed to `pbo_cscv` and `deflated_sharpe`. Before any capital, a walk-forward/holdout harness must be written, because none exists yet.

### Gaps
- The `Swing intraday composite indicator` research folder was empty before this note, so there are no earlier design notes for this indicator.
- Whether A1's paper-mode config conversion (`to_sleeve_fraction`, [src/paper.cpp L188–191](/home/user/Agent/trading-agent/src/paper.cpp)) should also apply to a new sleeve depends on the allocation decision, which is not yet made. The allocation has no line for a composite sleeve ([include/ta/portfolio.hpp L10–18](/home/user/Agent/trading-agent/include/ta/portfolio.hpp)).
