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
        Piece src_piece = pieces[move.source], dest_piece = pieces[move.dest];
        // should probably have a more performant unchecked version.
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

        throw std::logic_error("NOT IMPLEMENTED");
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