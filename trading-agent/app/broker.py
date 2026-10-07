"""Paper broker: accounts per market, orders, brackets, fills on live prices, fees.

Nothing here talks to an exchange. Prices arrive through `on_price` (live ticks and closed
1-minute bars); orders fill against them with the same worst-case rules as the backtest
engine: if one price update touches both a position's stop and its target, the stop wins.
State lives in SQLite (survives restarts) and every action is appended to a hash-chained
journal (`ta_paper verify` format). Times are epoch seconds (UTC).
"""
from __future__ import annotations

import datetime as dt
import math
import os
import sqlite3
from dataclasses import dataclass
from zoneinfo import ZoneInfo

from .journal import Journal

IST = ZoneInfo("Asia/Kolkata")


@dataclass(frozen=True)
class MarketSpec:
    name: str
    currency: str
    start_cash: float
    qty_step: float        # smallest tradable quantity
    max_leverage: float    # position notional <= equity x this
    slippage: float        # adverse fraction on market and stop fills
    shorts: str            # "yes", "intraday" (squared off 15:20 IST) or "no"


MARKETS = {
    "crypto": MarketSpec("crypto", "USD", 10_000.0, 0.0001, 3.0, 0.0002, "yes"),
    "us": MarketSpec("us", "USD", 10_000.0, 1.0, 1.0, 0.0001, "yes"),
    "india": MarketSpec("india", "INR", 1_000_000.0, 1.0, 5.0, 0.0002, "intraday"),
}
FUNDING_PER_8H = 0.0001  # crypto perpetual funding, charged to either side (as in the backtest)


class OrderError(ValueError):
    pass


def fees(market: str, side: int, qty: float, price: float, maker: bool = False, intraday: bool = True) -> float:
    """Charges for one fill. side +1 = buy, -1 = sell."""
    notional = abs(qty) * price
    if market == "crypto":  # Delta Exchange India perpetuals
        return notional * (0.0002 if maker else 0.0005)
    if market == "us":  # $0 commission; SEC fee and FINRA TAF on sells
        if side > 0:
            return 0.0
        return notional * 27.80 / 1e6 + min(8.30, 0.000166 * abs(qty))
    # India equity (discount-broker schedule)
    exchange, sebi = notional * 0.0000297, notional * 0.000001
    if intraday:
        brokerage = min(20.0, notional * 0.0003)
        stt = notional * 0.00025 if side < 0 else 0.0
        stamp = notional * 0.00003 if side > 0 else 0.0
    else:
        brokerage = 0.0
        stt = notional * 0.001
        stamp = notional * 0.00015 if side > 0 else 0.0
    gst = 0.18 * (brokerage + exchange + sebi)
    return brokerage + stt + stamp + exchange + sebi + gst


def round_step(q: float, step: float) -> float:
    return math.floor(q / step + 1e-9) * step


SCHEMA = """
CREATE TABLE IF NOT EXISTS accounts (market TEXT PRIMARY KEY, currency TEXT, start_cash REAL, cash REAL,
  realized REAL, fees REAL, funding_t INTEGER);
CREATE TABLE IF NOT EXISTS orders (id INTEGER PRIMARY KEY AUTOINCREMENT, market TEXT, symbol TEXT, side INTEGER,
  type TEXT, qty REAL, price REAL, stop REAL, target REAL, status TEXT, created INTEGER, updated INTEGER,
  fill_price REAL, note TEXT, signal TEXT);
CREATE TABLE IF NOT EXISTS positions (market TEXT, symbol TEXT, qty REAL, avg REAL, stop REAL, target REAL,
  opened INTEGER, fees REAL, risk REAL, signal TEXT, PRIMARY KEY (market, symbol));
CREATE TABLE IF NOT EXISTS trades (id INTEGER PRIMARY KEY AUTOINCREMENT, market TEXT, symbol TEXT, side INTEGER,
  qty REAL, entry REAL, exit REAL, entry_t INTEGER, exit_t INTEGER, pnl REAL, fees REAL, r REAL, reason TEXT,
  signal TEXT);
CREATE TABLE IF NOT EXISTS equity (market TEXT, t INTEGER, equity REAL, PRIMARY KEY (market, t));
"""


