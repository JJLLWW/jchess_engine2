#pragma once

#include "core.h"
#include "bitboard.h"

namespace jchess {
    // experimental
    struct Board {
        Board(FEN const& fen) { set_position(fen); }
        Board() : Board(FEN(starting_fen)) {}
        void set_position(FEN const& fen);
        void make_move(Move const& move);
        void unmake_move();
        std::string to_string();
        // BOARD STATE - maybe something for 50 move rule?
        Color side_to_move = WHITE;
        int castle_right_mask = WHITE_QS | WHITE_KS | BLACK_QS | BLACK_KS;
        int half_moves = 0;
        int full_moves = 0;
        std::optional<Square> enp_square;
        std::array<Piece, 64> pieces; // does this make sense or should it be one list per piece type?
        std::array<Bitboard, 12> piece_bbs; // one for white pawns, black kings etc.
        std::array<Bitboard, 2> color_bbs; // all white and black pieces
        Bitboard all_pieces_bb;
    private:
        std::stack<Move> moves;
    };
}