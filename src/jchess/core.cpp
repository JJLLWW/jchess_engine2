//
// Created by Jack Wright on 20/02/2024.
//

#include "core.h"

#include <sstream>
#include <cassert>
#include <vector>


namespace jchess {
    namespace {
        std::unordered_map<char, FEN::CastleBits> castle_bit_of = {
            {'K', FEN::WHITE_KS}, {'Q', FEN::WHITE_QS}, {'k', FEN::BLACK_KS}, {'q', FEN::BLACK_QS}
        };
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
    Square Square::from_alg_not(std::string const& alg_not) {
        if(alg_not.size() != 2) {
            throw std::invalid_argument("expecting a3, b7 format for squares");
        }
        int rank = alg_not[0] - 'a', file = alg_not[1] - '1';
        return {rank, file};
    }

    Piece piece_from_fen_char(char c) {
        assert(fen_piece_chars.find(c) != std::string::npos);
        bool is_white = isupper(c);
        auto lower_c = static_cast<char>(tolower(c));
        auto type = static_cast<PieceType>(lower_c);
        return {type, is_white};
    }

    std::vector<std::pair<Square, Piece>> read_fen_pieces(std::string const& pieces) {
        std::vector<std::pair<Square, Piece>> fen_pieces;
        int rank = 7, file = 0;
        for(char c : pieces) {
            if(isnumber(c)) {
                file += (c - '1');
            } else if(c == '/') {
                --rank;
                file = 0;
            } else {
                fen_pieces.emplace_back(Square(rank, file), piece_from_fen_char(c));
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
        is_white_turn = fields[1] == "w";
        for(char c : fields[2]) {
            if(castle_bit_of.count(c) > 0) {
                castle_right_mask |= castle_bit_of[c];
            }
        }
        if(fields[3] != "-") {
            enp_square = Square::from_alg_not(fields[3]);
        }
        half_moves = std::stoi(fields[4]);
        full_moves = std::stoi(fields[5]);
    }

    Square::Square(int rank, int file) {
        assert(0 <= rank && rank < 8 && 0 <= file && file < 8);
        _square = rank*8 + file;
    }
}