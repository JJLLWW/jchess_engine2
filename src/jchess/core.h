#pragma once

#include <string>
#include <utility>
#include <vector>
#include <optional>
#include <stack>
#include <array>


namespace jchess {
    const std::string starting_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    const std::string piece_chars = "PRNBKQprnbkq";
    const std::string castle_bits = "KQkq";

    enum Color { WHITE, BLACK };

    enum Piece {
        W_PAWN, W_ROOK, W_KNIGHT, W_BISHOP, W_KING, W_QUEEN,
        B_PAWN, B_ROOK, B_KNIGHT, B_BISHOP, B_KING, B_QUEEN,
        NO_PIECE // maybe mistake
    };

    Piece piece_from_char(char c);
    char char_from_piece(Piece piece);
    Color color_from_piece(Piece piece);

    enum Square {
        A1, B1, C1, D1, E1, F1, G1, H1,
        A2, B2, C2, D2, E2, F2, G2, H2,
        A3, B3, C3, D3, E3, F3, G3, H3,
        A4, B4, C4, D4, E4, F4, G4, H4,
        A5, B5, C5, D5, E5, F5, G5, H5,
        A6, B6, C6, D6, E6, F6, G6, H6,
        A7, B7, C7, D7, E7, F7, G7, H7,
        A8, B8, C8, D8, E8, F8, G8, H8
    };

    Square square_from_rank_file(int rank, int file);
    Square square_from_alg_not(std::string const& alg_not);

    enum CastleBits { WHITE_KS = 1, WHITE_QS = 2, BLACK_KS = 4, BLACK_QS = 8 };

    CastleBits castle_bits_from_char(char c);

    struct FEN {
        FEN(const char *fen_string) : FEN(std::string(fen_string)) {}
        FEN(std::string const& fen_string);
        std::vector<std::pair<Square, Piece>> pieces;
        Color side_to_move;
        int castle_right_mask = 0;
        std::optional<Square> enp_square;
        int half_moves;
        int full_moves;
    private:
        std::vector<std::pair<Square, Piece>> read_fen_pieces(std::string const& pieces);
    };

    struct Move {
        Move(const char *uci_move) : Move(std::string(uci_move)) {}
        Move(std::string const& uci_move);
        Square source;
        Square dest;
        std::optional<Piece> promotion;
        bool is_null = false;
    };
}