#pragma once

#include "board.h"

#include <vector>

// maybe this should actually be a class, a lot of attack bitboards wont change
// from move to move...
namespace jchess {
    std::vector<Move> generate_moves(Board const& board);
}