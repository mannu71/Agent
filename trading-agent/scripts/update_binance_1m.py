#!/usr/bin/env python3
"""Brings 1-minute files from fetch_binance_klines.py up to the last closed minute.

usage: update_binance_1m.py DIR [SYMBOL ...]        e.g. data/crypto_1m BTC ETH SOL XRP BNB
       (default symbols: every DIR/*.csv; the pair traded is SYMBOL + "USDT")

Pages forward from each file's last row through Binance's live klines endpoint and appends
only closed bars, so reruns never duplicate or rewrite rows. Same columns as the bulk files:
date ("YYYY-MM-DD HH:MM" UTC bar open), open, high, low, close, volume, taker_buy.
"""
import datetime as dt
import json
import os
import sys
import time
import urllib.request

URL = "https://data-api.binance.vision/api/v3/klines?symbol={s}&interval=1m&startTime={t}&limit=1000"


def last_minute(path):
    with open(path, "rb") as f:
        f.seek(0, os.SEEK_END)
        f.seek(max(0, f.tell() - 4096))
        line = f.read().decode().strip().splitlines()[-1]
    stamp = dt.datetime.strptime(line.split(",")[0], "%Y-%m-%d %H:%M").replace(tzinfo=dt.timezone.utc)
    return int(stamp.timestamp() * 1000)


def update(path, pair):
    t = last_minute(path) + 60_000
    now = int(time.time() * 1000)
    added = 0
    with open(path, "a") as out:
        while t < now:
            req = urllib.request.Request(URL.format(s=pair, t=t), headers={"User-Agent": "Mozilla/5.0"})
            with urllib.request.urlopen(req, timeout=30) as r:
                rows = json.load(r)
            rows = [k for k in rows if k[6] < now]  # close time passed: the bar is final
            if not rows:
                break
            for k in rows:
                ts = dt.datetime.fromtimestamp(k[0] / 1000, dt.timezone.utc).strftime("%Y-%m-%d %H:%M")
                out.write(f"{ts},{k[1]},{k[2]},{k[3]},{k[4]},{k[5]},{k[9]}\n")
            added += len(rows)
            t = rows[-1][0] + 60_000
    return added


def main():
    args = sys.argv[1:]
    if not args:
        sys.exit(__doc__)
    d = args[0]
    syms = args[1:] or sorted(f[:-4] for f in os.listdir(d) if f.endswith(".csv"))
    for s in syms:
        n = update(os.path.join(d, f"{s}.csv"), f"{s}USDT")
        print(f"{s}: +{n} bars")


if __name__ == "__main__":
    main()
