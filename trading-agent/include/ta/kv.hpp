#pragma once

#include <map>
#include <string>
#include <utility>
#include <vector>

namespace ta {

// One line of persistent state or journal payload: "<type> key=value key=value ...".
// Values are percent-escaped so they never contain spaces, '=', '|' or newlines.
// Doubles are written with 17 significant digits so they round-trip exactly.
struct KvRecord {
    std::string type;
    std::vector<std::pair<std::string, std::string>> fields;

    KvRecord& set(const std::string& k, const std::string& v);
    KvRecord& set(const std::string& k, double v);
    KvRecord& set(const std::string& k, long v);
    KvRecord& set(const std::string& k, int v) { return set(k, static_cast<long>(v)); }
    KvRecord& set(const std::string& k, bool v) { return set(k, static_cast<long>(v ? 1 : 0)); }

    bool has(const std::string& k) const;
    std::string str(const std::string& k, const std::string& def = "") const;
    double num(const std::string& k, double def = 0) const;
    long integer(const std::string& k, long def = 0) const;

    std::string encode() const;
    static KvRecord parse(const std::string& line);
};

std::string fmt_double(double v);

// Flat "key = value" configuration file; '#' starts a comment.
using Config = std::map<std::string, std::string>;
Config load_config(const std::string& path);

// Overwrite `field` when `key` is present in the config.
void config_set(const Config& c, const std::string& key, double& field);
void config_set(const Config& c, const std::string& key, int& field);
void config_set(const Config& c, const std::string& key, std::size_t& field);
void config_set(const Config& c, const std::string& key, std::string& field);
void config_set(const Config& c, const std::string& key, bool& field);  // 0/1, true/false

}  // namespace ta
