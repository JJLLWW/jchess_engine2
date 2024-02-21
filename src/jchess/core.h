//
// Created by Jack Wright on 20/02/2024.
//

#ifndef UNTITLED4_CORE_H
#define UNTITLED4_CORE_H

#endif //UNTITLED4_CORE_H

#include "bitboard.h"

#include <string>
#include <utility>
#include <vector>
#include <optional>
#include <stack>
#include <array>


namespace jchess {
    const std::string starting_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    const std::string fen_piece_chars = "pnbrkqPNBRKQ";
    enum class PieceType : char { PAWN = 'p', KNIGHT = 'n', BISHOP = 'b', ROOK = 'r', KING = 'k', QUEEN = 'q'};

    enum Color { WHITE, BLACK };

    struct Piece {
        Piece(char c);
        Piece(PieceType type, bool is_white) : type{type}, is_white{is_white} {}
        PieceType type;
        bool is_white;
    };

    enum SquareName {
        A1, B1, C1, D1, E1, F1, G1, H1,
        A2, B2, C2, D2, E2, F2, G2, H2,
        A3, B3, C3, D3, E3, F3, G3, H3,
        A4, B4, C4, D4, E4, F4, G4, H4,
        A5, B5, C5, D5, E5, F5, G5, H5,
        A6, B6, C6, D6, E6, F6, G6, H6,
        A7, B7, C7, D7, E7, F7, G7, H7,
        A8, B8, C8, D8, E8, F8, G8, H8
    };

    struct Square {
        Square(std::string const& alg_not);
        Square(int square) : square{square} {}
        Square(int rank, int file);
        int square;
    };

    enum CastleBits { WHITE_KS = 1, WHITE_QS = 2, BLACK_KS = 4, BLACK_QS = 8 };

    struct FEN {
        FEN(const char *fen_string) : FEN(std::string(fen_string)) {}
        FEN(std::string const& fen_string);
        std::vector<std::pair<Square, Piece>> pieces;
        bool is_white_turn;
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
        Square source = 0;
        Square dest = 0;
        std::optional<PieceType> promotion;
        bool is_null = false;
    };

    // do not re-order
    enum PieceBBIndex : int {
        W_PAWN, W_ROOK, W_KNIGHT, W_BISHOP, W_KING, W_QUEEN,
        B_PAWN, B_ROOK, B_KNIGHT, B_BISHOP, B_KING, B_QUEEN
    };

    PieceBBIndex get_piece_index(Piece const& piece);

    struct Board {
        Board(FEN const& fen) { set_position(fen); }
        Board() : Board(FEN(starting_fen)) {} // default constructor should just set up as if the normal initial position
        void set_position(FEN const& fen);
        void make_move(Move const& move);
        void unmake_move();
        // BOARD STATE
        bool is_white_turn = true;
        int castle_right_mask = WHITE_QS | WHITE_KS | BLACK_QS | BLACK_KS;
        int half_moves = 0;
        int full_moves = 0;
        std::optional<Square> enp_square;
        std::vector<std::pair<Square, Piece>> pieces;
        std::array<Bitboard, 12> piece_bbs; // one for white pawns, black kings etc.
        std::array<Bitboard, 2> color_bbs; // all white and black pieces
        Bitboard all_pieces_bb;
        // probably want a few more bitboards.
    private:
        std::stack<Move> moves;
        void set_piece_bbs_from_pieces();
    };
}