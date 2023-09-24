#include "uci.h"

namespace jchess::uci {
    std::optional<DebugCommand> UciCommandParser::parseDebugCommand(const std::vector<std::string> &words) {
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

    std::optional<SetOptionCommand> UciCommandParser::parseSetOptionCommand(std::vector<std::string> const& words) {
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

    std::optional<PositionCommand> UciCommandParser::parsePositionCommand(std::vector<std::string> const& words) {
        PositionCommand positionCommand;

        return std::nullopt;
    }
}
