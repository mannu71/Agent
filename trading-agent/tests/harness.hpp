// Minimal self-contained test harness: no external dependencies.
#pragma once

#include <cmath>
#include <cstdio>
#include <filesystem>
#include <functional>
#include <iostream>
#include <random>
#include <string>
#include <utility>
#include <vector>

#include "ta/bar.hpp"

namespace th {

inline int& failures() {
    static int f = 0;
    return f;
}

inline std::vector<std::pair<std::string, std::function<void()>>>& registry() {
    static std::vector<std::pair<std::string, std::function<void()>>> r;
    return r;
}

struct Register {
    Register(const char* name, std::function<void()> f) { registry().emplace_back(name, std::move(f)); }
};

// Fresh scratch directory per test, under the build tree.
inline std::string temp_dir(const std::string& name) {
    const auto p = std::filesystem::temp_directory_path() / ("ta_test_" + name);
    std::filesystem::remove_all(p);
    std::filesystem::create_directories(p);
    return p.string();
}

inline std::string date_of(int k) {
    char buf[16];
    std::snprintf(buf, sizeof buf, "D%05d", k);
    return buf;
}

// Calendar dates for tests that need real ISO dates (weekdays not modelled).
inline std::string iso_day(int k) {
    int y = 2015, m = 1, d = 1;
    static const int mdays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    for (int i = 0; i < k; ++i) {
        const bool leap = (y % 4 == 0 && y % 100 != 0) || y % 400 == 0;
        const int len = mdays[m - 1] + (m == 2 && leap ? 1 : 0);
        if (++d > len) {
            d = 1;
            if (++m > 12) {
                m = 1;
                ++y;
            }
        }
    }
    char buf[48];
    std::snprintf(buf, sizeof buf, "%04d-%02d-%02d", y, m, d);
    return buf;
}

// Portable Gaussian random walk (Box-Muller over mt19937), deterministic per seed.
inline ta::Series random_walk(unsigned seed, int n, double start, double vol, double volume,
                              bool iso_dates = false) {
    std::mt19937 rng(seed);
    auto u01 = [&] { return (static_cast<double>(rng()) + 0.5) / 4294967296.0; };
    auto gauss = [&] { return std::sqrt(-2.0 * std::log(u01())) * std::cos(6.283185307179586 * u01()); };
    ta::Series s;
    double p = start;
    for (int k = 0; k < n; ++k) {
        const double o = p * std::exp(gauss() * vol / 3);
        const double c = o * std::exp(gauss() * vol);
        const double h = std::max(o, c) * std::exp(std::fabs(gauss()) * vol / 2);
        const double l = std::min(o, c) * std::exp(-std::fabs(gauss()) * vol / 2);
        s.push_back({iso_dates ? iso_day(k) : date_of(k), o, h, l, c, volume});
        p = c;
    }
    return s;
}

}  // namespace th

#define TEST(name)                                       \
    static void name();                                  \
    static const th::Register reg_##name(#name, name);   \
    static void name()

#define CHECK(cond)                                                                    \
    do {                                                                               \
        if (!(cond)) {                                                                 \
            ++th::failures();                                                          \
            std::cerr << __FILE__ << ":" << __LINE__ << ": CHECK(" #cond ") failed\n"; \
        }                                                                              \
    } while (0)

#define CHECK_NEAR(a, b, tol)                                                                    \
    do {                                                                                         \
        const double va = (a), vb = (b);                                                         \
        if (!(std::fabs(va - vb) <= (tol))) {                                                    \
            ++th::failures();                                                                    \
            std::cerr << __FILE__ << ":" << __LINE__ << ": " #a " = " << va << ", expected " << vb \
                      << "\n";                                                                   \
        }                                                                                        \
    } while (0)
