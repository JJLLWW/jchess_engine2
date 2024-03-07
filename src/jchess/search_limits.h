#pragma once

#include "uci.h"
#include "search.h"
#include "board.h"

namespace jchess {
    void limits_from_uci_go(SearchLimits& limits, UciGo const& uci_go, Color color);
    long long compute_time_to_search_msec(int my_time, int op_time, int my_inc, int op_inc);
}