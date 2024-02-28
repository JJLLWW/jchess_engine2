#include "jchess/engine.h"

using namespace jchess;

int main() {
    jchess::Engine engine;
    jchess::uci_loop_with_engine(std::cin, engine);
}
