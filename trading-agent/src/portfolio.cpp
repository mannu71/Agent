#include "ta/portfolio.hpp"

#include <cmath>

namespace ta {

std::string Allocation::validate() const {
    const double parts[] = {core, buffer, a1, a2, b, c, d, reserve};
    double sum = 0;
    for (double p : parts) {
        if (p < 0) return "allocation shares must be >= 0";
        sum += p;
    }
    if (std::fabs(sum - 1.0) > 1e-9) return "allocation shares must sum to 1";
    if (core < 0.70) return "core must be at least 70% of capital";
    if (a1 + a2 > 0.13 + 1e-12) return "A1 + A2 must not exceed 13% of capital";
    return "";
}

double to_sleeve_fraction(double fraction_of_active_book, double active_book, double sleeve_capital) {
    if (sleeve_capital <= 0) return 0;
    return fraction_of_active_book * active_book / sleeve_capital;
}

RiskConfig active_book_breaker_config() {
    RiskConfig c;
    c.drawdown_halve = 0.10;
    c.drawdown_off = 0.20;
    c.daily_loss_limit = 0.02;  // flatten intraday, no new entries
    return c;
}

}  // namespace ta
