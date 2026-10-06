#!/usr/bin/env python3
"""Updates the real-data inputs of the intraday paper sleeves (A2 gaps, D1 Nifty last half hour).

usage: fetch_intraday.py FROM TO [--data DIR]     (dates YYYY-MM-DD; DIR defaults to data)

  DIR/index/nifty50_5m.csv   Nifty 50 5-minute bars (Yahoo ^NSEI), D1's signal and traded series
  DIR/intraday/SYMBOL.csv    5-minute bars of every A2 gap candidate between FROM and TO
  DIR/catalysts.csv          results / quarterly-update filings of those candidates (NSE)
  DIR/bands.csv              F&O stocks and cash-market price bands (NSE, current lists)

Yahoo serves only the last ~60 days of 5-minute bars, so files are merged, never
overwritten: run this every evening and the history grows. Bars of a session that has
not closed yet are dropped. Candidates are read from DIR/nse (run the bhavcopy ingest
first): open / prior close - 1 >= 6%, open >= Rs 50, 20-day traded value >= Rs 10 crore,
i.e. A2's own filters; the engine re-checks everything.

A catalyst is an NSE announcement whose subject or text names financial results or a
quarterly business update ("Financial Result", "results for the quarter", "business
update", "update for Q2"...). Board-meeting notices, certificates and the like are not.
"""
import csv
import datetime as dt
import http.cookiejar
import json
import os
import re
import sys
import time
import urllib.parse
import urllib.request
from zoneinfo import ZoneInfo

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from fetch_yahoo import fetch  # noqa: E402

IST = ZoneInfo("Asia/Kolkata")
UA = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/128.0 Safari/537.36"
CATALYST = re.compile(r"financial result|results? for the (quarter|half|year|period)|business update|"
                      r"update for q[1-4]|quarterly update")
MIN_GAP, MIN_PRICE, MIN_ATV = 0.06, 50.0, 1e8


def read_bars(path):
    rows = {}
    if os.path.exists(path):
        with open(path) as f:
            for r in csv.DictReader(f):
                rows[r["date"]] = tuple(float(r[k]) for k in ("open", "high", "low", "close", "volume"))
    return rows


def merge_bars(path, new):
    """Merges new 5-minute bars into path, dropping today's bars until the session closes."""
    now = dt.datetime.now(IST)
    today = now.strftime("%Y-%m-%d")
    rows = read_bars(path)
    rows.update(new)
    if now.strftime("%H:%M") < "15:35":
        rows = {k: v for k, v in rows.items() if not k.startswith(today)}
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["date", "open", "high", "low", "close", "volume"])
        for k in sorted(rows):
            o, h, l, c, v = rows[k]
            w.writerow([k, f"{o:.4f}", f"{h:.4f}", f"{l:.4f}", f"{c:.4f}", int(v)])
    return len(rows)


def candidates(nse_dir, start, end):
    """[(date, symbol)] of A2 gap candidates in [start, end] from the daily bhavcopy files."""
    out = []
    for name in sorted(os.listdir(nse_dir)):
        if not name.endswith(".csv"):
            continue
        with open(os.path.join(nse_dir, name)) as f:
            rows = list(csv.DictReader(f))
        for i in range(21, len(rows)):
            d = rows[i]["date"]
            if d < start or d > end:
                continue
            o, pc = float(rows[i]["open"]), float(rows[i - 1]["close"])
            if pc <= 0 or o / pc - 1 < MIN_GAP or o < MIN_PRICE:
                continue
            atv = sum(float(r["close"]) * float(r["volume"]) for r in rows[i - 20:i]) / 20
            if atv >= MIN_ATV:
                out.append((d, name[:-4]))
    return out


class Nse:
    def __init__(self):
        self.op = urllib.request.build_opener(urllib.request.HTTPCookieProcessor(http.cookiejar.CookieJar()))
        self.get("https://www.nseindia.com/companies-listing/corporate-filings-announcements", raw=True)

    def get(self, url, raw=False):
        req = urllib.request.Request(url, headers={
            "User-Agent": UA, "Accept": "application/json,text/html,*/*",
            "Referer": "https://www.nseindia.com/companies-listing/corporate-filings-announcements"})
        with self.op.open(req, timeout=30) as r:
            data = r.read()
        time.sleep(0.3)
        return data if raw else json.loads(data)


