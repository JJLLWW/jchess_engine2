#pragma once

#include "board_state.h"
#include "magic_bitboard.h"

#include <vector>

namespace jchess {
    // (NOT PIN AWARE)
    Bitboard get_knight_moves(Square source, Bitboard own_pieces);
    Bitboard get_bishop_moves(Square source, Bitboard own_pieces, Bitboard enemy_pieces);
    Bitboard get_rook_moves(Square source, Bitboard own_pieces, Bitboard enemy_pieces);
    Bitboard get_queen_moves(Square source, Bitboard own_pieces, Bitboard enemy_pieces);

    // (NOT PIN AWARE)
    Bitboard xray_rook_moves(Bitboard all_pieces, Bitboard blockers, Square rook_sq);
    Bitboard xray_bishop_moves(Bitboard all_pieces, Bitboard blockers, Square bishop_sq);
    Bitboard xray_queen_moves(Bitboard all_pieces, Bitboard blockers, Square queen_sq);

    Bitboard get_all_attacked_squares(BoardState const& state, Color color); // BoardState method? / field?
    Bitboard get_attackers_of(Square square, BoardState const& state, Color color); // BoardState method? / field?

    class MoveGenerator {
    public:
        MoveGenerator() = default;
        std::vector<Move> get_legal_moves(BoardState const& state, Color color);
    private:
        void get_all_pawn_moves(std::vector<Move>& moves, BoardState const& state, Color color); // pin aware
        Bitboard get_pawn_moves(Square square, BoardState const& state, Color color); // pin aware
        void get_king_non_castle_moves(std::vector<Move>& moves, BoardState const& state, Color color); // why member?
        void compute_pin_info(BoardState const& state, Color color, Bitboard checker);
        void get_all_piece_moves(std::vector<Move> &moves, PieceType type, BoardState const &state, Color color); // pin aware
    private:
        std::array<Bitboard, 64> pin_masks; // can I store this info in the board state + in a more efficient way??
    };
}