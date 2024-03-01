#include "engine.h"

#include <iostream>
#include <thread> // sleep_until

namespace jchess {
    void uci_loop_with_engine(std::istream& input, Engine& engine) {
        // just ignore setoption for now
        const auto uci_handler = [&engine](UciCommand const& cmd) {
            if(std::holds_alternative<UciNoArgCmd>(cmd)) {
                engine.handle_uci_no_arg(std::get<UciNoArgCmd>(cmd));
            } else if(std::holds_alternative<UciPosition>(cmd)) {
                engine.handle_uci_position(std::get<UciPosition>(cmd));
            } else if(std::holds_alternative<UciGo>(cmd)) {
                engine.handle_uci_go(std::get<UciGo>(cmd));
            }
        };
        uci_loop(input, uci_handler);
    }

    void Engine::handle_uci_no_arg(jchess::UciNoArgCmd const& cmd) {
        switch(cmd) {
            case UciNoArgCmd::UCI:
                std::cout << "id name JChess" << std::endl;
                std::cout << "id author Foo Bar" << std::endl;
                std::cout << "uciok" << std::endl;
                break;
            case UciNoArgCmd::ISREADY:
                std::cout << "readyok" << std::endl;
                break;
            case UciNoArgCmd::STOP:
                // set some state to stop the running search
                break;
            default:
                break; // ignore non-essential
        }
    }

    void Engine::handle_uci_position(jchess::UciPosition const& pos) {
        board = Board{pos.position};
        for(std::string const& move_str : pos.moves) {
            board.make_move(Move{move_str});
        }
    }

    void Engine::handle_uci_go(jchess::UciGo const& go) {
        SearchLimits limits;
        limits.max_time_ms = go.movetime;

        // for now only handle searches where client has given an explicit timeout
        auto info = searcher.search(board, limits);

        // return the "best" move as a bestmove command
        std::cout << "bestmove " << move_to_string(info.best_move) << std::endl;
    }
}