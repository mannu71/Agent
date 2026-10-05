#include "ta/csv.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace ta {

namespace {

std::vector<std::string> split(const std::string& line, char sep) {
    std::vector<std::string> out;
    std::stringstream ss(line);
    std::string field;
    while (std::getline(ss, field, sep)) out.push_back(field);
    return out;
}

std::string trim(const std::string& s) {
    const auto b = s.find_first_not_of(" \t\r\n");
    if (b == std::string::npos) return "";
    const auto e = s.find_last_not_of(" \t\r\n");
    return s.substr(b, e - b + 1);
}

}  // namespace

Series load_series(const std::string& path) {
    std::ifstream in(path);
    if (!in) throw std::runtime_error("cannot open " + path);

    Series out;
    std::string line;
    std::size_t line_no = 0;
    while (std::getline(in, line)) {
        ++line_no;
        line = trim(line);
        if (line.empty() || line_no == 1) continue;  // header
        const auto f = split(line, ',');
        if (f.size() < 6) {
            throw std::runtime_error(path + ":" + std::to_string(line_no) + ": expected 6 columns");
        }
        try {
            out.push_back(Bar{trim(f[0]), std::stod(f[1]), std::stod(f[2]), std::stod(f[3]),
                              std::stod(f[4]), std::stod(f[5])});
        } catch (const std::exception&) {
            throw std::runtime_error(path + ":" + std::to_string(line_no) + ": bad number");
        }
    }
    std::sort(out.begin(), out.end(), [](const Bar& a, const Bar& b) { return a.date < b.date; });
    return out;
}

Universe load_universe(const std::string& dir) {
    Universe u;
    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        if (entry.path().extension() != ".csv") continue;
        u[entry.path().stem().string()] = load_series(entry.path().string());
    }
    return u;
}

std::set<std::string> load_symbol_list(const std::string& path) {
    std::ifstream in(path);
    if (!in) throw std::runtime_error("cannot open " + path);
    std::set<std::string> out;
    std::string line;
    while (std::getline(in, line)) {
        line = trim(line);
        if (!line.empty() && line[0] != '#') out.insert(line);
    }
    return out;
}

}  // namespace ta
