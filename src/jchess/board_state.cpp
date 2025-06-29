#include "board_state.h"
#include "bitboard.h"
#include "moves.h"

#include <bit>
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
            if(type_from_piece(piece) == KING) {
                king_sq[color_from_piece(piece)] = square;
            }
            pieces[square] = piece;
            bb_add_square(piece_bbs[piece], square);
        }
        color_bbs[WHITE] = std::reduce(piece_bbs.begin(), piece_bbs.begin() + 6, 0ull, std::bit_or<uint64_t>{});
        color_bbs[BLACK] = std::reduce(piece_bbs.begin() + 6, piece_bbs.end(), 0ull, std::bit_or<uint64_t>{});
        orth_slider_bb[WHITE] = piece_bbs[W_QUEEN] | piece_bbs[W_ROOK];
        orth_slider_bb[BLACK] = piece_bbs[B_QUEEN] | piece_bbs[B_ROOK];
        diag_slider_bb[WHITE] = piece_bbs[W_QUEEN] | piece_bbs[W_BISHOP];
        diag_slider_bb[BLACK] = piece_bbs[B_QUEEN] | piece_bbs[B_BISHOP];
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
            bb_remove_square(diag_slider_bb[piece_color], square);
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
        if(type == QUEEN || type == BISHOP) {
            bb_add_square(diag_slider_bb[piece_color], square);
        }
        if(type == KING) {
            king_sq[piece_color] = square;
        }
        pieces[square] = piece;
    }
    bool is_attack(Square src, Square dest, PieceType type, Color color, BoardState const& state) {
        switch(type) {
            case KNIGHT:
                return bb_from_square(dest) & get_knight_moves(src, 0ull);
            case ROOK:
                return bb_from_square(dest) & get_rook_moves(src, state.color_bbs[color], state.color_bbs[!color]);
            case BISHOP:
                return bb_from_square(dest) & get_bishop_moves(src, state.color_bbs[color], state.color_bbs[!color]);
            case QUEEN:
                return bb_from_square(dest) & get_queen_moves(src, state.color_bbs[color], state.color_bbs[!color]);
            case PAWN:
                return bb_from_square(dest) & PAWN_ATTACKS[color][src];
            case KING:
                return horizontal_distance(src, dest) == 1 && vertical_distance(src, dest) == 1;
        }
    }

    Bitboard get_attackers_of(Square square, BoardState const &state, Color color) {
        Bitboard knights = KNIGHT_ATTACKS[square] & state.piece_bbs[KNIGHT | color];
        Bitboard ortho = get_rook_moves(square, 0ull, state.all_pieces_bb) & state.orth_slider_bb[color];
        Bitboard diag = get_bishop_moves(square, 0ull, state.all_pieces_bb) & state.diag_slider_bb[color];
        Bitboard pawn = PAWN_ATTACKS[!color][square] & state.piece_bbs[PAWN | color];
        return knights | ortho | diag | pawn;
    }

    bool BoardState::in_check(Color color) const {
        return get_attackers_of(king_sq[color], *this, !color) != 0ull;
    }
}
