// Tiny "--key value" / "--flag" argument parser shared by the command-line tools.
#pragma once

#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace cli {

class Args {
public:
    // `flags` are options that take no value.
    Args(int argc, char** argv, int first, const std::set<std::string>& flags) {
        for (int k = first; k < argc; ++k) {
            const std::string a = argv[k];
            if (a.rfind("--", 0) == 0) {
                const std::string key = a.substr(2);
                if (flags.count(key)) {
                    opts_[key] = "1";
                } else {
                    if (k + 1 >= argc) throw std::runtime_error("missing value for " + a);
                    if (key == "set") sets_.push_back(argv[++k]);
                    else opts_[key] = argv[++k];
                }
            } else {
                positional_.push_back(a);
            }
        }
    }

    bool has(const std::string& k) const { return opts_.count(k) != 0; }
    std::string str(const std::string& k, const std::string& def = "") const {
        const auto it = opts_.find(k);
        return it == opts_.end() ? def : it->second;
    }
    double num(const std::string& k, double def) const { return has(k) ? std::stod(str(k)) : def; }
    const std::vector<std::string>& positional() const { return positional_; }
    std::string pos(std::size_t i) const {
        if (i >= positional_.size()) throw std::runtime_error("missing argument");
        return positional_[i];
    }
    // Repeated "--set key=value" pairs.
    const std::vector<std::string>& sets() const { return sets_; }

private:
    std::map<std::string, std::string> opts_;
    std::vector<std::string> positional_;
    std::vector<std::string> sets_;
};

}  // namespace cli
