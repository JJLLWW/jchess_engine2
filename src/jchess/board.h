#pragma once

#include "core.h"
#include "bitboard.h"
#include "board_state.h"
#include "movegen.h"

#include <boost/container/static_vector.hpp>

namespace jchess {
    namespace detail {
        // https://chess.stackexchange.com/questions/4113/longest-chess-game-possible-maximum-moves
        constexpr int MAX_MOVES_IN_GAME = 6000;
    }
    using PrevStateStack = std::stack<BoardState, boost::container::static_vector<BoardState, detail::MAX_MOVES_IN_GAME>>;

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
        void generate_legal_moves(MoveVector& moves, GenPolicy policy = GenPolicy::LEGAL);
        bool unmake_move();
        std::string to_string();
        BoardState const& get_board_state() const { return board_state; }
        Color get_side_to_move() const { return game_state.side_to_move; }
        bool in_check() const { return board_state.in_check(game_state.side_to_move); }
        uint64_t zobrist_hash() const; // TEMPORARY
        bool can_enp_capture() const;
    private:
        GameState game_state;
        BoardState board_state;
        MoveGenerator movegen;
    private:
        PrevStateStack prev_board_states;
    };
}