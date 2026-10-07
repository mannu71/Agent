"""US stock prices from Alpaca's free market-data API (IEX feed). Needs the user's free keys.

REST  https://data.alpaca.markets/v2/stocks/bars       history and gap backfill
WS    wss://stream.data.alpaca.markets/v2/iex           live minute bars and trades
Only the regular session (9:30-16:00 ET) is kept.
"""
import asyncio
import datetime as dt
import json
import time

from ..engine.concepts import Bar
from ..markets import is_open

REST = "https://data.alpaca.markets/v2/stocks/bars"
WS = "wss://stream.data.alpaca.markets/v2/iex"


def _minutes(iso):
    return int(dt.datetime.fromisoformat(iso.replace("Z", "+00:00")).timestamp()) // 60


def parse_bar(b):
    return Bar(_minutes(b["t"]), float(b["o"]), float(b["h"]), float(b["l"]), float(b["c"]), float(b["v"]), 0.0)


def regular(bar, minutes=1):
    return is_open("us", bar.t * 60) and is_open("us", (bar.t + minutes) * 60 - 1)


class AlpacaFeed:
    market = "us"

    def __init__(self, symbols, sink, key, secret):
        self.symbols, self.sink = list(symbols), sink
        self.headers = {"APCA-API-KEY-ID": key, "APCA-API-SECRET-KEY": secret}
        self.status = "starting"

    async def bars(self, session, timeframe, start, minutes):
        out = {s: [] for s in self.symbols}
        params = {"symbols": ",".join(self.symbols), "timeframe": timeframe, "feed": "iex", "limit": "10000",
                  "start": start.strftime("%Y-%m-%dT%H:%M:%SZ"), "adjustment": "split"}
        while True:
            async with session.get(REST, params=params, headers=self.headers, timeout=60) as r:
                r.raise_for_status()
                js = await r.json()
            for sym, rows in (js.get("bars") or {}).items():
                out[sym].extend(parse_bar(b) for b in rows)
            if not js.get("next_page_token"):
                break
            params["page_token"] = js["next_page_token"]
        now = int(time.time() // 60)
        return {s: [b for b in v if b.t + minutes <= now and (minutes >= 1440 or regular(b, minutes))]
                for s, v in out.items()}

    async def backfill_all(self, session):
        utc = dt.timezone.utc
        now = dt.datetime.now(utc)
        m1 = await self.bars(session, "1Min", now - dt.timedelta(days=5), 1)
        m15 = await self.bars(session, "15Min", now - dt.timedelta(days=100), 15)
        d1 = await self.bars(session, "1Day", now - dt.timedelta(days=500), 1440)
        for s in d1:
            for b in d1[s]:
                b.t = (b.t // 1440) * 1440
        return {s: (m1[s], m15[s], d1[s]) for s in self.symbols}

    async def run(self, session, last_minute):
        delay = 1
        while True:
            try:
                since = min((last_minute(s) or 0) for s in self.symbols)
                if since:
                    start = dt.datetime.fromtimestamp((since + 1) * 60, dt.timezone.utc)
                    for s, rows in (await self.bars(session, "1Min", start, 1)).items():
                        for b in rows:
                            await self.sink(self.market, s, b, True)
                async with session.ws_connect(WS, heartbeat=30, timeout=30) as ws:
                    await ws.send_str(json.dumps({"action": "auth", "key": self.headers["APCA-API-KEY-ID"],
                                                  "secret": self.headers["APCA-API-SECRET-KEY"]}))
                    await ws.send_str(json.dumps({"action": "subscribe", "bars": self.symbols, "trades": self.symbols}))
                    self.status, delay = "live" if is_open("us") else "market closed", 1
                    await self.sink(self.market, None, None, None)
                    async for m in ws:
                        if m.type.name != "TEXT":
                            break
                        for ev in json.loads(m.data):
                            if ev.get("T") == "error":
                                raise RuntimeError(ev.get("msg", "alpaca error"))
                            if ev.get("T") == "b":
                                b = parse_bar(ev)
                                if regular(b):
                                    await self.sink(self.market, ev["S"], b, True)
                            elif ev.get("T") == "t" and is_open("us"):
                                t = _minutes(ev["t"])
                                p = float(ev["p"])
                                await self.sink(self.market, ev["S"], Bar(t, p, p, p, p, 0.0, 0.0), False)
            except asyncio.CancelledError:
                raise
            except Exception as e:
                self.status = f"reconnecting ({type(e).__name__}: {str(e)[:60]})"
                await self.sink(self.market, None, None, None)
            await asyncio.sleep(delay)
            delay = min(60, delay * 2)
