#pragma once

#include <memory>

#include "uci.h"
#include "board.h"
#include "search.h"
#include "polyglot/pg_reader.h"
#include "syzygy/sz_wrapper.h"
#include "nnue/wrap_nnue.h"

namespace jchess {
    class Engine;

    void uci_loop_with_engine(std::istream& input, Engine& engine);

    enum FeatureFlag { FF_OPENING_BOOK = 1, FF_ENDGAME_TABLES = 2, FF_NNUE_EVAL = 4 };
    using FeatureFlags = uint64_t;

    struct EngineConfig {
        std::string feature_flag_cfg_file;
        std::string opening_book_file;
        std::string endgame_table_dir;
        std::string nnue_network_file;
        int endgame_dtz_depth;
    };

    class Engine {
    public:
        Engine();
        Engine(EngineConfig const& config);
        void handle_uci_no_arg(UciNoArgCmd const& cmd);
        void handle_uci_position(UciPosition const& cmd);
        void handle_uci_setoption(UciSetOption const& cmd);
        void handle_uci_go(UciGo const& go);
    private:
        EngineConfig config;
        FeatureFlags feature_flags = 0ull;
        Board board {};
        Searcher searcher {};
        Move best_move {"0000"};
        int search_limit = 0;
        // OPENING_BOOK
        polyglot::PGMappedBook book {}; // maybe this should be unique_ptr as well
        bool out_of_book = false;
        // ENDGAME
        std::unique_ptr<syzgy::SZEndgameTables> endgame_tables = nullptr;
    };
}