#pragma once

#include "uci.h"
#include "board.h"
#include "search.h"

namespace jchess {
    class Engine;

    void uci_loop_with_engine(std::istream& input, Engine& engine);

    class Engine {
    public:
        void handle_uci_no_arg(UciNoArgCmd const& cmd);
        void handle_uci_position(UciPosition const& pos);
        void handle_uci_go(UciGo const& go);
    private:
        Board board {};
        Searcher searcher {};
        Move best_move {"0000"};
        int search_limit = 0;
    };
}