#!/usr/bin/env python3
"""Downloads Delta Exchange India funding history as the date,value file Sleeve B reads
(value = the day's total funding as a fraction; positive = longs pay).

usage: fetch_delta_funding.py SYMBOL START(YYYY-MM-DD) OUT.csv     e.g. BTCUSD 2024-01-01

Delta publishes a daily candle of the funding rate under FUNDING:<SYMBOL>, quoted in
percent per 8-hour interval (product_specs.rate_exchange_interval = 28800 s). The day's
total is taken as 3 x the daily mean of open/high/low/close, divided by 100. History
starts with the India venue's launch (BTCUSD: December 2023).
"""
import csv
import datetime as dt
import json
import sys
import time
import urllib.request

BASE = "https://api.india.delta.exchange/v2/history/candles"


def main():
    if len(sys.argv) != 4:
        sys.exit(__doc__)
    symbol, start, out = sys.argv[1:]
    t0 = int(dt.datetime.strptime(start, "%Y-%m-%d").replace(tzinfo=dt.timezone.utc).timestamp())
    t1 = int(time.time())
    rows = {}
    step = 86400 * 400
    while t0 < t1:
        url = f"{BASE}?resolution=1d&symbol=FUNDING:{symbol}&start={t0}&end={min(t0 + step, t1)}"
        with urllib.request.urlopen(url, timeout=30) as r:
            payload = json.load(r)
        if not payload.get("success"):
            sys.exit(f"API error: {payload}")
        for c in payload["result"]:
            day = dt.datetime.fromtimestamp(int(c["time"]), dt.timezone.utc).strftime("%Y-%m-%d")
            mean_pct = (float(c["open"]) + float(c["high"]) + float(c["low"]) + float(c["close"])) / 4
            rows[day] = 3 * mean_pct / 100
        t0 += step
        time.sleep(0.3)
    with open(out, "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["date", "value"])
        for day in sorted(rows):
            w.writerow([day, f"{rows[day]:.8f}"])
    print(f"wrote {len(rows)} days of {symbol} funding to {out}")


if __name__ == "__main__":
    main()
