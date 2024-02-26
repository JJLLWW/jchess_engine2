#include "jchess/movegen.h"
#include "jchess/board.h"
#include <iostream>
#include <cassert>

using namespace jchess;

// have some weird situation where after white plays g4, black thinks they can move the
// c8 bishop to capture it through their own pawn. (???) <- all the problems are with
// the same bishop (???)

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

int main(int argc, char **argv) {
    BoardState state{starting_fen};
    MoveGenerator mg;

    // cryptic now misses 4 moves on depth 4 ???
    std::vector<uint64_t> expected {1, 20, 400, 8902, 197281, 4865609, 119060324, 3195901860, 84998978956 };

    for(int depth = 1; depth < expected.size(); ++depth) {
        if(depth == 4) {
            int i = 2;
        }
        Board board{starting_fen};
        uint64_t nodes = perft(depth, board);
        std::cout << "depth: " << depth << "  nodes: " << nodes << "  expected: " << expected[depth] << std::endl;
    }
}