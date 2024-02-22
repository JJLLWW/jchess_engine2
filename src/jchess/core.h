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
        NO_PIECE
    };

    Piece piece_from_char(char c);
    char char_from_piece(Piece piece);
    Color color_from_piece(Piece piece);

    // this may be a mistake
    enum Direction { LEFT = -1, RIGHT = 1, UP = 8, DOWN = -8 };

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
        Piece promotion = NO_PIECE;
        bool is_null_move;
    };

    // if this thing has state modification functions surely its attributes should be private.
    struct UnMove {
        UnMove() = default;
        UnMove(int castle_rights, std::optional<Square> enp);
        UnMove(int castle_rights, std::optional<Square> enp, CastleBits castle);
        void add_clear_square(Square square);
        void add_place_piece(Square square, Piece piece);
        int num_clear = 0;
        int num_place = 0;
        // use std::arrays for copy ctor
        std::array<Square, 2> clear_squares;
        std::array<Square, 2> place_squares;
        std::array<Piece, 2> place_pieces;
        std::optional<Square> enp_state;
        int castle_right_mask = 0;
    };
}