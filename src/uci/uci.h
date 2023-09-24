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
        static std::optional<DebugCommand> parseDebugCommand(std::vector<std::string> const& words) {
            DebugCommand debugCommand;
            if(words.size() == 1 || (words.size() == 2 && words[1] == "on")) {
                debugCommand.isDebugActive = true;
                return debugCommand;
            } else if(words.size() == 2 && words[1] == "off") {
                debugCommand.isDebugActive = false;
                return debugCommand;
            }
            return std::nullopt;
        }

        static std::optional<SetOptionCommand> parseSetOptionCommand(std::vector<std::string> const& words) {
            SetOptionCommand setOptionCommand;
            if(words.size() < 3 || words[1] != "name") {
                return std::nullopt;
            }
            setOptionCommand.optionName = words[2];
            if(words.size() == 5) {
                if(words[3] != "value") {
                    return std::nullopt;
                }
                setOptionCommand.optionValue = words[4];
            }
            return setOptionCommand;
        }

        static std::optional<PositionCommand> parsePositionCommand(std::vector<std::string> const& words) {
            PositionCommand positionCommand;

            return std::nullopt;
        }
    };
}