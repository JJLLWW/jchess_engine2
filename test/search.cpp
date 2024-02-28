#include <catch2/catch_test_macros.hpp>

#include "jchess/search.h"
#include "jchess/eval.h"
#include "jchess/core.h"

using namespace jchess;

TEST_CASE("see if looks ok") {
    Board board{starting_fen};
    Searcher searcher;
    auto info = searcher.search(3, board);
    int i = 2;
}
