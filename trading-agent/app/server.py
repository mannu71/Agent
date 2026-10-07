"""Concept Desk: local paper-trading app. Run:  python -m app.server  (or start.bat on Windows).

Serves the trading UI on http://localhost:8765, streams live prices from the feeds, runs the
strategy signals and fills paper orders. Paper only: nothing is ever sent to a broker.
"""
import argparse
import asyncio
import json
import os
import time
import webbrowser

from aiohttp import ClientSession, WSMsgType, web

from .broker import MARKETS, Broker, OrderError
from .engine.concepts import Bar, ConceptConfig
from .feeds.alpaca import AlpacaFeed
from .feeds.binance import BinanceFeed
from .feeds.yahoo import YahooFeed
from .markets import LABELS, WATCHLISTS, anchor_fn, is_open
from .signals import SignalEngine
from .store import Series

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
DATA = os.path.join(HERE, "data")
WEB = os.path.join(HERE, "web")
TFS = {"1m": 1, "5m": 5, "15m": 15, "1h": 60, "4h": 240, "1D": 1440}


def load_settings():
    path = os.path.join(DATA, "settings.json")
    s = {"alpaca_key": "", "alpaca_secret": "", "risk_pct": 0.5, "watchlists": WATCHLISTS}
    if os.path.exists(path):
        with open(path) as f:
            s.update(json.load(f))
    return s


def save_settings(s):
    os.makedirs(DATA, exist_ok=True)
    with open(os.path.join(DATA, "settings.json"), "w") as f:
        json.dump(s, f, indent=2)


def bar_json(b):
    return {"time": b.t * 60, "open": b.open, "high": b.high, "low": b.low, "close": b.close, "volume": b.volume}


