#pragma once

#include <map>
#include <string>
#include <vector>

namespace ta {

// One daily OHLCV bar. Dates are ISO "YYYY-MM-DD" so they sort as strings.
struct Bar {
    std::string date;
    double open = 0;
    double high = 0;
    double low = 0;
    double close = 0;
    double volume = 0;
};

using Series = std::vector<Bar>;

// symbol -> daily bars sorted by date
using Universe = std::map<std::string, Series>;

}  // namespace ta
