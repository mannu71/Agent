#include <fstream>

#include "harness.hpp"
#include "ta/data.hpp"
#include "ta/journal.hpp"
#include "ta/kv.hpp"

TEST(kv_round_trip_with_escaping) {
    ta::KvRecord r;
    r.type = "pos";
    r.set("symbol", std::string("M&M")).set("note", std::string("a b=c|d%")).set("px", 0.1 + 0.2).set("qty", 42L);
    const auto back = ta::KvRecord::parse(r.encode());
    CHECK(back.type == "pos");
    CHECK(back.str("symbol") == "M&M");
    CHECK(back.str("note") == "a b=c|d%");
    CHECK(back.num("px") == 0.1 + 0.2);  // exact round trip
    CHECK(back.integer("qty") == 42);
    CHECK(back.num("missing", 7) == 7);
}

TEST(config_overrides_fields) {
    const auto dir = th::temp_dir("config");
    {
        std::ofstream f(dir + "/c.cfg");
        f << "# comment\nrisk.per_trade = 0.003\nname = test  # trailing\n";
    }
    const auto c = ta::load_config(dir + "/c.cfg");
    double risk = 0.004;
    std::string name;
    int untouched = 5;
    ta::config_set(c, "risk.per_trade", risk);
    ta::config_set(c, "name", name);
    ta::config_set(c, "nope", untouched);
    CHECK_NEAR(risk, 0.003, 1e-15);
    CHECK(name == "test");
    CHECK(untouched == 5);
}

TEST(sha256_known_vector) {
    CHECK(ta::sha256_hex("abc") == "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
}

TEST(journal_chain_detects_tampering) {
    const auto dir = th::temp_dir("journal");
    const std::string path = dir + "/j.log";
    {
        ta::Journal j(path);
        for (int k = 0; k < 3; ++k) {
            ta::KvRecord r;
            r.type = "signal";
            r.set("k", static_cast<long>(k));
            j.append(r);
        }
    }
    {
        ta::Journal j(path);  // reopen continues the chain
        ta::KvRecord r;
        r.type = "signal";
        r.set("k", 3L);
        j.append(r);
    }
    auto v = ta::Journal::verify(path);
    CHECK(v.ok && v.lines == 4);

    // Rewrite one payload in place.
    std::ifstream in(path);
    std::string all((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    in.close();
    const auto pos = all.find("k=1");
    all.replace(pos, 3, "k=9");
    std::ofstream(path) << all;
    v = ta::Journal::verify(path);
    CHECK(!v.ok && v.first_bad_line == 2);
}

TEST(bhavcopy_all_three_formats) {
    const auto dir = th::temp_dir("bhav");
    std::ofstream(dir + "/a_legacy.csv")
        << "SYMBOL,SERIES,OPEN,HIGH,LOW,CLOSE,LAST,PREVCLOSE,TOTTRDQTY,TOTTRDVAL,TIMESTAMP,TOTALTRADES,ISIN,\n"
           "INFY,EQ,100,110,95,105,105,99,1000,105000,01-JAN-2024,10,INE009A01021,\n"
           "XYZ,BE,10,11,9,10,10,10,50,500,01-JAN-2024,3,INE000000000,\n"
           "GSEC,GS,100,100,100,100,100,100,1,100,01-JAN-2024,1,IN0000000000,\n";
    std::ofstream(dir + "/b_udiff.csv")
        << "TradDt,BizDt,Sgmt,Src,FinInstrmTp,FinInstrmId,ISIN,TckrSymb,SctySrs,XpryDt,FininstrmActlXpryDt,"
           "StrkPric,OptnTp,FinInstrmNm,OpnPric,HghPric,LwPric,ClsPric,LastPric,PrvsClsgPric,UndrlygPric,"
           "SttlmPric,OpnIntrst,ChngInOpnIntrst,TtlTradgVol,TtlTrfVal,TtlNbOfTxsExctd,SsnId,NewBrdLotQty,"
           "Rmks,Rsvd1,Rsvd2,Rsvd3,Rsvd4\n"
           "2024-07-08,2024-07-08,CM,NSE,STK,1594,INE009A01021,INFY,EQ,,,,,Infosys,106,112,104,111,111,105,,111,,,"
           "2000,222000,20,F1,1,,,,,\n";
    std::ofstream(dir + "/c_full.csv")
        << "SYMBOL, SERIES, DATE1, PREV_CLOSE, OPEN_PRICE, HIGH_PRICE, LOW_PRICE, LAST_PRICE, CLOSE_PRICE, "
           "AVG_PRICE, TTL_TRD_QNTY, TURNOVER_LACS, NO_OF_TRADES, DELIV_QTY, DELIV_PER\n"
           "INFY, EQ, 09-Jul-2024, 111, 111, 115, 110, 114, 114, 113, 3000, 3.4, 30, 1500, 50.0\n";

    const auto r = ta::ingest_bhavcopy_dir(dir, {"EQ", "BE", "BZ"});
    CHECK(r.files == 3);
    CHECK(r.universe.count("INFY") && r.universe.at("INFY").size() == 3);
    CHECK(!r.universe.count("GSEC"));
    const auto& infy = r.universe.at("INFY");
    CHECK(infy[0].date == "2024-01-01" && infy[1].date == "2024-07-08" && infy[2].date == "2024-07-09");
    CHECK_NEAR(infy[1].close, 111, 1e-12);
    CHECK_NEAR(infy[2].volume, 3000, 1e-12);
    CHECK(r.non_eq_days.size() == 1 && r.non_eq_days[0].first == "XYZ");
}

TEST(corporate_action_adjusts_history_only) {
    ta::Universe u;
    u["ABC"] = {{"2024-01-01", 200, 210, 190, 200, 100}, {"2024-01-02", 100, 105, 95, 100, 200}};
    ta::apply_corporate_actions(u, {{"ABC", "2024-01-02", 0.5}});
    CHECK_NEAR(u["ABC"][0].close, 100, 1e-12);
    CHECK_NEAR(u["ABC"][0].volume, 200, 1e-12);
    CHECK_NEAR(u["ABC"][1].close, 100, 1e-12);
}

TEST(exclusion_list_always_and_dated) {
    const auto dir = th::temp_dir("excl");
    std::ofstream(dir + "/x.csv") << "# asm list\nAAA\nBBB,2024-03-01\n";
    const auto x = ta::ExclusionList::load(dir + "/x.csv");
    CHECK(x.excluded("AAA", "2020-01-01"));
    CHECK(x.excluded("BBB", "2024-03-01"));
    CHECK(!x.excluded("BBB", "2024-03-02"));
    CHECK(!x.excluded("CCC", "2024-03-01"));
}

TEST(intraday_day_index) {
    ta::Series s = {{"2024-01-01 09:15", 1, 1, 1, 1, 1},
                    {"2024-01-01 09:20", 1, 1, 1, 2, 1},
                    {"2024-01-02 09:15", 1, 1, 1, 3, 1}};
    const auto idx = ta::index_by_day(s);
    CHECK(idx.size() == 2);
    CHECK(idx.at("2024-01-01").first == 0 && idx.at("2024-01-01").second == 2);
    CHECK(ta::time_of(s[1].date) == "09:20");
    CHECK_NEAR(ta::daily_closes(s).at("2024-01-01"), 2, 1e-12);
}
