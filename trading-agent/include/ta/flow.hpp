#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace ta {

// One intraday bar with order flow. Time is minutes since 1970-01-01 00:00 UTC at the
// bar's open; taker_buy is the base volume bought by aggressive (taker) orders, so the
// bar's delta (taker buys minus taker sells) is 2 * taker_buy - volume.
struct FlowBar {
    std::int64_t t = 0;
    double open = 0;
    double high = 0;
    double low = 0;
    double close = 0;
    double volume = 0;
    double taker_buy = 0;
    double delta() const { return 2.0 * taker_buy - volume; }
};

using FlowSeries = std::vector<FlowBar>;

// "YYYY-MM-DD HH:MM" (or "YYYY-MM-DD") <-> minutes since the epoch, UTC.
std::int64_t parse_minutes(const std::string& s);
std::string format_minutes(std::int64_t t);
inline std::int64_t day_of(std::int64_t t) { return t >= 0 ? t / 1440 : (t - 1439) / 1440; }

// CSV "date,open,high,low,close,volume[,taker_buy]" sorted by time; taker_buy 0 if absent.
FlowSeries load_flow(const std::string& path);

// Aggregates bars into buckets of `minutes` aligned to the UTC epoch (1440 = UTC days).
// A bucket's time is its open; empty buckets are skipped.
FlowSeries resample(const FlowSeries& bars, int minutes);

// Index of the first bar with t >= time (bars.size() if none).
std::size_t lower_index(const FlowSeries& bars, std::int64_t time);

}  // namespace ta
