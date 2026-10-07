import json
import os
import unittest

from app.engine.concepts import Bar, parse_minutes, resample
from app.feeds import alpaca, binance, yahoo
from app.markets import anchor_fn, is_open
from app.store import Series

FIX = os.path.join(os.path.dirname(os.path.abspath(__file__)), "fixtures")


def load(name):
    with open(os.path.join(FIX, name)) as f:
        return json.load(f)


class Feeds(unittest.TestCase):
    def test_binance_rest_and_ws(self):
        rows = load("binance_rest.json")
        b = binance.parse_rest(rows[0])
        self.assertEqual(b.t, rows[0][0] // 60000)
        self.assertEqual(b.taker_buy, float(rows[0][9]))
        sym, bar, closed = binance.parse_ws(load("binance_ws.json"))
        self.assertEqual(sym, "BTC")
        self.assertIsInstance(closed, bool)
        self.assertGreater(bar.high, 0)

    def test_yahoo_chart(self):
        bars, price = yahoo.parse_chart(load("yahoo_chart.json"), 1)
        self.assertEqual(len(bars), 5)
        self.assertEqual(bars[1].t - bars[0].t, 1)
        self.assertGreater(price, 0)
        daily, _ = yahoo.parse_chart(load("yahoo_chart.json"), 1440)
        self.assertEqual(daily[0].t % 1440, 0)

    def test_alpaca_bar_and_session(self):
        b = alpaca.parse_bar({"t": "2026-10-07T14:30:00Z", "o": 1, "h": 2, "l": 0.5, "c": 1.5, "v": 100})
        self.assertEqual(b.t, parse_minutes("2026-10-07 14:30"))
        self.assertTrue(alpaca.regular(b))  # 10:30 ET
        pre = alpaca.parse_bar({"t": "2026-10-07T12:00:00Z", "o": 1, "h": 1, "l": 1, "c": 1, "v": 1})
        self.assertFalse(alpaca.regular(pre))  # 08:00 ET pre-market
        self.assertFalse(is_open("us", parse_minutes("2026-10-10 15:00") * 60))  # Saturday

    def test_session_anchor(self):
        a = anchor_fn("india")
        t = parse_minutes("2026-10-07 05:00")  # 10:30 IST
        self.assertEqual(a(t), parse_minutes("2026-10-07 03:45"))  # 09:15 IST


def minutes(start, n, price=100.0, step=0.0):
    t0 = parse_minutes(start)
    return [Bar(t0 + k, price + k * step, price + k * step + 1, price + k * step - 1, price + k * step + 0.5, 10.0, 6.0)
            for k in range(n)]


class Store(unittest.TestCase):
    def test_15m_bars_close_on_their_last_minute(self):
        s = Series("crypto", "BTC")
        bars = minutes("2026-10-07 10:00", 44, step=0.1)
        s.seed(m1=bars[:30])
        self.assertEqual(len(s.m15), 2)
        closed = [s.update(b, True, 0) for b in bars[30:]]
        self.assertEqual(sum(closed), 0)  # 10:30-10:43 still forming
        self.assertTrue(s.update(minutes("2026-10-07 10:44", 1)[0], True, 0))
        self.assertEqual(len(s.m15), 3)
        ref = resample(bars + minutes("2026-10-07 10:44", 1), 15)
        self.assertEqual([b.to_list() for b in s.m15], [b.to_list() for b in ref])

    def test_missing_minute_still_closes_bucket(self):
        s = Series("us", "SPY", anchor_fn("us"))
        bars = minutes("2026-10-07 13:30", 14)  # 9:30-9:43 ET, 9:44 never prints
        s.seed(m1=bars[:1])
        for b in bars[1:]:
            s.update(b, True, 0)
        self.assertEqual(len(s.m15), 0)
        self.assertTrue(s.update(minutes("2026-10-07 13:45", 1)[0], True, 0))
        self.assertEqual(s.m15[0].t, parse_minutes("2026-10-07 13:30"))

    def test_closed_view_drops_forming_and_chart_keeps_it(self):
        s = Series("crypto", "ETH")
        s.seed(m1=minutes("2026-10-07 00:00", 130))  # 2h10m: the 02:00 hour is forming
        self.assertEqual([b.t for b in s.closed(60)][-1], parse_minutes("2026-10-07 01:00"))
        s.update(Bar(parse_minutes("2026-10-07 02:10"), 1, 2, 0.5, 1.5, 1, 0), False, 0)
        self.assertEqual(s.chart(60)[-1].t, parse_minutes("2026-10-07 02:00"))
        self.assertEqual(s.chart(1)[-1].close, 1.5)


if __name__ == "__main__":
    unittest.main()
