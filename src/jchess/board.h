#pragma once

#include "core.h"
#include "bitboard.h"
#include "board_state.h"
#include "movegen.h"

namespace jchess {
    namespace detail {
        // https://chess.stackexchange.com/questions/4113/longest-chess-game-possible-maximum-moves
        constexpr int MAX_MOVES_IN_GAME = 6000;
    }

    // this type is only used in board's implementation, it does not need to apply STL algorithms to its
    // stacks, so the most minimal implementation is used (1.5x speedup over a std::stack with boost::static-vector)
    template <typename T>
    class MoveInfoStack {
    public:
        constexpr bool empty() {
            return pos == 0;
        }
        constexpr void push(T const& elem) {
            data[pos++] = elem;
        }

        constexpr const T& top() {
            assert(pos > 0);
            return data[pos-1];
        }

        constexpr void pop() {
            assert(pos > 0);
            --pos;
        }
    private:
        std::array<T, detail::MAX_MOVES_IN_GAME> data;
        int pos = 0;
    };

    using PrevStateStack = MoveInfoStack<BoardState>;

    struct GameState {
        GameState() : GameState(FEN(starting_fen)) {}
        GameState(FEN const& fen);
        int half_moves = 0;
        int full_moves = 0;
        Color side_to_move = WHITE;
        auto operator<=>(GameState const& other) const = default;
    };

    using PrevGameStateStack = MoveInfoStack<GameState>;

    GameState get_game_state_after_move(Board const& board, Move const& move);
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
        GameState const& get_game_state() const { return game_state; }
        BoardState const& get_board_state() const { return board_state; }
        Color get_side_to_move() const { return game_state.side_to_move; }
        bool in_check() const { return board_state.in_check(game_state.side_to_move); }
        bool is_50_move_draw() const { return game_state.half_moves >= 100; }
        int get_num_pieces() const;
        int get_num_pawns() const;
        bool can_enp_capture() const;
        friend bool operator==(Board const& lhs, Board const& rhs) {
            return lhs.game_state == rhs.game_state && lhs.board_state == rhs.board_state;
        }
    private:
        GameState game_state;
        BoardState board_state;
        MoveGenerator movegen;
    private:
        // TODO: should do incremental update instead, only store half moves and castling rights
        PrevStateStack prev_board_states;
        PrevGameStateStack prev_game_states;
    };

    class ZobristHasher {
    public:
        ZobristHasher(const uint64_t *hash_values, int piece_start, int castle_start, int enp_start, int turn_start)
            : hash_values{hash_values}, piece_start{piece_start}, castle_start{castle_start}, enp_start{enp_start}, turn_start{turn_start} {}
        uint64_t hash_board(Board const& board) const;
    private:
        virtual int get_piece_offset(Square square, Piece piece) const = 0;
        virtual int get_castle_offset(CastleBits castle_type) const = 0;
        virtual int get_enp_offset(Board const& board) const = 0;
        virtual bool should_use_turn_value(Color color) const = 0;
    protected:
        inline static int NO_OFFSET = -1;
    private:
        const uint64_t *hash_values;
        const int piece_start;
        const int castle_start;
        const int enp_start;
        const int turn_start;
    };

    class BoardZobristHasher final : public ZobristHasher {
    public:
        BoardZobristHasher();
    private:
        int get_piece_offset(Square square, Piece piece) const override;
        int get_castle_offset(CastleBits castle_type) const override;
        int get_enp_offset(Board const& board) const override;
        bool should_use_turn_value(Color color) const override;
    };
}

namespace std {
    template<>
    class std::hash<jchess::Board> {
    public:
        uint64_t operator()(const jchess::Board& board) const {
            return hasher.hash_board(board);
        }
    private:
        jchess::BoardZobristHasher hasher {};
    };
}