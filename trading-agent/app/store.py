"""In-memory bar store per symbol: recent 1-minute bars, longer 15-minute history, daily bars.

1-minute bars arrive from the feeds (closed ones, plus the live forming bar); closed
15-minute bars are built from them so the signal engine sees exactly what a resample of
the 1-minute data would give. Times are minutes since the epoch (UTC).
"""
from .engine.concepts import Bar, resample

M1_CAP = 4 * 1440
M15_CAP = 120 * 96
D1_CAP = 400


def _bucket(t, minutes, anchor):
    if anchor is None:
        return (t // minutes) * minutes
    a = anchor(t)
    return a + ((t - a) // minutes) * minutes


class Series:
    def __init__(self, market, symbol, anchor=None):
        self.market, self.symbol, self.anchor = market, symbol, anchor
        self.m1, self.m15, self.d1 = [], [], []
        self.forming = None
        self.last_price = None
        self.last_t = None  # epoch seconds of the last price update
        self.has_flow = False

    # -------------------------------------------------------------- seeding
    def seed(self, m1=(), m15=(), d1=()):
        self.m1 = sorted(m1, key=lambda b: b.t)[-M1_CAP:]
        hist = sorted(m15, key=lambda b: b.t)
        # Rebuild the 15-minute bars that the 1-minute data covers, so both agree exactly.
        if self.m1:
            first = _bucket(self.m1[0].t, 15, self.anchor)
            hist = [b for b in hist if b.t < first]
            hist += resample([b for b in self.m1 if b.t >= first], 15, self.anchor)
        self.m15 = hist[-M15_CAP:]
        self._drop_forming_15()
        self.d1 = sorted(d1, key=lambda b: b.t)[-D1_CAP:]
        self.has_flow = any(b.taker_buy for b in self.m1[-500:])
        if self.m1:
            self.last_price = self.m1[-1].close

    def _drop_forming_15(self):
        """The last 15-minute bar stays out of m15 until its final minute has arrived."""
        if self.m15 and self.m1 and self.m15[-1].t + 15 > self.m1[-1].t + 1:
            self.m15.pop()

    # -------------------------------------------------------------- live updates
    def update(self, bar: Bar, closed: bool, now_s: int):
        """Applies a 1-minute bar. Returns True when a 15-minute bar has just closed."""
        self.last_price, self.last_t = bar.close, now_s
        if bar.taker_buy:
            self.has_flow = True
        if not closed:
            self.forming = bar
            return False
        if self.m1 and bar.t <= self.m1[-1].t:
            if bar.t == self.m1[-1].t:
                self.m1[-1] = bar
            return False
        self.m1.append(bar)
        if len(self.m1) > M1_CAP:
            del self.m1[: len(self.m1) - M1_CAP]
        if self.forming is not None and self.forming.t <= bar.t:
            self.forming = None
        # Close every 15-minute bucket that is complete: earlier buckets once a later minute
        # arrives (stocks can skip quiet minutes), and this one at its final minute.
        start = _bucket(bar.t, 15, self.anchor)
        done = start + 15 if bar.t + 1 == start + 15 else start
        last15 = self.m15[-1].t if self.m15 else None
        fresh = [b for b in self.m1[-60:] if b.t < done and (last15 is None or _bucket(b.t, 15, self.anchor) > last15)]
        closed_any = False
        for agg in resample(fresh, 15, self.anchor):
            self.m15.append(agg)
            self._roll_daily(agg)
            closed_any = True
        if len(self.m15) > M15_CAP:
            del self.m15[: len(self.m15) - M15_CAP]
        return closed_any

    def _roll_daily(self, b15):
        day = (b15.t // 1440) * 1440
        if self.d1 and self.d1[-1].t == day:
            d = self.d1[-1]
            d.high, d.low, d.close = max(d.high, b15.high), min(d.low, b15.low), b15.close
            d.volume += b15.volume
        elif not self.d1 or day > self.d1[-1].t:
            self.d1.append(Bar(day, b15.open, b15.high, b15.low, b15.close, b15.volume, b15.taker_buy))

    # -------------------------------------------------------------- views
    def closed(self, tf):
        """Closed bars at `tf` minutes for the signal engine (no forming bar)."""
        if tf == 1440:
            return [b for b in self.d1 if self.m1 and b.t + 1440 <= self.m1[-1].t + 1]
        if tf < 15:
            return resample(self.m1, tf, self.anchor) if tf > 1 else list(self.m1)
        bars = resample(self.m15, tf, self.anchor)
        if bars and self.m15 and bars[-1].t + tf > self.m15[-1].t + 15:
            bars.pop()  # still forming
        return bars

    def chart(self, tf, limit=600):
        """Bars for display, including the live forming bar."""
        if tf == 1440:
            return self.d1[-limit:]
        mins = list(self.m1) + ([self.forming] if self.forming is not None else [])
        if tf < 15:
            return (resample(mins, tf, self.anchor) if tf > 1 else mins)[-limit:]
        start = _bucket(mins[-1].t, 15, self.anchor) if mins else None
        recent = [b for b in mins if start is not None and b.t >= start]
        base = list(self.m15) + resample(recent, 15, self.anchor)
        return resample(base, tf, self.anchor)[-limit:]
