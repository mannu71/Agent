#include "ta/data.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace ta {

namespace {

std::string trim(const std::string& s) {
    const auto b = s.find_first_not_of(" \t\r\n\"");
    if (b == std::string::npos) return "";
    return s.substr(b, s.find_last_not_of(" \t\r\n\"") - b + 1);
}

std::string upper(std::string s) {
    for (auto& c : s) c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    return s;
}

std::vector<std::string> split_csv(const std::string& line) {
    std::vector<std::string> out;
    std::stringstream ss(line);
    std::string f;
    while (std::getline(ss, f, ',')) out.push_back(trim(f));
    if (!line.empty() && line.back() == ',') out.emplace_back();
    return out;
}

// "01-JAN-2020" / "01-Jan-2020" / "2020-01-01" / "20200101" -> "2020-01-01"
std::string normalize_date(const std::string& raw) {
    const std::string d = trim(raw);
    if (d.size() == 10 && d[4] == '-' && d[7] == '-') return d;
    if (d.size() == 8 && std::all_of(d.begin(), d.end(), ::isdigit)) {
        return d.substr(0, 4) + "-" + d.substr(4, 2) + "-" + d.substr(6, 2);
    }
    if (d.size() == 11 && d[2] == '-' && d[6] == '-') {
        static const char* months[] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN",
                                       "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};
        const std::string mon = upper(d.substr(3, 3));
        for (int m = 0; m < 12; ++m) {
            if (mon == months[m]) {
                char buf[11];
                std::snprintf(buf, sizeof buf, "%s-%02d-%s", d.substr(7, 4).c_str(), m + 1,
                              d.substr(0, 2).c_str());
                return buf;
            }
        }
    }
    throw std::runtime_error("unrecognised date: " + raw);
}

struct Columns {
    int symbol = -1, series = -1, date = -1, open = -1, high = -1, low = -1, close = -1, volume = -1;
    int prev = -1;  // optional
    bool complete() const {
        return symbol >= 0 && series >= 0 && date >= 0 && open >= 0 && high >= 0 && low >= 0 &&
               close >= 0 && volume >= 0;
    }
};

Columns detect_columns(const std::vector<std::string>& header) {
    Columns c;
    for (int k = 0; k < static_cast<int>(header.size()); ++k) {
        const std::string h = upper(header[k]);
        if (h == "SYMBOL" || h == "TCKRSYMB") c.symbol = k;
        else if (h == "SERIES" || h == "SCTYSRS") c.series = k;
        else if (h == "TIMESTAMP" || h == "TRADDT" || h == "DATE1") c.date = k;
        else if (h == "OPEN" || h == "OPNPRIC" || h == "OPEN_PRICE") c.open = k;
        else if (h == "HIGH" || h == "HGHPRIC" || h == "HIGH_PRICE") c.high = k;
        else if (h == "LOW" || h == "LWPRIC" || h == "LOW_PRICE") c.low = k;
        else if (h == "CLOSE" || h == "CLSPRIC" || h == "CLOSE_PRICE") c.close = k;
        else if (h == "TOTTRDQTY" || h == "TTLTRADGVOL" || h == "TTL_TRD_QNTY") c.volume = k;
        else if (h == "PREVCLOSE" || h == "PRVSCLSGPRIC" || h == "PREV_CLOSE") c.prev = k;
    }
    return c;
}

}  // namespace

std::vector<BhavRow> parse_bhavcopy(const std::string& path) {
    std::ifstream in(path);
    if (!in) throw std::runtime_error("cannot open " + path);
    std::string line;
    if (!std::getline(in, line)) return {};
    const Columns c = detect_columns(split_csv(line));
    if (!c.complete()) throw std::runtime_error(path + ": not a recognised bhavcopy header");

    std::vector<BhavRow> rows;
    std::size_t n = 1;
    while (std::getline(in, line)) {
        ++n;
        if (trim(line).empty()) continue;
        const auto f = split_csv(line);
        const int need = std::max({c.symbol, c.series, c.date, c.open, c.high, c.low, c.close, c.volume});
        if (static_cast<int>(f.size()) <= need) {
            throw std::runtime_error(path + ":" + std::to_string(n) + ": too few columns");
        }
        try {
            BhavRow r;
            r.symbol = f[c.symbol];
            r.series = upper(f[c.series]);
            r.bar = Bar{normalize_date(f[c.date]), std::stod(f[c.open]), std::stod(f[c.high]),
                        std::stod(f[c.low]), std::stod(f[c.close]), std::stod(f[c.volume])};
            if (c.prev >= 0 && c.prev < static_cast<int>(f.size()) && !f[c.prev].empty()) {
                r.prev_close = std::stod(f[c.prev]);
            }
            rows.push_back(std::move(r));
        } catch (const std::invalid_argument&) {
            throw std::runtime_error(path + ":" + std::to_string(n) + ": bad number");
        }
    }
    return rows;
}

