#pragma once

#include <string>
#include <optional>
#include <vector>

#include "fen.h"

namespace jchess::uci {
    struct UciCommand {};

    struct DebugCommand {
        bool isDebugActive = false;
    };

    struct IsReadyCommand {};

    struct SetOptionCommand {
        std::string optionName;
        std::optional<std::string> optionValue;
    };

    struct RegisterCommand {
        bool registerLater;
        std::string name;
        std::string code;
    };

    struct UciNewGameCommand {};

    struct PositionCommand {
        bool startPos;
        std::optional<fen::FEN> fen;
        std::vector<fen::Move> moves;
    };

    struct GoCommand {
        std::vector<fen::Move> searchMoves;
        bool ponder;
        bool infinite;
        int64_t whiteTimeLeft;
        int64_t blackTimeLeft;
        int64_t whiteIncrement;
        int64_t blackIncrement;
        int movesToGo;
        int depth, nodes, mate;
        int64_t moveTime;
    };

    struct StopCommand {};

    struct PonderHitCommand {};

    struct QuitCommand {};

    class UciCommandParser {
    public:
        static std::optional<DebugCommand> parseDebugCommand(std::vector<std::string> const& words);

        static std::optional<SetOptionCommand> parseSetOptionCommand(std::vector<std::string> const& words);

        static std::optional<PositionCommand> parsePositionCommand(std::vector<std::string> const& words);
    };
}