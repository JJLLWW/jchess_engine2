#include <catch2/catch_test_macros.hpp>

#include "jchess/search.h"
#include "jchess/eval.h"
#include "jchess/core.h"

using namespace jchess;

TEST_CASE("doesn't crash / hang forever") {
    Board board{starting_fen};
    Searcher searcher;
    SearchLimits limits;
    limits.max_nodes = 30000;
    auto info = searcher.search(board, limits);
}

TEST_CASE("time limited search doesn't hang") {
    Board board{starting_fen};
    Searcher searcher;
    SearchLimits limits;
    limits.max_time_ms = 100; // 1/10 second
    // messed up move when using time limited search.
    auto info = searcher.search(board, limits);
}

TEST_CASE("finds mate") {
    // 2 mates in one in this positon, f6d8 or g6g7
    std::string mate_fen = "6k1/1pR5/p3pQ2/6pP/3pr3/PP6/5PP1/6K1 w - - 22 55";
    Board board{mate_fen};
    Searcher searcher;
    SearchLimits limits{ .max_nodes = 1000000};
    auto info = searcher.search(board, limits);
    REQUIRE(((move_to_string(info.best_move) == "f6d8") || (move_to_string(info.best_move) == "f6g7")));
}
