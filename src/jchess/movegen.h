#pragma once

#include "board_state.h"
#include "magic_bitboard.h"

#include <boost/container/static_vector.hpp>

namespace jchess {
    namespace detail {
        constexpr int MAX_MOVES_IN_POS = 256;
    }

    // static vector is 2x faster than vector but very difficult to debug
#ifndef MOVE_DEBUG
    using MoveVector = boost::container::static_vector<Move, detail::MAX_MOVES_IN_POS>;
#else
    using MoveVector = std::vector<Move>;
#endif

    Bitboard get_all_attacked_squares(BoardState const& state, Color color);

    enum class GenPolicy {
        LEGAL,
        ONLY_CAPTURES
    };

    class MoveGenerator {
    public:
        MoveGenerator() = default;
        void get_legal_moves(MoveVector& moves, BoardState const& state, Color color, GenPolicy policy = GenPolicy::LEGAL);
    private:
        void get_all_pawn_moves(MoveVector& moves, BoardState const& state, Color color); // pin aware
        Bitboard get_pawn_moves(Square square, BoardState const& state, Color color); // pin aware
        void get_king_non_castle_moves(MoveVector& moves, BoardState const& state, Color color);
        void compute_dest_masks(BoardState const& state, Color color, Bitboard checker, GenPolicy policy);
        void get_all_piece_moves(MoveVector& moves, PieceType type, BoardState const &state, Color color); // pin aware
    private:
        std::array<Bitboard, 64> allowed_dest_mask; // there should be a more efficient way to store this
    };
}