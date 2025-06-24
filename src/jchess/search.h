#pragma once

#include "eval.h"
#include "board.h"
#include "nnue/wrap_nnue.h"

#include <condition_variable>
#include <chrono>
#include <iostream>
#include <unordered_set>
#include <memory>
#include <mutex>
#include <atomic>

namespace jchess {
    constexpr Score MIN_SCORE = -1000000;
    constexpr Score MAX_SCORE = 1000000;

    constexpr Score DRAW_SCORE = 0;
    constexpr int DEFAULT_MAX_DEPTH = 10;

    struct SearchLimits {
        long long max_time_ms = 0; // milliseconds
        uint64_t max_nodes = 0;
        int depth = 0;
        MoveVector search_moves;
        bool infinite = false;
        bool ponder = false;
    };

    std::ostream& operator<<(std::ostream& os, SearchLimits const& limits);

    struct SearchInfo {
        Move best_move {"0000"};
        MoveVector pv;
        Score score;
        std::optional<int> mate_depth;
        uint64_t num_nodes = 0;
        uint64_t time_micros = 0;
        bool terminated = false;
        int depth = 0;
    };

    std::ostream& operator<<(std::ostream& os, SearchInfo const& info);

    int move_ordering_rank(Move const& move, Board const& board);

    // TODO: transposition tables
    class Searcher {
    public:
        SearchInfo search(Board& board, SearchLimits const& limits);
        void search_mt(Board& board, SearchLimits limits);
        void enable_nnue_eval(std::unique_ptr<nnue_eval::NNUEEvaluator>&& nnue_eval);
        Score alpha_beta_search(int depth, Board& board, Score alpha, Score beta, Move& best_move, bool root = false, MoveVector const& root_restrict_moves = {});
        void stop_mt_search();
        void ponderhit();
    private:
        SearchInfo iterative_deepening_search(Board& board, int max_depth = DEFAULT_MAX_DEPTH, MoveVector const& root_restrict_moves = {});
        Score quiesence_search(Score alpha, Score beta, Board& board);
        bool search_should_stop();
        void send_uci_info();
    private:
        using Clock = std::chrono::system_clock;
        std::chrono::time_point<Clock> cutoff {Clock::now() + std::chrono::years(10)};
        uint64_t node_limit = -1ull;
        SearchInfo search_info {};
        std::unordered_set<uint64_t> prev_pos_hashes;
        std::unique_ptr<nnue_eval::NNUEEvaluator> nnue_eval = nullptr;
        // multithreaded search
        std::mutex mut;
        std::condition_variable cv;
        bool search_done = false;
        std::atomic<bool> search_cancelled = false;
    };
}
