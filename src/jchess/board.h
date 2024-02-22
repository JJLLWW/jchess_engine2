#pragma once

#include "core.h"
#include "bitboard.h"

namespace jchess {
    // shouldn't these methods just be private methods of Board?
    struct GameState {
        GameState() = default;
        GameState(FEN const& fen);
        void increase_half_move();
        void decrease_half_move();
        int half_moves = 0;
        int full_moves = 0;
        Color side_to_move = WHITE;
    };

    class Board {
    public:
        Board(FEN const& fen) { set_position(fen); }
        Board() : Board(FEN(starting_fen)) {}
        void set_position(FEN const& fen);
        void make_move(Move const& move);
        bool unmake_move();
        std::string to_string();
    private:
        GameState game_state;
        int castle_right_mask = WHITE_QS | WHITE_KS | BLACK_QS | BLACK_KS;
        std::optional<Square> enp_square;
        std::array<Piece, 64> pieces;
        std::array<Bitboard, 12> piece_bbs; // one for white pawns, black kings etc.
        std::array<Bitboard, 2> color_bbs; // all white and black pieces
        Bitboard all_pieces_bb;
    private:
        void remove_piece_from_square(Square square);
        void place_piece_on_square(Piece piece, Square square);
        std::optional<CastleBits> get_move_castle_type(Move const& move);
        std::optional<Square> get_new_enp_square(Move const& move);
        bool is_enp_capture(Move const& move);
        void make_castle_move(CastleBits castle);
        void make_enp_move(Move const& move);
        void make_regular_move(Move const& move);
        void apply_unmove(UnMove const& unmove);
        UnMove get_regular_unmove(Move const& move);
        UnMove get_enp_unmove(Move const& move);
        std::stack<UnMove> unmoves;
    };
}