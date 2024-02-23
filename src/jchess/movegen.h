#pragma once

#include "board.h"

#include <vector>

namespace jchess {
    // which side are moves being generated for?
    std::vector<Move> generate_legal_moves(BoardState const& board, Color color);
    std::vector<Move> generate_pseudo_legal_moves(BoardState const& board, Color color);
}