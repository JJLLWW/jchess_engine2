//
// Created by Jack Wright on 20/02/2024.
//

#ifndef UNTITLED4_CORE_H
#define UNTITLED4_CORE_H

#endif //UNTITLED4_CORE_H

#include <string>
#include <utility>
#include <vector>
#include <optional>

namespace jchess {
    const std::string starting_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    const std::string fen_piece_chars = "pnbrkqPNBRKQ";
    enum class PieceType : char { PAWN = 'p', KNIGHT = 'n', BISHOP = 'b', ROOK = 'r', KING = 'k', QUEEN = 'q'};
    struct Piece {
        Piece(PieceType type, bool is_white) : type{type}, is_white{is_white} {}
        PieceType type;
        bool is_white;
    };
    struct Square final {
        Square(int rank, int file);
        int _square;
        operator int() const {
            return _square;
        }
        static Square from_alg_not(std::string const& alg_not);
    };
    // should this be a static method of the square?
    // Square from_alg_not(std::string const& alg_not);
    struct FEN final {
        // the first one is really necessary to deal with string literals
        FEN(const char *fen_string) : FEN(std::string(fen_string)) {}
        FEN(std::string const& fen_string);
        enum CastleBits { WHITE_KS = 1, WHITE_QS = 2, BLACK_KS = 4, BLACK_QS = 8 };
        std::vector<std::pair<Square, Piece>> pieces;
        bool is_white_turn;
        int castle_right_mask = 0;
        std::optional<Square> enp_square;
        int half_moves;
        int full_moves;
    };
    std::vector<std::pair<Square, Piece>> read_fen_pieces(std::string const& pieces);
}