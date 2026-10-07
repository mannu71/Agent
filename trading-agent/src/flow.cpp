#include "ta/flow.hpp"

#include <algorithm>
#include <charconv>
#include <cstdio>
#include <fstream>
#include <stdexcept>

namespace ta {

namespace {

// Howard Hinnant's days_from_civil / civil_from_days.
std::int64_t days_from_civil(std::int64_t y, unsigned m, unsigned d) {
    y -= m <= 2;
    const std::int64_t era = (y >= 0 ? y : y - 399) / 400;
    const unsigned yoe = static_cast<unsigned>(y - era * 400);
    const unsigned doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
    const unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return era * 146097 + static_cast<std::int64_t>(doe) - 719468;
}

void civil_from_days(std::int64_t z, int& y, unsigned& m, unsigned& d) {
    z += 719468;
    const std::int64_t era = (z >= 0 ? z : z - 146096) / 146097;
    const unsigned doe = static_cast<unsigned>(z - era * 146097);
    const unsigned yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    const unsigned doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    const unsigned mp = (5 * doy + 2) / 153;
    d = doy - (153 * mp + 2) / 5 + 1;
    m = mp < 10 ? mp + 3 : mp - 9;
    y = static_cast<int>(static_cast<std::int64_t>(yoe) + era * 400 + (m <= 2));
}

int num(const std::string& s, std::size_t pos, std::size_t len) {
    int v = 0;
    const auto r = std::from_chars(s.data() + pos, s.data() + pos + len, v);
    if (r.ec != std::errc() || r.ptr != s.data() + pos + len) throw std::runtime_error("bad time: " + s);
    return v;
}

double field(const char*& p, const char* end) {
    double v = 0;
    while (p < end && *p == ' ') ++p;
    const auto r = std::from_chars(p, end, v);
    if (r.ec != std::errc()) throw std::runtime_error("bad number");
    p = r.ptr;
    if (p < end && *p == ',') ++p;
    return v;
}

}  // namespace

std::int64_t parse_minutes(const std::string& s) {
    if (s.size() < 10 || s[4] != '-' || s[7] != '-') throw std::runtime_error("bad time: " + s);
    const std::int64_t days = days_from_civil(num(s, 0, 4), static_cast<unsigned>(num(s, 5, 2)),
                                              static_cast<unsigned>(num(s, 8, 2)));
    std::int64_t mins = 0;
    if (s.size() >= 16) mins = num(s, 11, 2) * 60 + num(s, 14, 2);
    return days * 1440 + mins;
}

std::string format_minutes(std::int64_t t) {
    int y = 0;
    unsigned m = 0, d = 0;
    const std::int64_t day = day_of(t);
    civil_from_days(day, y, m, d);
    const auto mins = static_cast<int>(t - day * 1440);
    char buf[32];
    std::snprintf(buf, sizeof buf, "%04d-%02u-%02u %02d:%02d", y, m, d, mins / 60, mins % 60);
    return buf;
}

FlowSeries load_flow(const std::string& path) {
    std::ifstream in(path);
    if (!in) throw std::runtime_error("cannot open " + path);
    FlowSeries out;
    std::string line;
    std::getline(in, line);  // header
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        const auto comma = line.find(',');
        if (comma == std::string::npos) throw std::runtime_error(path + ": bad row " + line);
        FlowBar b;
        b.t = parse_minutes(line.substr(0, comma));
        const char* p = line.data() + comma + 1;
        const char* end = line.data() + line.size();
        b.open = field(p, end);
        b.high = field(p, end);
        b.low = field(p, end);
        b.close = field(p, end);
        b.volume = field(p, end);
        if (p < end) b.taker_buy = field(p, end);
        out.push_back(b);
    }
    std::sort(out.begin(), out.end(), [](const FlowBar& a, const FlowBar& b) { return a.t < b.t; });
    return out;
}

FlowSeries resample(const FlowSeries& bars, int minutes) {
    if (minutes <= 1) return bars;
    FlowSeries out;
    for (const auto& b : bars) {
        const std::int64_t bucket = (b.t >= 0 ? b.t / minutes : (b.t - minutes + 1) / minutes) * minutes;
        if (out.empty() || out.back().t != bucket) {
            FlowBar n = b;
            n.t = bucket;
            out.push_back(n);
        } else {
            FlowBar& o = out.back();
            o.high = std::max(o.high, b.high);
            o.low = std::min(o.low, b.low);
            o.close = b.close;
            o.volume += b.volume;
            o.taker_buy += b.taker_buy;
        }
    }
    return out;
}

std::size_t lower_index(const FlowSeries& bars, std::int64_t time) {
    return static_cast<std::size_t>(
        std::lower_bound(bars.begin(), bars.end(), time, [](const FlowBar& b, std::int64_t t) { return b.t < t; }) -
        bars.begin());
}

}  // namespace ta
