"""Institutional-concept detectors: a line-for-line port of src/concepts.cpp.

The app must produce exactly the setups the C++ backtest produced, so every rule, every
comparison and every arithmetic step follows the C++ code in the same order (see
tests/test_parity.py). Times are minutes since 1970-01-01 00:00 UTC.
"""
from __future__ import annotations

import math
import os
from dataclasses import dataclass, field

NAN = float("nan")
ALL_PATTERNS = ("sweep", "sweep_absorb", "sweep_cont", "fvg", "ob", "choch", "hl_pullback", "spring", "va80")
# Patterns that need taker-buy volume (order flow); hidden where a feed has none.
FLOW_PATTERNS = ("sweep_absorb",)

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
EXPLAIN = {
    "sweep": "Price poked past a recent swing level and closed back inside: trade the reversal (ICT stop hunt).",
    "sweep_absorb": "A sweep where aggressive buyers or sellers pushed through the level and still failed.",
    "sweep_cont": "Same sweep, traded in the direction of the poke: stop runs often continue.",
    "fvg": "A three-candle gap left by a big move: limit order at the middle of the gap.",
    "ob": "After a break of structure, the last opposite candle before the move: limit order at its edge.",
    "choch": "A downtrend (uptrend) closes beyond its last swing high (low): first sign of a turn.",
    "hl_pullback": "An uptrend makes a higher low (a downtrend a lower high): trade with the trend.",
    "spring": "A sideways range is broken on high volume and price closes back inside: trade back across.",
    "va80": "The day opened outside yesterday's value area and re-entered: target the far side.",
}


def day_of(t: int) -> int:
    return t // 1440  # floor division, like the C++ day_of for negatives too


class Bar:
    __slots__ = ("t", "open", "high", "low", "close", "volume", "taker_buy")

    def __init__(self, t, o, h, l, c, v=0.0, tb=0.0):
        self.t, self.open, self.high, self.low, self.close, self.volume, self.taker_buy = t, o, h, l, c, v, tb

    def delta(self) -> float:
        return 2.0 * self.taker_buy - self.volume

    def to_list(self):
        return [self.t, self.open, self.high, self.low, self.close, self.volume, self.taker_buy]


@dataclass
class ConceptConfig:
    swing_n: int = 3
    atr_n: int = 14
    rr: float = 2.0
    stop_buffer_atr: float = 0.1
    min_risk_frac: float = 0.0015
    hold_bars: int = 48
    order_expiry_bars: int = 20
    sweep_lookback: int = 50
    fvg_min_atr: float = 0.25
    displacement_atr: float = 1.0
    range_len: int = 40
    range_max_atr: float = 8.0
    climax_vol_mult: float = 1.5
    value_area: float = 0.70
    profile_bins: int = 50

    @staticmethod
    def load(path: str) -> "ConceptConfig":
        """Reads `smc.*` keys from knowledge/concepts.cfg (key = value)."""
        cfg = ConceptConfig()
        if not os.path.exists(path):
            return cfg
        for line in open(path, encoding="utf-8"):
            line = line.split("#", 1)[0].strip()
            if "=" not in line:
                continue
            k, v = (x.strip() for x in line.split("=", 1))
            if not k.startswith("smc."):
                continue
            name = k[4:]
            if not hasattr(cfg, name):
                raise ValueError(f"unknown config key: {k}")
            cur = getattr(cfg, name)
            setattr(cfg, name, int(v) if isinstance(cur, int) else float(v))
        return cfg


@dataclass
class Setup:
    t: int = 0
    pattern: str = ""
    symbol: str = ""
    tf: int = 0
    side: int = 1
    order: str = "M"
    ref: float = 0.0
    entry: float = 0.0
    stop: float = 0.0
    target: float = 0.0
    rr: float = 0.0
    expiry: int = 0
    max_hold: int = 0
    ctx: dict = field(default_factory=dict)  # trend, session, vol, vwap, flow


def swing_points(bars, n):
    m = len(bars)
    sh, sl = [False] * m, [False] * m
    for i in range(n, m - n):
        hi = lo = True
        bi = bars[i]
        for j in range(i - n, i + n + 1):
            if not (hi or lo):
                break
            if j == i:
                continue
            if bars[j].high >= bi.high:
                hi = False
            if bars[j].low <= bi.low:
                lo = False
        sh[i], sl[i] = hi, lo
    return sh, sl


