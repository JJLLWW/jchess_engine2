#include "jchess/core.h"
#include "jchess/uci.h"


int main() {
    std::istringstream iss{"uci\n isready\n quit"};
    std::vector<jchess::UciCommand> cmds;
    auto handler = [&cmds](jchess::UciCommand const& cmd) mutable {
        cmds.push_back(cmd);
    };
    jchess::uci_loop(iss, handler);
    int i = 2;
}
