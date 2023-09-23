#pragma once

#include <string>
#include <optional>
#include <vector>

#include "fen.h"

namespace jchess::uci {
    template <typename T>
    concept Command = requires(T commandObj) {
        typename T::Command;
    };
    struct UciCommand {};

    struct DebugCommand {
        using Command = DebugCommand;
        bool isDebugActive;
    };

    struct IsReadyCommand {
        using Command = IsReadyCommand;
    };

    struct SetOptionCommand {
        using Command = SetOptionCommand;
        std::string optionName;
        std::string optionValue;
    };

    struct RegisterCommand {
        using Command = RegisterCommand;
        bool registerLater;
        std::string name;
        std::string code;
    };

    struct UciNewGameCommand {
        using Command = UciNewGameCommand;
    };

    struct PositionCommand {
        bool startPos;
        std::optional<fen::FEN> fen;
        std::vector<fen::move> moves;
    };

    struct GoCommand {
        std::vector<fen::move> searchMoves;
        bool ponder;
        bool inifinte;
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
}