def atr_series(bars, n):
    out = [NAN] * len(bars)
    atr = 0.0
    for k in range(1, len(bars)):
        pc = bars[k - 1].close
        b = bars[k]
        tr = max(b.high - b.low, abs(b.high - pc), abs(b.low - pc))
        if k < n:
            atr += tr
        elif k == n:
            atr = (atr + tr) / n
            out[k] = atr
        else:
            atr = (atr * (n - 1) + tr) / n
            out[k] = atr
    return out


def _max_high(b, fr, to):
    h = -math.inf
    for i in range(fr, to):
        if b[i].high > h:
            h = b[i].high
    return h


def _min_low(b, fr, to):
    l = math.inf
    for i in range(fr, to):
        if b[i].low < l:
            l = b[i].low
    return l


def value_area(bars, fr, to, bins, share):
    """(poc, val, vah) of bars[fr:to], each bar's volume spread evenly over its range."""
    if fr >= to or bins < 1:
        return (0.0, 0.0, 0.0)
    lo, hi = _min_low(bars, fr, to), _max_high(bars, fr, to)
    if not hi > lo:
        return (lo, lo, lo)
    w = (hi - lo) / bins
    vol = [0.0] * bins
    total = 0.0
    for i in range(fr, to):
        b0 = min(bins - 1, int((bars[i].low - lo) / w))
        b1 = min(bins - 1, int((bars[i].high - lo) / w))
        per = bars[i].volume / (b1 - b0 + 1)
        for b in range(b0, b1 + 1):
            vol[b] += per
        total += bars[i].volume
    poc = max(range(bins), key=lambda i: (vol[i], -i))  # first maximum, like std::max_element
    a = b = poc
    inside = vol[poc]
    while inside < share * total and (a > 0 or b < bins - 1):
        up = vol[b + 1] if b < bins - 1 else -1
        dn = vol[a - 1] if a > 0 else -1
        if up >= dn:
            b += 1
            inside += vol[b]
        else:
            a -= 1
            inside += vol[a]
    return (lo + (poc + 0.5) * w, lo + a * w, lo + (b + 1) * w)


def _session_of(t):
    h = (t - day_of(t) * 1440) // 60
    return "asia" if h < 7 else "london" if h < 13 else "ny" if h < 21 else "late"


def _context(bars, daily, tf, cfg):
    atr = atr_series(bars, cfg.atr_n)
    atr_long = atr_series(bars, 100)
    day_trend = []
    s = 0.0
    for d in range(len(daily)):
        s += daily[d].close
        if d >= 50:
            s -= daily[d - 50].close
        day_trend.append("" if d + 1 < 50 else ("up" if daily[d].close > s / 50.0 else "down"))
    trend, vwap = [""] * len(bars), [0.0] * len(bars)
    d = 0
    cur_day = None
    pv = v = 0.0
    for k, b in enumerate(bars):
        close_t = b.t + tf
        while d < len(daily) and daily[d].t + 1440 <= close_t:
            d += 1
        trend[k] = day_trend[d - 1] if d > 0 else ""
        day = day_of(b.t)
        if day != cur_day:
            cur_day = day
            pv = v = 0.0
        typical = (b.high + b.low + b.close) / 3.0
        pv += typical * b.volume
        v += b.volume
        vwap[k] = pv / v if v > 0 else b.close
    return atr, atr_long, trend, vwap


