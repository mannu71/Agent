// ta_backtest: runs the Sleeve A1 momentum-breakout backtest on a folder of daily CSVs.

#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <set>
#include <string>

#include "ta/backtest.hpp"
#include "ta/csv.hpp"

namespace {

void usage() {
    std::cerr << "usage: ta_backtest <data_dir> [--equity N] [--start YYYY-MM-DD] [--end YYYY-MM-DD]\n"
                 "                   [--cost-rt FRAC] [--exclude FILE] [--trades OUT.csv]\n"
                 "                   [--equity-out OUT.csv]\n"
                 "data_dir holds one <SYMBOL>.csv per stock: date,open,high,low,close,volume\n";
}

}  // namespace

int main(int argc, char** argv) {
    if (argc < 2) {
        usage();
        return 2;
    }
    ta::BacktestConfig cfg;
    std::string data_dir = argv[1], exclude_file, trades_out, equity_out;
    for (int k = 2; k < argc; ++k) {
        const std::string a = argv[k];
        if (k + 1 >= argc) {
            usage();
            return 2;
        }
        const std::string v = argv[++k];
        if (a == "--equity") cfg.initial_equity = std::stod(v);
        else if (a == "--start") cfg.start_date = v;
        else if (a == "--end") cfg.end_date = v;
        else if (a == "--cost-rt") cfg.cost.buy_frac = cfg.cost.sell_frac = std::stod(v) / 2.0;
        else if (a == "--exclude") exclude_file = v;
        else if (a == "--trades") trades_out = v;
        else if (a == "--equity-out") equity_out = v;
        else {
            usage();
            return 2;
        }
    }

    try {
        const ta::Universe universe = ta::load_universe(data_dir);
        const std::set<std::string> excluded =
            exclude_file.empty() ? std::set<std::string>{} : ta::load_symbol_list(exclude_file);
        const ta::BacktestResult r = ta::run_backtest(universe, cfg, excluded);

        if (!trades_out.empty()) {
            std::ofstream out(trades_out);
            out << "symbol,entry_date,exit_date,entry_price,qty,risk_per_share,pnl,r_multiple,exit_reason\n";
            for (const auto& t : r.trades) {
                out << t.symbol << ',' << t.entry_date << ',' << t.exit_date << ',' << t.entry_price << ','
                    << t.qty << ',' << t.risk_per_share << ',' << t.pnl << ',' << t.r_multiple << ','
                    << t.exit_reason << '\n';
            }
        }
        if (!equity_out.empty()) {
            std::ofstream out(equity_out);
            out << "date,equity\n";
            for (const auto& [d, e] : r.equity_curve) out << d << ',' << std::fixed << e << '\n';
        }

        const auto& m = r.metrics;
        std::cout << std::fixed << std::setprecision(3)
                  << "symbols        " << universe.size() << '\n'
                  << "trades         " << m.trades << '\n'
                  << "win rate       " << m.win_rate << '\n'
                  << "avg R          " << m.avg_r << '\n'
                  << "profit factor  " << m.profit_factor << '\n'
                  << "total return   " << m.total_return << '\n'
                  << "CAGR           " << m.cagr << '\n'
                  << "max drawdown   " << m.max_drawdown << '\n'
                  << "Sharpe         " << m.sharpe << '\n'
                  << "risk state     " << ta::to_string(r.final_risk_state) << '\n'
                  << "(pre-tax; costs at " << cfg.cost.round_trip() * 100 << "% round trip)\n";
    } catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
