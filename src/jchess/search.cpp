#include "search.h"
#include <cassert>
#include <algorithm>

namespace jchess {
    SearchInfo Searcher::search(int depth, jchess::Board &board) {
        using clock = std::chrono::high_resolution_clock;
        using std::chrono::duration_cast;

        assert(depth >= 1);
        search_info = {}; // clear search info
        auto t1 = clock::now();

        alpha_beta_search(depth, board, MIN_SCORE, MAX_SCORE, true);

        auto t2 = clock::now();
        auto elapsed = duration_cast<std::chrono::microseconds>(t2 - t1);
        search_info.time_micros = elapsed.count();
        return search_info;
    }

    Score Searcher::alpha_beta_search(int depth, Board &board, Score alpha, Score beta, bool root) {
        assert(depth >= 0);
        if(depth == 0) {
            return eval(board); // TEMPORARY : recommended to do quiesence search here
        }

        if(Searcher::Clock::now() >= cutoff) {
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
            Score score = -alpha_beta_search(depth - 1, board, -beta, -alpha);
            board.unmake_move();
            if(score >= beta) { // impossible if at root node
                return beta;
            }
            if(root && score > alpha) {
                search_info.best_move = move;
                search_info.best_score = score;
            }
            alpha = std::max(score, alpha);

            if(Searcher::Clock::now() >= cutoff) {
                return 0;
            }
        }
        return alpha;
    }

    // play all captures until none avialable
    Score quiesence_search(Score alpha, Score beta, Board& board) {
        throw std::logic_error("NOT IMPLEMENTED YET");
        Score score = 666; // SHOULD EVAL HERE
        if(score >= beta) {
            return beta;
        }
        alpha = std::max(alpha, score);

        MoveVector moves;
        // TODO: generate captures only
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
}