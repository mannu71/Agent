#!/usr/bin/env bash
# Hourly live paper run of the institutional-concept agent on crypto (paper only).
# Pulls the newest closed 1-minute bars, recomputes every setup with the frozen rules, and
# appends new events to paper/crypto_concepts/tf{60,240}/journal.log (hash-chained).
# Rules were frozen on 2026-10-07 after a backtest on data to 2026-09-30, so everything from
# 2026-10-01 is out of sample; journal timestamps show when each event was first recorded.
set -euo pipefail
cd "$(dirname "$0")/.."
bin=${BUILD:-build-rel}
data=data/crypto_1m
syms="BTC ETH SOL XRP BNB"

if [ ! -s "$data/BTC.csv" ]; then  # fresh container: rebuild the history first
    mkdir -p "$data"
    for s in $syms; do scripts/fetch_binance_klines.py "${s}USDT" 2020-01 "$(date -u -d 'last month' +%Y-%m)" "$data/$s.csv" >/dev/null; done
fi
scripts/update_binance_1m.py "$data" $syms >/dev/null
for tf in 60 240; do
    "$bin"/ta_backtest setups "$data" --tf "$tf" --start 2020-01-01 --live "paper/crypto_concepts/tf$tf" \
        --live-from 2026-10-01 2>/dev/null
    "$bin"/ta_paper verify "paper/crypto_concepts/tf$tf" >/dev/null || { echo "journal check FAILED for tf$tf"; exit 1; }
done
