#include "search.h"
#include "uci.h"

#include <cassert>
#include <algorithm>
#include <sstream>
#include <spdlog/spdlog.h>

#include <mutex>

namespace jchess {
    SearchInfo Searcher::iterative_deepening_search(Board& board, int max_depth, MoveVector const& root_restrict_moves) {
        using namespace std::chrono;
        for(int depth=1; depth<=max_depth; ++depth) {
            Move iteration_best{"0000"};
            search_info.num_nodes = 0;
            auto t1 = Searcher::Clock::now();
            Score score = alpha_beta_search(depth, board, MIN_SCORE, MAX_SCORE, iteration_best, true, root_restrict_moves);
            auto t2 = Searcher::Clock::now();
            search_info.time_micros = duration_cast<microseconds>(t2 - t1).count();
            if(score != MIN_SCORE && score != MAX_SCORE) {
                search_info.score = score;
            }
            // if we already have mate, there's no point in continuing the search
            if(score == MAX_SCORE) {
                search_info.best_move = iteration_best;
                return search_info;
            }
            if(search_info.terminated) {
                // if there was no time to even do a depth 1 search, would rather return a potentially
                // awful move than a null move.
                if(depth == 1) {
                    search_info.best_move = iteration_best;
                }
                return search_info;
            }
            search_info.depth = depth;
            search_info.best_move = iteration_best;
            send_uci_info();
        }
        return search_info;
    }

    void Searcher::send_uci_info() {
        uint64_t nps = search_info.num_nodes * 1'000'000 / search_info.time_micros;
        std::ostringstream oss;
        oss << "info ";
        oss << "depth " << search_info.depth << " ";
        if(search_info.mate_depth.has_value()) {
            oss << "score mate " << search_info.mate_depth.value() << " ";
        } else {
            oss << "score cp " << search_info.score << " ";
        }
        oss << "nodes " << search_info.num_nodes << " ";
        oss << "nps " << nps << " ";
        oss << "time " << std::max(search_info.time_micros / 1000, (uint64_t)1ull) << " ";
        if(!search_info.pv.empty()) {
            oss << "multipv 1 pv";
            for (const auto &move: search_info.pv) {
                oss << " " << move_to_string(move);
            }
        } else {
            oss << "string iteration bestmove " << move_to_string(search_info.best_move);
        }
        thread_safe_line_out(oss.str());
    }

    std::ostream& operator<<(std::ostream& os, SearchLimits const& limits) {
        os << "max_time: " << limits.max_time_ms <<
            " max_nodes: " << limits.max_nodes <<
            " depth: " << limits.depth <<
            " infinite: " << limits.infinite <<
            " ponder: " << limits.ponder;
        return os;
    }

    std::ostream& operator<<(std::ostream& os, SearchInfo const& info) {
        os << "best: " << move_to_string(info.best_move) <<
            " nodes: " << info.num_nodes <<
            " time(micros): " << info.time_micros <<
            " depth: " << info.depth;
        return os;
    }

    int move_ordering_rank(const Move &move, Board const& board) {
        // checks > "good" captures > quiet > "bad" captures
        auto state = board.get_board_state();
        auto color = board.get_side_to_move();
        PieceType src_type = type_from_piece(state.pieces[move.source]);
        int check_weight = 0, capture_weight = 0;
        // checks
        if(is_attack(move.dest, state.king_sq[!color], src_type, color, state)) {
            check_weight = 2;
        }
        // captures
        if(state.pieces[move.dest] != NO_PIECE) {
            PieceType dest_type = type_from_piece(state.pieces[move.dest]);
            //  PAWN, ROOK, KNIGHT, BISHOP, KING, QUEEN
            int type_ranks[6] = {1, 3, 2, 2, 5, 4}; // higher means better piece
            capture_weight = type_ranks[dest_type] - type_ranks[src_type]; // [-4, 4] range
        }
        return check_weight + capture_weight;
    }

    SearchInfo Searcher::search(jchess::Board &board, SearchLimits const& limits) {
        using namespace std::chrono;

        search_info = {}; // clear search info

        node_limit = limits.max_nodes == 0 ? -1ull : limits.max_nodes;
        if(limits.max_time_ms > 0) {
            cutoff = Searcher::Clock::now() + milliseconds(limits.max_time_ms);
        }

        const int default_depth = limits.infinite ? 100000 : DEFAULT_MAX_DEPTH;
        auto t1 = Searcher::Clock::now();
        iterative_deepening_search(board, limits.depth == 0 ? default_depth : limits.depth, limits.search_moves);
        auto t2 = Searcher::Clock::now();

        auto elapsed = duration_cast<microseconds>(t2 - t1);
        search_info.time_micros = elapsed.count();
        return search_info;
    }

    void Searcher::stop_mt_search() {
        std::lock_guard lk{mut};
        search_done = true;
        search_cancelled = true;
        cv.notify_all();
    }

    void Searcher::ponderhit() {
        search_done = true;
        cv.notify_all();
    }

    void Searcher::search_mt(Board& board, SearchLimits limits) {
        search_done = false;
        search_cancelled = false;
        auto info = search(board, limits);
        std::ostringstream oss;
        oss << info;
        spdlog::debug("Search Info: {0}", oss.str());
        // in an infinite/ponder search we don't send the bestmove until the client requests it.
        if(limits.infinite || limits.ponder) {
            std::unique_lock lk{mut};
            cv.wait(lk, [this] { return search_done; });
        }
        thread_safe_line_out(std::string("bestmove ") + move_to_string(info.best_move));
    }

    Score Searcher::alpha_beta_search(int depth, Board &board, Score alpha, Score beta, Move& best_move, bool root, MoveVector const& root_restrict_moves) {
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
        if(root && !root_restrict_moves.empty()) {
            moves = root_restrict_moves;
        } else {
            board.generate_legal_moves(moves);
        }
        search_info.num_nodes += moves.size(); // doesn't make sense to do this here

        if(moves.empty()) {
            if(board.in_check()) {
                return MIN_SCORE; // checkmate
            } else {
                return DRAW_SCORE; // stalemate
            }
        }

        std::sort(moves.begin(), moves.end(), [&board](Move lhs, Move rhs){
            return move_ordering_rank(lhs, board) > move_ordering_rank(rhs, board);
        });
        prev_pos_hashes.insert(board_hash);
        for(const Move& move : moves) {
            board.make_move(move);
            Score score = -alpha_beta_search(depth - 1, board, -beta, -alpha, best_move);
            board.unmake_move();
            if(score >= beta) {
                // if we are at the root this is only possible if we have mate
                if (root) {
                    best_move = move;
                }
                prev_pos_hashes.erase(board_hash);
                return beta;
            }
            // edge case where if search deep enough in a mate in N position all moves will
            // have a checkmate score, so just return one of them.
            if(root && score == alpha && alpha == MIN_SCORE) {
                best_move = move;
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
    
    Score Searcher::quiesence_search(Score alpha, Score beta, Board& board) {
        if(search_should_stop()) {
            search_info.terminated = true;
            return 0;
        }
        Score score = nnue_eval ? nnue_eval->nnue_eval_board(board) : eval(board);
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

            if(search_should_stop()) {
                search_info.terminated = true;
                return 0;
            }
        }
        return alpha;
    }

    bool Searcher::search_should_stop() {
        return Searcher::Clock::now() > cutoff || search_info.num_nodes > node_limit || search_cancelled;
    }

    void Searcher::enable_nnue_eval(std::unique_ptr<nnue_eval::NNUEEvaluator>&& eval) {
        nnue_eval = std::move(eval);
    }
}
