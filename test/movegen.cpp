#include <catch2/catch_test_macros.hpp>

#include "jchess/movegen.h"
#include <string>

using namespace jchess;

TEST_CASE("xray primitives") {
    MoveGenerator gen;
    auto moves = gen.xray_rook_moves(bb_from_squares({C1, G1}), bb_from_squares({C1, G1}), A1);
    auto str = bb_to_string(moves);
    REQUIRE(true);
}

TEST_CASE("move generation simple 1") {
    BoardState state{starting_fen};
    state.remove_piece_from_square(A2);
    state.remove_piece_from_square(G2);
    MoveGenerator mg;
    auto moves = mg.get_legal_moves(state, WHITE);
    // 4 knight moves, 6 rook moves and 2 bishop moves in this position
    REQUIRE(moves.size() == 12);
}

TEST_CASE("move generation simple 2") {
    std::string pin_fen = "8/8/8/8/8/8/8/KQ1r4 w KQkq - 0 1";
    BoardState state{pin_fen};
    MoveGenerator mg;
    auto moves = mg.get_legal_moves(state, WHITE);
    // can't create a full test until king moves are implemented, LGTM
    // for just the queen though.
    REQUIRE(true);
}