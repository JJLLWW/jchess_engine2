#pragma once

#include "board.h"
#include "magic_bitboard.h"

#include <vector>

namespace jchess {
    namespace detail {
        Bitboard get_rectangle_between(Square sq1, Square sq2); // shouldn't this not be in the
        // public namespace at all
    }
    // all move generation functions require the sliding move database to be initialised so
    // need to be class members.
    // - vector of return values temporary, get it working before making it clever.
    class MoveGenerator {
    public:
        MoveGenerator();
        std::vector<Move> get_legal_moves(BoardState const& state, Color color);
        // higher level helpers
        Bitboard get_king_moves(Square source, BoardState const& state, Color color);
        Bitboard get_all_attacked_squares(BoardState const& state, Color color);
        void get_all_piece_moves(std::vector<Move>& moves, PieceType type, BoardState const& state, Color color);
        // these get_PIECE_moves functions are not pin aware, some of these moves may put the king in check.
        Bitboard get_slider_and_knight_moves(PieceType piece_type, Square source, Bitboard own_pieces, Bitboard enemy_pieces);
        Bitboard get_knight_moves(Square source, Bitboard own_pieces);
        Bitboard get_bishop_moves(Square source, Bitboard own_pieces, Bitboard enemy_pieces);
        Bitboard get_rook_moves(Square source, Bitboard own_pieces, Bitboard enemy_pieces);
        Bitboard get_queen_moves(Square source, Bitboard own_pieces, Bitboard enemy_pieces);
        // xray helpers - get all squares being attacked "through a piece"
        Bitboard xray_rook_moves(Bitboard all_pieces, Bitboard blockers, Square rook_sq);
        Bitboard xray_bishop_moves(Bitboard all_pieces, Bitboard blockers, Square bishop_sq);
        Bitboard xray_queen_moves(Bitboard all_pieces, Bitboard blockers, Square queen_sq);
        // internal detail
        void compute_pin_masks(BoardState const& state, Color color, Square king_sq);
    private:
        MagicDatabase magic_db;
        std::array<Bitboard, 64> king_attacks_tbl {};
        std::array<Bitboard, 64> knight_attacks_tbl {};
        std::array<Bitboard, 64> pawn_attacks_tbl[2] {}; // pawn_attacks_tbl[WHITE]
        // temporary
        void initialise_rectangle_between_tbl();
        Bitboard rectangle_between_tbl[64][64];
        std::array<Bitboard, 64> pin_masks;
    };
}