#!/usr/bin/env python3
"""Downloads NSE corporate actions and converts splits, bonus issues and consolidations
into the symbol,ex_date,factor file that `ta_ingest bhavcopy --actions` applies
(prices before ex_date x factor, volumes / factor).

usage: fetch_nse_actions.py START_YEAR END_YEAR OUT.csv [--check DATA_DIR]

  Bonus a:b (a new shares per b held)        factor = b / (a + b)
  Face value split / consolidation X -> Y    factor = Y / X
Dividends, rights issues and demergers are not price-adjusted (see README).

--infer   (with --check) also adds an inferred action for every unexplained overnight move
in the unadjusted data that lands within 2% of a clean split/bonus ratio (1/2, 2/3, 1/5,
1/10, ...) on a stock whose prior close is >= Rs 50: the NSE feed misses some actions,
especially in 2016-17. Inferred rows are listed for review.
--check DATA_DIR compares each factor with the unadjusted open / prior close on the ex-date
in DATA_DIR/<SYMBOL>.csv (ingest once without --actions first) and drops actions the price
move contradicts by more than 25%, listing them.
Source: www.nseindia.com/api/corporates-corporateActions (tested October 2026).
"""
import csv
import json
import re
import sys
import time
import urllib.request
from datetime import datetime

API = "https://www.nseindia.com/api/corporates-corporateActions?index=equities&from_date={a}&to_date={b}"
HEADERS = {
    "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/128.0 Safari/537.36",
    "Accept": "application/json",
    "Referer": "https://www.nseindia.com/companies-listing/corporate-filings-actions",
}
BONUS = re.compile(r"bonus\s*(\d+)\s*:\s*(\d+)", re.I)
FACE = re.compile(r"from\s*r[se]\.?\s*([\d.]+).*?to\s*r[se]\.?\s*([\d.]+)", re.I)


def factor_of(subject):
    f, kinds = 1.0, []
    for a, b in BONUS.findall(subject):
        a, b = int(a), int(b)
        if a > 0 and b > 0:
            f *= b / (a + b)
            kinds.append("bonus")
    low = subject.lower()
    if "split" in low or "sub-division" in low or "consolidat" in low:
        m = FACE.search(subject)
        if m:
            x, y = float(m.group(1)), float(m.group(2))
            if x > 0 and y > 0:
                f *= y / x
                kinds.append("split" if y < x else "consolidation")
    return f, kinds


def main():
    args = sys.argv[1:]
    if len(args) < 3:
        sys.exit(__doc__)
    y0, y1, out = int(args[0]), int(args[1]), args[2]
    check = args[args.index("--check") + 1] if "--check" in args else None
    rows = {}
    for y in range(y0, y1 + 1):
        req = urllib.request.Request(API.format(a=f"01-01-{y}", b=f"31-12-{y}"), headers=HEADERS)
        with urllib.request.urlopen(req, timeout=60) as r:
            data = json.load(r)
        for x in data:
            if x.get("series") not in ("EQ", "BE", "BZ", "SM", "ST"):
                continue
            f, kinds = factor_of(x.get("subject", ""))
            if not kinds or abs(f - 1) < 1e-9:
                continue
            ex = datetime.strptime(x["exDate"], "%d-%b-%Y").strftime("%Y-%m-%d")
            key = (x["symbol"], ex)
            if key in rows and rows[key][2] != x["subject"].strip():
                # A split and a bonus on the same ex-date are separate records: compound them.
                pf, pk, ps = rows[key]
                rows[key] = (pf * f, pk + "+" + "+".join(kinds), ps + " / " + x["subject"].strip())
            else:
                rows[key] = (f, "+".join(kinds), x["subject"].strip())
        print(f"{y}: {len(data)} actions fetched", file=sys.stderr)
        time.sleep(1)
    if check:
        # Drop actions the prices contradict (no discontinuity on the ex-date), so a
        # mis-filed record cannot inject a fake jump into the adjusted history.
        bad = []
        for (sym, ex), (f, kind, subj) in sorted(rows.items()):
            try:
                bars = list(csv.DictReader(open(f"{check}/{sym}.csv")))
            except FileNotFoundError:
                continue
            for k in range(1, len(bars)):
                if bars[k]["date"] >= ex:
                    if bars[k]["date"] == ex:
                        ratio = float(bars[k]["open"]) / float(bars[k - 1]["close"])
                        if abs(ratio / f - 1) > 0.25:
                            bad.append((sym, ex))
                            print(f"DROPPED {sym} {ex} factor {f:.4f} but open/prior close {ratio:.4f}: {subj}")
                    break
        for key in bad:
            del rows[key]
        print(f"{len(bad)} action(s) dropped because the price move contradicts them")
        if "--infer" in args:
            import glob, os
            clean = [1/2, 1/3, 2/3, 1/4, 3/4, 1/5, 2/5, 1/10, 1/20, 1/100, 1/6, 4/5, 3/5]
            have = {k for k in rows}
            inferred = 0
            for path in glob.glob(f"{check}/*.csv"):
                sym = os.path.basename(path)[:-4]
                bars = list(csv.DictReader(open(path)))
                for k in range(1, len(bars)):
                    pc = float(bars[k - 1]["close"])
                    if pc < 50:
                        continue
                    r = float(bars[k]["open"]) / pc
                    if r >= 0.7 or (sym, bars[k]["date"]) in have:
                        continue
                    near = min(clean, key=lambda c: abs(r / c - 1))
                    if abs(r / near - 1) < 0.02:
                        rows[(sym, bars[k]["date"])] = (near, "inferred", f"inferred from open/prior close {r:.4f}")
                        inferred += 1
                        print(f"INFERRED {sym} {bars[k]['date']} factor {near:.4f} (open/prior close {r:.4f})")
            print(f"{inferred} action(s) inferred from clean-ratio price gaps")
    with open(out, "w", newline="") as fh:
        w = csv.writer(fh)
        w.writerow(["symbol", "ex_date", "factor"])
        for (sym, ex), (f, _, _) in sorted(rows.items()):
            w.writerow([sym, ex, f"{f:.10g}"])
    print(f"wrote {len(rows)} price-adjusting actions to {out}")


if __name__ == "__main__":
    main()
