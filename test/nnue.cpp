#include <catch2/catch_test_macros.hpp>

#include "jchess/nnue/wrap_nnue.h"
#include "jchess/search.h"

using namespace jchess::nnue_eval;

TEST_CASE("sane evaluation fen") {
    std::string white_wins = "k7/8/8/8/8/4Q3/3QKQ2/8 w - - 0 1";
    std::string black_wins = "kq6/qq6/8/8/8/8/4K3/8 w - - 0 1";
    std::string netfile{"../data/nn-04a843f8932e.nnue"};
    NNUEEvaluator eval{netfile};
    int32_t score_w = eval.nnue_eval_fen(white_wins);
    int32_t score_b = eval.nnue_eval_fen(black_wins);
    int32_t score_bal = eval.nnue_eval_fen(jchess::starting_fen);
    REQUIRE(score_b < score_bal);
    REQUIRE(score_bal < score_w);
}

TEST_CASE("sane evaluation board") {
    std::string white_wins = "k7/8/8/8/8/4Q3/3QKQ2/8 w - - 0 1";
    std::string black_wins = "kq6/qq6/8/8/8/8/4K3/8 w - - 0 1";
    std::string netfile{"../data/nn-04a843f8932e.nnue"};
    NNUEEvaluator eval{netfile};
    int32_t score_w = eval.nnue_eval_board(jchess::Board{white_wins});
    int32_t score_b = eval.nnue_eval_board(jchess::Board{black_wins});
    int32_t score_bal = eval.nnue_eval_board(jchess::Board{jchess::starting_fen});
    REQUIRE(score_b < score_bal);
    REQUIRE(score_bal < score_w);
}

TEST_CASE("debug only") {
    using namespace jchess;
    std::string evil_fen = "r1bqkb1r/pppn1ppp/4p3/3n4/2BP1B2/2N1PN2/PP3PPP/R2QK2R b KQkq - 0 1";
    auto uniq = std::make_unique<NNUEEvaluator>("../data/nn-04a843f8932e.nnue");
    Searcher searcher;
    searcher.enable_nnue_eval(std::move(uniq));
    SearchLimits limits;
    limits.max_time_ms = 1000;
    Board board{evil_fen};
    searcher.search(board, limits);
    int i =2;
}