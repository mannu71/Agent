#!/usr/bin/env python3
"""Builds paper/crypto_concepts/ui.json, the data behind the live dashboard artifact.

usage: export_live_ui.py [ACCOUNT_DIR] [DATA_DIR]   (defaults: paper/crypto_concepts data/crypto_1m)

Reads the live views written by `ta_backtest setups --live` (setups.csv, open.csv, trades.csv,
memory.csv per timeframe) and the latest 1-minute prices, and writes one JSON document. The
hourly run pushes it to the dashboard's database; nothing here changes the paper record.
"""
import csv
import datetime as dt
import json
import os
import sys

NAMES = {
    "sweep": "Liquidity sweep reversal",
    "sweep_absorb": "Sweep + absorption",
    "sweep_cont": "Sweep continuation",
    "fvg": "Fair value gap",
    "ob": "Order block",
    "choch": "Change of character",
    "hl_pullback": "Higher-low pullback",
    "spring": "Wyckoff spring / upthrust",
    "va80": "80% value-area rule",
}
# Discovery backtest 2020-01..2023-12, net R per trade after costs, (long, short); see
# knowledge/concepts.md and RESULTS.md.
BACKTEST = {
    60: {"sweep": (-0.244, -0.186), "sweep_absorb": (-0.241, -0.181), "sweep_cont": (-0.254, -0.130),
         "fvg": (-0.029, -0.084), "ob": (-0.128, -0.186), "choch": (0.028, -0.092),
         "hl_pullback": (-0.144, -0.176), "spring": (-0.159, -0.199), "va80": (-0.054, -0.065)},
    240: {"sweep": (-0.151, -0.070), "sweep_absorb": (-0.161, -0.086), "sweep_cont": (-0.182, -0.107),
          "fvg": (0.074, -0.024), "ob": (-0.115, -0.150), "choch": (0.007, -0.086),
          "hl_pullback": (0.035, -0.060), "spring": (-0.011, -0.254), "va80": (-0.076, -0.018)},
}


def tail_rows(path, n):
    with open(path, "rb") as f:
        f.seek(0, os.SEEK_END)
        f.seek(max(0, f.tell() - n * 110))
        lines = f.read().decode().strip().splitlines()[1:]
    return [l.split(",") for l in lines][-n:]


def num(x):
    try:
        return round(float(x), 6)
    except (TypeError, ValueError):
        return None


def coins(data_dir):
    out, last = [], ""
    for name in sorted(os.listdir(data_dir)):
        if not name.endswith(".csv"):
            continue
        rows = tail_rows(os.path.join(data_dir, name), 1441)
        if not rows:
            continue
        price, first = float(rows[-1][4]), float(rows[0][1])
        out.append({"symbol": name[:-4], "price": price, "change_24h": (price / first - 1) * 100,
                    "high_24h": max(float(r[2]) for r in rows), "low_24h": min(float(r[3]) for r in rows),
                    "time": rows[-1][0]})
        last = max(last, rows[-1][0])
    return out, last


def timeframe(d, tf):
    memory = {}
    with open(os.path.join(d, "memory.csv")) as f:
        for r in csv.DictReader(f):
            memory[r["key"]] = {"cases": int(r["cases"]), "mean_r": num(r["mean_r"]), "lower_r": num(r["lower_r"]),
                                "approved": r["approved"] == "1"}
    # Situations with no trend label come from the first 50 days of 2020 (no daily history
    # yet); they can never recur, so the dashboard leaves them out.
    memory = {k: v for k, v in memory.items() if not k.endswith("|")}
    active, closed, score = [], [], {}
    with open(os.path.join(d, "setups.csv")) as f:
        for r in csv.DictReader(f):
            side = "long" if r["side"] == "1" else "short"
            key = f'{r["pattern"]}|{tf}|{side}|{r["trend"]}'
            item = {"time": r["time"], "pattern": r["pattern"], "name": NAMES.get(r["pattern"], r["pattern"]),
                    "symbol": r["symbol"], "side": side, "order": {"M": "market", "L": "limit", "S": "stop"}[r["order"]],
                    "status": r["status"], "agent": r["agent"] == "1", "entry": num(r["entry"]) or None,
                    "stop": num(r["stop"]), "target": num(r["target"]) or None, "r": num(r["r_net"]),
                    "exit_time": r["exit_time"], "exit_reason": r["exit_reason"], "trend": r["trend"],
                    "session": r["session"], "memory": memory.get(key)}
            s = score.setdefault(r["pattern"], {"pattern": r["pattern"], "name": item["name"], "setups": 0,
                                                "closed": 0, "wins": 0, "sum_r": 0.0})
            s["setups"] += 1
            if r["status"] in ("pending", "open"):
                active.append(item)
            elif r["status"] == "closed":
                closed.append(item)
                s["closed"] += 1
                s["wins"] += float(r["r_net"]) > 0
                s["sum_r"] += float(r["r_net"])
    for p, s in score.items():
        s["mean_r"] = s["sum_r"] / s["closed"] if s["closed"] else None
        s["backtest_long"], s["backtest_short"] = BACKTEST[tf].get(p, (None, None))
        del s["sum_r"]
    agent_open = list(csv.DictReader(open(os.path.join(d, "open.csv"))))
    agent_closed = list(csv.DictReader(open(os.path.join(d, "trades.csv"))))
    best = sorted(memory.items(), key=lambda kv: -kv[1]["lower_r"])[:6]
    return {
        "tf": tf,
        "label": "1 hour" if tf == 60 else "4 hours",
        "agent": {
            "open": [{k: (num(v) if k not in ("symbol", "pattern", "entry_time") else v) for k, v in r.items()}
                     for r in agent_open],
            "closed": len(agent_closed),
            "pnl": sum(float(r["pnl"]) for r in agent_closed),
            "approved": [k for k, v in memory.items() if v["approved"]],
            "closest": [{"key": k, **v} for k, v in best],
            "situations": len(memory),
        },
        "active": sorted(active, key=lambda x: x["time"], reverse=True),
        "recent": sorted(closed, key=lambda x: x["exit_time"], reverse=True)[:30],
        "scoreboard": sorted(score.values(), key=lambda s: -(s["mean_r"] if s["mean_r"] is not None else -9)),
        "totals": {"setups": sum(s["setups"] for s in score.values()), "closed": len(closed),
                   "mean_r": (sum(x["r"] for x in closed) / len(closed)) if closed else None},
    }


def main():
    acct = sys.argv[1] if len(sys.argv) > 1 else "paper/crypto_concepts"
    data = sys.argv[2] if len(sys.argv) > 2 else "data/crypto_1m"
    prices, last = coins(data)
    doc = {
        "updated": dt.datetime.now(dt.timezone.utc).strftime("%Y-%m-%d %H:%M"),
        "data_to": last,
        "live_from": "2026-10-01 00:00",
        "coins": prices,
        "timeframes": [timeframe(os.path.join(acct, f"tf{tf}"), tf) for tf in (60, 240)],
    }
    out = os.path.join(acct, "ui.json")
    with open(out, "w") as f:
        json.dump(doc, f, separators=(",", ":"))
    print(f"wrote {out} ({os.path.getsize(out)} bytes)")


if __name__ == "__main__":
    main()
