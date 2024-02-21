#include "jchess/core.h"
#include "jchess/uci.h"


int main() {
    std::istringstream iss{"uci\n isready\n quit"};
    std::vector<jchess::UciCommand> cmds;
    auto handler = [&cmds](jchess::UciCommand const& cmd) mutable {
        cmds.push_back(cmd);
    };
    jchess::uci_loop(iss, handler);
    jchess::FEN fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    jchess::Move move = "a7a8q";
    int i = 2;
}
