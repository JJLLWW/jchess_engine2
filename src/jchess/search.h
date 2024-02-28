#pragma once

#include "eval.h"
#include "board.h"

#include <chrono>

namespace jchess {
    constexpr Score MIN_SCORE = -1000000;
    constexpr Score MAX_SCORE = 1000000;

    constexpr Score DRAW_SCORE = 0;

    // TEMPORARY:
    Score quiesence_search(Score alpha, Score beta, Board& board);

    struct SearchInfo {
        Score best_score = MIN_SCORE;
        Move best_move {"0000"};
        uint64_t num_nodes = 0;
        uint64_t time_micros = 0;
    };

    // enough elaborate state to justify a class
    class Searcher {
    public:
        SearchInfo search(int depth, Board& board);
    private:
        // iterative deepening?
        Score alpha_beta_search(int depth, Board& board, Score alpha, Score beta, bool root = false);
    private:
        using Clock = std::chrono::system_clock;
        // time in the future where need to stop the search
        std::chrono::time_point<Clock> cutoff {Clock::now() + std::chrono::years(10)};
        SearchInfo search_info {};
    };
}