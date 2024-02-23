#include "movegen.h"

namespace jchess {
    // might be very painful to implement.
    std::vector<Move> generate_legal_moves(BoardState const& board, Color color) {
        throw std::logic_error("NOT IMPLEMENTED YET");
        return {};
    }

    // not sure if this will actually be used but easier to think about.
    std::vector<Move> generate_pseudo_legal_moves(BoardState const& board, Color color) {
        std::vector<Move> moves;
        // king moves
        // knight attacks.
        // pawn attacks. (enp nasty)
        return moves;
    }
}