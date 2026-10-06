#!/usr/bin/env python3
"""Live intraday check of a paper account's pending A1 buy-stops against today's real NSE
prices (Yahoo 5-minute bars, typically a few minutes delayed). Read-only: it does not change
the account; the end-of-day `ta_paper run` on the official bhavcopy records actual fills.

usage: live_check.py ACCOUNT_DIR [--tick 0.05] [--limit 0.005]

For each pending candidate: trigger = pivot + tick; a fill happens when the day's high
reaches the trigger, at max(open, trigger) if that is within the 0.5% limit; the initial
stop is fill x (1 - ADR20). Shows the latest price, P&L in R, and whether the stop has
been touched (the engine counts an entry-day low through the stop as a stop-out).
"""
import datetime as dt
import json
import sys
import urllib.parse
import urllib.request
from zoneinfo import ZoneInfo

IST = ZoneInfo("Asia/Kolkata")


def pending(account):
    out = []
    for line in open(f"{account}/state/a1.state"):
        parts = line.split()
        if not parts or parts[0] != "pending":
            continue
        kv = dict(p.split("=", 1) for p in parts[1:])
        out.append((urllib.parse.unquote(kv["symbol"]), float(kv["pivot"]), float(kv["adr"]), float(kv["score"])))
    return out


def today_bars(symbol):
    url = ("https://query1.finance.yahoo.com/v8/finance/chart/"
           f"{urllib.parse.quote(symbol + '.NS')}?range=1d&interval=5m")
    req = urllib.request.Request(url, headers={"User-Agent": "Mozilla/5.0"})
    with urllib.request.urlopen(req, timeout=30) as r:
        res = json.load(r)["chart"]["result"][0]
    q = res["indicators"]["quote"][0]
    bars = []
    for k, ts in enumerate(res.get("timestamp") or []):
        vals = [q[f][k] for f in ("open", "high", "low", "close")]
        if None in vals:
            continue
        bars.append((dt.datetime.fromtimestamp(ts, IST), *vals))
    return bars


def entries_allowed(account):
    for line in open(f"{account}/state/a1.state"):
        parts = line.split()
        if parts and parts[0] == "a1":
            return dict(p.split("=", 1) for p in parts[1:]).get("pending_allowed", "1") != "0"
    return True


def main():
    args = sys.argv[1:]
    if not args:
        sys.exit(__doc__)
    account = args[0]
    tick = float(args[args.index("--tick") + 1]) if "--tick" in args else 0.05
    limit_frac = float(args[args.index("--limit") + 1]) if "--limit" in args else 0.005
    now = dt.datetime.now(IST)
    print(f"live check {now:%Y-%m-%d %H:%M} IST  (Yahoo 5-minute bars, may lag a few minutes)\n")
    print(f"{'symbol':12} {'trigger':>9} {'open':>9} {'high':>9} {'last':>9}  status")
    triggered = 0
    for sym, pivot, adr, score in pending(account):
        trig = pivot + tick
        limit = trig * (1 + limit_frac)
        try:
            bars = [b for b in today_bars(sym) if b[0].date() == now.date()]
        except Exception as e:  # noqa: BLE001 - report and continue with the rest
            print(f"{sym:12} {trig:9.2f}  no data ({e.__class__.__name__})")
            continue
        if not bars:
            print(f"{sym:12} {trig:9.2f}  no bars today")
            continue
        o = bars[0][1]
        hi = max(b[2] for b in bars)
        last = bars[-1][4]
        if o > limit:
            status = f"gapped above limit {limit:.2f}: no fill"
        elif hi < trig:
            status = f"not triggered ({(trig / last - 1) * 100:.1f}% away)"
        else:
            triggered += 1
            fill = max(o, trig)
            stop = fill * (1 - adr)
            after = [b for b in bars if b[2] >= trig]
            low_after = min(b[3] for b in bars[bars.index(after[0]):])
            r = (last - fill) / (fill - stop)
            hit = " STOP TOUCHED" if low_after <= stop else ""
            status = f"FILLED ~{fill:.2f} at {after[0][0]:%H:%M}, stop {stop:.2f}, now {r:+.2f}R{hit}"
        print(f"{sym:12} {trig:9.2f} {o:9.2f} {hi:9.2f} {last:9.2f}  {status}")
    print(f"\n{triggered} of {len(pending(account))} buy-stops triggered so far. Paper only; "
          "official fills are recorded by the end-of-day run on the NSE bhavcopy.")
    if not entries_allowed(account):
        print("Regime gate is closed (pending_allowed=0): the account takes none of these entries today.")


if __name__ == "__main__":
    main()
