#include "jchess/movegen.h"
#include "jchess/board.h"
#include <iostream>
#include <cassert>
#include <functional>

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

uint64_t perft_starting_move(int depth, Board& board) {
    assert(depth > 0);

    uint64_t nodes = 0ull;
    auto moves = board.generate_legal_moves();
    for(const auto& move : moves) {
        board.make_move(move);
        uint64_t nmove = perft(depth - 1, board);
        std::string src = square_to_string(move.source);
        std::string dest = square_to_string(move.dest);
        std::cout << src << dest << ": " << nmove << std::endl;
        nodes += nmove;
        board.unmake_move();
    }
    return nodes;
}

void default_perft(std::vector<uint64_t> const& expected) {
    for(int depth = 1; depth < expected.size(); ++depth) {
        Board board{starting_fen};
        uint64_t nodes = perft(depth, board);
        std::cout << "depth: " << depth << "  nodes: " << nodes << "  expected: " << expected[depth] << std::endl;
    }
}

// handle multiple positions
int main(int argc, char **argv) {
    BoardState state{starting_fen};
    MoveGenerator mg;

    std::vector<uint64_t> expected {1, 20, 400, 8902, 197281, 4865609, 119060324, 3195901860, 84998978956 };

    if(argc == 1) {
        default_perft(expected);
    } else {
        int depth = std::stoi(argv[1]);
        std::string fen = argv[2];
        Board board{fen};
        for(int i=3; i<argc; ++i) {
            Move move(argv[i]);
            board.make_move(move);
        }
        uint64_t nodes = perft_starting_move(depth, board);
        std::cout << "depth: " << depth << "  nodes: " << nodes << "  expected: " << expected[depth] << std::endl;
    }

}