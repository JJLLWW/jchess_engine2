#pragma once

#include "../board.h"
#include <optional>

namespace jchess::syzgy {
    enum class WDL : unsigned { LOSS = 0, BLESSED_LOSS = 1, DRAW = 2, CURSED_WIN = 3, WIN = 4 };

    struct DTZEntry {
        bool stalemate = false;
        bool checkmate = false;
        WDL wdl;
        Move move = "0000";
        unsigned dtz;
    };


    // we don't want more than one instance of this class active.
    class SZEndgameTables {
    public:
        SZEndgameTables(std::string const& tables_root);
        ~SZEndgameTables();
        std::optional<WDL> probe_wdl_tables(Board const& board);
        std::optional<DTZEntry> probe_dtz_tables(Board const& board);
    };
}