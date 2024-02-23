#pragma once

#include "core.h"

#include <cstdint>
#include <array>

namespace jchess {
    using Bitboard = uint64_t;

    // all these potential attack squares are independent of the state of the board
    // apparently it is possible to precompute sliding piece lines with blockers using "magic" bitboards.
    // https://www.chessprogramming.org/Magic_Bitboards
    // - this might be too complicated for now
    std::array<Bitboard, 64> compute_all_king_attacks();
    std::array<Bitboard, 64> compute_all_knight_attacks();
    std::array<Bitboard, 64> compute_all_pawn_attacks(Color color);

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