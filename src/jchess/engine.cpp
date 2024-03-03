#include "engine.h"

#include <iostream>
#include <fstream>
#include <thread> // sleep_until
#include <sstream>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace jchess {
    namespace {
        const EngineConfig default_engine_config {
            .feature_flag_cfg_file = "./data/feature_flags.cfg",
            .opening_book_file = "./data/baron30.bin",
            .endgame_table_dir = "./tables/",
            .endgame_dtz_depth = 5
        };

        FeatureFlags read_from_config(std::string const& cfg_path) {
            // if we can't read the file, just carry on, it's not essential
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
                } else if(flag_name == "FF_ENDGAME_TABLES" && line.back() == '1') {
                    flags |= FF_ENDGAME_TABLES;
                }
            }
            return flags;
        }

        auto engine_logger = spdlog::basic_logger_st("engine_logger", "./logs/engine.txt", true);
    }

    void uci_loop_with_engine(std::istream& input, Engine& engine) {
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
        config = default_engine_config; // user hasn't specified a config
        spdlog::set_default_logger(engine_logger);
        spdlog::set_level(spdlog::level::debug);
        feature_flags = read_from_config(config.feature_flag_cfg_file);
        if(feature_flags & FF_OPENING_BOOK) {
            spdlog::debug("opening book enabled");
            book.map_file(config.opening_book_file);
            out_of_book = false;
        }
        if(feature_flags & FF_ENDGAME_TABLES) {
            spdlog::debug("endgame tables enabled");
            endgame_tables = std::make_unique<syzgy::SZEndgameTables>(config.endgame_table_dir);
        }
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
        // we are so far into the endgame that we can lookup moves in a table rather than search
        if((feature_flags & FF_ENDGAME_TABLES) && endgame_tables && board.get_num_pieces() <= config.endgame_dtz_depth) {
            auto dtz_entry = endgame_tables->probe_dtz_tables(board);
            // if there's some error with the table just fallback to regular search
            if(dtz_entry.has_value() && !dtz_entry.value().stalemate && !dtz_entry.value().checkmate) {
                Move move = dtz_entry.value().move;
                spdlog::debug("DTZ Table Move: {0}", move_to_string(move));
                std::cout << "bestmove " << move_to_string(move) << std::endl;
                return;
            }
        }

        // we still could find the current position in the opening book, fallback to search otherwise
        if((feature_flags & FF_OPENING_BOOK) && !out_of_book) {
            auto move = book.get_random_book_move(board);
            if(!move.has_value()) {
                out_of_book = true;
            } else {
                spdlog::debug("Book Move: {0}", move_to_string(move.value()));
                std::cout << "bestmove " << move_to_string(move.value()) << std::endl;
                return;
            }
        }

        // we need to do a manual search as can't lookup the current position
        SearchLimits limits;
        limits.max_time_ms = go.movetime;
        auto info = searcher.search(board, limits);
        std::ostringstream oss;
        oss << info;
        spdlog::debug("Search Info: {0}", oss.str());
        std::cout << "bestmove " << move_to_string(info.best_move) << std::endl;
    }

    void Engine::handle_uci_setoption(UciSetOption const& cmd) {
        if(cmd.name == "OwnBook") {
            if(cmd.value == "true") {
                feature_flags |= FF_OPENING_BOOK;
                out_of_book = false;
                book.map_file(config.opening_book_file);
            } else {
                feature_flags &= ~FF_OPENING_BOOK;
            }
        }
    }
}