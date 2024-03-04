#include "search.h"
#include <cassert>
#include <algorithm>

namespace jchess {
    SearchInfo Searcher::iterative_deepening_search(Board& board, int max_depth) {
        for(int depth=1; depth<=max_depth; ++depth) {
            Move iteration_best{"0000"};
            alpha_beta_search(depth, board, MIN_SCORE, MAX_SCORE, iteration_best, true);
            if(search_info.terminated) {
                // if there was no time to do a depth 1 search, would rather return a potentially
                // awful move than a null move.
                if(depth == 1) {
                    search_info.best_move = iteration_best;
                }
                return search_info; // we haven't updated the move from the last iteration
            }
            search_info.depth = depth;
            search_info.best_move = iteration_best;
        }
        return search_info;
    }

    std::ostream& operator<<(std::ostream& os, SearchInfo const& info) {
        os << "best: " << move_to_string(info.best_move) <<
            " nodes: " << info.num_nodes <<
            " time(micros): " << info.time_micros <<
            " depth: " << info.depth;
        return os;
    }

    SearchInfo Searcher::search(jchess::Board &board, SearchLimits const& limits) {
        using namespace std::chrono;

        search_info = {}; // clear search info

        node_limit = limits.max_nodes == 0 ? -1ull : limits.max_nodes;
        if(limits.max_time_ms > 0) {
            cutoff = Searcher::Clock::now() + milliseconds(limits.max_time_ms);
        }
        auto t1 = Searcher::Clock::now();
        iterative_deepening_search(board, limits.depth == 0 ? DEFAULT_MAX_DEPTH : limits.depth);
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

        uint64_t board_hash = std::hash<Board>{}(board);
        if(prev_pos_hashes.contains(board_hash)) {
            return DRAW_SCORE; // can get a stalemate through 3fold repetition
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

        prev_pos_hashes.insert(board_hash);
        for(const Move& move : moves) {
            board.make_move(move);
            Score score = -alpha_beta_search(depth - 1, board, -beta, -alpha, best_move);
            board.unmake_move();
            if(score >= beta) { // impossible if at root node
                prev_pos_hashes.erase(board_hash);
                return beta;
            }
            if(root && score > alpha) {
                best_move = move;
            }
            alpha = std::max(score, alpha);

            if(search_should_stop()) {
                search_info.terminated = true;
                prev_pos_hashes.erase(board_hash);
                return 0;
            }
        }
        prev_pos_hashes.erase(board_hash);

        return alpha;
    }

    // play all captures until none available - should this also be cancellable?
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