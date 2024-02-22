#pragma once

#include "core.h"

#include <cstdint>
#include <array>

namespace jchess {
    using Bitboard = uint64_t;

    std::array<Bitboard, 64> precompute_king_attacks();
    std::array<Bitboard, 64> precompute_knight_attacks();
    std::array<Bitboard, 64> precompute_pawn_attacks(Color color);

    // all these potential attack squares are independent of the state of the board
    // apparently it is possible to precompute sliding piece lines with blockers using "magic" bitboards.
    // https://www.chessprogramming.org/Magic_Bitboards
    const std::array<Bitboard, 64> king_attacks = precompute_king_attacks();
    const std::array<Bitboard, 64> knight_attacks = precompute_knight_attacks();
    const std::array<Bitboard, 64> white_pawn_attacks = precompute_pawn_attacks(WHITE);
    const std::array<Bitboard, 64> black_pawn_attacks = precompute_pawn_attacks(BLACK);

    Bitboard get_king_attacks(Bitboard kingSet);

    void bb_add_square(Bitboard& bb, Square square);
    void bb_remove_square(Bitboard& bb, Square square);
    bool bb_get_square(Bitboard& bb, Square square);

    Bitboard bb_south_one (Bitboard b);
    Bitboard bb_north_one (Bitboard b);
    Bitboard bb_east_one (Bitboard b);
    Bitboard bb_neast_one (Bitboard b);
    Bitboard bb_seast_one (Bitboard b);
    Bitboard bb_west_one (Bitboard b);
    Bitboard bb_swest_one (Bitboard b);
    Bitboard bb_nwest_one (Bitboard b);
}