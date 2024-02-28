#pragma once

#include "core.h"
#include "bitboard.h"
#include "board_state.h"
#include "movegen.h"

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

    BoardState get_state_after_move(BoardState const& current, Move const& move);
    std::optional<CastleBits> get_move_castle_type(BoardState const& state, Move const& move);

    class Board {
    public:
        Board() : Board(FEN(starting_fen)) {}
        Board(FEN const& fen) { set_position(fen); }
        void set_position(FEN const& fen);
        void make_move(Move const& move);
        std::vector<Move> generate_legal_moves();
        bool unmake_move();
        std::string to_string();
    private:
        GameState game_state;
        BoardState board_state;
        MoveGenerator movegen;
    private:
        std::stack<BoardState> prev_board_states;
    };
}