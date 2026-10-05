#include "ta/market.hpp"

#include <set>

namespace ta {

MarketData::MarketData(Universe u) : u_(std::move(u)) {
    std::set<std::string> dates;
    for (const auto& [sym, s] : u_) {
        auto& idx = index_[sym];
        for (std::size_t k = 0; k < s.size(); ++k) {
            idx[s[k].date] = k;
            dates.insert(s[k].date);
        }
    }
    dates_.assign(dates.begin(), dates.end());
}

const Bar* MarketData::bar(const std::string& sym, const std::string& date, std::size_t* idx) const {
    const auto s = index_.find(sym);
    if (s == index_.end()) return nullptr;
    const auto it = s->second.find(date);
    if (it == s->second.end()) return nullptr;
    if (idx) *idx = it->second;
    return &u_.at(sym)[it->second];
}

}  // namespace ta
