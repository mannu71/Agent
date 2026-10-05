#pragma once

#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "ta/bar.hpp"

namespace ta {

// ---------------------------------------------------------------- bhavcopy

struct BhavRow {
    std::string symbol;
    std::string series;
    Bar bar;
    double prev_close = 0;  // exchange's previous close, adjusted for corporate actions (0 if absent)
};

// Parses one NSE cash-market bhavcopy CSV. Recognises the legacy format
// (SYMBOL,SERIES,OPEN,...,TIMESTAMP), the UDiFF format used since July 2024
// (TradDt,...,TckrSymb,SctySrs,OpnPric,...) and sec_bhavdata_full (DATE1,...).
std::vector<BhavRow> parse_bhavcopy(const std::string& path);

// Corporate action: prices before ex_date are multiplied by `factor` and volumes
// divided by it. A 1:1 bonus is factor 0.5; a split from face value 10 to 2 is 0.2.
struct CorporateAction {
    std::string symbol;
    std::string ex_date;
    double factor = 1;
};
std::vector<CorporateAction> load_corporate_actions(const std::string& path);  // symbol,ex_date,factor
void apply_corporate_actions(Universe& u, const std::vector<CorporateAction>& actions);

struct IngestResult {
    Universe universe;
    // Split/bonus/consolidation factors inferred from the exchange's adjusted previous close:
    // factor = PREVCLOSE(ex-date) / CLOSE(prior session) when it differs from 1 by more
    // than the threshold. Not applied until apply_corporate_actions() is called.
    std::vector<CorporateAction> derived_actions;
    // (symbol, date) rows whose series was not "EQ", e.g. trade-for-trade BE/BZ.
    std::vector<std::pair<std::string, std::string>> non_eq_days;
    std::size_t files = 0;
    std::size_t rows = 0;
};

// Reads every *.csv bhavcopy in `dir`, keeps rows whose series is in `series`,
// and builds per-symbol daily series. Duplicate (symbol, date) rows keep the EQ row.
IngestResult ingest_bhavcopy_dir(const std::string& dir, const std::set<std::string>& series,
                                 double action_threshold = 0.02);

void write_series(const std::string& path, const Series& s);
void write_universe(const std::string& dir, const Universe& u);

// ---------------------------------------------------------------- exclusions

// Symbols barred from new entries: always ("SYMBOL") or on given dates
// ("SYMBOL,YYYY-MM-DD"), e.g. ASM/GSM/ESM lists and trade-for-trade days.
class ExclusionList {
public:
    ExclusionList() = default;
    explicit ExclusionList(std::set<std::string> always) : always_(std::move(always)) {}
    static ExclusionList load(const std::string& path);

    void add(const std::string& symbol) { always_.insert(symbol); }
    void add(const std::string& symbol, const std::string& date) { by_date_[symbol].insert(date); }
    bool excluded(const std::string& symbol, const std::string& date) const;

private:
    std::set<std::string> always_;
    std::map<std::string, std::set<std::string>> by_date_;
};

// ---------------------------------------------------------------- intraday

// Intraday bars use Bar::date = "YYYY-MM-DD HH:MM" (bar start time, exchange local).
std::string day_of(const std::string& ts);
std::string time_of(const std::string& ts);  // "HH:MM"

// day -> [first, last) index range into the intraday series
using DayIndex = std::map<std::string, std::pair<std::size_t, std::size_t>>;
DayIndex index_by_day(const Series& intraday);

// One close per day from intraday bars (last bar's close), for prior-close lookups.
std::map<std::string, double> daily_closes(const Series& intraday);

// Simple dated series (e.g. India VIX close, funding rate): "date,value" with header.
std::map<std::string, double> load_dated_values(const std::string& path);

}  // namespace ta