std::vector<CorporateAction> load_corporate_actions(const std::string& path) {
    std::ifstream in(path);
    if (!in) throw std::runtime_error("cannot open " + path);
    std::vector<CorporateAction> out;
    std::string line;
    while (std::getline(in, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || upper(line).rfind("SYMBOL", 0) == 0) continue;
        const auto f = split_csv(line);
        if (f.size() < 3) throw std::runtime_error(path + ": expected symbol,ex_date,factor");
        const double factor = std::stod(f[2]);
        if (!(factor > 0)) throw std::runtime_error(path + ": factor must be > 0 for " + f[0]);
        out.push_back({f[0], normalize_date(f[1]), factor});
    }
    return out;
}

void apply_corporate_actions(Universe& u, const std::vector<CorporateAction>& actions) {
    for (const auto& a : actions) {
        auto it = u.find(a.symbol);
        if (it == u.end()) continue;
        for (Bar& b : it->second) {
            if (b.date >= a.ex_date) break;  // series is sorted
            b.open *= a.factor;
            b.high *= a.factor;
            b.low *= a.factor;
            b.close *= a.factor;
            b.volume /= a.factor;
        }
    }
}

IngestResult ingest_bhavcopy_dir(const std::string& dir, const std::set<std::string>& series,
                                 double action_threshold) {
    IngestResult r;
    struct Day {
        bool is_eq = false;
        Bar bar;
        double prev_close = 0;
    };
    std::map<std::string, std::map<std::string, Day>> acc;  // symbol -> date -> row
    std::vector<std::filesystem::path> files;
    for (const auto& e : std::filesystem::directory_iterator(dir)) {
        if (e.path().extension() == ".csv" || e.path().extension() == ".CSV") files.push_back(e.path());
    }
    std::sort(files.begin(), files.end());
    for (const auto& p : files) {
        ++r.files;
        for (auto& row : parse_bhavcopy(p.string())) {
            if (!series.count(row.series)) continue;
            ++r.rows;
            const bool is_eq = row.series == "EQ";
            Day& slot = acc[row.symbol][row.bar.date];
            if (slot.bar.date.empty() || (is_eq && !slot.is_eq)) slot = {is_eq, row.bar, row.prev_close};
        }
    }
    for (auto& [sym, days] : acc) {
        Series& s = r.universe[sym];
        for (auto& [date, d] : days) {
            if (!s.empty() && d.prev_close > 0 && s.back().close > 0) {
                const double factor = d.prev_close / s.back().close;
                if (std::fabs(factor - 1.0) > action_threshold) r.derived_actions.push_back({sym, date, factor});
            }
            s.push_back(d.bar);
            if (!d.is_eq) r.non_eq_days.emplace_back(sym, date);
        }
    }
    return r;
}

void write_series(const std::string& path, const Series& s) {
    std::ofstream out(path);
    if (!out) throw std::runtime_error("cannot write " + path);
    out << "date,open,high,low,close,volume\n";
    out.precision(10);
    for (const Bar& b : s) {
        out << b.date << ',' << b.open << ',' << b.high << ',' << b.low << ',' << b.close << ','
            << b.volume << '\n';
    }
}

void write_universe(const std::string& dir, const Universe& u) {
    std::filesystem::create_directories(dir);
    for (const auto& [sym, s] : u) write_series((std::filesystem::path(dir) / (sym + ".csv")).string(), s);
}

ExclusionList ExclusionList::load(const std::string& path) {
    std::ifstream in(path);
    if (!in) throw std::runtime_error("cannot open " + path);
    ExclusionList x;
    std::string line;
    while (std::getline(in, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;
        const auto f = split_csv(line);
        if (f.size() >= 2 && !f[1].empty()) x.add(f[0], normalize_date(f[1]));
        else x.add(f[0]);
    }
    return x;
}

bool ExclusionList::excluded(const std::string& symbol, const std::string& date) const {
    if (always_.count(symbol)) return true;
    const auto it = by_date_.find(symbol);
    return it != by_date_.end() && it->second.count(date);
}

std::string day_of(const std::string& ts) { return ts.substr(0, 10); }
std::string time_of(const std::string& ts) { return ts.size() >= 16 ? ts.substr(11, 5) : ""; }

DayIndex index_by_day(const Series& intraday) {
    DayIndex idx;
    for (std::size_t k = 0; k < intraday.size(); ++k) {
        const std::string d = day_of(intraday[k].date);
        auto it = idx.find(d);
        if (it == idx.end()) idx[d] = {k, k + 1};
        else it->second.second = k + 1;
    }
    return idx;
}

std::map<std::string, double> daily_closes(const Series& intraday) {
    std::map<std::string, double> out;
    for (const Bar& b : intraday) out[day_of(b.date)] = b.close;
    return out;
}

std::map<std::string, double> load_dated_values(const std::string& path) {
    std::ifstream in(path);
    if (!in) throw std::runtime_error("cannot open " + path);
    std::map<std::string, double> out;
    std::string line;
    bool header = true;
    while (std::getline(in, line)) {
        if (header) {
            header = false;
            continue;
        }
        const auto f = split_csv(line);
        if (f.size() < 2 || f[0].empty()) continue;
        out[normalize_date(f[0])] = std::stod(f[1]);
    }
    return out;
}

}  // namespace ta
