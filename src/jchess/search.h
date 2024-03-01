#pragma once

#include "eval.h"
#include "board.h"

#include <chrono>

namespace jchess {
    constexpr Score MIN_SCORE = -1000000;
    constexpr Score MAX_SCORE = 1000000;

    constexpr Score DRAW_SCORE = 0;

    struct SearchLimits {
        long long max_time_ms = 0;
        uint64_t max_nodes = 0;
    };

    // the best_move and best_score don't really belong here.
    struct SearchInfo {
        Move best_move {"0000"};
        uint64_t num_nodes = 0;
        uint64_t time_micros = 0;
        bool terminated = false;
        int depth = 0;
    };

    // transposition tables would be useful here

    class Searcher {
    public:
        SearchInfo search(Board& board, SearchLimits const& limits);
    private:
        SearchInfo iterative_deepening_search(Board& board);
        Score alpha_beta_search(int depth, Board& board, Score alpha, Score beta, Move& best_move, bool root = false);
        Score quiesence_search(Score alpha, Score beta, Board& board);
        bool search_should_stop();
    private:
        using Clock = std::chrono::system_clock;
        // time in the future where need to stop the search
        std::chrono::time_point<Clock> cutoff {Clock::now() + std::chrono::years(10)};
        uint64_t node_limit = -1ull;
        SearchInfo search_info {};
    };
}