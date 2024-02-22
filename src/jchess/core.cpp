//
// Created by Jack Wright on 20/02/2024.
//

#include "core.h"

#include <sstream>
#include <cassert>
#include <vector>
#include <numeric>
#include <algorithm>


namespace jchess {
    namespace {
        std::vector<std::string> split_words(std::string const& input) {
            std::istringstream iss{input};
            std::string word;
            std::vector<std::string> words;
            while(iss >> word) {
                words.push_back(word);
            }
            return words;
        }
    }

    Square square_from_alg_not(std::string const& alg_not) {
        if(alg_not.size() != 2) {
            throw std::invalid_argument("expecting a3, b7 format for squares");
        }
        int file = alg_not[0] - 'a', rank = alg_not[1] - '1';
        assert(0 <= rank && rank < 8 && 0 <= file && file < 8);
        return square_from_rank_file(rank, file);
    }

    Square square_from_rank_file(int rank, int file) {
        return static_cast<Square>(rank*8 + file);
    }

    Piece piece_from_char(char c) {
        size_t offset = piece_chars.find(c);
        if(offset == std::string::npos) {
            std::string message = std::string("could not convert char '") + c + "' to piece";
            throw std::invalid_argument(message);
        }
        return static_cast<Piece>(offset);
    }

    char char_from_piece(Piece piece) {
        if(piece == NO_PIECE) {
            return ' ';
        } else {
            return piece_chars[piece];
        }
    }

    Color color_from_piece(Piece piece) {
        assert(piece != NO_PIECE);
        return (piece < 6) ? WHITE : BLACK;
    }

    CastleBits castle_bits_from_char(char c) {
        switch(c) {
            case 'K':
                return WHITE_KS;
            case 'Q':
                return WHITE_QS;
            case 'k':
                return BLACK_KS;
            case 'q':
                return BLACK_QS;
            default:
                std::string message = std::string("could not convert char '") + c + "' to castling bits";
                throw std::invalid_argument(message);
        }
    }

    std::vector<std::pair<Square, Piece>> FEN::read_fen_pieces(std::string const& pieces) {
        std::vector<std::pair<Square, Piece>> fen_pieces;
        int rank = 7, file = 0;
        for(char c : pieces) {
            if(isnumber(c)) {
                file += (c - '1');
            } else if(c == '/') {
                --rank;
                file = 0;
            } else {
                fen_pieces.emplace_back(square_from_rank_file(rank, file), piece_from_char(c));
                ++file;
            }
        }
        return fen_pieces;
    }

    FEN::FEN(std::string const& fen_string) {
        if(fen_string.size() > 500) {
            throw std::invalid_argument("input fen too long (> 500 chars)");
        }
        std::vector<std::string> fields = split_words(fen_string);
        if(fields.size() != 6) {
            throw std::invalid_argument("input fen does not have 6 fields");
        }
        pieces = read_fen_pieces(fields[0]);
        side_to_move = (fields[1] == "w") ? WHITE : BLACK;
        for(char c : fields[2]) {
            if(count(castle_bits.begin(), castle_bits.end(), c) > 0) {
                castle_right_mask |= castle_bits_from_char(c);
            }
        }
        if(fields[3] != "-") {
            enp_square = square_from_alg_not(fields[3]);
        }
        half_moves = std::stoi(fields[4]);
        full_moves = std::stoi(fields[5]);
    }

    Move::Move(const std::string &uci_move) {
        // castling is sent as if e1g1 so this does handle castling
        if(uci_move.size() > 5 || uci_move.size() < 4) {
            throw std::invalid_argument("uci move string is wrong length");
        }
        if(uci_move == "0000") {
            is_null_move = true;
        } else {
            source = square_from_alg_not(uci_move.substr(0, 2));
            dest = square_from_alg_not(uci_move.substr(2, 2));
            if(uci_move.size() == 5) {
                promotion = piece_from_char(uci_move[4]);
            }
        }
    }

    UnMove::UnMove(int castle_rights, std::optional<Square> enp)
        : castle_right_mask{castle_rights}, enp_state{enp} {}

    UnMove::UnMove(int castle_rights, std::optional<Square> enp, CastleBits castle_type)
        : castle_right_mask{castle_rights}, enp_state{enp} {
        // surely a better way here
        switch(castle_type) {
            case WHITE_KS:
                add_clear_square(F1);
                add_clear_square(G1);
                add_place_piece(E1, W_KING);
                add_place_piece(H1, W_ROOK);
                break;
            case WHITE_QS:
                add_clear_square(C1);
                add_clear_square(D1);
                add_place_piece(E1, W_KING);
                add_place_piece(A1, W_ROOK);
                break;
            case BLACK_KS:
                add_clear_square(F8);
                add_clear_square(G8);
                add_place_piece(E8, W_KING);
                add_place_piece(H8, W_ROOK);
                break;
            case BLACK_QS:
                add_clear_square(C8);
                add_clear_square(D8);
                add_place_piece(E8, W_KING);
                add_place_piece(A8, W_ROOK);
                break;
        }
    }

    void UnMove::add_clear_square(jchess::Square square) {
        clear_squares[num_clear++] = square;
    }

    void UnMove::add_place_piece(jchess::Square square, jchess::Piece piece) {
        place_squares[num_place] = square;
        place_pieces[num_place] = piece;
        num_place++;
    }
}