#include "jchess/search.h"
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
}