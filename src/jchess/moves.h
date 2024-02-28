#pragma once

#include "core.h"

namespace jchess {
    // (NOT PIN AWARE)
    Bitboard get_knight_moves(Square source, Bitboard own_pieces);
    Bitboard get_queen_moves(Square source, Bitboard own_pieces, Bitboard enemy_pieces);
    Bitboard get_bishop_moves(Square source, Bitboard own_pieces, Bitboard enemy_pieces);
    Bitboard get_rook_moves(Square source, Bitboard own_pieces, Bitboard enemy_pieces);

    // (NOT PIN AWARE)
    Bitboard xray_rook_moves(Bitboard all_pieces, Bitboard blockers, Square rook_sq);
    Bitboard xray_bishop_moves(Bitboard all_pieces, Bitboard blockers, Square bishop_sq);
    Bitboard xray_queen_moves(Bitboard all_pieces, Bitboard blockers, Square queen_sq);
}