class App:
    def __init__(self):
        self.settings = load_settings()
        self.broker = Broker(DATA)
        self.engine = SignalEngine(ConceptConfig.load(os.path.join(ROOT, "knowledge", "concepts.cfg")),
                                   os.path.join(ROOT, "knowledge"), DATA)
        self.series = {}     # (market, symbol) -> Series
        self.feeds = {}      # market -> feed
        self.tasks = {}
        self.status = {m: "loading history" for m in MARKETS}
        self.clients = {}    # ws -> {"market", "symbol", "tf"}
        self.session = None
        self.broker.listeners.append(self.on_broker)
        self._broker_dirty = False
        self._price_dirty = set()

    # ------------------------------------------------------------------ startup
    async def start(self, app):
        self.session = ClientSession()
        for market in MARKETS:
            asyncio.create_task(self.start_market(market))
        asyncio.create_task(self.flush_loop())

    async def stop(self, app):
        for t in self.tasks.values():
            t.cancel()
        await self.session.close()

    async def start_market(self, market):
        syms = list(self.settings["watchlists"].get(market, []))
        for s in syms:
            self.series[(market, s)] = Series(market, s, anchor_fn(market))
        try:
            if market == "crypto":
                feed = BinanceFeed(syms, self.sink)
                for s in syms:
                    m1, m15, d1 = await feed.backfill(self.session, s)
                    self.series[(market, s)].seed(m1, m15, d1)
            elif market == "us" and self.settings.get("alpaca_key"):
                feed = AlpacaFeed(syms, self.sink, self.settings["alpaca_key"], self.settings["alpaca_secret"])
                for s, (m1, m15, d1) in (await feed.backfill_all(self.session)).items():
                    self.series[(market, s)].seed(m1, m15, d1)
            else:
                feed = YahooFeed(market, syms, self.sink)
                for s in syms:
                    m1, m15, d1 = await feed.backfill(self.session, s)
                    self.series[(market, s)].seed(m1, m15, d1)
        except Exception as e:
            self.status[market] = f"history failed: {type(e).__name__} {str(e)[:80]}"
            await self.broadcast({"type": "status", "status": self.feed_status()})
            await asyncio.sleep(30)
            return await self.start_market(market)
        self.feeds[market] = feed
        for s in syms:
            se = self.series[(market, s)]
            if se.last_price is not None:
                self.broker.on_price(market, s, int(time.time()), se.last_price)
        if market != "crypto":
            self.status[market] = "building memory"
            await self.broadcast({"type": "status", "status": self.feed_status()})
            await asyncio.to_thread(self.engine.build_memory, market, [self.series[(market, s)] for s in syms])
        for s in syms:
            await self.recompute(market, s)
        self.tasks[market] = asyncio.create_task(feed.run(self.session, lambda sym, m=market: self.last_minute(m, sym)))

    def last_minute(self, market, sym):
        se = self.series.get((market, sym))
        return se.m1[-1].t if se and se.m1 else None

    # ------------------------------------------------------------------ live data
    async def sink(self, market, symbol, bar, closed):
        if symbol is None:  # status change only
            self.status[market] = self.feeds[market].status if market in self.feeds else self.status[market]
            await self.broadcast({"type": "status", "status": self.feed_status()})
            return
        se = self.series.get((market, symbol))
        if se is None:
            return
        now = int(time.time())
        if not closed and se.forming is not None and se.forming.t == bar.t and bar.volume == 0:
            f = se.forming  # a trade tick: merge into the forming minute
            bar = Bar(bar.t, f.open, max(f.high, bar.close), min(f.low, bar.close), bar.close, f.volume, f.taker_buy)
        fifteen = se.update(bar, closed, now)
        if closed:
            self.broker.on_price(market, symbol, now, bar.close, bar.high, bar.low, bar_start=bar.t * 60)
        else:
            self.broker.on_price(market, symbol, now, bar.close)
        self._price_dirty.add((market, symbol))
        if fifteen:
            asyncio.create_task(self.recompute(market, symbol))

    async def recompute(self, market, symbol):
        se = self.series[(market, symbol)]
        sigs = await asyncio.to_thread(self.engine.compute, se)
        await self.broadcast({"type": "signals", "market": market, "symbol": symbol, "signals": sigs})

    def on_broker(self):
        self._broker_dirty = True

    async def flush_loop(self):
        """Pushes prices, chart updates and account changes at most twice a second."""
        while True:
            await asyncio.sleep(0.5)
            if self._price_dirty:
                dirty, self._price_dirty = self._price_dirty, set()
                await self.broadcast({"type": "prices", "prices": [self.quote(m, s) for m, s in dirty]})
                for ws, sub in list(self.clients.items()):
                    key = (sub.get("market"), sub.get("symbol"))
                    if key in dirty:
                        bars = self.series[key].chart(sub["tf"], limit=2)
                        if bars:
                            await self.send(ws, {"type": "bar", "market": key[0], "symbol": key[1], "tf": sub["tf"],
                                                 "bar": bar_json(bars[-1])})
                self._broker_dirty = True  # unrealised P&L moves with prices
            if self._broker_dirty:
                self._broker_dirty = False
                await self.broadcast({"type": "account", **self.account_state()})

    # ------------------------------------------------------------------ views
    def quote(self, market, symbol):
        se = self.series[(market, symbol)]
        prev = se.d1[-2].close if len(se.d1) >= 2 else None
        if se.d1 and se.m1 and se.d1[-1].t < (se.m1[-1].t // 1440) * 1440:
            prev = se.d1[-1].close  # today's daily bar not formed yet
        price = se.last_price
        active = [s for s in self.engine.signals.get((market, symbol), []) if s["status"] in ("pending", "open")]
        return {"market": market, "symbol": symbol, "price": price,
                "change": (price / prev - 1) * 100 if price and prev else None,
                "signals": len(active), "trade_ok": any(s["verdict"]["status"] == "trade" for s in active),
                "flow": se.has_flow}

    def feed_status(self):
        return {m: {"label": LABELS[m], "status": self.status[m], "open": is_open(m),
                    "memory": self.engine.memory_source.get(m, "")} for m in MARKETS}

    def account_state(self):
        return {"accounts": {m: self.broker.account(m) for m in MARKETS},
                "positions": self.broker.positions(), "orders": self.broker.orders(),
                "trades": self.broker.trades(limit=100),
                "journal": {"lines": self.broker.journal.seq}}

    def snapshot(self):
        return {"type": "snapshot", "markets": {m: LABELS[m] for m in MARKETS},
                "watchlists": self.settings["watchlists"], "risk_pct": self.settings["risk_pct"],
                "alpaca": bool(self.settings.get("alpaca_key")), "status": self.feed_status(),
                "quotes": [self.quote(m, s) for (m, s) in self.series],
                "signals": [x for v in self.engine.signals.values() for x in v],
                **self.account_state()}

    # ------------------------------------------------------------------ websocket
    async def send(self, ws, msg):
        try:
            await ws.send_str(json.dumps(msg, default=float))
        except Exception:
            self.clients.pop(ws, None)

    async def broadcast(self, msg):
        for ws in list(self.clients):
            await self.send(ws, msg)

    async def ws_handler(self, request):
        ws = web.WebSocketResponse(heartbeat=30)
        await ws.prepare(request)
        self.clients[ws] = {}
        await self.send(ws, self.snapshot())
        async for m in ws:
            if m.type != WSMsgType.TEXT:
                continue
            try:
                await self.handle(ws, json.loads(m.data))
            except OrderError as e:
                await self.send(ws, {"type": "error", "message": str(e)})
            except Exception as e:
                await self.send(ws, {"type": "error", "message": f"{type(e).__name__}: {e}"})
        self.clients.pop(ws, None)
        return ws

    async def handle(self, ws, msg):
        kind = msg.get("type")
        if kind == "chart":
            market, symbol, tf = msg["market"], msg["symbol"], TFS.get(msg.get("tf", "15m"), 15)
            self.clients[ws] = {"market": market, "symbol": symbol, "tf": tf}
            se = self.series.get((market, symbol))
            bars = se.chart(tf) if se else []
            await self.send(ws, {"type": "chart", "market": market, "symbol": symbol, "tf": tf,
                                 "bars": [bar_json(b) for b in bars],
                                 "signals": self.engine.signals.get((market, symbol), [])})
        elif kind == "order":
            o = self.broker.place_order(
                msg["market"], msg["symbol"], msg["side"], msg.get("order_type", "market"),
                qty=float(msg["qty"]) if msg.get("qty") else None,
                risk_pct=float(msg["risk_pct"]) if msg.get("risk_pct") else None,
                price=float(msg["price"]) if msg.get("price") else None,
                stop=float(msg["stop"]) if msg.get("stop") else None,
                target=float(msg["target"]) if msg.get("target") else None,
                signal=msg.get("signal"), market_open=is_open(msg["market"]))
            extra = f" ({o['note']})" if o.get("note") else ""
            await self.send(ws, {"type": "notice", "message": f"Paper order #{o['id']} {o['status']}{extra}"})
        elif kind == "cancel":
            self.broker.cancel_order(int(msg["id"]))
        elif kind == "close":
            self.broker.close_position(msg["market"], msg["symbol"])
        elif kind == "bracket":
            self.broker.modify_position(msg["market"], msg["symbol"],
                                        float(msg["stop"]) if msg.get("stop") else None,
                                        float(msg["target"]) if msg.get("target") else None)
        elif kind == "reset":
            self.broker.reset(msg["market"])
        elif kind == "settings":
            restart_us = (msg.get("alpaca_key", self.settings["alpaca_key"]) != self.settings["alpaca_key"])
            for k in ("alpaca_key", "alpaca_secret", "risk_pct"):
                if k in msg:
                    self.settings[k] = msg[k]
            save_settings(self.settings)
            await self.send(ws, {"type": "notice", "message": "Settings saved"})
            if restart_us:
                if "us" in self.tasks:
                    self.tasks["us"].cancel()
                self.status["us"] = "loading history"
                asyncio.create_task(self.start_market("us"))


async def index(request):
    return web.FileResponse(os.path.join(WEB, "index.html"))


def make_app():
    a = App()
    app = web.Application()
    app.on_startup.append(a.start)
    app.on_cleanup.append(a.stop)
    app.router.add_get("/", index)
    app.router.add_get("/ws", a.ws_handler)
    app.router.add_static("/static", WEB)
    app["desk"] = a
    return app


def main():
    ap = argparse.ArgumentParser(description="Concept Desk paper-trading app")
    ap.add_argument("--port", type=int, default=8765)
    ap.add_argument("--no-browser", action="store_true")
    args = ap.parse_args()
    app = make_app()
    if not args.no_browser:
        async def open_browser(_):
            asyncio.get_running_loop().call_later(1.5, webbrowser.open, f"http://localhost:{args.port}")
        app.on_startup.append(open_browser)
    print(f"Concept Desk running on http://localhost:{args.port}  (paper trading only; Ctrl+C to stop)")
    web.run_app(app, host="127.0.0.1", port=args.port, print=None)


if __name__ == "__main__":
    main()
