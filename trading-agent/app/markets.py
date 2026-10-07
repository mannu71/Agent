"""Market metadata: watchlists, trading hours and session-aligned bar anchors."""
import datetime as dt
from zoneinfo import ZoneInfo

ET = ZoneInfo("America/New_York")
IST = ZoneInfo("Asia/Kolkata")

WATCHLISTS = {
    "crypto": ["BTC", "ETH", "SOL", "XRP", "BNB"],
    "us": ["SPY", "QQQ", "AAPL", "MSFT", "NVDA", "AMZN", "TSLA", "META"],
    "india": ["NIFTY", "RELIANCE", "TCS", "HDFCBANK", "ICICIBANK", "INFY", "SBIN"],
}
LABELS = {"crypto": "Crypto", "us": "US", "india": "India"}

# Session open/close in exchange local time, Monday-Friday (crypto trades all week).
SESSIONS = {"us": (ET, (9, 30), (16, 0)), "india": (IST, (9, 15), (15, 30))}


def yahoo_symbol(market, sym):
    if market == "india":
        return "^NSEI" if sym == "NIFTY" else f"{sym}.NS"
    return sym


def is_open(market, t=None):
    """Is the market trading at epoch seconds t (default now)?"""
    if market == "crypto":
        return True
    tz, o, c = SESSIONS[market]
    now = dt.datetime.fromtimestamp(t if t is not None else dt.datetime.now(dt.timezone.utc).timestamp(), tz)
    if now.weekday() >= 5:
        return False
    m = now.hour * 60 + now.minute
    return o[0] * 60 + o[1] <= m < c[0] * 60 + c[1]


def anchor_fn(market):
    """Function minutes -> session open (minutes) of that trading day, for bar alignment;
    None for crypto (bars align to UTC, as in the backtest engine)."""
    if market == "crypto":
        return None
    tz, o, _ = SESSIONS[market]
    cache = {}

    def anchor(t_min):
        local = dt.datetime.fromtimestamp(t_min * 60, tz)
        key = local.date()
        if key not in cache:
            op = dt.datetime(local.year, local.month, local.day, o[0], o[1], tzinfo=tz)
            cache[key] = int(op.timestamp()) // 60
        return cache[key]

    return anchor