def detect_setups(bars, daily, tf, symbol, patterns, cfg: ConceptConfig):
    out = []
    if len(bars) < 120:
        return out
    atr_v, atr_l, trend, vwap = _context(bars, daily, tf, cfg)
    on = set(patterns).__contains__

    def base(k, pattern, side):
        b = bars[k]
        ratio = atr_v[k] / atr_l[k]
        return Setup(t=b.t + tf, pattern=pattern, symbol=symbol, tf=tf, side=side, ref=b.close, expiry=b.t + tf + tf,
                     max_hold=cfg.hold_bars * tf,
                     ctx={"trend": trend[k], "session": _session_of(b.t),
                          "vol": "" if math.isnan(ratio) else "low" if ratio < 0.8 else "high" if ratio > 1.2 else "mid",
                          "vwap": "above" if b.close >= vwap[k] else "below",
                          "flow": "with" if b.delta() * side > 0 else "against"})

    def push(s, ref):
        if s.side * (ref - s.stop) <= 0:
            return
        if abs(ref - s.stop) < cfg.min_risk_frac * ref:
            return
        if s.target != 0 and s.side * (s.target - ref) <= 0:
            return
        out.append(s)

    def market(k, pattern, side, stop):
        s = base(k, pattern, side)
        s.order, s.stop, s.rr = "M", stop, cfg.rr
        push(s, bars[k].close)

    def market_to(k, pattern, side, stop, target):
        s = base(k, pattern, side)
        s.order, s.stop, s.target = "M", stop, target
        push(s, bars[k].close)

    def limit(k, pattern, side, entry, stop):
        s = base(k, pattern, side)
        s.order, s.entry, s.stop = "L", entry, stop
        s.target = entry + side * cfg.rr * abs(entry - stop)
        s.expiry = s.t + cfg.order_expiry_bars * tf
        push(s, entry)

    sh, sl = swing_points(bars, cfg.swing_n)
    n = cfg.swing_n
    open_highs, open_lows, highs, lows = [], [], [], []  # (price, idx)
    high_broken = low_broken = True
    spring_cool = upthrust_cool = 0
    va_day = None
    va = (0.0, 0.0, 0.0)
    va_ok = va_done = False
    va_inside = va_from = 0
    day_start = prev_day_start = 0
    day_low = day_high = 0.0
    L = cfg.range_len

    for k in range(2, len(bars)):
        b = bars[k]
        atr = atr_v[k]
        if math.isnan(atr) or math.isnan(atr_l[k]):
            continue
        buf = cfg.stop_buffer_atr * atr

        if k >= 2 * n:
            i = k - n
            if sh[i]:
                highs.append((bars[i].high, i))
                open_highs.append((bars[i].high, i))
                high_broken = False
            if sl[i]:
                higher_low = bool(lows) and bars[i].low > lows[-1][0]
                higher_high = len(highs) >= 2 and highs[-1][0] > highs[-2][0]
                lows.append((bars[i].low, i))
                open_lows.append((bars[i].low, i))
                low_broken = False
                if on("hl_pullback") and higher_low and higher_high:
                    market(k, "hl_pullback", 1, bars[i].low - buf)
            if sh[i] and on("hl_pullback"):
                lower_high = len(highs) >= 2 and bars[i].high < highs[-2][0]
                lower_low = len(lows) >= 2 and lows[-1][0] < lows[-2][0]
                if lower_high and lower_low:
                    market(k, "hl_pullback", -1, bars[i].high + buf)

        up = len(highs) >= 2 and len(lows) >= 2 and highs[-1][0] > highs[-2][0] and lows[-1][0] > lows[-2][0]
        down = len(highs) >= 2 and len(lows) >= 2 and highs[-1][0] < highs[-2][0] and lows[-1][0] < lows[-2][0]
        if highs and not high_broken and b.close > highs[-1][0]:
            high_broken = True
            if on("choch") and down and lows:
                market(k, "choch", 1, lows[-1][0] - buf)
            if on("ob"):
                fr = (k - 20 if k > 20 else 0) if not lows else lows[-1][1]
                for j in range(k - 1, fr - 1, -1):
                    if bars[j].close < bars[j].open:
                        if bars[j].high < b.close:
                            limit(k, "ob", 1, bars[j].high, bars[j].low - buf)
                        break
        if lows and not low_broken and b.close < lows[-1][0]:
            low_broken = True
            if on("choch") and up and highs:
                market(k, "choch", -1, highs[-1][0] + buf)
            if on("ob"):
                fr = (k - 20 if k > 20 else 0) if not highs else highs[-1][1]
                for j in range(k - 1, fr - 1, -1):
                    if bars[j].close > bars[j].open:
                        if bars[j].low > b.close:
                            limit(k, "ob", -1, bars[j].low, bars[j].high + buf)
                        break

        def sweep(levels, side):
            swept = NAN
            keep = []
            for price, idx in levels:
                if k - idx > cfg.sweep_lookback:
                    continue
                beyond = b.high > price if side < 0 else b.low < price
                if not beyond:
                    keep.append((price, idx))
                    continue
                back = b.close < price if side < 0 else b.close > price
                if back and (math.isnan(swept) or side * (swept - price) > 0):
                    swept = price
            levels[:] = keep
            if math.isnan(swept):
                return
            stop = b.high + buf if side < 0 else b.low - buf
            if on("sweep"):
                market(k, "sweep", side, stop)
            if on("sweep_absorb") and b.delta() * side < 0:
                market(k, "sweep_absorb", side, stop)
            if on("sweep_cont"):
                market(k, "sweep_cont", -side, b.low - buf if side < 0 else b.high + buf)

        sweep(open_highs, -1)
        sweep(open_lows, 1)

        if on("fvg"):
            c1, c2 = bars[k - 2], bars[k - 1]
            disp = (c2.high - c2.low) >= cfg.displacement_atr * atr_v[k - 1]
            if disp and b.low > c1.high and b.low - c1.high >= cfg.fvg_min_atr * atr:
                limit(k, "fvg", 1, (b.low + c1.high) / 2.0, c1.low - buf)
            if disp and b.high < c1.low and c1.low - b.high >= cfg.fvg_min_atr * atr:
                limit(k, "fvg", -1, (b.high + c1.low) / 2.0, c1.high + buf)

        if on("spring") and k > L:
            hi, lo = _max_high(bars, k - L, k), _min_low(bars, k - L, k)
            if hi - lo <= cfg.range_max_atr * atr_v[k - 1]:
                vol = 0.0
                for j in range(k - L, k):
                    vol += bars[j].volume
                climax = b.volume >= cfg.climax_vol_mult * vol / float(L)
                if climax and b.low < lo and b.close > lo and k >= spring_cool:
                    market_to(k, "spring", 1, b.low - buf, hi)
                    spring_cool = k + L // 2
                if climax and b.high > hi and b.close < hi and k >= upthrust_cool:
                    market_to(k, "spring", -1, b.high + buf, lo)
                    upthrust_cool = k + L // 2

        if on("va80"):
            day = day_of(b.t)
            if day != va_day:
                prev_day_start, day_start = day_start, k
                va_ok = va_day is not None and day == va_day + 1
                va_day = day
                if va_ok:
                    va = value_area(bars, prev_day_start, day_start, cfg.profile_bins, cfg.value_area)
                va_done = False
                va_inside = 0
                va_from = 0 if not va_ok else 1 if b.open < va[1] else -1 if b.open > va[2] else 0
                day_low, day_high = b.low, b.high
            day_low = min(day_low, b.low)
            day_high = max(day_high, b.high)
            if va_from != 0 and not va_done:
                inside = va[1] < b.close < va[2]
                va_inside = va_inside + 1 if inside else 0
                if va_inside >= 2:
                    if va_from > 0:
                        market_to(k, "va80", 1, day_low - buf, va[2])
                    else:
                        market_to(k, "va80", -1, day_high + buf, va[1])
                    va_done = True
    return out


