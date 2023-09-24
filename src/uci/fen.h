#pragma once

#include <string>
#include <optional>
#include <unordered_map>

namespace {
    inline constexpr bool isRank(char c) {
        return '1' <= c && c <= '8';
    }
    inline constexpr bool isFile(char c) {
        return 'a' <= c && c <= 'h';
    }
    std::unordered_map<char, jchess::fen::Promotion>
}

namespace jchess::fen {
    struct FEN {};

    enum class Promotion {
        NONE, KNIGHT, BISHOP, ROOK, QUEEN
    };

    // castling is still determined by source and destination
    struct Move {
        int sourceRank, sourceFile;
        int targetRank, targetFile;
        Promotion promotion;
        static std::optional<Move> fromString(std::string const& word) {
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

        }
    };
}