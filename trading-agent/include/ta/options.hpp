#pragma once

#include <string>
#include <vector>

namespace ta {

// Sleeve C: defined-risk index option structures (iron condors, iron flies, credit
// spreads). No net-of-cost backtest of retail Nifty condors exists, so this module
// validates and sizes a proposed structure; it does not claim an edge.
enum class OptionType { Call, Put };

struct OptionLeg {
    OptionType type = OptionType::Put;
    double strike = 0;
    int lots = 0;          // +long, -short
    double premium = 0;    // per unit, paid (long) or received (short)
};

struct OptionStructure {
    std::string underlying = "NIFTY";
    std::string expiry;    // YYYY-MM-DD
    double lot_size = 65;
    std::vector<OptionLeg> legs;
};

// "P:22000:-1:85,P:21800:1:40,C:24000:-1:70,C:24200:1:30"
OptionStructure parse_structure(const std::string& legs, const std::string& expiry, double lot_size);

// P&L per structure (all legs at their lot counts) at expiry for a given spot, premiums included.
double payoff_at_expiry(const OptionStructure& s, double spot);
double net_credit(const OptionStructure& s);  // premiums received minus paid, rupees
// Maximum loss at expiry in rupees (positive number); +inf when undefined (naked short).
double max_loss(const OptionStructure& s);
double max_profit(const OptionStructure& s);
std::vector<double> breakevens(const OptionStructure& s);

struct OptionsConfig {
    double budget_frac = 0.0075;        // max loss per expiry: 0.5-1% of the active book
    double cost_per_leg_lot = 62.5;     // rupees per lot per leg, round trip (April 2026 STT)
    std::string expiry_exit_time = "13:00";  // close everything before the final hours
};

struct OptionsContext {
    std::string today;                  // YYYY-MM-DD
    double active_book = 0;             // E_A in rupees
    double existing_max_loss = 0;       // max loss already open on this expiry
    bool vol_gate_red = false;
    bool event_blackout = false;
};

struct OptionsCheck {
    bool ok = false;
    std::vector<std::string> violations;
    int multiplier = 0;                 // how many copies of the structure fit the budget
    double max_loss_each = 0;           // per copy, including round-trip costs
    double credit_after_costs_each = 0;
};

// Applies every Sleeve C rule: defined risk with a long wing behind each short leg, no
// entry on expiry day, gates and blackouts, positive credit after costs, budget sizing.
OptionsCheck check_structure(const OptionStructure& s, const OptionsConfig& cfg, const OptionsContext& ctx);

}  // namespace ta
