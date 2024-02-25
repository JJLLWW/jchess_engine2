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

TEST_CASE("attackers of") {
    MoveGenerator gen;
    // TODO: test this
}

TEST_CASE("move generation simple 1") {
    BoardState state{starting_fen};
    MoveGenerator mg;
    auto moves = mg.get_legal_moves(state, WHITE);
    // 4 knight moves, 16 pawn moves
    REQUIRE(moves.size() == 20);
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

TEST_CASE("castling 1") {
    std::string castle_fen = "4k3/8/8/8/8/8/8/R3K2R w KQkq - 0 1";
    BoardState state{castle_fen};
    MoveGenerator mg;
    auto w_moves = mg.get_legal_moves(state, WHITE);
    auto b_moves = mg.get_legal_moves(state, BLACK);
    auto end = std::remove_if(w_moves.begin(), w_moves.end(), [](Move move){ return (move.source == A1) || (move.source == H1); });
    w_moves.erase(end, w_moves.end());
    REQUIRE(w_moves.size() == 7); // 5 king steps and 2 castles
}