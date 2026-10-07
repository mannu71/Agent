import datetime as dt
import os
import shutil
import subprocess
import tempfile
import unittest
from zoneinfo import ZoneInfo

from app.broker import Broker, OrderError, fees
from app.journal import Journal

ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..")
T0 = int(dt.datetime(2026, 10, 7, 12, 0, tzinfo=dt.timezone.utc).timestamp())


class BrokerTest(unittest.TestCase):
    def setUp(self):
        self.dir = tempfile.mkdtemp()
        self.now = T0
        self.b = Broker(self.dir, clock=lambda: self.now)

    def tearDown(self):
        shutil.rmtree(self.dir)

    def tick(self, market, sym, px, **kw):
        self.now += 60
        return self.b.on_price(market, sym, self.now, px, **kw)

    def test_market_bracket_hits_target(self):
        self.tick("crypto", "BTC", 100.0)
        o = self.b.place_order("crypto", "BTC", "buy", "market", qty=10, stop=98.0, target=104.0)
        self.assertEqual(o["status"], "filled")
        fill = 100.0 * 1.0002
        self.assertAlmostEqual(o["fill_price"], fill)
        a = self.b.account("crypto")
        self.assertAlmostEqual(a["cash"], 10_000 - 10 * fill * 0.0005)
        self.tick("crypto", "BTC", 104.5, high=104.5, low=101.0)
        self.assertEqual(self.b.positions("crypto"), [])
        t = self.b.trades("crypto")[0]
        self.assertEqual(t["reason"], "target")
        pnl = (104.0 - fill) * 10 - 10 * fill * 0.0005 - 10 * 104.0 * 0.0002
        self.assertAlmostEqual(t["pnl"], pnl)
        self.assertAlmostEqual(t["r"], pnl / (10 * (fill - 98.0)))
        self.assertAlmostEqual(self.b.account("crypto")["cash"], 10_000 + pnl)

    def test_stop_wins_when_one_bar_touches_both(self):
        self.tick("crypto", "ETH", 100.0)
        self.b.place_order("crypto", "ETH", "buy", "market", qty=1, stop=98.0, target=104.0)
        self.tick("crypto", "ETH", 100.0, high=105.0, low=97.0)
        self.assertEqual(self.b.trades("crypto")[0]["reason"], "stop")

    def test_limit_fill_then_no_target_in_same_bar(self):
        self.tick("crypto", "SOL", 100.0)
        self.b.place_order("crypto", "SOL", "buy", "limit", qty=2, price=99.0, stop=97.0, target=103.0)
        self.assertEqual(len(self.b.orders("crypto")), 1)
        self.tick("crypto", "SOL", 100.5, high=103.5, low=98.5)  # filled at 99; target may have come first
        pos = self.b.positions("crypto")
        self.assertEqual(len(pos), 1)
        self.assertAlmostEqual(pos[0]["avg"], 99.0)
        self.tick("crypto", "SOL", 103.2, high=103.2, low=102.0)
        self.assertEqual(self.b.trades("crypto")[0]["reason"], "target")

    def test_stop_order_and_short(self):
        self.tick("crypto", "XRP", 1.0)
        self.b.place_order("crypto", "XRP", "sell", "stop", qty=100, price=0.98, stop=1.01)
        self.tick("crypto", "XRP", 0.975)
        p = self.b.positions("crypto")[0]
        self.assertLess(p["qty"], 0)
        self.assertAlmostEqual(p["avg"], 0.975 * (1 - 0.0002))

    def test_quantity_from_risk(self):
        self.tick("us", "SPY", 500.0)
        o = self.b.place_order("us", "SPY", "buy", "market", risk_pct=1.0, stop=495.0)
        self.assertEqual(o["qty"], 20.0)  # 1% of $10,000 = $100 risk / $5 per share

    def test_rejections(self):
        self.tick("us", "AAPL", 200.0)
        with self.assertRaises(OrderError):
            self.b.place_order("us", "AAPL", "buy", "market", qty=10, stop=205.0)  # stop above a buy
        with self.assertRaises(OrderError):
            self.b.place_order("us", "AAPL", "buy", "market", qty=100)  # $20,000 > 1x $10,000
        with self.assertRaises(OrderError):
            self.b.place_order("us", "AAPL", "buy", "market", qty=1, market_open=False)
        with self.assertRaises(OrderError):
            self.b.place_order("us", "MSFT", "buy", "market", qty=1)  # no price yet

    def test_india_charges_by_hand(self):
        buy = fees("india", 1, 100, 1000.0)
        self.assertAlmostEqual(buy, 20 + 3 + 2.97 + 0.1 + 0.18 * (20 + 2.97 + 0.1))
        sell = fees("india", -1, 100, 1000.0)
        self.assertAlmostEqual(sell, 20 + 25 + 2.97 + 0.1 + 0.18 * (20 + 2.97 + 0.1))
        self.assertAlmostEqual(fees("us", -1, 100, 100.0), 10_000 * 27.8 / 1e6 + 0.0166)
        self.assertEqual(fees("us", 1, 100, 100.0), 0.0)

    def test_india_short_squared_off(self):
        ist = ZoneInfo("Asia/Kolkata")
        self.now = int(dt.datetime(2026, 10, 7, 11, 0, tzinfo=ist).timestamp())
        self.b.on_price("india", "SBIN", self.now, 800.0)
        self.b.place_order("india", "SBIN", "sell", "market", qty=10)
        self.now = int(dt.datetime(2026, 10, 7, 15, 21, tzinfo=ist).timestamp())
        self.b.on_price("india", "SBIN", self.now, 795.0)
        self.assertEqual(self.b.positions("india"), [])
        self.assertEqual(self.b.trades("india")[0]["reason"], "square_off")
        with self.assertRaises(OrderError):
            self.b.place_order("india", "SBIN", "sell", "market", qty=10)

    def test_restart_keeps_state_and_journal_verifies(self):
        self.tick("crypto", "BNB", 600.0)
        self.b.place_order("crypto", "BNB", "buy", "market", qty=1, stop=590.0, target=620.0)
        self.b.place_order("crypto", "BNB", "buy", "limit", qty=1, price=590.0)
        self.b.cancel_order(self.b.orders("crypto")[0]["id"])
        before = (self.b.account("crypto")["cash"], self.b.positions("crypto")[0]["qty"])
        b2 = Broker(self.dir, clock=lambda: self.now)
        self.assertEqual((b2.account("crypto")["cash"], b2.positions("crypto")[0]["qty"]), before)
        b2.journal.append("note", text="restart works")
        ok, lines, bad = Journal.verify(os.path.join(self.dir, "journal.log"))
        self.assertTrue(ok and lines >= 6 and bad == 0)
        cpp = os.path.join(ROOT, "build-rel", "ta_paper")
        if os.path.exists(cpp):  # the C++ verifier accepts the Python journal
            out = subprocess.run([cpp, "verify", self.dir], capture_output=True, text=True)
            self.assertEqual(out.returncode, 0, out.stdout + out.stderr)


