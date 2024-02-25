#include <catch2/catch_test_macros.hpp>

#include "jchess/bitboard.h"

using namespace jchess;

TEST_CASE("bitboard add/remove square") {
    Bitboard empty = 0;
    bb_add_square(empty, A8);
    bb_add_square(empty, H8);
    REQUIRE(empty == 0x8100000000000000);
}

TEST_CASE("bitscan") {
    Bitboard bb = 0xF00000000000000F;
    int msb_idx = bit_scan(bb, true);
    int lsb_idx = bit_scan(bb, false);
    REQUIRE(lsb_idx == 0);
    REQUIRE(msb_idx == 63);
}

TEST_CASE("rank, file and diag primitives") {
    REQUIRE(bb_from_squares({A1, A2, A3, A4, A5, A6, A7, A8}) == FILE_BBS[A]);
    REQUIRE(bb_from_squares({A1, B1, C1, D1, E1, F1, G1, H1}) == RANK_BBS[RANK_1]);
    REQUIRE(bb_from_squares({A1, B2, C3, D4, E5, F6, G7, H8}) == DIAG_BBS[diagonal_from_square(C3)]);
    REQUIRE(bb_from_squares({A8, B7, C6, D5, E4, F3, G2, H1}) == ANTI_DIAG_BBS[antidiag_from_square(D5)]);
}

TEST_CASE("ray primitives") {
    REQUIRE(bb_from_squares({B5, C6, D7, E8}) == RAY_BBS[NEAST][A4]);
}

TEST_CASE("subsets of mask") {
    Bitboard mask = 0xFFF; // 12 set bits
    auto subsets = get_subsets_of_mask(mask);
    REQUIRE(subsets.size() == (1 << 12));
    Bitboard mask2 = 0xF;
    subsets = get_subsets_of_mask(mask2);
    for(auto subset : subsets) {
        REQUIRE((subset & ~mask) == 0);
    }
}

TEST_CASE("pawn move generation") {
    auto w_pawns = compute_all_pawn_attacks(WHITE);
    auto b_pawns = compute_all_pawn_attacks(BLACK);
    REQUIRE(w_pawns[D3] == bb_from_squares({C4, E4}));
    REQUIRE(w_pawns[A2] == bb_from_squares({B3}));
    REQUIRE(w_pawns[A8] == 0ull);
    REQUIRE(b_pawns[D3] == bb_from_squares({C2, E2}));
    REQUIRE(b_pawns[H2] == bb_from_squares({G1}));
    REQUIRE(b_pawns[A1] == 0ull);
}

TEST_CASE("knight move generation") {
    auto knight = compute_all_knight_attacks();
    REQUIRE(knight[A1] == bb_from_squares({B3, C2}));
    REQUIRE(knight[C3] == bb_from_squares({B1, A2, A4, B5, D5, E4, E2, D1}));
}

TEST_CASE("king move generation") {
    auto king = compute_all_king_attacks();
    REQUIRE(king[A1] == bb_from_squares({B1, B2, A2}));
    REQUIRE(king[C3] == bb_from_squares({B3, B2, C2, D2, D3, D4, C4, B4}));
}

TEST_CASE("ray moves with blockers") {
    Bitboard blockers = 0ull;
    bb_add_square(blockers, D4);
    bb_add_square(blockers, C3);
    Bitboard rays = get_ray_attacks(blockers, SWEST, G7);
    REQUIRE(rays == bb_from_squares({D4, E5, F6}));
}

TEST_CASE("castle right bitboards") {
    REQUIRE(WHITE_QS_BB == bb_from_squares({A1, B1, C1, D1, E1}));
    REQUIRE(BLACK_QS_BB == bb_from_squares({A8, B8, C8, D8, E8}));
    REQUIRE(WHITE_KS_BB == bb_from_squares({E1, F1, G1, H1}));
    REQUIRE(BLACK_KS_BB == bb_from_squares({E8, F8, G8, H8}));
}