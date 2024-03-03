#include <catch2/catch_test_macros.hpp>

#include "jchess/syzygy/sz_wrapper.h"

using namespace jchess;
using namespace jchess::syzgy;

TEST_CASE("basic wdl dtz 1") {
    std::string table_dir = "../tables";
    std::string endgame_fen = "4k3/2n5/8/8/8/8/2K2N2/8 w - - 0 1";
    SZEndgameTables tables{table_dir};
    Board board{endgame_fen};
    auto res = tables.probe_wdl_tables(board);
    // you can't checkmate with one knight each.
    REQUIRE((res.has_value() && res.value() == WDL::DRAW));
    auto res2 = tables.probe_dtz_tables(board);
    REQUIRE((res2.has_value() && res2.value().wdl == WDL::DRAW));
}