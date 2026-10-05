#!/usr/bin/env python3
"""Downloads Binance spot daily candles (00:00 UTC close) for research backtests of Sleeve B.

usage: fetch_binance_daily.py SYMBOL OUT.csv      e.g. BTCUSDT data/crypto/BTC.csv

Binance is an offshore venue an Indian resident should not trade on; it is used here only
as the longest clean public price history (from August 2017). The paper-traded venue is
Delta Exchange India (see fetch_delta_candles.py). The last row is today's unfinished
candle and is dropped.
"""
import csv
import datetime as dt
import json
import sys
import time
import urllib.request

URL = "https://data-api.binance.vision/api/v3/klines?symbol={s}&interval=1d&startTime={t}&limit=1000"


def main():
    if len(sys.argv) != 3:
        sys.exit(__doc__)
    symbol, out = sys.argv[1:]
    rows, t = [], 0
    while True:
        with urllib.request.urlopen(URL.format(s=symbol, t=t), timeout=30) as r:
            batch = json.load(r)
        if not batch:
            break
        rows += batch
        t = batch[-1][0] + 86_400_000
        if len(batch) < 1000:
            break
        time.sleep(0.3)
    today = dt.datetime.now(dt.timezone.utc).strftime("%Y-%m-%d")
    with open(out, "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["date", "open", "high", "low", "close", "volume"])
        n = 0
        for k in rows:
            day = dt.datetime.fromtimestamp(k[0] / 1000, dt.timezone.utc).strftime("%Y-%m-%d")
            if day >= today:
                continue
            w.writerow([day, k[1], k[2], k[3], k[4], k[5]])
            n += 1
    print(f"wrote {n} daily candles of {symbol} to {out}")


if __name__ == "__main__":
    main()
