"""Case simulation and the memory's recall rule: a port of src/casebook.cpp.

simulate_case plays one setup out on 1-minute bars exactly as the C++ engine does, and
Memory answers "has this kind of setup made money before?" from finished cases only.
"""
from __future__ import annotations

import csv
import math
import os
from dataclasses import dataclass

from .concepts import Setup


@dataclass
class ExecCost:
    taker: float = 0.0005
    maker: float = 0.0002
    slippage: float = 0.0002
    funding_per_8h: float = 0.0001


@dataclass
class Case:
    setup: Setup
    filled: bool = False
    entry_t: int = 0
    exit_t: int = 0
    entry_px: float = 0.0
    exit_px: float = 0.0
    stop_px: float = 0.0
    target_px: float = 0.0
    r_gross: float = 0.0
    r_net: float = 0.0
    exit_reason: str = ""


def lower_index(m1, t):
    lo, hi = 0, len(m1)
    while lo < hi:
        mid = (lo + hi) // 2
        if m1[mid].t < t:
            lo = mid + 1
        else:
            hi = mid
    return lo


def simulate_case(s: Setup, m1, cost: ExecCost) -> Case:
    c = Case(setup=s)
    side = s.side
    j = lower_index(m1, s.t)
    fill = fee_in = 0.0
    same_bar_target = True
    while j < len(m1) and m1[j].t < s.expiry:
        b = m1[j]
        if s.order == "M":
            fill = b.open * (1.0 + side * cost.slippage)
            fee_in = cost.taker
        elif s.order == "L":
            if (b.low > s.entry) if side > 0 else (b.high < s.entry):
                j += 1
                continue
            fill = min(b.open, s.entry) if side > 0 else max(b.open, s.entry)
            fee_in = cost.maker
            same_bar_target = False
        else:
            if (b.high < s.entry) if side > 0 else (b.low > s.entry):
                j += 1
                continue
            fill = (max(b.open, s.entry) if side > 0 else min(b.open, s.entry)) * (1.0 + side * cost.slippage)
            fee_in = cost.taker
            same_bar_target = False
        break
    if fill <= 0 or j >= len(m1):
        return c
    if side * (fill - s.stop) <= 0:
        return c
    c.filled = True
    c.entry_t = m1[j].t
    c.entry_px = fill
    c.stop_px = s.stop
    risk = abs(fill - s.stop)
    c.target_px = s.target if s.target != 0 else (fill + side * s.rr * risk if s.rr > 0 else 0.0)
    if c.target_px != 0 and side * (c.target_px - fill) <= 0:
        c.target_px = 0.0

    exit_px = fee_out = 0.0
    k = j
    while k < len(m1):
        b = m1[k]
        if k > j and b.t >= c.entry_t + s.max_hold:
            exit_px = b.open * (1.0 - side * cost.slippage)
            fee_out = cost.taker
            c.exit_reason = "time"
            break
        first = k == j
        gap_stop = (not first) and ((b.open <= s.stop) if side > 0 else (b.open >= s.stop))
        stop_hit = (b.low <= s.stop) if side > 0 else (b.high >= s.stop)
        if gap_stop or stop_hit:
            exit_px = (b.open if gap_stop else s.stop) * (1.0 - side * cost.slippage)
            fee_out = cost.taker
            c.exit_reason = "stop"
            break
        target_hit = c.target_px != 0 and ((b.high >= c.target_px) if side > 0 else (b.low <= c.target_px)) and \
            ((not first) or same_bar_target)
        if target_hit:
            gap_target = (not first) and ((b.open >= c.target_px) if side > 0 else (b.open <= c.target_px))
            exit_px = b.open if gap_target else c.target_px
            fee_out = cost.maker
            c.exit_reason = "target"
            break
        k += 1
    if k >= len(m1):
        k = len(m1) - 1
        exit_px = m1[k].close * (1.0 - side * cost.slippage)
        fee_out = cost.taker
        c.exit_reason = "end_of_data"
    c.exit_t = m1[k].t
    c.exit_px = exit_px
    c.r_gross = side * (exit_px - fill) / risk
    hours = float(c.exit_t - c.entry_t) / 60.0
    costs = fee_in * fill + fee_out * exit_px + cost.funding_per_8h * fill * hours / 8.0
    c.r_net = c.r_gross - costs / risk
    return c


