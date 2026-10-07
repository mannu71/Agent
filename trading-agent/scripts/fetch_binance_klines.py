#!/usr/bin/env python3
"""Downloads Binance spot 1-minute klines (with taker-buy volume) from data.binance.vision.

usage: fetch_binance_klines.py SYMBOL FROM_MONTH TO_MONTH OUT.csv      e.g. BTCUSDT 2020-01 2026-09 data/crypto_1m/BTC.csv

Output columns: date ("YYYY-MM-DD HH:MM", UTC bar open), open, high, low, close, volume,
taker_buy (base-asset volume bought by takers: buy minus sell = 2 x taker_buy - volume is
the bar's order-flow delta). Monthly archives are cached next to OUT in raw/ and merged,
so reruns only fetch new months. Binance switched spot timestamps to microseconds in 2025;
both units are handled.
"""
import csv
import datetime as dt
import io
import os
import sys
import urllib.error
import urllib.request
import zipfile

BASE = "https://data.binance.vision/data/spot/monthly/klines/{s}/1m/{s}-1m-{m}.zip"


def months(a, b):
    y, m = map(int, a.split("-"))
    y2, m2 = map(int, b.split("-"))
    while (y, m) <= (y2, m2):
        yield f"{y:04d}-{m:02d}"
        y, m = (y + 1, 1) if m == 12 else (y, m + 1)


def main():
    args = sys.argv[1:]
    if len(args) != 4:
        sys.exit(__doc__)
    sym, a, b, out = args
    raw = os.path.join(os.path.dirname(out) or ".", "raw")
    os.makedirs(raw, exist_ok=True)
    rows = 0
    with open(out + ".tmp", "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["date", "open", "high", "low", "close", "volume", "taker_buy"])
        for m in months(a, b):
            z = os.path.join(raw, f"{sym}-1m-{m}.zip")
            if not os.path.exists(z):
                try:
                    req = urllib.request.Request(BASE.format(s=sym, m=m), headers={"User-Agent": "Mozilla/5.0"})
                    with urllib.request.urlopen(req, timeout=120) as r:
                        data = r.read()
                except urllib.error.HTTPError as e:
                    if e.code == 404:
                        continue  # month not listed (before launch, or not published yet)
                    raise
                with open(z + ".part", "wb") as zf:
                    zf.write(data)
                os.replace(z + ".part", z)
            with zipfile.ZipFile(z) as zf:
                text = zf.read(zf.namelist()[0]).decode()
            for line in csv.reader(io.StringIO(text)):
                if not line or not line[0].isdigit():
                    continue  # header row in some archives
                t = int(line[0])
                t = t / 1e6 if t > 10**14 else t / 1e3
                ts = dt.datetime.fromtimestamp(t, dt.timezone.utc).strftime("%Y-%m-%d %H:%M")
                w.writerow([ts, line[1], line[2], line[3], line[4], line[5], line[9]])
                rows += 1
            print(f"{sym} {m}", flush=True)
    os.replace(out + ".tmp", out)
    print(f"wrote {rows} bars of {sym} to {out}")


if __name__ == "__main__":
    main()
