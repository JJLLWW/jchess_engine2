//
// Created by Jack Wright on 21/02/2024.
//

#include "bitboard.h"

namespace jchess {
    void bb_add_square(Bitboard& bb, Square square) {
        bb |= (1 << square);
    }
    void bb_remove_square(Bitboard& bb, Square square) {
        bb &= ~(1 << square);
    }
    bool bb_get_square(Bitboard& bb, Square square) {
        return bb & (1 << square);
    }
}
