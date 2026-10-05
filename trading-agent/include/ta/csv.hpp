#pragma once

#include <set>
#include <string>

#include "ta/bar.hpp"

namespace ta {

// Reads a CSV with header "date,open,high,low,close,volume".
// Throws std::runtime_error on malformed rows. Output is sorted by date.
Series load_series(const std::string& path);

// Loads every "*.csv" in a directory; the file stem is the symbol.
Universe load_universe(const std::string& dir);

// One symbol per line; blank lines and lines starting with '#' are ignored.
std::set<std::string> load_symbol_list(const std::string& path);

}  // namespace ta
