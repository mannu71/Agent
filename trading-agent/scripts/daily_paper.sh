#!/usr/bin/env bash
# Evening update of both paper accounts on real data. Run after the NSE bhavcopy is out
# (usually 18:00-19:30 IST): scripts/daily_paper.sh [YYYY-MM-DD, default today in IST]
#   paper/live       A1 swing breakouts + B crypto trend
#   paper/intraday   A2 results gaps + D1 Nifty last half hour (paper-only shadows)
set -euo pipefail
cd "$(dirname "$0")/.."
d=${1:-$(TZ=Asia/Kolkata date +%F)}
bin=${BUILD:-build-rel}

scripts/fetch_nse_bhavcopy.sh "$d" "$d" data/raw/bhav
"$bin"/ta_ingest bhavcopy data/raw/bhav data/nse --actions data/corporate_actions_all.csv --non-eq-out data/t2t.csv
scripts/fetch_yahoo.py ^NSEI data/index/nifty50.csv
scripts/fetch_yahoo.py ^INDIAVIX data/index/india_vix.csv --value
scripts/fetch_binance_daily.py BTCUSDT data/crypto/BTC.csv
scripts/fetch_binance_daily.py ETHUSDT data/crypto/ETH.csv
scripts/fetch_delta_funding.py BTCUSD 2023-12-01 data/funding/BTC.csv
scripts/fetch_delta_funding.py ETHUSD 2023-12-01 data/funding/ETH.csv
scripts/fetch_intraday.py "$d" "$d"

"$bin"/ta_paper run paper/live
"$bin"/ta_paper run paper/intraday
"$bin"/ta_paper verify paper/live
"$bin"/ta_paper verify paper/intraday
