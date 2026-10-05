#!/usr/bin/env python3
"""Downloads daily candles from Delta Exchange India's public API into the
date,open,high,low,close,volume CSV the agent reads (one file per symbol).

UNTESTED: written without network access. Endpoint and field names follow Delta's
public v2 API documentation (GET /v2/history/candles with resolution, symbol, start,
end in Unix seconds; result rows carry time/open/high/low/close/volume). Verify one
request by hand before relying on it. Daily candles should close at 00:00 UTC, the
cut-off Sleeve B assumes.

usage: fetch_delta_candles.py SYMBOL START(YYYY-MM-DD) END(YYYY-MM-DD) OUT.csv
"""
import csv
import datetime as dt
import json
import sys
import time
import urllib.request

BASE = "https://api.india.delta.exchange/v2/history/candles"


def to_unix(day):
    return int(dt.datetime.strptime(day, "%Y-%m-%d").replace(tzinfo=dt.timezone.utc).timestamp())


def main():
    if len(sys.argv) != 5:
        sys.exit(__doc__)
    symbol, start, end, out = sys.argv[1:]
    rows = {}
    t0, t1 = to_unix(start), to_unix(end) + 86400
    step = 86400 * 1000  # stay under the per-request candle limit
    while t0 < t1:
        url = f"{BASE}?resolution=1d&symbol={symbol}&start={t0}&end={min(t0 + step, t1)}"
        with urllib.request.urlopen(urllib.request.Request(url, headers={"Accept": "application/json"})) as r:
            payload = json.load(r)
        if not payload.get("success", False):
            sys.exit(f"API error: {payload}")
        for c in payload.get("result", []):
            day = dt.datetime.fromtimestamp(int(c["time"]), dt.timezone.utc).strftime("%Y-%m-%d")
            rows[day] = (c["open"], c["high"], c["low"], c["close"], c.get("volume", 0))
        t0 += step
        time.sleep(0.5)
    with open(out, "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["date", "open", "high", "low", "close", "volume"])
        for day in sorted(rows):
            w.writerow([day, *rows[day]])
    print(f"wrote {len(rows)} candles to {out}")


if __name__ == "__main__":
    main()
