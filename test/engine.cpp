#include <catch2/catch_test_macros.hpp>

#include "jchess/engine.h"

using namespace jchess;

//TEST_CASE("endgame tables activated") {
//    EngineConfig test_config {
//        .feature_flag_cfg_file = "../data/feature_flags.cfg",
//        .opening_book_file = "../data/baron30.bin",
//        .endgame_table_dir = "../tables",
//        .nnue_network_file = "../data/nn-04a843f8932e.nnue"
//    };
//    std::string endgame_fen {"4k3/2n5/8/8/8/8/2K2N2/8 w - - 0 1"};
//    Engine engine{test_config};
//    engine.handle_uci_position(UciPosition{.position=endgame_fen});
//    engine.handle_uci_go(UciGo{.movetime = 1000});
//    int i = 2;
//}