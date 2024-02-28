#pragma once

#include "board_state.h"
#include "magic_bitboard.h"

#include <boost/container/static_vector.hpp>

namespace jchess {
    namespace detail {
        constexpr int MAX_MOVES_IN_POS = 256;
    }
    using MoveVector = boost::container::static_vector<Move, detail::MAX_MOVES_IN_POS>;

    Bitboard get_all_attacked_squares(BoardState const& state, Color color); // BoardState method? / field?

    class MoveGenerator {
    public:
        MoveGenerator() = default;
        // user passes in the vector by non-const reference instead, maybe return number of legal moves
        void get_legal_moves(MoveVector& moves, BoardState const& state, Color color);
    private:
        // if these stay as members all except pin_info can be marked const
        void get_all_pawn_moves(MoveVector& moves, BoardState const& state, Color color); // pin aware
        Bitboard get_pawn_moves(Square square, BoardState const& state, Color color); // pin aware
        void get_king_non_castle_moves(MoveVector& moves, BoardState const& state, Color color); // why member?
        void compute_pin_info(BoardState const& state, Color color, Bitboard checker);
        void get_all_piece_moves(MoveVector& moves, PieceType type, BoardState const &state, Color color); // pin aware
    private:
        std::array<Bitboard, 64> pin_masks; // can I store this info in a more efficient way??
        // - for king in check: can store one bitboard for squares all pieces
        // are forced to go to, then handle the enp pawn case separately.
        // - for normal pins: can find which squares contain pinned pieces and only set the mask for those squares.
        // can then leave pin_masks uninitialised.
    };
}