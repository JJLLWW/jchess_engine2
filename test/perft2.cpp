#include "jchess/movegen.h"
#include "jchess/board.h"

#include <iostream>
#include <fstream>
#include <cassert>
#include <unordered_set>
#include <chrono>

#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace jchess;

uint64_t perft(int depth, Board& board) {
    assert(depth >= 0);

    if(depth == 0) {
        return 1ull;
    }

    uint64_t nodes = 0ull;
    MoveVector moves;
    board.generate_legal_moves(moves);
    for(const auto& move : moves) {
        board.make_move(move);
        nodes += perft(depth - 1, board);
        board.unmake_move();
    }

    return nodes;
}

void perft_one_case(int max_depth, std::string const& fen, std::vector<uint64_t> const& expected) {
    using std::chrono::high_resolution_clock;
    using std::chrono::duration;

    Board board{fen};
    for(int depth = 1; depth <= max_depth; ++depth) {
        auto t1 = high_resolution_clock::now();
        uint64_t actual = perft(depth, board);
        auto t2 = high_resolution_clock::now();
        auto n_ms = duration<double, std::milli>(t2 - t1);
        std::cout
            << "depth: " << depth
            << " actual: " << actual
            << " expected: " << expected[depth-1]
            << " time: " << n_ms.count() << "ms" << std::endl;
    }
}

// pass path to json file of test cases to code
int main(int argc, char **argv) {
    if(argc == 1) {
        std::cerr << "expected positional argument for name of config file" << std::endl;
        std::exit(1);
    }
    std::string config_name {argv[1]};
    std::ifstream config{config_name};
    json data = json::parse(config);
    std::unordered_set<std::string> exclude;
    for(std::string name : data["exclude"]) {
        exclude.insert(name);
    }
    for(const auto& position : data["positions"]) {
        if(exclude.count(position["name"]) == 0) {
            std::string fen = position["fen"];
            std::vector<uint64_t> expected;
            for(uint64_t value : position["expected"]) {
                expected.push_back(value);
            }
            std::cout << "Position: " << position["name"] << std::endl;
            perft_one_case(position["depth"], fen, expected);
        }
    }

}