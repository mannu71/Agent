// ta_paper: forward paper-trading account using the same engines as the backtests.

#include <iostream>

#include "args.hpp"
#include "ta/journal.hpp"
#include "ta/paper.hpp"

namespace {

const char* kUsage = R"(usage: ta_paper <command> <account_dir> [options]

  init <dir> --capital N --start YYYY-MM-DD [--set key=value ...]
        create an account; data paths and limit overrides go in --set
        (e.g. --set data.equity_dir=data/nse --set data.crypto_dir=data/crypto)
  run <dir> [--until YYYY-MM-DD]   process every new day of data, print positions and orders
  status <dir>                      show equity, risk states, positions, next buy-stops
  scorecard <dir> [--horizon 5] [--k 10]   screener rank IC, precision@k, disable rule
  kill <dir> --reason TEXT          flatten everything and latch all sleeves off
  reset <dir> --sleeve a1|a2|b|d1|book --confirm   human reset after reviewing a stop
  verify <dir>                      check the journal's hash chain

Fills are simulated against daily/intraday bars; no orders reach a broker.
)";

}  // namespace

int main(int argc, char** argv) {
    try {
        const cli::Args a(argc, argv, 1, {"confirm"});
        if (a.positional().size() < 2) {
            std::cerr << kUsage;
            return 2;
        }
        const std::string cmd = a.pos(0), dir = a.pos(1);
        if (cmd == "init") {
            ta::Config settings;
            for (const auto& kv : a.sets()) {
                const auto eq = kv.find('=');
                if (eq == std::string::npos) throw std::runtime_error("--set expects key=value: " + kv);
                settings[kv.substr(0, eq)] = kv.substr(eq + 1);
            }
            ta::PaperAccount::init(dir, a.num("capital", 0), a.str("start"), settings);
            std::cout << "created paper account in " << dir << "\nedit " << dir << "/account.cfg, then: ta_paper run "
                      << dir << "\n";
            return 0;
        }
        if (cmd == "verify") {
            const auto v = ta::Journal::verify(dir + "/journal.log");
            std::cout << (v.ok ? "journal OK, " : "journal TAMPERED at line " + std::to_string(v.first_bad_line) + ", ")
                      << v.lines << " line(s)\n";
            return v.ok ? 0 : 4;
        }
        ta::PaperAccount acct(dir);
        if (cmd == "run") std::cout << acct.run(a.str("until"));
        else if (cmd == "status") std::cout << acct.status();
        else if (cmd == "scorecard") std::cout << acct.scorecard(static_cast<std::size_t>(a.num("horizon", 5)),
                                                                 static_cast<std::size_t>(a.num("k", 10)));
        else if (cmd == "kill") std::cout << acct.kill(a.str("reason", "manual"));
        else if (cmd == "reset") {
            if (!a.has("confirm")) throw std::runtime_error("reset needs --confirm after a human review");
            std::cout << acct.reset(a.str("sleeve"));
        } else {
            std::cerr << kUsage;
            return 2;
        }
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << '\n';
        return 1;
    }
}
