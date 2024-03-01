#include <catch2/catch_test_macros.hpp>

#include "jchess/board.h"

using namespace jchess;

TEST_CASE("capture only movegen 1") {
    // https://www.chessprogramming.org/Perft_Results - position 2
    Board board{"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"};
    MoveVector moves;
    board.generate_legal_moves(moves, GenPolicy::ONLY_CAPTURES);
    REQUIRE(moves.size() == 8);
}

TEST_CASE("capture only movegen 2") {
    // https://www.chessprogramming.org/Perft_Results - position 3
    Board board{"8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1"};
    MoveVector moves;
    board.generate_legal_moves(moves, GenPolicy::ONLY_CAPTURES);
    REQUIRE(moves.size() == 1);
}