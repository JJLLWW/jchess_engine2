#include <catch2/catch_test_macros.hpp>

#include "jchess/board.h"

using namespace jchess;

TEST_CASE("Board State FEN ctor") {
    BoardState starting{starting_fen};
    REQUIRE(starting.all_pieces_bb == 0xFFFF00000000FFFF);
    REQUIRE(starting.color_bbs[WHITE] == 0xFFFF);
    REQUIRE(starting.color_bbs[BLACK] == 0xFFFF000000000000);
    REQUIRE(starting.piece_bbs[B_PAWN] == 0x00FF000000000000);
}

TEST_CASE("Getting next move from Board State") {
    BoardState starting{starting_fen};
    BoardState pawn_dbl = get_state_after_move(starting, Move("e2e4"));
    REQUIRE((pawn_dbl.enp_square.has_value() && pawn_dbl.enp_square.value() == E3));
    BoardState qs_rook = get_state_after_move(starting, Move("a1a3"));
    REQUIRE((qs_rook.castle_right_mask & WHITE_QS) == 0);
}