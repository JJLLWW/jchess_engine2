#pragma once

#include "eval.h"
#include "board.h"

#include <chrono>
#include <iostream>
#include <unordered_set>

namespace jchess {
    constexpr Score MIN_SCORE = -1000000;
    constexpr Score MAX_SCORE = 1000000;

    constexpr Score DRAW_SCORE = 0;
    constexpr int DEFAULT_MAX_DEPTH = 10;

    struct SearchLimits {
        long long max_time_ms = 0; // milliseconds
        uint64_t max_nodes = 0;
        int depth = 0;
    };

    struct SearchInfo {
        Move best_move {"0000"};
        uint64_t num_nodes = 0;
        uint64_t time_micros = 0;
        bool terminated = false;
        int depth = 0;
    };

    std::ostream& operator<<(std::ostream& os, SearchInfo const& info);

    int move_ordering_rank(Move const& move, Board const& board);

    // TODO: transposition tables, move ordering
    class Searcher {
    public:
        SearchInfo search(Board& board, SearchLimits const& limits);
    private:
        SearchInfo iterative_deepening_search(Board& board, int max_depth = DEFAULT_MAX_DEPTH);
        Score alpha_beta_search(int depth, Board& board, Score alpha, Score beta, Move& best_move, bool root = false);
        Score quiesence_search(Score alpha, Score beta, Board& board);
        bool search_should_stop();
    private:
        using Clock = std::chrono::system_clock;
        // time in the future where need to stop the search
        std::chrono::time_point<Clock> cutoff {Clock::now() + std::chrono::years(10)};
        uint64_t node_limit = -1ull;
        SearchInfo search_info {};
        std::unordered_set<uint64_t> prev_pos_hashes;
    };
}