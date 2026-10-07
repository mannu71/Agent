"""The Python engine must reproduce the C++ engine exactly (fixtures from ta_backtest setups)."""
import csv
import os
import unittest

from app.engine.casebook import ExecCost, simulate_case
from app.engine.concepts import ALL_PATTERNS, ConceptConfig, detect_setups, format_minutes, load_flow_csv, resample

HERE = os.path.dirname(os.path.abspath(__file__))
FIX = os.path.join(HERE, "fixtures")
CFG = os.path.join(HERE, "..", "..", "knowledge", "concepts.cfg")


def close(a, b):
    return abs(a - b) <= 1e-9 * max(1.0, abs(a), abs(b))


class Parity(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.m1 = load_flow_csv(os.path.join(FIX, "BTC_1m.csv.gz"))
        cls.cfg = ConceptConfig.load(CFG)

    def check(self, tf):
        bars = resample(self.m1, tf)
        if bars and bars[-1].t + tf > self.m1[-1].t + 1:
            bars.pop()
        daily = resample(self.m1, 1440)
        setups = detect_setups(bars, daily, tf, "BTC", ALL_PATTERNS, self.cfg)
        with open(os.path.join(FIX, f"BTC_tf{tf}.cases.csv")) as f:
            expect = list(csv.DictReader(f))
        self.assertEqual(len(setups), len(expect))
        for s, e in zip(setups, expect):
            c = simulate_case(s, self.m1, ExecCost())
            got = (format_minutes(s.t), s.pattern, str(s.side), s.order, s.ctx["trend"], s.ctx["session"],
                   s.ctx["vol"], s.ctx["vwap"], s.ctx["flow"], str(int(c.filled)), c.exit_reason)
            want = (e["time"], e["pattern"], e["side"], e["order"], e["trend"], e["session"], e["vol"], e["vwap"],
                    e["flow"], e["filled"], e["exit_reason"])
            self.assertEqual(got, want)
            if c.filled:
                self.assertEqual(format_minutes(c.entry_t), e["entry_time"])
                self.assertEqual(format_minutes(c.exit_t), e["exit_time"])
            for a, key in ((c.entry_px, "entry"), (c.stop_px, "stop"), (c.target_px, "target"), (c.exit_px, "exit"),
                           (c.r_gross, "r_gross"), (c.r_net, "r_net")):
                self.assertTrue(close(a, float(e[key])), f"{key} {a} != {e[key]} at {e['time']} {e['pattern']}")

    def test_tf15(self):
        self.check(15)

    def test_tf60(self):
        self.check(60)

    def test_no_look_ahead(self):
        bars = resample(self.m1, 60)
        daily = resample(self.m1, 1440)
        full = detect_setups(bars, daily, 60, "BTC", ALL_PATTERNS, self.cfg)
        cut = bars[: len(bars) * 2 // 3]
        end = cut[-1].t + 60
        part = detect_setups(cut, [d for d in daily if d.t + 1440 <= end], 60, "BTC", ALL_PATTERNS, self.cfg)
        early = [(s.t, s.pattern, s.side, s.stop) for s in full if s.t <= end]
        self.assertTrue(part)
        self.assertEqual(early, [(s.t, s.pattern, s.side, s.stop) for s in part])


if __name__ == "__main__":
    unittest.main()
