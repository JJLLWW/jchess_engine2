#pragma once

#include "core.h"
#include "bitboard.h"

namespace jchess {
    struct GameState {
        GameState() : GameState(FEN(starting_fen)) {}
        GameState(FEN const& fen);
        void increase_half_move();
        void decrease_half_move();
        int half_moves = 0;
        int full_moves = 0;
        Color side_to_move = WHITE;
    };

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
        Bitboard all_pieces_bb = 0;
    };

    BoardState get_state_after_move(BoardState const& current, Move const& move);
    std::optional<CastleBits> get_move_castle_type(BoardState const& state, Move const& move);

    class Board {
    public:
        Board() : Board(FEN(starting_fen)) {}
        Board(FEN const& fen) { set_position(fen); }
        void set_position(FEN const& fen);
        void make_move(Move const& move);
        bool unmake_move();
        std::vector<Move> get_legal_moves();
        std::string to_string();
    private:
        GameState game_state;
        BoardState board_state;
    private:
        std::stack<BoardState> prev_board_states;
    };
}