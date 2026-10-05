#include "ta/options.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <set>
#include <sstream>
#include <stdexcept>

namespace ta {

namespace {

double intrinsic(const OptionLeg& l, double spot) {
    return l.type == OptionType::Call ? std::max(0.0, spot - l.strike) : std::max(0.0, l.strike - spot);
}

// Payoff slope (per unit of spot) beyond the highest strike.
double upper_slope(const OptionStructure& s) {
    double slope = 0;
    for (const auto& l : s.legs) {
        if (l.type == OptionType::Call) slope += l.lots * s.lot_size;
    }
    return slope;
}

std::vector<double> kink_points(const OptionStructure& s) {
    std::set<double> pts = {0.0};
    for (const auto& l : s.legs) pts.insert(l.strike);
    return {pts.begin(), pts.end()};
}

}  // namespace

OptionStructure parse_structure(const std::string& legs, const std::string& expiry, double lot_size) {
    OptionStructure s;
    s.expiry = expiry;
    s.lot_size = lot_size;
    std::stringstream ss(legs);
    std::string item;
    while (std::getline(ss, item, ',')) {
        std::stringstream is(item);
        std::string type, strike, lots, prem;
        if (!std::getline(is, type, ':') || !std::getline(is, strike, ':') || !std::getline(is, lots, ':') ||
            !std::getline(is, prem, ':')) {
            throw std::runtime_error("bad leg '" + item + "', expected TYPE:STRIKE:LOTS:PREMIUM");
        }
        OptionLeg l;
        if (type == "C" || type == "c") l.type = OptionType::Call;
        else if (type == "P" || type == "p") l.type = OptionType::Put;
        else throw std::runtime_error("bad option type in '" + item + "'");
        l.strike = std::stod(strike);
        l.lots = std::stoi(lots);
        l.premium = std::stod(prem);
        if (l.lots == 0 || l.strike <= 0 || l.premium < 0) throw std::runtime_error("bad leg '" + item + "'");
        s.legs.push_back(l);
    }
    if (s.legs.empty()) throw std::runtime_error("no legs");
    return s;
}

double net_credit(const OptionStructure& s) {
    double c = 0;
    for (const auto& l : s.legs) c -= l.lots * l.premium * s.lot_size;
    return c;
}

double payoff_at_expiry(const OptionStructure& s, double spot) {
    double p = net_credit(s);
    for (const auto& l : s.legs) p += l.lots * intrinsic(l, spot) * s.lot_size;
    return p;
}

double max_loss(const OptionStructure& s) {
    if (upper_slope(s) < 0) return std::numeric_limits<double>::infinity();
    double worst = std::numeric_limits<double>::infinity();
    for (double k : kink_points(s)) worst = std::min(worst, payoff_at_expiry(s, k));
    return std::max(0.0, -worst);
}

double max_profit(const OptionStructure& s) {
    if (upper_slope(s) > 0) return std::numeric_limits<double>::infinity();
    double best = -std::numeric_limits<double>::infinity();
    for (double k : kink_points(s)) best = std::max(best, payoff_at_expiry(s, k));
    return best;
}

std::vector<double> breakevens(const OptionStructure& s) {
    std::vector<double> out;
    auto pts = kink_points(s);
    pts.push_back(pts.back() * 2 + 1);  // far right, to catch a crossing beyond the last strike
    for (std::size_t k = 1; k < pts.size(); ++k) {
        const double a = pts[k - 1], b = pts[k];
        const double pa = payoff_at_expiry(s, a), pb = payoff_at_expiry(s, b);
        if ((pa < 0 && pb > 0) || (pa > 0 && pb < 0)) out.push_back(a + (b - a) * (-pa) / (pb - pa));
        else if (pb == 0 && k + 1 < pts.size()) out.push_back(b);
    }
    return out;
}

OptionsCheck check_structure(const OptionStructure& s, const OptionsConfig& cfg, const OptionsContext& ctx) {
    OptionsCheck r;
    // Defined risk: every short lot needs a long lot of the same type further out of the money.
    for (const auto& sh : s.legs) {
        if (sh.lots >= 0) continue;
        int cover = 0;
        for (const auto& lg : s.legs) {
            if (lg.lots <= 0 || lg.type != sh.type) continue;
            const bool further_otm = sh.type == OptionType::Put ? lg.strike < sh.strike : lg.strike > sh.strike;
            if (further_otm) cover += lg.lots;
        }
        if (cover < -sh.lots) {
            r.violations.push_back("naked short " + std::string(sh.type == OptionType::Call ? "call " : "put ") +
                                   std::to_string(static_cast<long>(sh.strike)));
        }
    }
    const double ml = max_loss(s);
    if (!std::isfinite(ml)) r.violations.push_back("undefined maximum loss");
    if (ctx.today == s.expiry) r.violations.push_back("no new positions on expiry day");
    if (ctx.vol_gate_red) r.violations.push_back("volatility gate red");
    if (ctx.event_blackout) r.violations.push_back("event blackout");

    double leg_lots = 0;
    for (const auto& l : s.legs) leg_lots += std::abs(l.lots);
    const double costs = leg_lots * cfg.cost_per_leg_lot;
    r.credit_after_costs_each = net_credit(s) - costs;
    if (r.credit_after_costs_each <= 0) r.violations.push_back("credit does not cover round-trip costs");

    if (std::isfinite(ml)) {
        r.max_loss_each = ml + costs;
        const double budget = cfg.budget_frac * ctx.active_book - ctx.existing_max_loss;
        r.multiplier = r.max_loss_each > 0 ? static_cast<int>(std::floor(std::max(0.0, budget) / r.max_loss_each)) : 0;
        if (r.multiplier < 1) r.violations.push_back("max loss exceeds this expiry's budget");
    }
    r.ok = r.violations.empty();
    if (!r.ok) r.multiplier = 0;
    return r;
}

}  // namespace ta
