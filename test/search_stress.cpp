#include "jchess/search.h"
#include "jchess/nnue/wrap_nnue.h"

#include <iostream>
#include <chrono>

using namespace jchess;
using namespace std::chrono;

int main() {
    SearchLimits limits { .depth = 7 };
    Board board{starting_fen};
    Searcher searcher;
    std::cout << "starting long search..." << std::endl;
    auto t1 = high_resolution_clock::now();
    searcher.search(board, limits);
    auto t2 = high_resolution_clock::now();
    auto elapsed = duration_cast<milliseconds>(t2 - t1);
    std::cout << "search finished in " << elapsed.count() << "ms" << std::endl;

    std::cout << "starting long nnue search..." << std::endl;
    searcher.enable_nnue_eval(std::make_unique<nnue_eval::NNUEEvaluator>("../data/nn-04a843f8932e.nnue"));
    auto t3 = high_resolution_clock::now();
    limits.depth = 6; // bit sluggish
    searcher.search(board, limits);
    auto t4 = high_resolution_clock::now();
    auto elapsed2 = duration_cast<milliseconds>(t4 - t3);
    std::cout << "search finished in " << elapsed2.count() << "ms" << std::endl;
}