def catalysts(nse, symbol, start, end):
    """Matching filing timestamps ("YYYY-MM-DD HH:MM") for symbol between start and end."""
    q = urllib.parse.urlencode({"index": "equities", "symbol": symbol,
                                "from_date": start.strftime("%d-%m-%Y"), "to_date": end.strftime("%d-%m-%Y")})
    out = []
    for a in nse.get(f"https://www.nseindia.com/api/corporate-announcements?{q}"):
        text = f"{a.get('desc') or ''} {a.get('attchmntText') or ''}".lower()
        if CATALYST.search(text) and a.get("an_dt"):
            t = dt.datetime.strptime(a["an_dt"], "%d-%b-%Y %H:%M:%S")
            out.append((t.strftime("%Y-%m-%d %H:%M"), (a.get("desc") or "")[:60]))
    return out


def write_bands(path):
    def csv_rows(url):
        req = urllib.request.Request(url, headers={"User-Agent": UA})
        with urllib.request.urlopen(req, timeout=30) as r:
            return list(csv.reader(r.read().decode("utf-8", "replace").splitlines()))
    fo = {r[1].strip() for r in csv_rows("https://nsearchives.nseindia.com/content/fo/fo_mktlots.csv")[1:]
          if len(r) > 1 and r[1].strip() and r[1].strip().upper() != "SYMBOL"}
    bands = {}
    for r in csv_rows("https://nsearchives.nseindia.com/content/equities/sec_list.csv")[1:]:
        if len(r) > 3 and r[1].strip() in ("EQ", "BE", "BZ") and r[3].strip().replace(".", "").isdigit():
            bands[r[0].strip()] = r[3].strip()
    with open(path, "w") as f:
        f.write("symbol,band\n")
        for s in sorted(fo):
            f.write(f"{s},FO\n")
        for s in sorted(bands):
            if s not in fo:
                f.write(f"{s},{bands[s]}\n")
    return len(fo), len(bands)


def main():
    args = sys.argv[1:]
    if len(args) < 2:
        sys.exit(__doc__)
    start, end = args[0], args[1]
    data = args[args.index("--data") + 1] if "--data" in args else "data"

    n = merge_bars(os.path.join(data, "index", "nifty50_5m.csv"), fetch("^NSEI", "5m"))
    print(f"nifty 5m: {n} bars")
    fo, cash = write_bands(os.path.join(data, "bands.csv"))
    print(f"bands: {fo} F&O symbols, {cash} cash-market bands")

    cands = candidates(os.path.join(data, "nse"), start, end)
    print(f"{len(cands)} gap candidate(s) {start}..{end}")
    by_sym = {}
    for d, s in cands:
        by_sym.setdefault(s, []).append(d)

    cat_path = os.path.join(data, "catalysts.csv")
    cats = set()
    if os.path.exists(cat_path):
        with open(cat_path) as f:
            cats = {tuple(x.strip() for x in line.split(",", 1)) for line in f if "," in line and not line.startswith("symbol")}
    nse = Nse()
    for sym, dates in sorted(by_sym.items()):
        try:
            n = merge_bars(os.path.join(data, "intraday", f"{sym}.csv"), fetch(f"{sym}.NS", "5m"))
        except Exception as e:  # delisted or unknown on Yahoo: A2 skips the event without bars
            n = f"no Yahoo bars ({e.__class__.__name__})"
        lo = dt.date.fromisoformat(min(dates)) - dt.timedelta(days=5)
        hi = dt.date.fromisoformat(max(dates))
        try:
            found = catalysts(nse, sym, lo, hi)
        except Exception as e:
            found = []
            print(f"  {sym}: NSE announcements failed ({e.__class__.__name__})")
        for t, desc in found:
            cats.add((sym, t))
        print(f"  {sym} gaps {','.join(dates)}: 5m {n}; filings {[f'{t} {d}' for t, d in found] or 'none'}")
    with open(cat_path, "w") as f:
        f.write("symbol,timestamp\n")
        for s, t in sorted(cats):
            f.write(f"{s},{t}\n")
    print(f"catalysts: {len(cats)} filing(s) in {cat_path}")


if __name__ == "__main__":
    main()
