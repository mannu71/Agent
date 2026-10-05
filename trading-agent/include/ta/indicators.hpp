#pragma once

#include <cstddef>

#include "ta/bar.hpp"

namespace ta {

// Every function looks only at bars [0, i] -- never ahead -- and returns NaN
// when there is not enough history.

double sma_close(const Series& s, std::size_t i, std::size_t n);

// Average daily range as a fraction: mean(high / low - 1) over the last n bars.
double adr_frac(const Series& s, std::size_t i, std::size_t n);

// Mean of close * volume over the last n bars (rupees traded per day).
double avg_traded_value(const Series& s, std::size_t i, std::size_t n);

// close[i] / close[i - n] - 1
double ret(const Series& s, std::size_t i, std::size_t n);

// Standard deviation of daily log returns over the last n returns, annualised with sqrt(252).
double ann_vol(const Series& s, std::size_t i, std::size_t n);

// Highest high / lowest low over the last n bars, including bar i.
double highest_high(const Series& s, std::size_t i, std::size_t n);
double lowest_low(const Series& s, std::size_t i, std::size_t n);

}  // namespace ta
