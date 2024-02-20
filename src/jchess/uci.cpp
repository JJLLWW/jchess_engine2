//
// Created by Jack Wright on 20/02/2024.
//

#include "uci.h"
#include "core.h"

#include <iostream>
#include <string>
#include <sstream>
#include <unordered_set>

namespace jchess {
    UciSetOption read_setoption_args(std::istringstream& tokens) {
        std::string token;
        UciSetOption args;
        while(tokens >> token) {
            if(token == "name") {
                tokens >> token;
                args.name = token;
            } else if(token == "value") {
                tokens >> token;
                args.value = token;
            }
        }
        return args;
    }

    UciPosition read_position_args(std::istringstream& tokens) {
        std::string token;
        UciPosition args;
        while(tokens >> token) {
            if(token == "fen") {
                tokens >> token;
                args.position = token;
            } else if(token == "startpos") {
                args.position = jchess::starting_fen;
            } else if(token == "moves") {
                while(tokens >> token) {
                    args.moves.push_back(token);
                }
            }
        }
        return args;
    }

    UciGo read_go_args(std::istringstream& tokens) {
        std::string token;
        UciGo args;
        std::unordered_set<std::string> non_searchmoves_subcmds {
            "ponder", "wtime", "btime", "winc", "binc", "movestogo",
            "depth", "nodes", "mate", "movetime", "infinite"
            };
        auto extract_int = [&token](std::istringstream& tokens){ tokens >> token; return std::stoi(token); };
        while(tokens >> token) {
            if(token == "searchmoves") {
                while(tokens >> token) {
                    if(non_searchmoves_subcmds.count(token) > 0) {
                        break;
                    } else {
                        args.search_moves.push_back(token);
                    }
                }
            }
            // surely a smarter way here.
            if(token == "ponder") {
                args.ponder = true;
            } else if(token == "infinite") {
                args.infinite = true;
            }
            if(token == "wtime") {
                args.wtime = extract_int(tokens);
            } else if(token == "btime") {
                args.btime = extract_int(tokens);
            } else if(token == "winc") {
                args.winc = extract_int(tokens);
            } else if(token == "binc") {
                args.binc = extract_int(tokens);
            } else if(token == "movestogo") {
                args.movestogo = extract_int(tokens);
            } else if(token == "depth") {
                args.depth = extract_int(tokens);
            } else if(token == "nodes") {
                args.nodes = extract_int(tokens);
            } else if(token == "mate") {
                args.mate = extract_int(tokens);
            } else if(token == "movetime") {
                args.movetime = extract_int(tokens);
            }
        }
        return args;
    }

    UciCommand read_command(std::string const& command) {
        std::istringstream tokens{command};
        std::string token;
        std::unordered_map<std::string, UciNoArgCmd> no_arg_cmds {
            {"uci", UciNoArgCmd::UCI},
            {"isready", UciNoArgCmd::ISREADY},
            {"register", UciNoArgCmd::REGISTER},
            {"ucinewgame", UciNoArgCmd::UCINEWGAME},
            {"stop", UciNoArgCmd::STOP},
            {"ponderhit", UciNoArgCmd::PONDERHIT},
            {"quit", UciNoArgCmd::QUIT}
        };
        while(tokens >> token) {
            if(no_arg_cmds.count(token) > 0) {
                return no_arg_cmds[token];
            } else if(token == "debug") {
                tokens >> token;
                if(token != "off") {
                    return UciNoArgCmd::DEBUG;
                }
            } else if(token == "setoption") {
                return read_setoption_args(tokens);
            } else if(token == "position") {
                return read_position_args(tokens);
            } else if(token == "go") {
                return read_go_args(tokens);
            }
        }
    }

    bool is_quit(UciCommand const& command) {
        return std::holds_alternative<UciNoArgCmd>(command) && std::get<UciNoArgCmd>(command) == UciNoArgCmd::QUIT;
    }

    void uci_loop(std::istream& input, std::function<void(UciCommand)> const& cmd_handler) {
        std::string line;
        while(std::getline(input, line)) {
            UciCommand cmd = read_command(line);
            cmd_handler(cmd);
            if(is_quit(cmd)) {
                return;
            }
        }
    }
}