def resample(bars, minutes, anchor=None):
    """Aggregates bars into `minutes` buckets. Without `anchor` buckets align to the UTC
    epoch (as the C++ engine does); `anchor(t)` returns the session open (minutes) of the
    trading day that contains t, for exchange-aligned bars."""
    if minutes <= 1:
        return list(bars)
    out = []
    for b in bars:
        if anchor is None:
            bucket = (b.t // minutes) * minutes
        else:
            a = anchor(b.t)
            bucket = a + ((b.t - a) // minutes) * minutes
        if not out or out[-1].t != bucket:
            out.append(Bar(bucket, b.open, b.high, b.low, b.close, b.volume, b.taker_buy))
        else:
            o = out[-1]
            o.high = max(o.high, b.high)
            o.low = min(o.low, b.low)
            o.close = b.close
            o.volume += b.volume
            o.taker_buy += b.taker_buy
    return out


def parse_minutes(s: str) -> int:
    """'YYYY-MM-DD HH:MM' (UTC) -> minutes since the epoch."""
    import datetime as _dt
    d = _dt.datetime(int(s[0:4]), int(s[5:7]), int(s[8:10]), int(s[11:13]) if len(s) >= 16 else 0,
                     int(s[14:16]) if len(s) >= 16 else 0)
    return (d - _dt.datetime(1970, 1, 1)).days * 1440 + d.hour * 60 + d.minute


def format_minutes(t: int) -> str:
    import datetime as _dt
    d = _dt.datetime(1970, 1, 1) + _dt.timedelta(minutes=t)
    return d.strftime("%Y-%m-%d %H:%M")


def load_flow_csv(path: str):
    """Reads date,open,high,low,close,volume[,taker_buy] (optionally .gz) into Bars."""
    import gzip
    opener = gzip.open if path.endswith(".gz") else open
    out = []
    with opener(path, "rt") as f:
        next(f)
        for line in f:
            p = line.rstrip("\n").split(",")
            out.append(Bar(parse_minutes(p[0]), float(p[1]), float(p[2]), float(p[3]), float(p[4]), float(p[5]),
                           float(p[6]) if len(p) > 6 and p[6] else 0.0))
    out.sort(key=lambda b: b.t)
    return out
