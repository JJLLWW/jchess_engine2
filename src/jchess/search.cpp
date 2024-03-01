#include "search.h"
#include <cassert>
#include <algorithm>

namespace jchess {
    SearchInfo Searcher::iterative_deepening_search(Board& board) {
        const int MAX_DEPTH = 10; // for now
        SearchInfo prev_info;
        for(int depth=1; depth<=MAX_DEPTH; ++depth) {
            Move iteration_best{"0000"};
            alpha_beta_search(depth, board, MIN_SCORE, MAX_SCORE, iteration_best, true);
            if(search_info.terminated) {
                return prev_info;
            }
            search_info.best_move = iteration_best;
        }
        return search_info.terminated ? search_info : prev_info;
    }

    SearchInfo Searcher::search(jchess::Board &board, SearchLimits const& limits) {
        using namespace std::chrono;

        search_info = {}; // clear search info

        node_limit = limits.max_nodes == 0 ? -1ull : limits.max_nodes;
        if(limits.max_time_ms > 0) {
            cutoff = Searcher::Clock::now() + milliseconds(limits.max_time_ms);
        }
        auto t1 = Searcher::Clock::now();
        // limits should eventually support an explicit ply depth where deepening doesn't make sense
        iterative_deepening_search(board);
        auto t2 = Searcher::Clock::now();

        auto elapsed = duration_cast<microseconds>(t2 - t1);
        search_info.time_micros = elapsed.count();
        return search_info;
    }

    Score Searcher::alpha_beta_search(int depth, Board &board, Score alpha, Score beta, Move& best_move, bool root) {
        assert(depth >= 0);
        if(depth == 0) {
            return quiesence_search(alpha, beta, board);
        }

        if(search_should_stop()) {
            search_info.terminated = true;
            return 0;
        }

        MoveVector moves;
        board.generate_legal_moves(moves);
        search_info.num_nodes += moves.size();

        if(moves.empty()) { // move will be set to a null move if called with a "finished" position
            if(board.in_check()) {
                return MIN_SCORE; // checkmate
            } else {
                return DRAW_SCORE; // stalemate
            }
        }

        for(const Move& move : moves) {
            board.make_move(move);
            Score score = -alpha_beta_search(depth - 1, board, -beta, -alpha, best_move);
            board.unmake_move();
            if(score >= beta) { // impossible if at root node
                return beta;
            }
            if(root && score > alpha) {
                best_move = move;
            }
            alpha = std::max(score, alpha);

            if(search_should_stop()) {
                search_info.terminated = true;
                return 0;
            }
        }
        return alpha;
    }

    // play all captures until none available
    Score Searcher::quiesence_search(Score alpha, Score beta, Board& board) {
        Score score = eval(board);
        if(score >= beta) {
            return beta;
        }
        alpha = std::max(alpha, score);

        MoveVector moves;
        board.generate_legal_moves(moves, GenPolicy::ONLY_CAPTURES);

        for(const Move& move : moves) {
            board.make_move(move);
            score = -quiesence_search(-beta, -alpha, board);
            board.unmake_move();

            if(score >= beta) {
                return beta;
            }
            alpha = std::max(alpha, score);
        }
        return alpha;
    }

    bool Searcher::search_should_stop() {
        return Searcher::Clock::now() > cutoff || search_info.num_nodes > node_limit;
    }
}