#pragma once

#include "core.h"

#include <cstdint>

namespace jchess {
    using Bitboard = uint64_t;
    void bb_add_square(Bitboard& bb, Square square);
    void bb_remove_square(Bitboard& bb, Square square);
    bool bb_get_square(Bitboard& bb, Square square);
}