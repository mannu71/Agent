#!/usr/bin/env bash
# Downloads NSE cash-market bhavcopies for a date range and unzips them into OUT_DIR,
# ready for: ta_ingest bhavcopy OUT_DIR data/nse
#
# Tested against nsearchives.nseindia.com in October 2026. NSE changes its archive URLs
# from time to time and refuses clients without browser-like headers.
#   UDiFF (from 2024-07-08): nsearchives.nseindia.com/content/cm/BhavCopy_NSE_CM_0_0_0_YYYYMMDD_F_0000.csv.zip
#   legacy (before):         nsearchives.nseindia.com/content/historical/EQUITIES/YYYY/MON/cmDDMONYYYYbhav.csv.zip
set -euo pipefail
if [ $# -ne 3 ]; then
  echo "usage: $0 START(YYYY-MM-DD) END(YYYY-MM-DD) OUT_DIR" >&2
  exit 2
fi
start=$1 end=$2 out=$3
mkdir -p "$out"
ua="Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/124.0 Safari/537.36"
d=$start
while [ "$(date -d "$d" +%Y%m%d)" -le "$(date -d "$end" +%Y%m%d)" ]; do
  if [ "$(date -d "$d" +%u)" -le 5 ]; then
    ymd=$(date -d "$d" +%Y%m%d)
    if [ -e "$out/.done_$ymd" ]; then d=$(date -d "$d + 1 day" +%Y-%m-%d); continue; fi
    if [ "$ymd" -ge 20240708 ]; then
      url="https://nsearchives.nseindia.com/content/cm/BhavCopy_NSE_CM_0_0_0_${ymd}_F_0000.csv.zip"
    else
      mon=$(date -d "$d" +%b | tr '[:lower:]' '[:upper:]')
      url="https://nsearchives.nseindia.com/content/historical/EQUITIES/$(date -d "$d" +%Y)/${mon}/cm$(date -d "$d" +%d)${mon}$(date -d "$d" +%Y)bhav.csv.zip"
    fi
    zip="$out/$ymd.zip"
    if curl -sf -A "$ua" -H "Referer: https://www.nseindia.com/" -o "$zip" "$url"; then
      unzip -oq "$zip" -d "$out" && rm -f "$zip" && touch "$out/.done_$ymd"
      echo "ok   $d"
    else
      rm -f "$zip"
      echo "skip $d (holiday or not published)"
    fi
    sleep 0.3  # be polite to the archive
  fi
  d=$(date -d "$d + 1 day" +%Y-%m-%d)
done
