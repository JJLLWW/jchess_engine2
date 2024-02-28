#include "eval.h"
#include "temp_eval.h"

namespace jchess {
    Score eval(Board const& board) {
        Color to_move = board.get_side_to_move();
        BoardState const& state = board.get_board_state();
        if(!temp_eval::done_init) {
            temp_eval::init_tables();
            temp_eval::done_init = true;
        }
        Score score = temp_eval::eval(state.pieces, board.get_side_to_move());
        return score;
    }
}