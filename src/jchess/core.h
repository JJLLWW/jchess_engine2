//
// Created by Jack Wright on 20/02/2024.
//

#ifndef UNTITLED4_CORE_H
#define UNTITLED4_CORE_H

#endif //UNTITLED4_CORE_H

#include <string>
#include <utility>
#include <optional>

namespace jchess {
    // should Square be a class with 2 constructors instead of these free functions?
    // 0 <- bottom left, 7 <- bottom right, 56 <- top left,  63 <- top right
    enum class PieceType : char { PAWN = 'p', KNIGHT = 'k', BISHOP = 'b', ROOK = 'r', KING = 'k', QUEEN = 'q'};
    struct Piece {
        PieceType type;
        bool is_white;
    };
    using Square = int;
    // both in 0-7 range. 0 bottom rank, 7 top rank, 0 left file, 7 right file.
    Square from_rank_and_file(int rank, int file);
    Square from_alg_not(std::string const& alg_not);
    struct FEN {
        enum CastleBits { WHITE_KS = 1, WHITE_QS = 2, BLACK_KS = 4, BLACK_QS = 8 };
        std::unordered_map<Square, Piece> pieces;
        bool is_white_turn;
        int castle_right_mask = 0;
        std::optional<Square> enp_square;
        int half_moves;
        int full_moves;
    };
    std::unordered_map<Square, Piece> read_fen_pieces(std::string const& pieces);
    FEN fen_from_string(std::string const& fen_string);
    const std::string starting_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
}