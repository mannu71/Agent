#include "ta/indicators.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

namespace ta {

namespace {
constexpr double kNaN = std::numeric_limits<double>::quiet_NaN();

bool has_window(const Series& s, std::size_t i, std::size_t n) {
    return n > 0 && i < s.size() && i + 1 >= n;
}
}  // namespace

double sma_close(const Series& s, std::size_t i, std::size_t n) {
    if (!has_window(s, i, n)) return kNaN;
    double sum = 0;
    for (std::size_t k = i + 1 - n; k <= i; ++k) sum += s[k].close;
    return sum / static_cast<double>(n);
}

double adr_frac(const Series& s, std::size_t i, std::size_t n) {
    if (!has_window(s, i, n)) return kNaN;
    double sum = 0;
    for (std::size_t k = i + 1 - n; k <= i; ++k) sum += s[k].high / s[k].low - 1.0;
    return sum / static_cast<double>(n);
}

double avg_traded_value(const Series& s, std::size_t i, std::size_t n) {
    if (!has_window(s, i, n)) return kNaN;
    double sum = 0;
    for (std::size_t k = i + 1 - n; k <= i; ++k) sum += s[k].close * s[k].volume;
    return sum / static_cast<double>(n);
}

double ret(const Series& s, std::size_t i, std::size_t n) {
    if (i >= s.size() || i < n) return kNaN;
    return s[i].close / s[i - n].close - 1.0;
}

double ann_vol(const Series& s, std::size_t i, std::size_t n) {
    if (i >= s.size() || i < n || n < 2) return kNaN;
    double sum = 0, sum_sq = 0;
    for (std::size_t k = i + 1 - n; k <= i; ++k) {
        const double r = std::log(s[k].close / s[k - 1].close);
        sum += r;
        sum_sq += r * r;
    }
    const double nn = static_cast<double>(n);
    const double var = (sum_sq - sum * sum / nn) / (nn - 1.0);
    return std::sqrt(std::max(var, 0.0)) * std::sqrt(252.0);
}

double atr_frac(const Series& s, std::size_t i, std::size_t n) {
    if (i >= s.size() || i < n || n == 0) return kNaN;
    double sum = 0;
    for (std::size_t k = i + 1 - n; k <= i; ++k) {
        const double pc = s[k - 1].close;
        const double tr = std::max({s[k].high - s[k].low, std::fabs(s[k].high - pc), std::fabs(s[k].low - pc)});
        sum += tr / s[k].close;
    }
    return sum / static_cast<double>(n);
}

double sma_volume(const Series& s, std::size_t i, std::size_t n) {
    if (!has_window(s, i, n)) return kNaN;
    double sum = 0;
    for (std::size_t k = i + 1 - n; k <= i; ++k) sum += s[k].volume;
    return sum / static_cast<double>(n);
}

double highest_high(const Series& s, std::size_t i, std::size_t n) {
    if (!has_window(s, i, n)) return kNaN;
    double h = s[i + 1 - n].high;
    for (std::size_t k = i + 1 - n; k <= i; ++k) h = std::max(h, s[k].high);
    return h;
}

double lowest_low(const Series& s, std::size_t i, std::size_t n) {
    if (!has_window(s, i, n)) return kNaN;
    double l = s[i + 1 - n].low;
    for (std::size_t k = i + 1 - n; k <= i; ++k) l = std::min(l, s[k].low);
    return l;
}

}  // namespace ta