if __name__ == "__main__":
    unittest.main()


class BuyingPower(unittest.TestCase):
    def test_risk_sizing_is_cut_to_buying_power(self):
        d = tempfile.mkdtemp()
        try:
            b = Broker(d, clock=lambda: T0)
            b.on_price("us", "SPY", T0, 500.0)
            o = b.place_order("us", "SPY", "buy", "market", risk_pct=0.5, stop=499.8)  # wants 250 shares
            self.assertEqual(o["qty"], 20.0)  # $10,000 at 1x / $500
            self.assertIn("buying power", o["note"])
        finally:
            shutil.rmtree(d)

    def test_bar_range_ignores_prices_before_an_order(self):
        d = tempfile.mkdtemp()
        try:
            now = [T0]
            b = Broker(d, clock=lambda: now[0])
            b.on_price("crypto", "BTC", T0, 100.0)
            now[0] = T0 + 30  # order placed mid-minute
            b.place_order("crypto", "BTC", "buy", "market", qty=1, stop=99.0, target=103.0)
            # the minute's low (98) happened before the order: no stop-out
            b.on_price("crypto", "BTC", T0 + 60, 100.5, high=100.6, low=98.0, bar_start=T0)
            self.assertEqual(len(b.positions("crypto")), 1)
        finally:
            shutil.rmtree(d)
