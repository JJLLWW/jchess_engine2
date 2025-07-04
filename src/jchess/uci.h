#pragma once

#include "core.h"
#include "board.h"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <variant>
#include <functional>


namespace jchess {
    enum class UciNoArgCmd {
        UCI, DEBUG, ISREADY, REGISTER, UCINEWGAME, STOP, PONDERHIT, QUIT
    };
    struct UciSetOption {
        std::string name;
        std::string value;
    };
    struct UciPosition {
        std::string position;
        std::vector<std::string> moves;
    };
    struct UciGo {
        MoveVector search_moves = {};
        bool ponder = false;
        int wtime = -1;
        int btime = -1;
        int winc = 0;
        int binc = 0;
        int movestogo = 0;
        int depth = 0;
        int nodes = -1;
        int mate = -1;
        int movetime = 0; // milliseconds
        bool infinite = false;
    };
    using UciCommand = std::variant<UciNoArgCmd, UciSetOption, UciPosition, UciGo>;
    std::optional<UciCommand> read_command(std::string const& command);

    void uci_loop(std::istream& input, std::function<void(UciCommand)> const& cmd_handler);
    UciSetOption read_setoption_args(std::istringstream& tokens);
    UciPosition read_position_args(std::istringstream& tokens);
    UciGo read_go_args(std::istringstream& tokens);
    bool is_quit(UciCommand const& command);

    // as of writing this, clang still has no support for the <syncstream> C++20 header.
    void thread_safe_line_out(std::string const& line);
}