# Concept Desk: paper-trading app

A trading app that runs on your own computer. It shows live charts for crypto, US stocks and
Indian (NSE) stocks, marks the institutional-concept strategy signals on them, and lets you
paper-trade a signal with one click or place your own orders.

**It is paper trading only.** It has no broker connection, and no order ever reaches an
exchange.

Every one of these strategies lost money after costs in the 2020–2026 backtests
(`../knowledge/concepts.md`). The app shows each signal's track record so you can see that for
yourself.

## Start it on Windows

1. **Install Python.** Get Python 3.12 from <https://www.python.org/downloads/>. In the
   installer, tick **"Add python.exe to PATH"**.
2. **Download the app.** Open
   <https://github.com/mannu71/Agent/archive/refs/heads/claude/intra-trading-agent-research-b8t3jo.zip>
   and unzip it anywhere, for example to `Documents`.
3. **Start it.** Open the unzipped folder, go into `trading-agent\app`, and double-click
   **`start.bat`**.
   - The first start sets up a private Python environment, which takes about a minute.
   - Your browser then opens **http://localhost:8765**.
   - If Windows asks about network access, allow it on private networks. The app only talks
     to market-data sites and your browser.
4. **Keep the black window open** while you trade. Closing it stops the app; your paper
   account is saved.

On Mac or Linux, run `./start.sh` from the `app` folder instead.

## Live prices

| Market | Source | Speed |
|---|---|---|
| Crypto (BTC ETH SOL XRP BNB) | Binance public market data, no account needed | Live, updates every second |
| US stocks/ETFs | Alpaca (free account), otherwise Yahoo | Live with Alpaca keys; about 1 minute delayed without |
| India (NIFTY 50 and large NSE stocks) | Yahoo | Refreshed every minute; may lag the exchange slightly |

**To get live US prices:**
1. Create a free account at <https://alpaca.markets> and open the paper-trading dashboard.
2. Generate API keys there.
3. In the app, open **Settings** and paste the key ID and secret.

The keys stay on your computer in `app/data/settings.json`.

## Reading the screen

- **Watchlist (left).** Switch between Crypto, US and India. The badge shows how many signals
  are active for each symbol. It turns green if any of them is one the memory says to trade.
- **Chart (centre).** Choose a timeframe from 1m to 1D. Arrows mark active strategy signals;
  faint dots mark finished ones. When you select a signal, its entry, stop and target are
  drawn on the chart. Your paper positions and orders are drawn as lines too.
- **Strategy signals (right).** Each card shows:
  - the concept and its timeframe, BUY or SELL, and entry, stop and target;
  - **Memory**, the record of every past setup of the same kind (concept, timeframe,
    direction, daily trend):
    - **Trade**: at least 30 past setups, averaging at least +0.05R after costs, reliably
      above zero.
    - **Skip**: the record says this kind of setup has lost money.
    - **Untested**: not enough history in this market yet.
- **Paper trade this.** Fills the order ticket from the signal: a limit or market entry, its
  stop and target, sized to risk 0.5% of the account. You can change anything before you
  place it.
  - If a tight stop would need more money than the account has, the size is reduced to fit,
    and the app tells you.
  - "Paper trade late" enters a signal that is already running, at today's price.
- **Order ticket.** Choose buy or sell, then market, limit or stop. Size by risk % or by
  quantity. The ticket shows the risk, reward:risk and estimated fees.
- **Bottom tabs:**
  - **Positions:** live P&L in money and in R. You can edit the stop-loss and target, or
    close the position.
  - **Open orders:** cancel any of them.
  - **History:** closed trades.
  - **Accounts:** equity, fees, and a reset to the starting cash.

R is profit in units of the risk you took: −1R is a full stop-out, and +2R is twice the risk
won.

## How paper fills work

- **Market orders** fill at the live price, plus slippage.
- **Limit and stop orders** fill when the live price reaches them.
- **Stop-loss and target** are checked on every price update and every finished minute. If
  one minute touched both, the stop counts (worst case, as in the backtests).
- **Fees per market:**
  - **Crypto** (Delta Exchange India): 0.05% for market orders, 0.02% for limit orders, plus
    funding every 8 hours.
  - **US:** $0 commission, plus the SEC/FINRA fees on sells.
  - **India intraday:** brokerage (₹20 or 0.03%, whichever is lower), STT 0.025% on the sell,
    exchange and SEBI charges, stamp duty, and GST.
  - **India delivery:** positions held overnight are charged delivery rates.
- **Buying power:** crypto 3× equity, US 1×, India 5× (intraday).
- **Indian shorts** are intraday only and close automatically at 15:20 IST.

Starting cash is $10,000 for crypto, $10,000 for US and ₹10,00,000 for India.

Everything is saved in `app/data/paper.db`. Every order, fill and exit is also written to a
tamper-evident journal, `app/data/journal.log` (each line is chained to the one before by a
hash).

## For developers

- The signal engine (`engine/`) is a line-for-line port of the C++ backtest engine.
  `tests/test_parity.py` checks it reproduces the C++ results exactly.
- Run the tests from `trading-agent/`: `python -m unittest discover -s app/tests -t .`
- The charts use TradingView Lightweight Charts™ (Apache 2.0, vendored in `web/`).
