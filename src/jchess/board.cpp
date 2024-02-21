#include "board.h"

#include <sstream>
#include <numeric>

namespace jchess {
    void Board::set_position(const jchess::FEN &fen) {
        // TODO: cleanup.
        side_to_move = fen.side_to_move;
        castle_right_mask = fen.castle_right_mask;
        half_moves = fen.half_moves;
        full_moves = fen.full_moves;
        enp_square = fen.enp_square;
        std::fill(pieces.begin(), pieces.end(), NO_PIECE);
        for(const auto& [square, piece] : fen.pieces) {
            pieces[square] = piece;
            bb_add_square(piece_bbs[piece], square);
        }
        color_bbs[WHITE] = std::reduce(piece_bbs.begin(), piece_bbs.begin() + 6, 0, std::bit_or<int>{});
        color_bbs[BLACK] = std::reduce(piece_bbs.begin() + 6, piece_bbs.end(), 0, std::bit_or<int>{});
        all_pieces_bb = color_bbs[WHITE] | color_bbs[BLACK];
    }

    void Board::make_move(jchess::Move const& move) {
        // TODO: how to store enough information to unmake the move?
        // TODO: this should update the enpassant square
        if(move.is_null) {
            return;
        }
        Piece src_piece = pieces[move.source], dest_piece = pieces[move.dest];
        // should probably have a quicker unchecked version as well
        if(src_piece == NO_PIECE) {
            throw std::logic_error("move source square has no piece");
        }
        if(dest_piece != NO_PIECE && color_from_piece(src_piece) == color_from_piece(dest_piece)) {
            throw std::logic_error("trying to capture same color piece");
        }
        Color src_color = color_from_piece(src_piece);
        bb_remove_square(all_pieces_bb, move.source);
        bb_remove_square(color_bbs[src_color], move.source);
        bb_remove_square(piece_bbs[src_piece], move.source);
        pieces[move.source] = NO_PIECE;
        if(dest_piece != NO_PIECE) {
            Color dest_color = color_from_piece(dest_piece);
            bb_remove_square(color_bbs[dest_color], move.dest);
            bb_remove_square(piece_bbs[dest_piece], move.dest);
        }
        Piece target_piece = move.promotion ? move.promotion.value() : src_piece;
        bb_add_square(all_pieces_bb, move.dest);
        bb_add_square(color_bbs[color_from_piece(target_piece)], move.dest);
        bb_add_square(piece_bbs[target_piece], move.dest);
        pieces[move.dest] = target_piece;
        moves.push(move); // this might not make sense, only push the information necessary to unmake
        throw std::logic_error("FORGOT TO IMPLEMENT UPDATING ENP SQUARE")
    }

    std::string Board::to_string() {
        std::ostringstream oss;
        for(int rank = 7; rank >= 0; --rank) {
            for(int file = 0; file < 8; ++ file) {
                Piece piece = pieces[square_from_rank_file(rank, file)];
                oss << char_from_piece(piece);
            }
            oss << std::endl;
        }
        return oss.str();
    }
}