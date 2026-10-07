"""Crypto prices from Binance's public market-data endpoints (no account needed).

REST  https://data-api.binance.vision/api/v3/klines   history and gap backfill
WS    wss://data-stream.binance.vision/stream          live 1-minute klines (updates each second)
Klines carry taker-buy volume, so order-flow concepts work for crypto.
"""
import asyncio
import json
import time

from ..engine.concepts import Bar

REST = "https://data-api.binance.vision/api/v3/klines"
WS = "wss://data-stream.binance.vision/stream?streams="


def parse_rest(row):
    return Bar(int(row[0]) // 60000, float(row[1]), float(row[2]), float(row[3]), float(row[4]), float(row[5]),
               float(row[9]))


def parse_ws(msg):
    """(symbol, Bar, closed) from a combined-stream kline message, or None."""
    d = msg.get("data", msg)
    if d.get("e") != "kline":
        return None
    k = d["k"]
    sym = k["s"][:-4] if k["s"].endswith("USDT") else k["s"]
    bar = Bar(int(k["t"]) // 60000, float(k["o"]), float(k["h"]), float(k["l"]), float(k["c"]), float(k["v"]),
              float(k["V"]))
    return sym, bar, bool(k["x"])


class BinanceFeed:
    market = "crypto"

    def __init__(self, symbols, sink):
        self.symbols, self.sink = list(symbols), sink
        self.status = "starting"

    async def klines(self, session, sym, interval, start_ms=None, limit=1000):
        params = {"symbol": f"{sym}USDT", "interval": interval, "limit": str(limit)}
        if start_ms is not None:
            params["startTime"] = str(start_ms)
        async with session.get(REST, params=params, timeout=30) as r:
            r.raise_for_status()
            return [parse_rest(x) for x in await r.json()]

    async def history(self, session, sym, interval, minutes_back, step_min):
        """Closed bars from now - minutes_back, paging 1,000 at a time."""
        now_ms = int(time.time() * 1000)
        start = now_ms - minutes_back * 60_000
        out = []
        while start < now_ms:
            rows = await self.klines(session, sym, interval, start)
            if not rows:
                break
            out.extend(rows)
            start = (rows[-1].t + step_min) * 60_000
            if len(rows) < 1000:
                break
        cutoff = now_ms // 60_000
        return [b for b in out if b.t + step_min <= cutoff]  # drop the bar still forming

    async def backfill(self, session, sym):
        m1 = await self.history(session, sym, "1m", 3 * 1440, 1)
        m15 = await self.history(session, sym, "15m", 100 * 1440, 15)
        d1 = await self.history(session, sym, "1d", 400 * 1440, 1440)
        return m1, m15, d1

    async def gap(self, session, sym, since_min):
        rows = await self.history(session, sym, "1m", max(1, int(time.time() // 60) - since_min), 1)
        return [b for b in rows if b.t > since_min]

    async def run(self, session, last_minute):
        """Streams forever; after any disconnect backfills the gap, then reconnects."""
        url = WS + "/".join(f"{s.lower()}usdt@kline_1m" for s in self.symbols)
        delay = 1
        while True:
            try:
                for s in self.symbols:
                    since = last_minute(s)
                    if since is not None:
                        for b in await self.gap(session, s, since):
                            await self.sink(self.market, s, b, True)
                async with session.ws_connect(url, heartbeat=30, timeout=30) as ws:
                    self.status, delay = "live", 1
                    await self.sink(self.market, None, None, None)
                    async for m in ws:
                        if m.type.name != "TEXT":
                            break
                        p = parse_ws(json.loads(m.data))
                        if p:
                            await self.sink(self.market, p[0], p[1], p[2])
            except asyncio.CancelledError:
                raise
            except Exception as e:  # network trouble: show it, back off, retry
                self.status = f"reconnecting ({type(e).__name__})"
                await self.sink(self.market, None, None, None)
            await asyncio.sleep(delay)
            delay = min(60, delay * 2)
