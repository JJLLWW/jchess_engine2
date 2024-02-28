#pragma once

#include "board.h"

namespace jchess {
    using Score = int;
    // just say eval scores are given in centipawns
    Score eval(Board const& board);
}