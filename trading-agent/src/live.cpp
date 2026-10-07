#include "ta/live.hpp"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <set>
#include <sstream>

#include "ta/journal.hpp"
#include "ta/kv.hpp"

namespace fs = std::filesystem;

namespace ta {

namespace {

std::string case_key(const Setup& s) {
    return s.pattern + "|" + s.symbol + "|" + std::to_string(s.tf) + "|" + (s.side > 0 ? "long" : "short") + "|" +
           format_minutes(s.t) + "|" + std::string(1, s.order);
}

// Event id -> payload of every event already in the journal.
std::map<std::string, KvRecord> journalled(const std::string& path) {
    std::map<std::string, KvRecord> out;
    std::ifstream in(path);
    std::string line;
    while (std::getline(in, line)) {
        // seq|time|prev|payload|hash; the payload escapes '|'.
        const auto a = line.find('|'), b = line.find('|', a + 1), c = line.find('|', b + 1), d = line.rfind('|');
        if (c == std::string::npos || d <= c) continue;
        const KvRecord r = KvRecord::parse(line.substr(c + 1, d - c - 1));
        if (r.type != "revision") out[r.type + "#" + r.str("key")] = r;
    }
    return out;
}

std::string status_of(const Case& c, std::int64_t last) {
    if (!c.filled) return c.setup.expiry > last ? "pending" : "unfilled";
    return c.exit_reason == "end_of_data" ? "open" : "closed";
}

}  // namespace

LiveReport write_live(const std::string& dir, const std::vector<Case>& cases, const PortfolioResult& agent,
                      std::int64_t live_from, const std::map<std::string, std::int64_t>& last_t) {
    fs::create_directories(dir);
    const std::string jpath = (fs::path(dir) / "journal.log").string();
    auto seen = journalled(jpath);
    Journal journal(jpath);
    LiveReport rep;
    std::ostringstream digest;
    digest << std::fixed << std::setprecision(2);

    auto emit = [&](const std::string& type, const std::string& key, KvRecord r, const char* compare) {
        r.type = type;
        r.set("key", key);
        const auto it = seen.find(type + "#" + key);
        if (it == seen.end()) {
            journal.append(r);
            seen[type + "#" + key] = r;
            ++rep.new_events;
            return true;
        }
        if (compare && std::fabs(it->second.num(compare) - r.num(compare)) > 1e-9) {
            KvRecord v;
            v.type = "revision";
            v.set("key", key).set("event", type).set("field", std::string(compare));
            v.set("old", it->second.num(compare)).set("new", r.num(compare));
            journal.append(v);
            ++rep.revisions;
        }
        return false;
    };

    std::set<std::size_t> taken;
    for (std::size_t k = 0; k < agent.trades.size(); ++k) taken.insert(agent.case_of_trade[k]);

    std::ofstream setups((fs::path(dir) / "setups.csv").string());
    setups << "time,pattern,symbol,tf,side,order,status,agent,entry_time,entry,stop,target,exit_time,exit,r_net,"
              "exit_reason,trend,session,vol,vwap,flow\n";
    std::map<std::string, std::vector<double>> closed_r;
    std::map<std::string, int> open_n, setup_n, new_by_pattern;
    for (std::size_t i = 0; i < cases.size(); ++i) {
        const Case& c = cases[i];
        const Setup& s = c.setup;
        if (s.t < live_from) continue;
        const auto lt = last_t.find(s.symbol);
        const std::string st = status_of(c, lt == last_t.end() ? 0 : lt->second);
        const std::string key = case_key(s);
        const bool by_agent = taken.count(i) != 0;
        ++rep.setups;
        ++setup_n[s.pattern];

        KvRecord r;
        r.set("pattern", s.pattern).set("symbol", s.symbol).set("tf", static_cast<long>(s.tf));
        r.set("side", static_cast<long>(s.side)).set("time", format_minutes(s.t)).set("order", std::string(1, s.order));
        r.set("entry", s.entry).set("stop", s.stop).set("target", s.target).set("trend", s.ctx.trend);
        if (emit("setup", key, r, nullptr)) ++new_by_pattern[s.pattern];
        if (c.filled) {
            KvRecord f;
            f.set("entry_time", format_minutes(c.entry_t)).set("entry", c.entry_px).set("stop", c.stop_px);
            emit("fill", key, f, "entry");
            if (by_agent) {
                KvRecord t;
                t.set("entry_time", format_minutes(c.entry_t)).set("entry", c.entry_px);
                if (emit("agent_trade", key, t, "entry")) {
                    digest << "AGENT OPENS " << s.symbol << ' ' << s.pattern << ' ' << (s.side > 0 ? "long" : "short")
                           << " at " << c.entry_px << ", stop " << c.stop_px << ", target " << c.target_px << '\n';
                }
            }
        }
        if (st == "closed") {
            ++rep.closed_setups;
            closed_r[s.pattern].push_back(c.r_net);
            KvRecord x;
            x.set("exit_time", format_minutes(c.exit_t)).set("exit", c.exit_px).set("r_net", c.r_net);
            x.set("reason", c.exit_reason);
            emit("exit", key, x, "r_net");
        } else if (st == "open") {
            ++rep.open_setups;
            ++open_n[s.pattern];
        }
        setups << format_minutes(s.t) << ',' << s.pattern << ',' << s.symbol << ',' << s.tf << ',' << s.side << ','
               << s.order << ',' << st << ',' << (by_agent ? 1 : 0) << ','
               << (c.filled ? format_minutes(c.entry_t) : "") << ',' << c.entry_px << ',' << s.stop << ','
               << c.target_px << ',' << (c.filled ? format_minutes(c.exit_t) : "") << ',' << c.exit_px << ','
               << (c.filled ? c.r_net : 0) << ',' << c.exit_reason << ',' << s.ctx.trend << ',' << s.ctx.session << ','
               << s.ctx.vol << ',' << s.ctx.vwap << ',' << s.ctx.flow << '\n';
    }

    // Agent trades: closed ones are final, open ones are marked to the last price.
    std::ofstream trades((fs::path(dir) / "trades.csv").string());
    std::ofstream open((fs::path(dir) / "open.csv").string());
    trades << trades_csv_header() << '\n';
    open << "symbol,pattern,side,entry_time,entry,stop,target,last_price,unrealised_r\n";
    for (std::size_t k = 0; k < agent.trades.size(); ++k) {
        const Trade& t = agent.trades[k];
        const Case& c = cases[agent.case_of_trade[k]];
        if (c.exit_reason == "end_of_data") {
            ++rep.agent_open;
            rep.agent_open_r += t.r_multiple;
            open << t.symbol << ',' << t.sleeve << ',' << t.side << ',' << t.entry_date << ',' << t.entry_price << ','
                 << c.stop_px << ',' << c.target_px << ',' << c.exit_px << ',' << t.r_multiple << '\n';
            continue;
        }
        ++rep.agent_closed;
        rep.agent_pnl += t.pnl;
        trades << to_csv(t) << '\n';
        KvRecord x;
        x.set("exit_time", t.exit_date).set("pnl", t.pnl).set("r", t.r_multiple).set("reason", t.exit_reason);
        if (emit("agent_exit", case_key(c.setup), x, "pnl")) {
            digest << "AGENT CLOSES " << t.symbol << ' ' << t.sleeve << ' ' << t.exit_reason << ' ' << t.r_multiple
                   << "R, P&L " << t.pnl << '\n';
        }
    }

    std::ofstream sum((fs::path(dir) / "summary.txt").string());
    sum << std::fixed << std::setprecision(3) << "live since " << format_minutes(live_from) << " UTC; data to ";
    for (const auto& [s, t] : last_t) sum << s << ' ' << format_minutes(t) << "  ";
    sum << "\n\nevery setup (shadow log): pattern, setups, open, closed, mean net R of closed\n";
    for (const auto& [p, n] : setup_n) {
        const auto& v = closed_r[p];
        double m = 0;
        for (double x : v) m += x;
        sum << "  " << std::left << std::setw(13) << p << std::right << std::setw(5) << n << std::setw(5) << open_n[p]
            << std::setw(5) << v.size() << std::setw(9) << (v.empty() ? 0.0 : m / v.size()) << '\n';
    }
    sum << "\nagent (memory-gated): closed " << rep.agent_closed << ", P&L " << rep.agent_pnl << "; open "
        << rep.agent_open << ", unrealised " << rep.agent_open_r << "R\n"
        << "backtest reference: knowledge/concepts.md (every concept lost after costs in 2020-26)\n";
    int approved = 0;
    for (const auto& [k, st] : agent.memory_now) approved += st.approved ? 1 : 0;
    sum << "\nmemory now: " << agent.memory_now.size() << " situations with >= 30 finished cases, " << approved
        << " approved for trading (mean >= +0.05R and lower bound > 0)\n";
    std::vector<std::pair<double, std::string>> best;
    for (const auto& [k, st] : agent.memory_now) best.emplace_back(st.lower, k);
    std::sort(best.rbegin(), best.rend());
    for (std::size_t i = 0; i < best.size() && i < 8; ++i) {
        const auto& st = agent.memory_now.at(best[i].second);
        sum << "  " << (st.approved ? "APPROVED " : "         ") << std::left << std::setw(36) << best[i].second
            << std::right << " n " << std::setw(5) << st.n << "  mean " << std::setw(7) << st.mean << "R  lower "
            << std::setw(7) << st.lower << "R\n";
    }
    if (!new_by_pattern.empty()) {
        digest << "new setups:";
        for (const auto& [p, n] : new_by_pattern) digest << ' ' << p << ' ' << n;
        digest << '\n';
    }
    digest << "setups since start " << rep.setups << " (open " << rep.open_setups << ", closed " << rep.closed_setups
           << "); agent closed " << rep.agent_closed << " P&L " << rep.agent_pnl << ", open " << rep.agent_open << " ("
           << rep.agent_open_r << "R)";
    if (rep.revisions) digest << "; " << rep.revisions << " revision(s) journalled";
    rep.digest = digest.str();
    return rep;
}

}  // namespace ta
