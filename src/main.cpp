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
    std::string str = board.to_string();
    std::cout << str << std::endl;
    int i = 2;
}
