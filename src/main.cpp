#include "jchess/core.h"
#include "jchess/uci.h"
#include "jchess/board.h"

int main() {
    std::istringstream iss{"uci\n isready\n quit"};
    std::vector<jchess::UciCommand> cmds;
    auto handler = [&cmds](jchess::UciCommand const& cmd) mutable {
        cmds.push_back(cmd);
    };
    jchess::uci_loop(iss, handler);
    jchess::FEN fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    jchess::Board board{fen};
    std::cout << board.to_string() << std::endl;
    board.make_move(jchess::Move("a2a4Q"));
    std::cout << board.to_string() << std::endl;
    int i = 2;
}
