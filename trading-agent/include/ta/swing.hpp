#pragma once

#include <map>
#include <string>
#include <vector>

#include "ta/kv.hpp"
#include "ta/market.hpp"
#include "ta/trade.hpp"

namespace ta {

// Exit rules shared by the long-only swing sleeves (A1, A2). Day 0 is the entry day;
// "day 3" is the third close after it.
struct ExitRules {
    int partial_day = 3;             // sell partial at this close if in profit, stop -> entry
    double partial_frac = 1.0 / 3.0;
    int time_stop_day = 20;          // exit at this close if below +time_stop_min_r; 0 disables
    double time_stop_min_r = 1.0;
    int max_hold_days = 120;         // forced exit at this close
    std::size_t trail_fast = 10;     // trail on SMA(trail_fast) when ADR >= fast_trail_min_adr
    std::size_t trail_slow = 20;     // ... else on SMA(trail_slow)
    double fast_trail_min_adr = 0.05;
};

struct SwingPosition {
    Trade trade;          // filled in as the position lives
    long qty = 0;         // units still held
    double stop = 0;
    double adr_at_entry = 0;
    double last_close = 0;
    int days_held = 0;
    bool partial_done = false;
    bool exit_next_open = false;

    KvRecord to_kv() const;
    static SwingPosition from_kv(const KvRecord& r);
};

// Cash plus long positions, with the shared exit logic. Every fill pays CostModel costs.
class SwingBook {
public:
    SwingBook(std::string sleeve, CostModel cost, ExitRules exits, double cash);

    double cash() const { return cash_; }
    const std::map<std::string, SwingPosition>& positions() const { return positions_; }
    bool holds(const std::string& sym) const { return positions_.count(sym) != 0; }
    int count() const { return static_cast<int>(positions_.size()); }

    // Opens a position and charges the buy cost.
    SwingPosition& open(const std::string& sym, const std::string& date, double fill, double stop, long qty,
                        double adr, double close, std::vector<KvRecord>* events);
    // Sells `qty` (all when qty >= held); removes the position when flat.
    void sell(const std::string& sym, long qty, double px, const std::string& date, const std::string& reason,
              std::vector<KvRecord>* events);

    // Next-open trail exits and stop hits (gap through stop fills at the open).
    void exits_at_open(const MarketData& md, const std::string& date, std::vector<KvRecord>* events);
    // Partial, max-hold, time-stop and trailing-exit checks at the close.
    void manage_at_close(const MarketData& md, const std::string& date, std::vector<KvRecord>* events);

    double market_value(const MarketData& md, const std::string& date, bool at_open) const;
    // Rupees lost if every stop below entry were hit: sum of qty * max(0, entry - stop).
    double open_risk() const;
    // Kill switch: sell everything at `date`'s close (or the last known close).
    void flatten(const MarketData& md, const std::string& date, const std::string& reason,
                 std::vector<KvRecord>* events);

    std::vector<Trade> drain_closed();
    const std::vector<Trade>& closed() const { return closed_; }

    void save(std::vector<KvRecord>& out) const;
    void load(const std::vector<KvRecord>& records);  // reads "book" and "pos" records

private:
    std::string sleeve_;
    CostModel cost_;
    ExitRules exits_;
    double cash_;
    std::map<std::string, SwingPosition> positions_;
    std::vector<Trade> closed_;
};

}  // namespace ta