class Broker:
    def __init__(self, data_dir: str, clock=None):
        os.makedirs(data_dir, exist_ok=True)
        self.db = sqlite3.connect(os.path.join(data_dir, "paper.db"))
        self.db.row_factory = sqlite3.Row
        self.db.executescript(SCHEMA)
        self.journal = Journal(os.path.join(data_dir, "journal.log"))
        self.clock = clock or (lambda: int(dt.datetime.now(dt.timezone.utc).timestamp()))
        self.last = {}  # (market, symbol) -> (t, price)
        self.listeners = []
        for m, spec in MARKETS.items():
            if not self.db.execute("SELECT 1 FROM accounts WHERE market=?", (m,)).fetchone():
                self.db.execute("INSERT INTO accounts VALUES (?,?,?,?,0,0,NULL)",
                                (m, spec.currency, spec.start_cash, spec.start_cash))
                self.journal.append("account_open", market=m, currency=spec.currency, cash=spec.start_cash)
        self.db.commit()

    # ------------------------------------------------------------------ queries
    def account(self, market):
        a = dict(self.db.execute("SELECT * FROM accounts WHERE market=?", (market,)).fetchone())
        unreal = sum(p["unrealised"] for p in self.positions(market))
        a["unrealised"] = unreal
        a["equity"] = a["cash"] + unreal
        return a

    def positions(self, market=None):
        rows = self.db.execute("SELECT * FROM positions" + (" WHERE market=?" if market else ""),
                               (market,) if market else ()).fetchall()
        out = []
        for r in rows:
            p = dict(r)
            px = self.last.get((p["market"], p["symbol"]), (0, p["avg"]))[1]
            p["last"] = px
            p["unrealised"] = (px - p["avg"]) * p["qty"]
            p["r"] = p["unrealised"] / (abs(p["qty"]) * p["risk"]) if p["risk"] else None
            out.append(p)
        return out

    def orders(self, market=None, status="open"):
        q = "SELECT * FROM orders WHERE status=?" + (" AND market=?" if market else "") + " ORDER BY id DESC"
        return [dict(r) for r in self.db.execute(q, (status, market) if market else (status,)).fetchall()]

    def trades(self, market=None, limit=200):
        q = "SELECT * FROM trades" + (" WHERE market=?" if market else "") + " ORDER BY id DESC LIMIT ?"
        return [dict(r) for r in self.db.execute(q, (market, limit) if market else (limit,)).fetchall()]

    def equity_curve(self, market):
        return [dict(r) for r in self.db.execute("SELECT t, equity FROM equity WHERE market=? ORDER BY t", (market,))]

    # ------------------------------------------------------------------ actions
    def place_order(self, market, symbol, side, type="market", qty=None, risk_pct=None, price=None, stop=None,
                    target=None, signal=None, market_open=True):
        spec = MARKETS.get(market)
        if spec is None:
            raise OrderError(f"unknown market {market}")
        side = 1 if side in (1, "buy", "long") else -1 if side in (-1, "sell", "short") else 0
        if not side:
            raise OrderError("side must be buy or sell")
        if type not in ("market", "limit", "stop"):
            raise OrderError("order type must be market, limit or stop")
        last = self.last.get((market, symbol))
        if type == "market":
            if not last:
                raise OrderError(f"no live price for {symbol} yet")
            if not market_open:
                raise OrderError("market is closed: use a limit or stop order")
            ref = last[1]
        else:
            if not price or price <= 0:
                raise OrderError("limit and stop orders need a price")
            ref = price
        if stop is not None and side * (ref - stop) <= 0:
            raise OrderError("stop-loss must be below the entry for a buy and above it for a sell")
        if target is not None and side * (target - ref) <= 0:
            raise OrderError("target must be above the entry for a buy and below it for a sell")
        acct = self.account(market)
        pos = self._position(market, symbol)
        note = None
        if qty is None:
            if not risk_pct or stop is None:
                raise OrderError("give a quantity, or a risk % with a stop-loss")
            qty = acct["equity"] * risk_pct / 100.0 / abs(ref - stop)
            # A tight stop can ask for more than the account can hold: size down to fit.
            held = pos["qty"] if pos else 0.0
            room = acct["equity"] * spec.max_leverage / ref - (abs(held) if held * side > 0 else -abs(held))
            if qty > room:
                qty = max(0.0, room)
                note = (f"size cut to buying power: risk {qty * abs(ref - stop) / acct['equity'] * 100:.2f}% "
                        f"instead of {risk_pct:g}%")
        qty = round_step(float(qty), spec.qty_step)
        if qty <= 0:
            raise OrderError("quantity rounds to zero")
        new_qty = (pos["qty"] if pos else 0.0) + side * qty
        if spec.shorts == "no" and new_qty < 0:
            raise OrderError("short selling is not allowed in this market")
        if spec.shorts == "intraday" and new_qty < 0:
            local = dt.datetime.fromtimestamp(self.clock(), IST)
            if local.hour * 60 + local.minute >= 15 * 60 + 20:
                raise OrderError("intraday shorts close at 15:20 IST; no new shorts after that")
        if abs(new_qty) * ref > acct["equity"] * spec.max_leverage + 1e-9:
            raise OrderError(f"not enough buying power: max {spec.max_leverage:g}x equity")
        now = self.clock()
        cur = self.db.execute(
            "INSERT INTO orders (market,symbol,side,type,qty,price,stop,target,status,created,updated,signal,note) "
            "VALUES (?,?,?,?,?,?,?,?,'open',?,?,?,?)",
            (market, symbol, side, type, qty, price, stop, target, now, now, signal, note))
        oid = cur.lastrowid
        self.journal.append("order", id=oid, market=market, symbol=symbol, side=side, type=type, qty=qty,
                            price=price or 0.0, stop=stop or 0.0, target=target or 0.0, signal=signal or "")
        self.db.commit()
        if type == "market":
            px = last[1] * (1 + side * spec.slippage)
            self._fill(oid, px, now, maker=False)
        self._emit()
        return self._order(oid)

    def cancel_order(self, oid):
        o = self._order(oid)
        if not o or o["status"] != "open":
            raise OrderError("order is not open")
        self.db.execute("UPDATE orders SET status='cancelled', updated=? WHERE id=?", (self.clock(), oid))
        self.journal.append("cancel", id=oid)
        self.db.commit()
        self._emit()

    def modify_position(self, market, symbol, stop=None, target=None):
        p = self._position(market, symbol)
        if not p:
            raise OrderError("no open position")
        side = 1 if p["qty"] > 0 else -1
        ref = self.last.get((market, symbol), (0, p["avg"]))[1]
        if stop is not None and side * (ref - stop) <= 0:
            raise OrderError("stop-loss is already through the current price")
        if target is not None and side * (target - ref) <= 0:
            raise OrderError("target is already through the current price")
        self.db.execute("UPDATE positions SET stop=?, target=? WHERE market=? AND symbol=?",
                        (stop, target, market, symbol))
        self.journal.append("bracket", market=market, symbol=symbol, stop=stop or 0.0, target=target or 0.0)
        self.db.commit()
        self._emit()

    def close_position(self, market, symbol, reason="manual"):
        p = self._position(market, symbol)
        if not p:
            raise OrderError("no open position")
        last = self.last.get((market, symbol))
        if not last:
            raise OrderError("no live price")
        side = -1 if p["qty"] > 0 else 1
        px = last[1] * (1 + side * MARKETS[market].slippage)
        self._close(market, symbol, abs(p["qty"]), px, self.clock(), reason, maker=False)
        self._emit()

    def reset(self, market):
        spec = MARKETS[market]
        for t in ("orders", "positions", "trades", "equity"):
            self.db.execute(f"DELETE FROM {t} WHERE market=?", (market,))
        self.db.execute("UPDATE accounts SET cash=?, start_cash=?, realized=0, fees=0, funding_t=NULL WHERE market=?",
                        (spec.start_cash, spec.start_cash, market))
        self.journal.append("account_reset", market=market, cash=spec.start_cash)
        self.db.commit()
        self._emit()

    # ------------------------------------------------------------------ prices
    def on_price(self, market, symbol, t, last, high=None, low=None, bar_start=None):
        """A live tick (high = low = last) or a closed 1-minute bar's range. A bar's range
        only applies to orders and positions that existed when the bar began (`bar_start`,
        epoch seconds): an order placed mid-minute must not fill on prices from before it."""
        high = last if high is None else high
        low = last if low is None else low
        full = (high, low)

        def rng(created):
            return full if bar_start is None or created <= bar_start else (last, last)
        self.last[(market, symbol)] = (t, last)
        changed = filled_now = False
        spec = MARKETS[market]
        for o in self.orders(market):
            if o["symbol"] != symbol:
                continue
            side, p = o["side"], o["price"]
            high, low = rng(o["created"])
            if o["type"] == "limit" and ((low <= p) if side > 0 else (high >= p)):
                fill = min(p, last) if (side > 0 and high == low) else max(p, last) if (side < 0 and high == low) else p
                self._fill(o["id"], fill, t, maker=True)
                changed = filled_now = True
            elif o["type"] == "stop" and ((high >= p) if side > 0 else (low <= p)):
                base = max(p, last) if side > 0 else min(p, last)
                self._fill(o["id"], base * (1 + side * spec.slippage), t, maker=False)
                changed = filled_now = True
        pos = self._position(market, symbol)
        if pos:
            side = 1 if pos["qty"] > 0 else -1
            stop, target = pos["stop"], pos["target"]
            high, low = rng(pos["opened"])
            if stop is not None and ((low <= stop) if side > 0 else (high >= stop)):
                base = min(stop, last) if side > 0 else max(stop, last)  # a gap through the stop fills worse
                self._close(market, symbol, abs(pos["qty"]), base * (1 - side * spec.slippage), t, "stop", maker=False)
                changed = True
            # After a fill inside this price range the target may have traded before the fill.
            elif target is not None and not (filled_now and high != low) and \
                    ((high >= target) if side > 0 else (low <= target)):
                self._close(market, symbol, abs(pos["qty"]), target, t, "target", maker=True)
                changed = True
        changed |= self._housekeeping(market, t)
        if changed:
            self._emit()
        return changed

    def _housekeeping(self, market, t):
        changed = False
        if market == "crypto":  # funding at 00:00, 08:00, 16:00 UTC
            a = self.db.execute("SELECT funding_t FROM accounts WHERE market='crypto'").fetchone()
            boundary = (t // 28800) * 28800
            if a["funding_t"] is None:
                self.db.execute("UPDATE accounts SET funding_t=? WHERE market='crypto'", (boundary,))
            elif boundary > a["funding_t"]:
                for p in self.positions("crypto"):
                    cost = abs(p["qty"]) * p["last"] * FUNDING_PER_8H * ((boundary - a["funding_t"]) // 28800)
                    self.db.execute("UPDATE accounts SET cash=cash-?, fees=fees+? WHERE market='crypto'", (cost, cost))
                    self.db.execute("UPDATE positions SET fees=fees+? WHERE market='crypto' AND symbol=?",
                                    (cost, p["symbol"]))
                    self.journal.append("funding", symbol=p["symbol"], cost=cost)
                    changed = True
                self.db.execute("UPDATE accounts SET funding_t=? WHERE market='crypto'", (boundary,))
            self.db.commit()
        if market == "india":  # intraday shorts are squared off at 15:20 IST
            local = dt.datetime.fromtimestamp(t, IST)
            if local.hour * 60 + local.minute >= 15 * 60 + 20:
                for p in self.positions("india"):
                    opened = dt.datetime.fromtimestamp(p["opened"], IST).date()
                    if p["qty"] < 0 and opened <= local.date():
                        px = p["last"] * (1 + MARKETS["india"].slippage)
                        self._close("india", p["symbol"], abs(p["qty"]), px, t, "square_off", maker=False)
                        changed = True
        minute = (t // 60) * 60
        self.db.execute("INSERT OR REPLACE INTO equity VALUES (?,?,?)", (market, minute, self.account(market)["equity"]))
        self.db.commit()
        return changed

    # ------------------------------------------------------------------ internals
    def _order(self, oid):
        r = self.db.execute("SELECT * FROM orders WHERE id=?", (oid,)).fetchone()
        return dict(r) if r else None

    def _position(self, market, symbol):
        r = self.db.execute("SELECT * FROM positions WHERE market=? AND symbol=?", (market, symbol)).fetchone()
        return dict(r) if r else None

    def _fill(self, oid, px, t, maker):
        o = self._order(oid)
        market, symbol, side, qty = o["market"], o["symbol"], o["side"], o["qty"]
        self.db.execute("UPDATE orders SET status='filled', fill_price=?, updated=? WHERE id=?", (px, t, oid))
        self.journal.append("fill", id=oid, price=px, t=t)
        pos = self._position(market, symbol)
        remaining = qty
        if pos and pos["qty"] * side < 0:  # reduces or flips an opposite position
            closing = min(abs(pos["qty"]), qty)
            self._close(market, symbol, closing, px, t, "order", maker=maker)
            remaining = qty - closing
        if remaining > 1e-12:
            fee = fees(market, side, remaining, px, maker=maker)
            pos = self._position(market, symbol)
            if pos:  # add to a same-side position
                total = pos["qty"] + side * remaining
                avg = (pos["avg"] * abs(pos["qty"]) + px * remaining) / abs(total)
                self.db.execute("UPDATE positions SET qty=?, avg=?, fees=fees+? WHERE market=? AND symbol=?",
                                (total, avg, fee, market, symbol))
                if o["stop"] is not None or o["target"] is not None:
                    self.db.execute("UPDATE positions SET stop=?, target=? WHERE market=? AND symbol=?",
                                    (o["stop"], o["target"], market, symbol))
            else:
                risk = abs(px - o["stop"]) if o["stop"] is not None else None
                self.db.execute("INSERT INTO positions VALUES (?,?,?,?,?,?,?,?,?,?)",
                                (market, symbol, side * remaining, px, o["stop"], o["target"], t, fee, risk, o["signal"]))
            self.db.execute("UPDATE accounts SET cash=cash-?, fees=fees+? WHERE market=?", (fee, fee, market))
        self.db.commit()

    def _close(self, market, symbol, qty, px, t, reason, maker):
        pos = self._position(market, symbol)
        side = 1 if pos["qty"] > 0 else -1
        intraday = True
        if market == "india":
            intraday = dt.datetime.fromtimestamp(pos["opened"], IST).date() == dt.datetime.fromtimestamp(t, IST).date()
        fee = fees(market, -side, qty, px, maker=maker, intraday=intraday)
        if market == "india" and not intraday:  # entry leg re-priced at delivery charges
            fee += fees(market, side, qty, pos["avg"], intraday=False) - fees(market, side, qty, pos["avg"])
        share = qty / abs(pos["qty"])
        entry_fees = pos["fees"] * share
        gross = (px - pos["avg"]) * qty * side
        pnl = gross - fee - entry_fees
        r = pnl / (qty * pos["risk"]) if pos["risk"] else None
        self.db.execute("UPDATE accounts SET cash=cash+?, realized=realized+?, fees=fees+? WHERE market=?",
                        (gross - fee, pnl, fee, market))
        self.db.execute("INSERT INTO trades (market,symbol,side,qty,entry,exit,entry_t,exit_t,pnl,fees,r,reason,signal) "
                        "VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?)",
                        (market, symbol, side, qty, pos["avg"], px, pos["opened"], t, pnl, fee + entry_fees, r, reason,
                         pos["signal"]))
        left = pos["qty"] - side * qty
        if abs(left) < 1e-12:
            self.db.execute("DELETE FROM positions WHERE market=? AND symbol=?", (market, symbol))
        else:
            self.db.execute("UPDATE positions SET qty=?, fees=fees-? WHERE market=? AND symbol=?",
                            (left, entry_fees, market, symbol))
        self.journal.append("exit", market=market, symbol=symbol, qty=qty, price=px, pnl=pnl, reason=reason,
                            r=r if r is not None else "")
        self.db.commit()

    def _emit(self):
        for fn in list(self.listeners):
            fn()
