#include "eval.h"
#include "temp_eval.h"

namespace jchess {
    namespace {
        constexpr int PAWN_VAL = 100;
        constexpr int BISHOP_VAL = 300;
        constexpr int KNIGHT_VAL = 300;
        constexpr int ROOK_VAL = 500;
        constexpr int QUEEN_VAL = 900;
    }
    Score eval(Board const& board) {
        Color color = board.get_side_to_move();
        BoardState const& state = board.get_board_state();
        // maybe do this in temp_eval so easier to get rid of later.
        if(!temp_eval::done_init) {
            temp_eval::init_tables();
            temp_eval::done_init = true;
        }

        // material
        int npawn = std::popcount(state.piece_bbs[PAWN | color]) - std::popcount(state.piece_bbs[PAWN | !color]);
        int nbishop = std::popcount(state.piece_bbs[BISHOP | color]) - std::popcount(state.piece_bbs[BISHOP | !color]);
        int nknight = std::popcount(state.piece_bbs[KNIGHT | color]) - std::popcount(state.piece_bbs[KNIGHT | !color]);
        int nrook = std::popcount(state.piece_bbs[ROOK | color]) - std::popcount(state.piece_bbs[ROOK | !color]);
        int nqueen = std::popcount(state.piece_bbs[QUEEN | color]) - std::popcount(state.piece_bbs[QUEEN | !color]);

        Score material = npawn * PAWN_VAL + nbishop * BISHOP_VAL + nknight * KNIGHT_VAL + nrook * ROOK_VAL + nqueen * QUEEN_VAL;

        // temp eval doesn't bother to consider material, it's positional only.
        Score piece_square = temp_eval::eval(state.pieces, color);
        return material + piece_square;
    }
}