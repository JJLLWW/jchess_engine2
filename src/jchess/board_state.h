#pragma once

#include "core.h"
#include <compare>

namespace jchess {
    struct BoardState {
        BoardState() : BoardState(FEN(starting_fen)) {}
        BoardState(FEN const& fen);
        BoardState(BoardState const& other) = default;
        BoardState& operator=(BoardState const& other) = default;
        void remove_piece_from_square(Square square);
        void place_piece_on_square(Piece piece, Square square);
        int castle_right_mask = WHITE_QS | WHITE_KS | BLACK_QS | BLACK_KS;
        std::optional<Square> enp_square;
        std::array<Piece, 64> pieces;
        std::array<Bitboard, 12> piece_bbs = {}; // one for white pawns, black kings etc.
        std::array<Bitboard, 2> color_bbs = {}; // all white and black pieces
        Bitboard orth_slider_bb[2] = {}; // white/black rooks and queens
        Bitboard diag_slider_bb[2] = {}; // white/black bishops and queens
        Square king_sq[2] = {}; // white/black king squares
        Bitboard all_pieces_bb = 0;
        bool in_check(Color color) const;
        bool operator==(const BoardState& other) const {
            return (piece_bbs == other.piece_bbs) && enp_square == other.enp_square;
        }
    };

    bool is_attack(Square src, Square dest, PieceType type, Color color, BoardState const& state);

    Bitboard get_attackers_of(Square square, BoardState const& state, Color color);

    bool can_castle(BoardState const& state, Color color, bool queen_side, Bitboard attacked);
}