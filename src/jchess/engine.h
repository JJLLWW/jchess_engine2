#pragma once

#include "uci.h"
#include "board.h"
#include "search.h"
#include "polyglot/pg_reader.h"

namespace jchess {
    class Engine;

    void uci_loop_with_engine(std::istream& input, Engine& engine);

    enum FeatureFlag { FF_OPENING_BOOK = 1 };
    using FeatureFlags = uint64_t;

    class Engine {
    public:
        Engine();
        void handle_uci_no_arg(UciNoArgCmd const& cmd);
        void handle_uci_position(UciPosition const& cmd);
        void handle_uci_setoption(UciSetOption const& cmd);
        void handle_uci_go(UciGo const& go);
    private:
        FeatureFlags feature_flags = 0ull; // UCI OwnBook option
        Board board {};
        Searcher searcher {};
        Move best_move {"0000"};
        int search_limit = 0;
        // OPENING_BOOK
        polyglot::PGMappedBook book {};
        bool out_of_book = false;
    };
}