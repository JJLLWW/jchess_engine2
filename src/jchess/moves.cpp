#include "moves.h"
#include "magic_bitboard.h"

namespace jchess {
    Bitboard get_bishop_moves(Square source, Bitboard own_pieces, Bitboard enemy_pieces) {
        Bitboard blockers = (own_pieces | enemy_pieces) & ~bb_from_square(source);
        Bitboard with_capture_own = get_bishop_attacks(source, blockers);
        return with_capture_own & ~own_pieces;
    }

    Bitboard get_rook_moves(Square source, Bitboard own_pieces, Bitboard enemy_pieces) {
        Bitboard blockers = (own_pieces | enemy_pieces) & ~bb_from_square(source);
        Bitboard with_capture_own = get_rook_attacks(source, blockers);
        return with_capture_own & ~own_pieces;
    }

    Bitboard get_knight_moves(Square source, Bitboard own_pieces) {
        return KNIGHT_ATTACKS[source] & ~own_pieces;
    }

    Bitboard get_queen_moves(Square source, Bitboard own_pieces, Bitboard enemy_pieces) {
        Bitboard bishop_moves = get_bishop_moves(source, own_pieces, enemy_pieces);
        Bitboard rook_moves = get_rook_moves(source, own_pieces, enemy_pieces);
        return bishop_moves | rook_moves;
    }

    Bitboard xray_rook_moves(Bitboard all_pieces, Bitboard blockers, Square rook_sq) {
        // https://www.chessprogramming.org/X-ray_Attacks_(Bitboards)#ModifyingOccupancy
        Bitboard attacks = get_rook_moves(rook_sq, 0ull, all_pieces);
        blockers &= attacks;
        return attacks ^ get_rook_moves(rook_sq, 0ull, all_pieces ^ blockers);
    }

    Bitboard xray_bishop_moves(Bitboard all_pieces, Bitboard blockers, Square bishop_sq) {
        Bitboard attacks = get_bishop_moves(bishop_sq, 0ull, all_pieces);
        blockers &= attacks;
        return attacks ^ get_bishop_moves(bishop_sq, 0ull, all_pieces ^ blockers);
    }

    Bitboard xray_queen_moves(Bitboard all_pieces, Bitboard blockers, Square queen_sq) {
        return xray_rook_moves(all_pieces, blockers, queen_sq) | xray_bishop_moves(all_pieces, blockers, queen_sq);
    }
}