#include <random>
#include <limits>
#include <iostream>

// std::random can't be used at compile time, so unfortunately this script is necessary.

constexpr int NUM_ZOBRIST_CONSTANTS = 781;
constexpr uint64_t SEED = 294260281235ull;
constexpr uint64_t MIN = 0;
constexpr uint64_t MAX = std::numeric_limits<uint64_t>::max();

int main() {
    std::mt19937_64 eng;
    eng.seed(SEED);
    std::uniform_int_distribution<uint64_t> dist(MIN, MAX);

    for(int i=0; i<NUM_ZOBRIST_CONSTANTS; ++i) {
        std::cout << dist(eng) << "ull, ";
    }

    std::cout << std::endl;
}