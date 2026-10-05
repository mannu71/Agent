// ta_ingest: turns a folder of NSE bhavcopy CSVs into per-symbol daily files.

#include <fstream>
#include <iostream>
#include <sstream>

#include "args.hpp"
#include "ta/data.hpp"

int main(int argc, char** argv) {
    try {
        const cli::Args a(argc, argv, 1, {});
        if (a.positional().size() < 3 || a.pos(0) != "bhavcopy") {
            std::cerr << "usage: ta_ingest bhavcopy <bhavcopy_dir> <out_dir> [--actions F] [--series EQ,BE,BZ]\n"
                         "                 [--non-eq-out F]\n"
                         "Reads legacy, UDiFF and sec_bhavdata_full CSVs (unzipped). --actions applies\n"
                         "split/bonus factors (symbol,ex_date,factor). --non-eq-out writes symbol,date rows\n"
                         "for trade-for-trade days, usable as a dated exclusion list.\n";
            return 2;
        }
        std::set<std::string> series;
        std::stringstream ss(a.str("series", "EQ,BE,BZ"));
        std::string s;
        while (std::getline(ss, s, ',')) series.insert(s);

        auto r = ta::ingest_bhavcopy_dir(a.pos(1), series);
        if (a.has("actions")) ta::apply_corporate_actions(r.universe, ta::load_corporate_actions(a.str("actions")));
        ta::write_universe(a.pos(2), r.universe);
        if (a.has("non-eq-out")) {
            std::ofstream out(a.str("non-eq-out"));
            out << "# symbol,date of non-EQ (trade-for-trade) sessions\n";
            for (const auto& [sym, date] : r.non_eq_days) out << sym << ',' << date << '\n';
        }
        std::cout << "files " << r.files << "  rows " << r.rows << "  symbols " << r.universe.size()
                  << "  non-EQ days " << r.non_eq_days.size() << "\nwritten to " << a.pos(2) << '\n';
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << '\n';
        return 1;
    }
}
