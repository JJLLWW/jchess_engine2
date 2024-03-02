#include "engine.h"

#include <iostream>
#include <fstream>
#include <thread> // sleep_until

namespace jchess {
    namespace {
        FeatureFlags read_from_config(std::string const& cfg_path) {
            // be a bit hacky here
            std::ifstream fflag_file{cfg_path};
            std::string line;
            FeatureFlags flags = 0ull;
            while(std::getline(fflag_file, line)) {
                if(line.empty() || line.starts_with('#')) {
                    continue;
                }
                std::string flag_name = line.substr(0, line.size() - 2);
                if(flag_name == "FF_OPENING_BOOK" && line.back() == '1') {
                    flags |= FF_OPENING_BOOK;
                }
            }
            return flags;
        }
    }
    void uci_loop_with_engine(std::istream& input, Engine& engine) {
        // just ignore setoption for now
        const auto uci_handler = [&engine](UciCommand const& cmd) {
            if(std::holds_alternative<UciNoArgCmd>(cmd)) {
                engine.handle_uci_no_arg(std::get<UciNoArgCmd>(cmd));
            } else if(std::holds_alternative<UciPosition>(cmd)) {
                engine.handle_uci_position(std::get<UciPosition>(cmd));
            } else if(std::holds_alternative<UciGo>(cmd)) {
                engine.handle_uci_go(std::get<UciGo>(cmd));
            } else if(std::holds_alternative<UciSetOption>(cmd)) {
                engine.handle_uci_setoption(std::get<UciSetOption>(cmd));
            }
        };
        uci_loop(input, uci_handler);
    }

    Engine::Engine() {
        feature_flags = read_from_config("../data/feature_flags.cfg");
    }

    void Engine::handle_uci_no_arg(jchess::UciNoArgCmd const& cmd) {
        switch(cmd) {
            case UciNoArgCmd::UCI:
                std::cout << "id name JChess" << std::endl;
                std::cout << "id author Foo Bar" << std::endl;
                std::cout << "option name OwnBook type check default " << ((feature_flags & FF_OPENING_BOOK) ? "true" : "false") << std::endl;
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
        if((feature_flags & FF_OPENING_BOOK) && !out_of_book) {
            auto move = book.get_random_book_move(board);
            if(!move.has_value()) {
                out_of_book = true;
            } else {
                std::cout << "bestmove " << move_to_string(move.value()) << std::endl;
                return;
            }
        }

        // for now only handle searches where client has given an explicit timeout
        SearchLimits limits;
        limits.max_time_ms = go.movetime;
        auto info = searcher.search(board, limits);
        std::cout << "bestmove " << move_to_string(info.best_move) << std::endl;
    }

    void Engine::handle_uci_setoption(UciSetOption const& cmd) {
        if(cmd.name == "OwnBook") {
            if(cmd.value == "true") {
                feature_flags |= FF_OPENING_BOOK;
                out_of_book = false;
                book.map_file("../data/baron30.bin"); // hard code for now
            } else {
                feature_flags &= ~FF_OPENING_BOOK;
            }
        }
    }
}