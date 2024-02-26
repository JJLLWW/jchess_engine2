#include "jchess/movegen.h"
#include "jchess/board.h"

#include <iostream>
#include <fstream>
#include <cassert>
#include <unordered_set>

#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace jchess;

uint64_t perft(int depth, Board& board) {
    assert(depth >= 0);

    if(depth == 0) {
        return 1ull;
    }

    uint64_t nodes = 0ull;
    auto moves = board.generate_legal_moves();
    for(const auto& move : moves) {
        board.make_move(move);
        nodes += perft(depth - 1, board);
        board.unmake_move();
    }

    return nodes;
}

void perft_one_case(int max_depth, std::string const& fen, std::vector<uint64_t> const& expected) {
    Board board{fen};
    for(int depth = 1; depth <= max_depth; ++depth) {
        uint64_t actual = perft(depth, board);
        std::cout << "depth: " << depth << " actual: " << actual << " expected: " << expected[depth-1] << std::endl;
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