FIELDS = ("concept", "tf", "side", "trend")


def memory_key(s: Setup, fields=FIELDS) -> str:
    parts = []
    for f in fields:
        if f == "concept":
            parts.append(s.pattern)
        elif f == "tf":
            parts.append(str(s.tf))
        elif f == "side":
            parts.append("long" if s.side > 0 else "short")
        elif f == "symbol":
            parts.append(s.symbol)
        else:
            parts.append(s.ctx.get(f, ""))
    return "|".join(parts)


@dataclass
class Verdict:
    status: str          # "trade", "skip" or "untested"
    cases: int = 0
    mean_r: float = 0.0
    lower_r: float = 0.0

    def to_dict(self):
        return {"status": self.status, "cases": self.cases, "mean_r": self.mean_r, "lower_r": self.lower_r}


class Memory:
    """Finished-case statistics per situation, with the recall rule from casebook.cpp:
    at least `min_cases`, mean >= `min_mean_r`, one-sided 95% lower bound above zero."""

    def __init__(self, min_cases=30, min_mean_r=0.05, z=1.645):
        self.min_cases, self.min_mean_r, self.z = min_cases, min_mean_r, z
        self.stats = {}  # key -> [n, sum, sum_sq]

    def add(self, key, r):
        st = self.stats.setdefault(key, [0, 0.0, 0.0])
        st[0] += 1
        st[1] += r
        st[2] += r * r

    def merge_summary(self, key, n, mean, sd):
        """Adds n cases known only by mean and sample sd (from a saved memory table)."""
        if n <= 0:
            return
        s = mean * n
        ss = sd * sd * (n - 1) + s * s / n if n > 1 else s * s
        st = self.stats.setdefault(key, [0, 0.0, 0.0])
        st[0] += n
        st[1] += s
        st[2] += ss

    def verdict(self, key) -> Verdict:
        st = self.stats.get(key)
        if not st or st[0] < self.min_cases:
            return Verdict("untested", st[0] if st else 0, (st[1] / st[0]) if st and st[0] else 0.0, 0.0)
        n, s, ss = st
        mean = s / n
        var = max(0.0, (ss - s * s / n) / (n - 1))
        lower = mean - self.z * math.sqrt(var / n)
        ok = mean >= self.min_mean_r and lower > 0
        return Verdict("trade" if ok else "skip", n, mean, lower)

    def save(self, path):
        os.makedirs(os.path.dirname(path) or ".", exist_ok=True)
        with open(path, "w", newline="") as f:
            w = csv.writer(f)
            w.writerow(["key", "cases", "mean_r", "sd_r"])
            for k, (n, s, ss) in sorted(self.stats.items()):
                mean = s / n
                sd = math.sqrt(max(0.0, (ss - s * s / n) / (n - 1))) if n > 1 else 0.0
                w.writerow([k, n, repr(mean), repr(sd)])

    @staticmethod
    def load(path, **kw) -> "Memory":
        m = Memory(**kw)
        if os.path.exists(path):
            for r in csv.DictReader(open(path)):
                m.merge_summary(r["key"], int(r["cases"]), float(r["mean_r"]), float(r["sd_r"]))
        return m


def build_memory(setups, m1, cost: ExecCost, until=None, memory=None) -> Memory:
    """Plays every setup out and records finished cases (exit before `until`, if given)."""
    memory = memory or Memory()
    for s in setups:
        c = simulate_case(s, m1, cost)
        if not c.filled or c.exit_reason == "end_of_data":
            continue
        if until is not None and c.exit_t + 1 > until:
            continue
        memory.add(memory_key(s), c.r_net)
    return memory
