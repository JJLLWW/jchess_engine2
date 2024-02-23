#include <catch2/catch_test_macros.hpp>

#include "jchess/bitboard.h"

using namespace jchess;

TEST_CASE("bitboard add/remove square") {
    Bitboard empty = 0;
    bb_add_square(empty, A8);
    bb_add_square(empty, H8);
    REQUIRE(empty == 0x8100000000000000);
}
