#include "board_state.h"
#include "bitboard.h"

#include <numeric>

namespace jchess {
    bool can_castle(BoardState const& state, Color color, bool queen_side, Bitboard attacked) {
        CastleBits castle_type = castle_flag_from(color, queen_side);
        if(!(state.castle_right_mask & castle_type)) {
            return false; // no right to castle
        }
        Bitboard castle_squares = castle_square_bb(castle_type);
        if(std::popcount(castle_squares & state.all_pieces_bb) > 2) {
            return false; // castling obstructed by piece
        }
        if(attacked & ~(FILE_BBS[A] | FILE_BBS[B] | FILE_BBS[H]) & castle_squares) {
            return false; // squares king "moves through" are being attacked.
        }

        return true;
    }

    BoardState::BoardState(FEN const& fen) {
        castle_right_mask = fen.castle_right_mask;
        enp_square = fen.enp_square;
        std::fill(pieces.begin(), pieces.end(), NO_PIECE);
        for(const auto& [square, piece] : fen.pieces) {
            pieces[square] = piece;
            bb_add_square(piece_bbs[piece], square);
        }
        color_bbs[WHITE] = std::reduce(piece_bbs.begin(), piece_bbs.begin() + 6, 0ull, std::bit_or<uint64_t>{});
        color_bbs[BLACK] = std::reduce(piece_bbs.begin() + 6, piece_bbs.end(), 0ull, std::bit_or<uint64_t>{});
        orth_slider_bb[WHITE] = piece_bbs[W_QUEEN] | piece_bbs[W_ROOK];
        orth_slider_bb[BLACK] = piece_bbs[B_QUEEN] | piece_bbs[B_ROOK];
        all_pieces_bb = color_bbs[WHITE] | color_bbs[BLACK];
    }

    void BoardState::remove_piece_from_square(Square square) {
        Piece piece = pieces[square];
        if(piece != NO_PIECE) {
            Color piece_color = color_from_piece(piece);
            bb_remove_square(all_pieces_bb, square);
            bb_remove_square(color_bbs[piece_color], square);
            bb_remove_square(piece_bbs[piece], square);
            // if not a slider, we aren't doing anything.
            bb_remove_square(orth_slider_bb[piece_color], square);
            pieces[square] = NO_PIECE;
        }
    }

    void BoardState::place_piece_on_square(Piece piece, Square square) {
        Color piece_color = color_from_piece(piece);
        PieceType type = type_from_piece(piece);
        bb_add_square(all_pieces_bb, square);
        bb_add_square(color_bbs[piece_color], square);
        bb_add_square(piece_bbs[piece], square);
        if(type == QUEEN || type == ROOK) {
            bb_add_square(orth_slider_bb[piece_color], square);
        }
        pieces[square] = piece;
    }
}