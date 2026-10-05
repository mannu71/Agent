#pragma once

#include <string>

#include "ta/risk.hpp"

namespace ta {

// Capital split from the blueprint, refined by the rulebook. Shares of total capital.
struct Allocation {
    double core = 0.75;    // T-bills, liquid and arbitrage funds: not traded by the agent
    double buffer = 0.05;  // cash at the broker for margin and mark-to-market
    double a1 = 0.10;      // momentum-leader breakouts
    double a2 = 0.00;      // episodic-pivot gaps: paper only until every gate passes
    double b = 0.04;       // crypto trend
    double c = 0.03;       // defined-risk index options (max-loss budget)
    double d = 0.00;       // intraday: zero until D1 clears 2x costs
    double reserve = 0.03; // active-book cash freed when A1 went 13% -> 10%; earmarked for A2

    double active_share() const { return 1.0 - core; }  // E_A as a share of total
    // Empty string when valid, else the reason.
    std::string validate() const;
};

// Rulebook risk units are quoted as a fraction of the active book E_A. A sleeve that
// holds `sleeve_capital` must scale them to its own equity.
double to_sleeve_fraction(double fraction_of_active_book, double active_book, double sleeve_capital);

// Whole-active-book breaker: halve everything at -10%, stop everything at -20%
// (latched until a human resets it).
RiskConfig active_book_breaker_config();

// Open risk and positions held by *other* sleeves that share a limit with this one.
// A1 and A2 share a 3%-of-E_A heat cap and the 12-position limit.
struct SharedBook {
    double external_open_risk = 0;   // rupees at risk to stops in the other sleeve
    int external_positions = 0;
    double heat_cap = 0;             // rupees; 0 disables the check
};

}  // namespace ta
