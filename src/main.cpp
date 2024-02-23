#include "jchess/core.h"
#include "jchess/uci.h"
#include "jchess/board.h"

using namespace jchess;

int main() {
    Bitboard bb = get_rook_blocker_mask(A1);
    auto str = bb_to_string(bb);
    Bitboard bb2 = get_rook_blocker_mask(C4);
    auto str2 = bb_to_string(bb2);
    int i = 2;
}
