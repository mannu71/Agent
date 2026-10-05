#include "ta/regime.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include "ta/indicators.hpp"

namespace ta {

const char* to_string(Gate g) {
    switch (g) {
        case Gate::Green: return "green";
        case Gate::Red: return "red";
        case Gate::Unknown: return "unknown";
    }
    return "?";
}

long days_from_iso(const std::string& iso) {
    if (iso.size() < 10) throw std::runtime_error("bad ISO date: " + iso);
    long y = std::stol(iso.substr(0, 4));
    const long m = std::stol(iso.substr(5, 2));
    const long d = std::stol(iso.substr(8, 2));
    // Howard Hinnant's days_from_civil.
    y -= m <= 2;
    const long era = (y >= 0 ? y : y - 399) / 400;
    const long yoe = y - era * 400;
    const long doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
    const long doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return era * 146097 + doe - 719468;
}

double percentile_rank(const std::vector<double>& history, double x) {
    if (history.empty()) return 0;
    const auto n = std::count_if(history.begin(), history.end(), [x](double v) { return v <= x; });
    return static_cast<double>(n) / static_cast<double>(history.size());
}

Gate trend_gate(const Series& index, std::size_t i, std::size_t sma_n) {
    const double sma = sma_close(index, i, sma_n);
    if (std::isnan(sma)) return Gate::Unknown;
    return index[i].close > sma ? Gate::Green : Gate::Red;
}

Gate realized_vol_gate(const Series& s, std::size_t i, const RegimeConfig& cfg) {
    if (i < cfg.realized_vol_n + cfg.vol_lookback) return Gate::Unknown;
    std::vector<double> hist;
    for (std::size_t k = i - cfg.vol_lookback; k < i; ++k) hist.push_back(ann_vol(s, k, cfg.realized_vol_n));
    const double now = ann_vol(s, i, cfg.realized_vol_n);
    return percentile_rank(hist, now) >= cfg.vol_red_pct ? Gate::Red : Gate::Green;
}

Gate vix_gate(const std::map<std::string, double>& vix, const std::string& date, const RegimeConfig& cfg) {
    auto it = vix.upper_bound(date);  // first value dated after `date`
    if (it == vix.begin()) return Gate::Unknown;
    --it;
    const double now = it->second;
    std::vector<double> hist;
    for (auto h = it; h != vix.begin() && hist.size() < cfg.vol_lookback;) {
        --h;
        hist.push_back(h->second);
    }
    if (hist.size() < cfg.vol_lookback / 2) return Gate::Unknown;
    return percentile_rank(hist, now) >= cfg.vol_red_pct ? Gate::Red : Gate::Green;
}

Gate crash_gate(const Series& index, std::size_t i, const RegimeConfig& cfg) {
    // A sharp fall followed by a sharp rebound: the state in which momentum crashes.
    if (i < 63) return Gate::Unknown;
    const double fall = std::min(ret(index, i, 63), ret(index, i - 21, 42));
    const double rebound = ret(index, i, 21);
    return fall <= cfg.crash_fall && rebound >= cfg.crash_rebound ? Gate::Red : Gate::Green;
}

Gate crowding_gate(double funding_annualized, double oi_now, double oi_prev, const RegimeConfig& cfg) {
    if (std::isnan(funding_annualized)) return Gate::Unknown;
    if (funding_annualized <= cfg.funding_red_annual) return Gate::Green;
    if (std::isnan(oi_now) || std::isnan(oi_prev)) return Gate::Red;
    return oi_now > oi_prev ? Gate::Red : Gate::Green;
}

EventCalendar EventCalendar::load(const std::string& path) {
    std::ifstream in(path);
    if (!in) throw std::runtime_error("cannot open " + path);
    EventCalendar c;
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty() || line[0] == '#' || line.rfind("date", 0) == 0) continue;
        std::stringstream ss(line);
        std::string date, tag;
        std::getline(ss, date, ',');
        std::getline(ss, tag);
        c.add(date.substr(0, 10), tag);
    }
    return c;
}

bool EventCalendar::blackout(const std::string& date, const RegimeConfig& cfg,
                             const std::vector<std::string>* sessions) const {
    auto window = [&](const std::set<std::string>& tags, bool before) {
        int w = before ? cfg.event_days_before : cfg.event_days_after;
        bool matched = false;
        for (std::string tag : tags) {
            for (auto& c : tag) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            const auto& by_tag = before ? cfg.event_before_by_tag : cfg.event_after_by_tag;
            for (const auto& [key, days] : by_tag) {
                if (tag.find(key) == std::string::npos) continue;
                w = matched ? std::max(w, days) : days;  // widest matching window wins
                matched = true;
            }
        }
        return w;
    };
    const bool by_session = cfg.event_trading_days && sessions && !sessions->empty();
    const long d = by_session ? std::lower_bound(sessions->begin(), sessions->end(), date) - sessions->begin()
                              : days_from_iso(date);
    for (const auto& [ev, tags] : events_) {
        const long e = by_session ? std::lower_bound(sessions->begin(), sessions->end(), ev) - sessions->begin()
                                  : days_from_iso(ev);
        if (d >= e - window(tags, true) && d <= e + window(tags, false)) return true;
    }
    return false;
}

EquityRegime evaluate_equity_regime(const EquityRegimeInputs& in, const std::string& date,
                                    const RegimeConfig& cfg) {
    EquityRegime r;
    if (in.index) {
        // Last index bar on or before `date`.
        const auto it = std::upper_bound(in.index->begin(), in.index->end(), date,
                                         [](const std::string& d, const Bar& b) { return d < b.date; });
        if (it != in.index->begin()) {
            const auto i = static_cast<std::size_t>(std::distance(in.index->begin(), it) - 1);
            r.trend = trend_gate(*in.index, i, cfg.trend_sma);
            r.crash = crash_gate(*in.index, i, cfg);
            if (!in.vix) r.vol = realized_vol_gate(*in.index, i, cfg);
        }
    }
    if (in.vix) r.vol = vix_gate(*in.vix, date, cfg);
    if (in.events) r.event_blackout = in.events->blackout(date, cfg, in.sessions);
    return r;
}

}  // namespace ta
