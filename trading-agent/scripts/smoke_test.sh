#!/usr/bin/env bash
# End-to-end smoke test on synthetic random-walk data: ingest -> backtest every sleeve ->
# paper account init/run/status/scorecard/kill/verify. Random walks contain no edge, so
# losing results here are the expected, correct outcome.
# usage: scripts/smoke_test.sh [BUILD_DIR] [WORK_DIR]
set -euo pipefail
build=${1:-build}
work=${2:-$(mktemp -d)}
mkdir -p "$work"
python3 - "$work" <<'PY'
import datetime as dt, math, os, random, sys
work = sys.argv[1]
random.seed(7)
days = [dt.date(2020, 1, 1) + dt.timedelta(d) for d in range(1300)]
wk = [d for d in days if d.weekday() < 5][:900]
os.makedirs(f"{work}/bhav", exist_ok=True)
syms = {f"SYM{n:02d}": (random.uniform(150, 2000), random.uniform(0.015, 0.035)) for n in range(60)}
price = {s: p for s, (p, _) in syms.items()}
mon = "JAN FEB MAR APR MAY JUN JUL AUG SEP OCT NOV DEC".split()
for d in wk:  # legacy-format bhavcopy, one file per day
    with open(f"{work}/bhav/cm{d:%d}{mon[d.month-1]}{d:%Y}bhav.csv", "w") as f:
        f.write("SYMBOL,SERIES,OPEN,HIGH,LOW,CLOSE,LAST,PREVCLOSE,TOTTRDQTY,TOTTRDVAL,TIMESTAMP,TOTALTRADES,ISIN,\n")
        for s, (_, vol) in syms.items():
            p = price[s]; o = p*math.exp(random.gauss(0, vol/3)); c = o*math.exp(random.gauss(0, vol))
            h = max(o, c)*math.exp(abs(random.gauss(0, vol/2))); l = min(o, c)*math.exp(-abs(random.gauss(0, vol/2)))
            q = int(random.uniform(3e5, 3e6))
            f.write(f"{s},EQ,{o:.2f},{h:.2f},{l:.2f},{c:.2f},{c:.2f},{p:.2f},{q},{q*c:.0f},{d:%d}-{mon[d.month-1]}-{d:%Y},100,INE000000000,\n")
            price[s] = c
os.makedirs(f"{work}/crypto", exist_ok=True)
for a, p, vol in (("BTC", 30000, 0.035), ("ETH", 2000, 0.045)):
    with open(f"{work}/crypto/{a}.csv", "w") as f:
        f.write("date,open,high,low,close,volume\n")
        for d in days:
            o = p; c = o*math.exp(random.gauss(0, vol))
            f.write(f"{d},{o:.2f},{max(o,c)*1.01:.2f},{min(o,c)*0.99:.2f},{c:.2f},1000000\n"); p = c
with open(f"{work}/nifty_fut.csv", "w") as f:
    f.write("date,open,high,low,close,volume\n"); p = 18000.0
    for d in wk:
        for m in range(9*60+15, 15*60+30, 5):
            o = p; c = o*math.exp(random.gauss(0, 0.0012))
            f.write(f"{d} {m//60:02d}:{m%60:02d},{o:.2f},{max(o,c)+1:.2f},{min(o,c)-1:.2f},{c:.2f},1000\n"); p = c
PY
set -x
"$build/ta_ingest" bhavcopy "$work/bhav" "$work/nse" --non-eq-out "$work/t2t.csv"
"$build/ta_backtest" a1 "$work/nse" --gate --trades "$work/a1_trades.csv"
"$build/ta_backtest" b "$work/crypto" --cost-bps 25
"$build/ta_backtest" d1 "$work/nifty_fut.csv"
"$build/ta_backtest" options --legs "P:22000:-1:85,P:21800:1:40,C:24000:-1:70,C:24200:1:30" \
  --expiry 2026-10-13 --today 2026-10-06 --active-book 2500000
"$build/ta_paper" init "$work/acct" --capital 10000000 --start 2021-03-01 \
  --set data.equity_dir="$work/nse" --set data.crypto_dir="$work/crypto" --set data.nifty_fut="$work/nifty_fut.csv"
"$build/ta_paper" run "$work/acct" --until 2022-06-30
"$build/ta_paper" run "$work/acct"
"$build/ta_paper" scorecard "$work/acct"
"$build/ta_paper" verify "$work/acct"
"$build/ta_paper" kill "$work/acct" --reason smoke-test
"$build/ta_paper" verify "$work/acct"
