#pragma once

#include <string>
#include <optional>
#include <utility>


namespace jchess::fen {
    enum class PieceType {
        PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING
    };

    struct Piece {
        std::pair<int, int> square;
        PieceType type;
    };

    enum CastleFlags {
        WHITE_QS = 1,
        WHITE_KS = 2,
        BLACK_QS = 4,
        BLACK_KS = 8
    };

    struct FEN {
        using CastleFlags = int;
        std::vector<Piece> pieces;
        bool isWhitesMove;
        CastleFlags castleFlags;
        std::optional<std::pair<int, int>> enpSquare;
        int nHalfMove;
        int nMove;
        static std::optional<FEN> fromString(std::string const& line);
    };

    enum class Promotion {
        NONE, KNIGHT, BISHOP, ROOK, QUEEN
    };

    // castling is still determined by source and destination
    struct Move {
        int sourceRank, sourceFile;
        int targetRank, targetFile;
        Promotion promotion = Promotion::NONE;
        static std::optional<Move> fromString(std::string const& word);
    };
}