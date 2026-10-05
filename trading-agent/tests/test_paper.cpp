#include <filesystem>
#include <fstream>

#include "harness.hpp"
#include "ta/data.hpp"
#include "ta/journal.hpp"
#include "ta/paper.hpp"

namespace fs = std::filesystem;

namespace {

// Synthetic NSE-like and crypto data in ISO dates, written to disk like real inputs.
std::string make_data(const std::string& name) {
    const auto root = th::temp_dir(name);
    ta::Universe eq;
    for (int k = 0; k < 40; ++k) eq["S" + std::to_string(k)] = th::random_walk(300 + k, 700, 300 + 10 * k, 0.03, 1e6, true);
    ta::write_universe(root + "/nse", eq);
    ta::Universe cr = {{"BTC", th::random_walk(41, 700, 30000, 0.035, 1e9, true)},
                       {"ETH", th::random_walk(42, 700, 2000, 0.045, 1e9, true)}};
    ta::write_universe(root + "/crypto", cr);
    return root;
}

ta::Config data_settings(const std::string& root) {
    return {{"data.equity_dir", root + "/nse"}, {"data.crypto_dir", root + "/crypto"}};
}

std::string last_line(const std::string& path) {
    std::ifstream in(path);
    std::string line, last;
    while (std::getline(in, line)) {
        if (!line.empty()) last = line;
    }
    return last;
}

int count_lines(const std::string& path) {
    std::ifstream in(path);
    std::string line;
    int n = 0;
    while (std::getline(in, line)) n += !line.empty();
    return n;
}

}  // namespace

TEST(paper_incremental_runs_match_one_run) {
    const auto root = make_data("paper_inc");
    const std::string one = root + "/acct_one", many = root + "/acct_many";
    ta::PaperAccount::init(one, 1e7, th::iso_day(300), data_settings(root));
    ta::PaperAccount::init(many, 1e7, th::iso_day(300), data_settings(root));

    ta::PaperAccount(one).run();
    // Catch up in four separate processes' worth of runs.
    for (int stop : {400, 450, 600}) ta::PaperAccount(many).run(th::iso_day(stop));
    ta::PaperAccount(many).run();

    CHECK(fs::exists(one + "/equity.csv") && fs::exists(one + "/trades.csv"));
    CHECK(last_line(one + "/equity.csv") == last_line(many + "/equity.csv"));
    CHECK(count_lines(one + "/trades.csv") == count_lines(many + "/trades.csv"));
    CHECK(count_lines(one + "/trades.csv") > 1);
    CHECK(ta::Journal::verify(one + "/journal.log").ok);
    CHECK(ta::Journal::verify(many + "/journal.log").ok);
    // A second run with no new data processes nothing and changes nothing.
    const std::string before = last_line(one + "/equity.csv");
    const std::string rep = ta::PaperAccount(one).run();
    CHECK(rep.find("processed 0 day(s)") != std::string::npos);
    CHECK(last_line(one + "/equity.csv") == before);
}

TEST(paper_kill_switch_flattens_and_latches) {
    const auto root = make_data("paper_kill");
    const std::string acct = root + "/acct";
    ta::PaperAccount::init(acct, 1e7, th::iso_day(300), data_settings(root));
    ta::PaperAccount(acct).run(th::iso_day(500));
    const std::string out = ta::PaperAccount(acct).kill("test");
    CHECK(out.find("kill switch fired") != std::string::npos);
    const std::string status = ta::PaperAccount(acct).status();
    CHECK(status.find("breaker off") != std::string::npos);
    CHECK(status.find("positions 0") != std::string::npos);
    // Later runs open nothing while latched.
    const int trades_before = count_lines(acct + "/trades.csv");
    ta::PaperAccount(acct).run();
    CHECK(count_lines(acct + "/trades.csv") == trades_before);
    // Reset only with an explicit sleeve; the breaker comes back to normal.
    CHECK(ta::PaperAccount(acct).reset("book").find("reset book") != std::string::npos);
    CHECK(ta::PaperAccount(acct).status().find("breaker normal") != std::string::npos);
}

TEST(paper_journal_tamper_detected) {
    const auto root = make_data("paper_tamper");
    const std::string acct = root + "/acct";
    ta::PaperAccount::init(acct, 1e7, th::iso_day(300), data_settings(root));
    ta::PaperAccount(acct).run(th::iso_day(450));
    std::ifstream in(acct + "/journal.log");
    std::string all((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    in.close();
    const auto pos = all.find("fill ");
    CHECK(pos != std::string::npos);
    if (pos == std::string::npos) return;
    const auto q = all.find("qty=", pos);
    all.replace(q, 5, "qty=9");
    std::ofstream(acct + "/journal.log") << all;
    CHECK(!ta::Journal::verify(acct + "/journal.log").ok);
}

TEST(paper_rejects_bad_config) {
    const auto root = make_data("paper_badcfg");
    ta::Config s = data_settings(root);
    s["a1.risk_per_trade"] = "0.004";
    const std::string acct = root + "/acct";
    ta::PaperAccount::init(acct, 1e7, th::iso_day(300), s);
    std::ofstream(acct + "/account.cfg", std::ios::app) << "a1.risk_per_trad = 0.01\n";  // typo
    bool threw = false;
    try {
        ta::PaperAccount p(acct);
    } catch (const std::exception&) {
        threw = true;
    }
    CHECK(threw);
}
