#include "ta/swing.hpp"

#include <algorithm>
#include <cmath>

#include "ta/indicators.hpp"

namespace ta {

namespace {
void emit(std::vector<KvRecord>* events, KvRecord r) {
    if (events) events->push_back(std::move(r));
}
}  // namespace

KvRecord SwingPosition::to_kv() const {
    KvRecord r = trade.to_kv();
    r.type = "pos";
    r.set("held", qty).set("stop", stop).set("adr", adr_at_entry).set("last_close", last_close);
    r.set("days_held", static_cast<long>(days_held)).set("partial_done", partial_done);
    r.set("exit_next_open", exit_next_open);
    return r;
}

SwingPosition SwingPosition::from_kv(const KvRecord& r) {
    SwingPosition p;
    p.trade = Trade::from_kv(r);
    p.qty = r.integer("held");
    p.stop = r.num("stop");
    p.adr_at_entry = r.num("adr");
    p.last_close = r.num("last_close");
    p.days_held = static_cast<int>(r.integer("days_held"));
    p.partial_done = r.integer("partial_done") != 0;
    p.exit_next_open = r.integer("exit_next_open") != 0;
    return p;
}

SwingBook::SwingBook(std::string sleeve, CostModel cost, ExitRules exits, double cash)
    : sleeve_(std::move(sleeve)), cost_(cost), exits_(exits), cash_(cash) {}

SwingPosition& SwingBook::open(const std::string& sym, const std::string& date, double fill, double stop,
                               long qty, double adr, double close, std::vector<KvRecord>* events) {
    SwingPosition pos;
    pos.trade.sleeve = sleeve_;
    pos.trade.symbol = sym;
    pos.trade.entry_date = date;
    pos.trade.entry_price = fill;
    pos.trade.qty = qty;
    pos.trade.risk_per_share = fill - stop;
    pos.qty = qty;
    pos.stop = stop;
    pos.adr_at_entry = adr;
    pos.last_close = close;

    const double notional = static_cast<double>(qty) * fill;
    const double cost = notional * cost_.buy_frac;
    cash_ -= notional + cost;
    pos.trade.pnl = -cost;

    KvRecord e;
    e.type = "fill";
    e.set("sleeve", sleeve_).set("date", date).set("symbol", sym).set("side", std::string("buy"));
    e.set("qty", qty).set("price", fill).set("stop", stop);
    emit(events, e);
    return positions_[sym] = pos;
}

void SwingBook::sell(const std::string& sym, long qty, double px, const std::string& date,
                     const std::string& reason, std::vector<KvRecord>* events) {
    auto it = positions_.find(sym);
    if (it == positions_.end()) return;
    SwingPosition& pos = it->second;
    qty = std::min(qty, pos.qty);
    if (qty <= 0) return;
    const double notional = static_cast<double>(qty) * px;
    const double cost = notional * cost_.sell_frac;
    cash_ += notional - cost;
    pos.trade.pnl += static_cast<double>(qty) * (px - pos.trade.entry_price) - cost;
    pos.qty -= qty;

    KvRecord e;
    e.type = "fill";
    e.set("sleeve", sleeve_).set("date", date).set("symbol", sym).set("side", std::string("sell"));
    e.set("qty", qty).set("price", px).set("reason", reason);
    emit(events, e);

    if (pos.qty == 0) {
        pos.trade.exit_date = date;
        pos.trade.exit_reason = reason;
        const double risk = static_cast<double>(pos.trade.qty) * pos.trade.risk_per_share;
        pos.trade.r_multiple = risk > 0 ? pos.trade.pnl / risk : 0;
        closed_.push_back(pos.trade);
        emit(events, pos.trade.to_kv());
        positions_.erase(it);
    }
}

void SwingBook::exits_at_open(const MarketData& md, const std::string& date, std::vector<KvRecord>* events) {
    std::vector<std::string> syms;
    for (const auto& [sym, pos] : positions_) syms.push_back(sym);
    for (const auto& sym : syms) {
        const SwingPosition& pos = positions_.at(sym);
        const Bar* b = md.bar(sym, date);
        if (!b) continue;
        if (pos.exit_next_open) sell(sym, pos.qty, b->open, date, "trail", events);
        else if (b->open <= pos.stop) sell(sym, pos.qty, b->open, date, "stop_gap", events);
        else if (b->low <= pos.stop) sell(sym, pos.qty, pos.stop, date, "stop", events);
    }
}

void SwingBook::manage_at_close(const MarketData& md, const std::string& date, std::vector<KvRecord>* events) {
    std::vector<std::string> syms;
    for (const auto& [sym, pos] : positions_) syms.push_back(sym);
    for (const auto& sym : syms) {
        std::size_t i = 0;
        const Bar* b = md.bar(sym, date, &i);
        if (!b) continue;
        SwingPosition& pos = positions_.at(sym);
        if (pos.trade.entry_date != date) ++pos.days_held;
        pos.last_close = b->close;
        const double entry = pos.trade.entry_price;

        const bool partial_on = exits_.partial_frac > 0 && exits_.partial_day > 0;
        if (partial_on && !pos.partial_done && pos.days_held == exits_.partial_day && b->close > entry) {
            const auto part = static_cast<long>(std::floor(static_cast<double>(pos.trade.qty) * exits_.partial_frac));
            pos.partial_done = true;
            pos.stop = std::max(pos.stop, entry);
            if (part > 0 && part < pos.qty) sell(sym, part, b->close, date, "partial", events);
        }
        if (!holds(sym)) continue;
        SwingPosition& p = positions_.at(sym);

        const double one_r = entry + exits_.time_stop_min_r * p.trade.risk_per_share;
        const std::size_t trail_n = p.adr_at_entry >= exits_.fast_trail_min_adr ? exits_.trail_fast : exits_.trail_slow;
        const double trail = sma_close(md.series(sym), i, trail_n);
        if (p.days_held >= exits_.max_hold_days) {
            sell(sym, p.qty, b->close, date, "max_hold", events);
        } else if (exits_.time_stop_day > 0 && p.days_held == exits_.time_stop_day && b->close < one_r) {
            sell(sym, p.qty, b->close, date, "time_stop", events);
        } else if (!std::isnan(trail) && b->close < trail) {
            p.exit_next_open = true;
        }
    }
}

double SwingBook::market_value(const MarketData& md, const std::string& date, bool at_open) const {
    double v = 0;
    for (const auto& [sym, pos] : positions_) {
        const Bar* b = md.bar(sym, date);
        const double px = b ? (at_open ? b->open : b->close) : pos.last_close;
        v += static_cast<double>(pos.qty) * px;
    }
    return v;
}

double SwingBook::open_risk() const {
    double r = 0;
    for (const auto& [sym, pos] : positions_) {
        r += static_cast<double>(pos.qty) * std::max(0.0, pos.trade.entry_price - pos.stop);
    }
    return r;
}

void SwingBook::flatten(const MarketData& md, const std::string& date, const std::string& reason,
                        std::vector<KvRecord>* events) {
    std::vector<std::string> syms;
    for (const auto& [sym, pos] : positions_) syms.push_back(sym);
    for (const auto& sym : syms) {
        const Bar* b = md.bar(sym, date);
        const SwingPosition& pos = positions_.at(sym);
        sell(sym, pos.qty, b ? b->close : pos.last_close, date, reason, events);
    }
}

std::vector<Trade> SwingBook::drain_closed() {
    std::vector<Trade> out;
    out.swap(closed_);
    return out;
}

void SwingBook::save(std::vector<KvRecord>& out) const {
    KvRecord b;
    b.type = "book";
    b.set("sleeve", sleeve_).set("cash", cash_);
    out.push_back(b);
    for (const auto& [sym, pos] : positions_) out.push_back(pos.to_kv());
}

void SwingBook::load(const std::vector<KvRecord>& records) {
    positions_.clear();
    for (const auto& r : records) {
        if (r.type == "book") cash_ = r.num("cash");
        else if (r.type == "pos") positions_[r.str("symbol")] = SwingPosition::from_kv(r);
    }
}

}  // namespace ta
