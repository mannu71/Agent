#pragma once

#include <map>
#include <string>
#include <vector>

#include "ta/kv.hpp"
#include "ta/market.hpp"
#include "ta/regime.hpp"
#include "ta/risk.hpp"
#include "ta/trade.hpp"

namespace ta {

// Sleeve B: the Donchian ensemble from "Catching Crypto Trends" (Zarattini et al.),
// long/flat, with the rulebook's Indian limits. Daily bars close at 00:00 UTC.
struct CryptoTrendConfig {
    std::vector<std::size_t> lookbacks = {5, 10, 20, 30, 60, 90, 150, 250, 360};
    double vol_target = 0.25;          // per-lookback weight = min(vol_target / sigma90, max_lookback_weight)
    double max_lookback_weight = 2.0;
    double asset_cap = 1.0;            // India: never more than 1x notional per asset
    std::size_t vol_n = 90;
    double periods_per_year = 365;
    double rebalance_band = 0.20;      // vol-only changes trade when |current - target| > 20% of target
    double cost_bps = 25;              // per side; 10 = the paper (replication), 50 = stress
    double tax_rate = 0.312;           // worst case: 30% + 4% cess on every realised gain, no offset
    double funding_annual_default = 0.10;  // used on days without a funding observation (BIS carry)
    RiskConfig risk;                   // sleeve drawdown halve/off
    RegimeConfig regime;               // crowding gate threshold

    // A healthy trend sleeve at ~15% volatility has a median 3-year drawdown near 19% and
    // a 90th percentile near 33% (Rej et al.), so the blueprint's -7.5%/-15% would latch
    // off a working sleeve about half the time. Halve at -20%, off at -30% (human review).
    CryptoTrendConfig() {
        risk.drawdown_halve = 0.20;
        risk.drawdown_off = 0.30;
    }

    // Reproduce-first mode: the paper's 10 bps, no funding, no drawdown or crowding overlay,
    // so trade counts and the drawdown can be compared with "Catching Crypto Trends".
    void replication_mode();
};

// Per-asset auxiliary data: funding is the day's total funding rate as a fraction
// (positive = longs pay); open interest is optional.
struct CryptoAux {
    std::map<std::string, double> funding;
    std::map<std::string, double> open_interest;
};

class CryptoTrendEngine {
public:
    CryptoTrendEngine(const MarketData& md, CryptoTrendConfig cfg, std::map<std::string, CryptoAux> aux,
                      double initial_equity);

    // Advances signal state through every date before `date` without trading, so a paper
    // account opened mid-history starts with the same signals a backtest would have.
    void prime(const std::string& date);

    // `risk_multiplier` comes from the active-book breaker (1, 0.5 or 0).
    void step(const std::string& date, double risk_multiplier = 1.0, std::vector<KvRecord>* events = nullptr);

    double equity() const { return last_equity_; }
    const std::string& last_date() const { return last_date_; }
    double tax_paid() const { return tax_paid_; }
    double funding_paid() const { return funding_paid_; }
    double costs_paid() const { return costs_paid_; }
    // Number of entries per lookback, for the reproduce-first check against the paper.
    const std::map<std::size_t, long>& entries_by_lookback() const { return entries_; }
    double exposure(const std::string& asset) const;  // units held
    int holdings() const { return static_cast<int>(holdings_.size()); }
    RiskManager& risk() { return risk_; }
    const RiskManager& risk() const { return risk_; }

    std::vector<Trade> drain_closed();
    void flatten(const std::string& date, const std::string& reason, std::vector<KvRecord>* events);

    std::vector<KvRecord> save() const;
    void load(const std::vector<KvRecord>& records);

private:
    struct Signal {
        bool long_ = false;
        double stop = 0;
    };
    struct Holding {
        double qty = 0;
        double avg_cost = 0;
        double last_close = 0;
        Trade trade;  // open round trip, for the trade log
    };

    // Updates every lookback's signal for `asset` at bar i; returns true if any flipped.
    bool update_signals(const std::string& asset, std::size_t i);
    double target_weight(const std::string& asset, std::size_t i) const;
    void trade_to(const std::string& asset, double target_qty, double px, const std::string& date,
                  const std::string& reason, std::vector<KvRecord>* events);
    double mark(const std::string& date) const;

    const MarketData& md_;
    CryptoTrendConfig cfg_;
    std::map<std::string, CryptoAux> aux_;
    RiskManager risk_;
    double cash_;
    std::map<std::string, std::map<std::size_t, Signal>> signals_;
    std::map<std::string, Holding> holdings_;
    std::map<std::size_t, long> entries_;
    std::vector<Trade> closed_;
    double tax_paid_ = 0, funding_paid_ = 0, costs_paid_ = 0;
    double last_equity_;
    std::string last_date_;
    std::string primed_until_;
};

struct CryptoBacktestResult {
    std::vector<Trade> trades;
    EquityCurve equity_curve;
    Metrics metrics;
    double tax_paid = 0, funding_paid = 0, costs_paid = 0;
    std::map<std::size_t, long> entries_by_lookback;
    RiskState final_risk_state = RiskState::Normal;
};

CryptoBacktestResult run_crypto_backtest(const MarketData& md, const CryptoTrendConfig& cfg,
                                         const std::map<std::string, CryptoAux>& aux, double initial_equity,
                                         const std::string& start = "", const std::string& end = "");

}  // namespace ta
