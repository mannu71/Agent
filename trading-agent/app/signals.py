"""Live strategy signals: runs the concept engine on each symbol's closed bars and labels
every setup with its live status and the memory's verdict."""
import os
import time

from .engine.casebook import ExecCost, Memory, build_memory, memory_key, simulate_case
from .engine.concepts import ALL_PATTERNS, EXPLAIN, FLOW_PATTERNS, NAMES, detect_setups, format_minutes

TIMEFRAMES = (15, 60, 240)
LOOKBACK_BARS = 2000      # bars handed to the detectors per timeframe
SHOW_MINUTES = 3 * 1440   # signals newer than this are listed
COSTS = {"crypto": ExecCost(), "us": ExecCost(0.0, 0.0, 0.0001, 0.0), "india": ExecCost(0.0006, 0.0006, 0.0002, 0.0)}


def patterns_for(series):
    return [p for p in ALL_PATTERNS if series.has_flow or p not in FLOW_PATTERNS]


def setups_for(series, tf, cfg, lookback=LOOKBACK_BARS):
    bars = series.closed(tf)[-lookback:]
    daily = series.closed(1440)
    return detect_setups(bars, daily, tf, series.symbol, patterns_for(series), cfg)


def describe(series, s, memory, now_min):
    """One signal as the UI shows it."""
    m1 = series.m1 if series.m1 and series.m1[0].t <= s.t else series.m15
    case = simulate_case(s, m1, COSTS[series.market])
    entry = s.entry if s.order == "L" else s.ref
    target = s.target if s.target else entry + s.side * s.rr * abs(entry - s.stop)
    if not case.filled:
        status = "pending" if s.expiry > now_min else "expired"
    else:
        status = "open" if case.exit_reason == "end_of_data" else "closed"
    v = memory.verdict(memory_key(s))
    return {
        "id": f"{series.market}|{series.symbol}|{s.tf}|{s.pattern}|{s.side}|{s.t}",
        "market": series.market, "symbol": series.symbol, "tf": s.tf, "pattern": s.pattern,
        "name": NAMES[s.pattern], "explain": EXPLAIN[s.pattern], "side": "long" if s.side > 0 else "short",
        "order": {"M": "market", "L": "limit", "S": "stop"}[s.order], "entry": entry, "stop": s.stop,
        "target": target, "rr": abs(target - entry) / abs(entry - s.stop) if entry != s.stop else 0,
        "time": format_minutes(s.t), "t": s.t, "expiry": format_minutes(s.expiry), "expiry_t": s.expiry,
        "status": status, "r": case.r_net if case.filled else None, "exit_reason": case.exit_reason,
        "verdict": v.to_dict(), "context": s.ctx,
    }


class SignalEngine:
    def __init__(self, cfg, knowledge_dir, data_dir):
        self.cfg, self.data_dir = cfg, data_dir
        self.memory = {
            "crypto": Memory.load(os.path.join(knowledge_dir, "memory_crypto.csv")),
            "us": Memory.load(os.path.join(data_dir, "memory_us.csv")),
            "india": Memory.load(os.path.join(data_dir, "memory_india.csv")),
        }
        self.memory_source = {"crypto": "2020-26 backtest, 5 coins", "us": "", "india": ""}
        self.signals = {}  # (market, symbol) -> list of signal dicts

    def compute(self, series):
        now = int(time.time() // 60)
        out = []
        for tf in TIMEFRAMES:
            for s in setups_for(series, tf, self.cfg):
                if s.t >= now - SHOW_MINUTES:
                    out.append(describe(series, s, self.memory[series.market], now))
        out.sort(key=lambda x: x["t"], reverse=True)
        self.signals[(series.market, series.symbol)] = out
        return out

    def build_memory(self, market, series_list):
        """Memory for a market without a backtest table: every finished case in the history
        the feeds loaded (15-minute resolution, so fills are approximate)."""
        mem = Memory()
        for se in series_list:
            for tf in TIMEFRAMES:
                build_memory(setups_for(se, tf, self.cfg, lookback=100_000), se.m15, COSTS[market], memory=mem)
        path = os.path.join(self.data_dir, f"memory_{market}.csv")
        mem.save(path)
        self.memory[market] = mem
        days = max((len(se.m15) for se in series_list), default=0) // 26 if market != "crypto" else 0
        self.memory_source[market] = f"last ~{days} trading days of loaded history" if days else "loaded history"
        return mem
