#include "search_limits.h"
#include <cmath>

namespace jchess {
    long long compute_time_to_search_msec(int my_time, int op_time, int my_inc, int op_inc) {
        const long long max_time = 8000ll;
        const double time_factor = std::clamp((double)my_time/(double)op_time, 0.5, 1.5);

        const double time_dbl = (my_time * time_factor) / 40.0;
        auto time_ll = static_cast<long long>(std::ceil(time_dbl));

        return std::min(max_time, time_ll);
    }

    void limits_from_uci_go(SearchLimits& limits, UciGo const& uci_go, Color color) {
        limits.max_nodes = (uci_go.nodes == -1) ? -1ull : uci_go.nodes;
        limits.depth = uci_go.depth;
        limits.search_moves = uci_go.search_moves;
        limits.infinite = uci_go.infinite;
        limits.ponder = uci_go.ponder;
        limits.max_time_ms = uci_go.movetime;
        // TODO: go mate N support

        if(uci_go.movetime == 0 && !uci_go.infinite) {
            // (ignore uci movestogo, assume sudden death
            int my_time = (color == WHITE) ? uci_go.wtime : uci_go.btime;
            int op_time = (color == WHITE) ? uci_go.btime : uci_go.wtime;
            int my_inc = (color == WHITE) ? uci_go.winc : uci_go.binc;
            int op_inc = (color == WHITE) ? uci_go.binc : uci_go.winc;

            limits.max_time_ms = compute_time_to_search_msec(my_time, op_time, my_inc, op_inc);
        }
    }
}
