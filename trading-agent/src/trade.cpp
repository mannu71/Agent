#include "ta/trade.hpp"

#include <algorithm>
#include <cmath>
#include <sstream>

namespace ta {

KvRecord Trade::to_kv() const {
    KvRecord r;
    r.type = "trade";
    r.set("sleeve", sleeve).set("symbol", symbol).set("entry_date", entry_date).set("exit_date", exit_date);
    r.set("side", static_cast<long>(side)).set("entry_price", entry_price).set("qty", qty);
    r.set("risk_per_share", risk_per_share).set("pnl", pnl).set("r", r_multiple).set("exit_reason", exit_reason);
    return r;
}

Trade Trade::from_kv(const KvRecord& r) {
    Trade t;
    t.sleeve = r.str("sleeve");
    t.symbol = r.str("symbol");
    t.entry_date = r.str("entry_date");
    t.exit_date = r.str("exit_date");
    t.side = static_cast<int>(r.integer("side", 1));
    t.entry_price = r.num("entry_price");
    t.qty = r.integer("qty");
    t.risk_per_share = r.num("risk_per_share");
    t.pnl = r.num("pnl");
    t.r_multiple = r.num("r");
    t.exit_reason = r.str("exit_reason");
    return t;
}

std::string trades_csv_header() {
    return "sleeve,symbol,side,entry_date,exit_date,entry_price,qty,risk_per_share,pnl,r_multiple,exit_reason";
}

std::string to_csv(const Trade& t) {
    std::ostringstream o;
    o.precision(10);
    o << t.sleeve << ',' << t.symbol << ',' << t.side << ',' << t.entry_date << ',' << t.exit_date << ','
      << t.entry_price << ',' << t.qty << ',' << t.risk_per_share << ',' << t.pnl << ',' << t.r_multiple << ','
      << t.exit_reason;
    return o.str();
}

Metrics compute_metrics(const std::vector<Trade>& trades, const EquityCurve& equity_curve,
                        double periods_per_year) {
    Metrics m;
    m.trades = static_cast<int>(trades.size());
    double wins = 0, gross_win = 0, gross_loss = 0, sum_r = 0;
    for (const Trade& t : trades) {
        if (t.pnl > 0) {
            ++wins;
            gross_win += t.pnl;
        } else {
            gross_loss -= t.pnl;
        }
        sum_r += t.r_multiple;
    }
    if (m.trades > 0) {
        m.win_rate = wins / m.trades;
        m.avg_r = sum_r / m.trades;
    }
    m.profit_factor = gross_loss > 0 ? gross_win / gross_loss : 0;

    if (equity_curve.size() >= 2) {
        const double first = equity_curve.front().second;
        const double last = equity_curve.back().second;
        m.total_return = last / first - 1.0;
        const double years = static_cast<double>(equity_curve.size() - 1) / periods_per_year;
        m.cagr = last > 0 ? std::pow(last / first, 1.0 / years) - 1.0 : -1.0;

        double peak = first, sum = 0, sum_sq = 0;
        for (std::size_t k = 1; k < equity_curve.size(); ++k) {
            const double e = equity_curve[k].second;
            peak = std::max(peak, e);
            m.max_drawdown = std::max(m.max_drawdown, 1.0 - e / peak);
            const double r = e / equity_curve[k - 1].second - 1.0;
            sum += r;
            sum_sq += r * r;
        }
        const double n = static_cast<double>(equity_curve.size() - 1);
        if (n >= 2) {
            const double mean = sum / n;
            const double sd = std::sqrt(std::max(0.0, (sum_sq - sum * sum / n) / (n - 1.0)));
            m.sharpe = sd > 0 ? mean / sd * std::sqrt(periods_per_year) : 0;
        }
    }
    return m;
}

}  // namespace ta
