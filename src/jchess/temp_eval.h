#pragma once

namespace temp_eval {
    bool done_init = false;
    void init_tables();
    int eval(std::array<jchess::Piece, 64> const& board_jc, int side2move);
}