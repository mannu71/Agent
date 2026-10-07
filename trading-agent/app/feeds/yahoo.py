"""Yahoo Finance chart API: India (NSE) prices, and US prices until Alpaca keys are added.

Polled about every 60 seconds during the session. Free and keyless, but it can lag the
exchange slightly and has no buyer/seller (taker) volume.
"""
import asyncio
import time

from ..engine.concepts import Bar
from ..markets import is_open, yahoo_symbol

URL = "https://query1.finance.yahoo.com/v8/finance/chart/{sym}"
HEADERS = {"User-Agent": "Mozilla/5.0"}


def parse_chart(js, minutes):
    """Bars (minutes since epoch) and the last traded price from a chart response."""
    res = js["chart"]["result"][0]
    q = res["indicators"]["quote"][0]
    out = []
    for k, ts in enumerate(res.get("timestamp") or []):
        o, h, l, c, v = (q[f][k] for f in ("open", "high", "low", "close", "volume"))
        if None in (o, h, l, c):
            continue
        t = int(ts) // 60
        if minutes == 1440:
            t = (t // 1440) * 1440  # daily bars keyed by UTC day, as the engine expects
        out.append(Bar(t, float(o), float(h), float(l), float(c), float(v or 0), 0.0))
    return out, res.get("meta", {}).get("regularMarketPrice")


class YahooFeed:
    def __init__(self, market, symbols, sink, poll_s=60):
        self.market, self.symbols, self.sink, self.poll_s = market, list(symbols), sink, poll_s
        self.status = "starting"

    async def chart(self, session, sym, interval, rng):
        params = {"interval": interval, "range": rng}
        async with session.get(URL.format(sym=yahoo_symbol(self.market, sym)), params=params, headers=HEADERS,
                               timeout=30) as r:
            r.raise_for_status()
            return parse_chart(await r.json(content_type=None), {"1m": 1, "15m": 15, "1d": 1440}[interval])

    async def backfill(self, session, sym):
        m1, _ = await self.chart(session, sym, "1m", "5d")
        m15, _ = await self.chart(session, sym, "15m", "60d")
        d1, _ = await self.chart(session, sym, "1d", "2y")
        now = int(time.time() // 60)
        return [b for b in m1 if b.t + 1 <= now], [b for b in m15 if b.t + 15 <= now], d1

    async def run(self, session, last_minute):
        delay = self.poll_s
        while True:
            try:
                if is_open(self.market) or self.status == "starting":
                    now = int(time.time() // 60)
                    for sym in self.symbols:
                        bars, price = await self.chart(session, sym, "1m", "1d")
                        since = last_minute(sym)
                        for b in bars:
                            if since is not None and b.t <= since:
                                continue
                            await self.sink(self.market, sym, b, b.t + 1 <= now)
                    self.status = "delayed ~1 min" if is_open(self.market) else "market closed"
                else:
                    self.status = "market closed"
                await self.sink(self.market, None, None, None)
                delay = self.poll_s
            except asyncio.CancelledError:
                raise
            except Exception as e:
                self.status = f"retrying ({type(e).__name__})"
                await self.sink(self.market, None, None, None)
                delay = min(300, delay * 2)
            await asyncio.sleep(delay)
