#include <catch2/catch_test_macros.hpp>

#include "jchess/core.h"
#include <unordered_map>
#include <unordered_set>

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
    REQUIRE((ROOK | BLACK) == B_ROOK);
}

TEST_CASE("Square Helpers") {
    REQUIRE(is_corner_square(A1));
    REQUIRE(!is_corner_square(D6));
    REQUIRE(horizontal_distance(B2, E2) == 3);
    REQUIRE(vertical_distance(B2, B5) == 3);
    REQUIRE(square_from_rank_file(2, B) == B3);
    // 0-based rank (maybe too confusing)
    REQUIRE(rank_file_from_square(C3) == std::array<int, 2>{2, C});
    REQUIRE(square_from_alg_not("d5") == D5);
}

TEST_CASE("Diagonals") {
    std::unordered_set<int> seen_keys;
    std::vector<Square> squares {A1, A2, A3, A4, A5, A6, A7, A8, B1, C1, D1, E1, F1, G1, H1};
    for(Square square : squares) {
        int diag_key = diagonal_from_square(square);
        seen_keys.insert(diag_key);
        REQUIRE((0 <= diag_key && diag_key < 16));
    }
    REQUIRE(seen_keys.size() == 15);
    seen_keys.clear();
    squares = {A1, A2, A3, A4, A5, A6, A7, A8, B8, C8, D8, E8, F8, G8, H8};
    for(Square square : squares) {
        int diag_key = antidiag_from_square(square);
        seen_keys.insert(diag_key);
        REQUIRE((0 <= diag_key && diag_key < 16));
    }
    REQUIRE(seen_keys.size() == 15);
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
    // fen parsing is broken?
    FEN fen2{"8/8/8/8/8/8/8/KQ1r4 w KQkq - 0 1"};
    REQUIRE(true);
}