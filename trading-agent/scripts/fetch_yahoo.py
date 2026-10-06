#!/usr/bin/env python3
"""Downloads Yahoo Finance chart data into the agent's CSV format.

usage: fetch_yahoo.py SYMBOL OUT.csv [--interval 1d|5m] [--value]
  ^NSEI       Nifty 50 index           ^INDIAVIX   India VIX (use --value -> date,value)
  --interval 5m gives the last ~60 days of 5-minute bars ("YYYY-MM-DD HH:MM", IST),
  which is all Yahoo serves intraday. Dates and times are in the exchange's time zone.
"""
import csv
import datetime as dt
import json
import sys
import urllib.parse
import urllib.request
from zoneinfo import ZoneInfo


def fetch(symbol, interval="1d"):
    """Returns {"YYYY-MM-DD" or "YYYY-MM-DD HH:MM": (open, high, low, close, volume)}."""
    q = {"interval": interval}
    if interval == "1d":
        q.update(period1="0", period2=str(int(dt.datetime.now().timestamp())))
    else:
        q["range"] = "60d"
    url = f"https://query1.finance.yahoo.com/v8/finance/chart/{urllib.parse.quote(symbol)}?{urllib.parse.urlencode(q)}"
    req = urllib.request.Request(url, headers={"User-Agent": "Mozilla/5.0"})
    with urllib.request.urlopen(req, timeout=30) as r:
        res = json.load(r)["chart"]["result"][0]
    tz = ZoneInfo(res["meta"].get("exchangeTimezoneName", "UTC"))
    quote = res["indicators"]["quote"][0]
    rows = {}
    for k, ts in enumerate(res.get("timestamp", [])):
        o, h, l, c, v = (quote[f][k] for f in ("open", "high", "low", "close", "volume"))
        if None in (o, h, l, c):
            continue  # Yahoo leaves gaps as nulls
        t = dt.datetime.fromtimestamp(ts, tz)
        key = t.strftime("%Y-%m-%d") if interval == "1d" else t.strftime("%Y-%m-%d %H:%M")
        rows[key] = (o, h, l, c, v or 0)
    return rows


def main():
    args = sys.argv[1:]
    if len(args) < 2:
        sys.exit(__doc__)
    symbol, out = args[0], args[1]
    interval = args[args.index("--interval") + 1] if "--interval" in args else "1d"
    value_only = "--value" in args
    rows = fetch(symbol, interval)
    with open(out, "w", newline="") as f:
        w = csv.writer(f)
        if value_only:
            w.writerow(["date", "value"])
            for key in sorted(rows):
                w.writerow([key, f"{rows[key][3]:.4f}"])
        else:
            w.writerow(["date", "open", "high", "low", "close", "volume"])
            for key in sorted(rows):
                o, h, l, c, v = rows[key]
                w.writerow([key, f"{o:.4f}", f"{h:.4f}", f"{l:.4f}", f"{c:.4f}", int(v)])
    print(f"wrote {len(rows)} rows of {symbol} to {out}")


if __name__ == "__main__":
    main()
