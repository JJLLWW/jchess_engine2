#include <catch2/catch_test_macros.hpp>

#include "jchess/core.h"
#include <unordered_map>

using namespace jchess;

TEST_CASE("Piece Helpers") {
    std::unordered_map<Piece, char> test_cases = {
        {B_KNIGHT, 'n'}, {W_KING, 'K'}, {B_BISHOP, 'b'}, {W_ROOK, 'R'}
    };
    REQUIRE(type_from_piece(B_KING) == KING);
    REQUIRE(color_from_piece(B_KING) == BLACK);
    for(const auto [piece, ch] : test_cases) {
        REQUIRE(char_from_piece(piece) == ch);
        REQUIRE(piece_from_char(ch) == piece);
    }
}

TEST_CASE("Square Helpers") {
    REQUIRE(is_corner_square(A1));
    REQUIRE(!is_corner_square(D6));
    REQUIRE(horizontal_distance(B2, E2) == 3);
    REQUIRE(vertical_distance(B2, B5) == 3);
    REQUIRE(square_from_rank_file(2, 1) == B3);
    REQUIRE(square_from_alg_not("d5") == D5);
}

TEST_CASE("Castle Bits") {
    REQUIRE(castle_bits_from_char('Q') == WHITE_QS);
}

TEST_CASE("Moves") {
    REQUIRE(Move("0000").is_null_move == true);
    Move move {"e4e6q"};
    REQUIRE(move.source == E4);
    REQUIRE(move.dest == E6);
    REQUIRE(move.promotion == B_QUEEN);
    REQUIRE(Move("e2e3").promotion == NO_PIECE);
}

TEST_CASE("FEN parsing") {
    FEN fen{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w Qk - 0 1"};
    REQUIRE(fen.enp_square.has_value() == false);
    REQUIRE(fen.pieces.size() == 32);
    REQUIRE(fen.castle_right_mask == (WHITE_QS | BLACK_KS));
}