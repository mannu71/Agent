#pragma once

#include <string>
#include <vector>

#include "ta/a1.hpp"
#include "ta/a2.hpp"
#include "ta/crypto_trend.hpp"
#include "ta/d1.hpp"
#include "ta/kv.hpp"
#include "ta/options.hpp"
#include "ta/portfolio.hpp"

namespace ta {

// Config-file keys for every tunable limit, so limits live in version-controlled
// configuration instead of being recompiled. Every key is optional.
void apply_settings(const Config& c, Allocation& a);
void apply_settings(const Config& c, A1Config& a1);
void apply_settings(const Config& c, A2Config& a2);
void apply_settings(const Config& c, CryptoTrendConfig& b);
void apply_settings(const Config& c, D1Config& d1);
void apply_settings(const Config& c, OptionsConfig& o);

// All recognised keys with their default values, for `ta_paper init` and the README.
std::vector<std::pair<std::string, std::string>> default_settings();

// Throws if the config contains a key that is not recognised (catches typos that
// would otherwise silently leave a limit at its default).
void check_settings_keys(const Config& c);

}  // namespace ta
