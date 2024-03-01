#include <catch2/catch_test_macros.hpp>

#include "jchess/search.h"
#include "jchess/eval.h"
#include "jchess/core.h"

using namespace jchess;

TEST_CASE("doesn't crash / hang forever") {
    Board board{starting_fen};
    Searcher searcher;
    SearchLimits limits;
    limits.max_nodes = 3000000;
    auto info = searcher.search(board, limits);
    int i = 2;
}

TEST_CASE("time limited search doesn't hang") {
    Board board{starting_fen};
    Searcher searcher;
    SearchLimits limits;
    limits.max_time_ms = 1000; // 1 second
    // messed up move when using time limited search.
    auto info = searcher.search(board, limits);
    int i = 2;
}
