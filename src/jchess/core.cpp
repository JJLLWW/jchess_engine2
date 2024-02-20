//
// Created by Jack Wright on 20/02/2024.
//

#include "core.h"

#include <sstream>
#include <cassert>
#include <iostream>
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
    Square from_rank_and_file(int rank, int file) {
        assert(0 <= rank && rank < 8 && 0 <= file && file < 8);
        return rank*8 + file;
    }
    Square from_alg_not(std::string const& alg_not) {
        if(alg_not.size() != 2) {
            throw std::invalid_argument("expecting a3, b7 format for squares");
        }
        int rank = alg_not[0] - 'a', file = alg_not[1] - '1';
        return from_rank_and_file(rank, file);
    }

    std::unordered_map<Square, Piece> read_fen_pieces(std::string const& pieces) {
        std::unordered_map<Square, Piece> piece_map;
        int rank = 7, file = 0;
        for(char c : pieces) {
            if(isnumber(c)) {
                file += (c - '1');
            } else if(c == '/') {
                --rank;
                file = 0;
            } else {
                auto type = static_cast<PieceType>(tolower(c));
                Square square = from_rank_and_file(rank, file);
                piece_map[square] = Piece {type, static_cast<bool>(isupper(c)) };
                ++file;
            }
        }
        return piece_map;;
    }

    FEN fen_from_string(std::string const& fen_string) {
        FEN fen;
        if(fen_string.size() > 500) {
            throw std::invalid_argument("input fen too long (> 500 chars)");
        }
        std::vector<std::string> fields = split_words(fen_string);
        if(fields.size() != 6) {
            throw std::invalid_argument("input fen does not have 6 fields");
        }
        fen.pieces = read_fen_pieces(fields[0]);
        fen.is_white_turn = fields[1] == "w";
        for(char c : fields[2]) {
            if(castle_bit_of.count(c) > 0) {
                fen.castle_right_mask |= castle_bit_of[c];
            }
        }
        if(fields[3] != "-") {
            fen.enp_square = from_alg_not(fields[3]);
        }
        fen.half_moves = std::stoi(fields[4]);
        fen.full_moves = std::stoi(fields[5]);
        return fen;
    }
}