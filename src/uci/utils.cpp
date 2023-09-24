#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

namespace jchess::utils {
    std::vector<std::string> getWordsOfString(std::string const& line) {
        std::istringstream iss(line);
        std::vector<std::string> words;
        std::copy(std::istream_iterator<std::string>(iss),
            std::istream_iterator<std::string>(),
            std::back_inserter(words));
        return words;
    }
}