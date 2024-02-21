//
// Created by Jack Wright on 20/02/2024.
//

#include "core.h"

#include <sstream>
#include <cassert>
#include <vector>
#include <numeric>


namespace jchess {
    namespace {
        std::unordered_map<char, CastleBits> castle_bit_of = {
            {'K', WHITE_KS}, {'Q', WHITE_QS}, {'k', BLACK_KS}, {'q', BLACK_QS}
        };
        std::unordered_map<PieceType, PieceBBIndex> bb_index_of_piece = {
            {PieceType::PAWN, W_PAWN}, {PieceType::ROOK, W_ROOK},
            {PieceType::BISHOP, W_BISHOP}, {PieceType::KNIGHT, W_KNIGHT},
            {PieceType::KING, W_KING}, {PieceType::QUEEN, W_QUEEN}
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
    Square::Square(std::string const& alg_not) {
        if(alg_not.size() != 2) {
            throw std::invalid_argument("expecting a3, b7 format for squares");
        }
        int file = alg_not[0] - 'a', rank = alg_not[1] - '1';
        assert(0 <= rank && rank < 8 && 0 <= file && file < 8);
        square = rank*8 + file;
    }

    Piece::Piece(char c) {
        assert(fen_piece_chars.find(c) != std::string::npos);
        is_white = isupper(c);
        auto lower_c = static_cast<char>(tolower(c));
        type = static_cast<PieceType>(lower_c);
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
                fen_pieces.emplace_back(Square(rank, file), Piece(c));
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
            enp_square = Square(fields[3]);
        }
        half_moves = std::stoi(fields[4]);
        full_moves = std::stoi(fields[5]);
    }

    Square::Square(int rank, int file) {
        assert(0 <= rank && rank < 8 && 0 <= file && file < 8);
        square = rank*8 + file;
    }

    Move::Move(const std::string &uci_move) {
        if(uci_move.size() > 5 || uci_move.size() < 4) {
            throw std::invalid_argument("uci move string is wrong length");
        }
        if(uci_move == "0000") {
            is_null = true;
        } else {
            source = Square(uci_move.substr(0, 2));
            dest = Square(uci_move.substr(2, 2));
            if(uci_move.size() == 5) {
                promotion = Piece(uci_move[4]).type;
            }
        }
    }

    PieceBBIndex get_piece_index(Piece const& piece) {
        int as_int = bb_index_of_piece[piece.type] + piece.is_white ? 0 : 6;
        return static_cast<PieceBBIndex>(as_int);
    }

    void Board::set_position(const jchess::FEN &fen) {
        // TODO: cleanup.
        is_white_turn = fen.is_white_turn;
        castle_right_mask = fen.castle_right_mask;
        half_moves = fen.half_moves;
        full_moves = fen.full_moves;
        is_white_turn = fen.is_white_turn;
        enp_square = fen.enp_square;
        pieces = fen.pieces;
        set_piece_bbs_from_pieces();
    }

    void Board::set_piece_bbs_from_pieces() {
        for(uint64_t& bb : piece_bbs) {
            bb = 0;
        }
        for(const auto& [square, piece] : pieces) {
            piece_bbs[get_piece_index(piece)] |= (1 << square.square);
        }
        color_bbs[WHITE] = std::reduce(all_pieces_bb.begin(), all_pieces_bb.begin() + 6, 0, std::bit_or<int>{});
        color_bbs[BLACK] = std::reduce(all_pieces_bb.begin() + 6, all_pieces_bb.end(), 0, std::bit_or<int>{});
        all_pieces_bb = color_bbs[WHITE] | color_bbs[BLACK];
    }

    void Board::make_move(jchess::Move const& move) {
        throw std::logic_error("NOT IMPLEMENTED");
        assert((all_pieces_bb & (1 << move.source.square)) == 1);
        moves.push(move);
    }
}