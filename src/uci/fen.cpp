#include "fen.h"
#include "utils.h"

namespace {
    inline constexpr bool isRank(char c) {
        return '1' <= c && c <= '8';
    }
    inline constexpr bool isFile(char c) {
        return 'a' <= c && c <= 'h';
    }
    using Promotion = jchess::fen::Promotion;

    std::unordered_map<char, Promotion> promotions {
        {'n', Promotion::KNIGHT},
        {'b', Promotion::BISHOP},
        {'r', Promotion::ROOK},
        {'q', Promotion::QUEEN}
    };

    std::optional<std::pair<int, int>> readSquare(std::string const& word) {
        if(word.size() == 2 && isFile(word[0]) && isRank(word[1])) {
            return std::make_pair(word[0] - 'a', word[1] - '1');
        }
        return std::nullopt;
    }

    bool readPiecePlacement(std::string const& word, std::vector<jchess::fen::Piece>& pieces) {
        return false;
    }

    int readCastleFlags(std::string const& word) {
        int castleFlags = 0;
        for(char c : word) {
            switch(c) {
                case 'K':
                    castleFlags |= jchess::fen::WHITE_KS;
                    break;
                case 'Q':
                    castleFlags |= jchess::fen::WHITE_QS;
                    break;
                case 'k':
                    castleFlags |= jchess::fen::BLACK_KS;
                    break;
                case 'q':
                    castleFlags |= jchess::fen::BLACK_QS;
                    break;
                default:
                    break;
            }
        }
        return castleFlags;
    }
}

namespace jchess::fen {
    std::optional<Move> Move::fromString(std::string const& word) {
        if(
            word.size() < 4 ||
            !isFile(word[0]) || !isRank(word[1]) ||
            !isFile(word[2]) || !isRank(word[3])
            ) {
            return std::nullopt;
        }
        Move move;
        move.sourceFile = word[0] - 'a';
        move.sourceRank = word[1] - '1';
        move.targetFile = word[2] - 'a';
        move.targetRank = word[3] - '1';
        if(word.size() == 5) {
            if(promotions.count(word[4]) == 0) {
                return std::nullopt;
            }
            move.promotion = promotions[word[4]];
        }
        return move;
    }

    std::optional<FEN> FEN::fromString(std::string const& line) {
        std::vector<std::string> words = utils::getWordsOfString(line);
        if(words.size() != 6 || (words[1] != "w" && words[1] != "b")) {
            return std::nullopt;
        }
        FEN fen;
        if(!readPiecePlacement(words[0], fen.pieces)) {
            return std::nullopt;
        }
        fen.isWhitesMove = (words[1] == "w");
        fen.castleFlags = readCastleFlags(words[2]);
        fen.enpSquare = readSquare(words[3]);
        fen.nHalfMove = std::stoi(words[4]);
        fen.nMove = std::stoi(words[5]);
        return fen;
    }
}