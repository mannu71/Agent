#include "ta/kv.hpp"

#include <cstdio>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace ta {

namespace {

std::string escape(const std::string& s) {
    std::string out;
    for (const unsigned char ch : s) {
        if (ch == ' ' || ch == '=' || ch == '|' || ch == '%' || ch == '\n' || ch == '\r' || ch == '\t') {
            char buf[4];
            std::snprintf(buf, sizeof buf, "%%%02X", ch);
            out += buf;
        } else {
            out += static_cast<char>(ch);
        }
    }
    return out;
}

std::string unescape(const std::string& s) {
    std::string out;
    for (std::size_t k = 0; k < s.size(); ++k) {
        if (s[k] == '%' && k + 2 < s.size()) {
            out += static_cast<char>(std::stoi(s.substr(k + 1, 2), nullptr, 16));
            k += 2;
        } else {
            out += s[k];
        }
    }
    return out;
}

std::string trim(const std::string& s) {
    const auto b = s.find_first_not_of(" \t\r\n");
    if (b == std::string::npos) return "";
    return s.substr(b, s.find_last_not_of(" \t\r\n") - b + 1);
}

}  // namespace

std::string fmt_double(double v) {
    char buf[40];
    std::snprintf(buf, sizeof buf, "%.17g", v);
    return buf;
}

KvRecord& KvRecord::set(const std::string& k, const std::string& v) {
    for (auto& f : fields) {
        if (f.first == k) {
            f.second = v;
            return *this;
        }
    }
    fields.emplace_back(k, v);
    return *this;
}

KvRecord& KvRecord::set(const std::string& k, double v) { return set(k, fmt_double(v)); }
KvRecord& KvRecord::set(const std::string& k, long v) { return set(k, std::to_string(v)); }

bool KvRecord::has(const std::string& k) const {
    for (const auto& f : fields) {
        if (f.first == k) return true;
    }
    return false;
}

std::string KvRecord::str(const std::string& k, const std::string& def) const {
    for (const auto& f : fields) {
        if (f.first == k) return f.second;
    }
    return def;
}

double KvRecord::num(const std::string& k, double def) const {
    return has(k) ? std::stod(str(k)) : def;
}

long KvRecord::integer(const std::string& k, long def) const {
    return has(k) ? std::stol(str(k)) : def;
}

std::string KvRecord::encode() const {
    std::string out = escape(type);
    for (const auto& [k, v] : fields) out += " " + escape(k) + "=" + escape(v);
    return out;
}

KvRecord KvRecord::parse(const std::string& line) {
    KvRecord r;
    std::istringstream in(line);
    std::string tok;
    in >> tok;
    r.type = unescape(tok);
    while (in >> tok) {
        const auto eq = tok.find('=');
        if (eq == std::string::npos) throw std::runtime_error("bad kv token: " + tok);
        r.fields.emplace_back(unescape(tok.substr(0, eq)), unescape(tok.substr(eq + 1)));
    }
    return r;
}

Config load_config(const std::string& path) {
    std::ifstream in(path);
    if (!in) throw std::runtime_error("cannot open config " + path);
    Config c;
    std::string line;
    int n = 0;
    while (std::getline(in, line)) {
        ++n;
        const auto hash = line.find('#');
        if (hash != std::string::npos) line = line.substr(0, hash);
        line = trim(line);
        if (line.empty()) continue;
        const auto eq = line.find('=');
        if (eq == std::string::npos) {
            throw std::runtime_error(path + ":" + std::to_string(n) + ": expected key = value");
        }
        c[trim(line.substr(0, eq))] = trim(line.substr(eq + 1));
    }
    return c;
}

void config_set(const Config& c, const std::string& key, double& field) {
    if (const auto it = c.find(key); it != c.end()) field = std::stod(it->second);
}
void config_set(const Config& c, const std::string& key, int& field) {
    if (const auto it = c.find(key); it != c.end()) field = std::stoi(it->second);
}
void config_set(const Config& c, const std::string& key, std::size_t& field) {
    if (const auto it = c.find(key); it != c.end()) field = std::stoul(it->second);
}
void config_set(const Config& c, const std::string& key, bool& field) {
    if (const auto it = c.find(key); it != c.end()) {
        const std::string& v = it->second;
        if (v == "1" || v == "true" || v == "yes" || v == "on") field = true;
        else if (v == "0" || v == "false" || v == "no" || v == "off") field = false;
        else throw std::runtime_error("expected a boolean for " + key + ", got " + v);
    }
}

void config_set(const Config& c, const std::string& key, std::string& field) {
    if (const auto it = c.find(key); it != c.end()) field = it->second;
}

}  // namespace ta
