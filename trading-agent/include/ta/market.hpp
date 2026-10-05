#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "ta/bar.hpp"

namespace ta {

// Read-only view over a universe with O(1) (symbol, date) -> bar lookup.
class MarketData {
public:
    explicit MarketData(Universe u);

    const Universe& universe() const { return u_; }
    const Series& series(const std::string& sym) const { return u_.at(sym); }
    bool has(const std::string& sym) const { return u_.count(sym) != 0; }

    // Bar for `sym` on `date`, or nullptr if it did not trade. `idx` receives its index.
    const Bar* bar(const std::string& sym, const std::string& date, std::size_t* idx = nullptr) const;

    // Sorted union of all dates in the universe.
    const std::vector<std::string>& dates() const { return dates_; }

private:
    Universe u_;
    std::unordered_map<std::string, std::unordered_map<std::string, std::size_t>> index_;
    std::vector<std::string> dates_;
};

}  // namespace ta
