#include "board.h"

#include <sstream>
#include <numeric>
#include <cassert>

namespace jchess {
    GameState::GameState(FEN const& fen) {
        half_moves = fen.half_moves;
        full_moves = fen.full_moves;
        side_to_move = fen.side_to_move;
    }

    void GameState::increase_half_move() {
        ++half_moves;
        if(side_to_move == BLACK) {
            ++full_moves;
        }
        side_to_move = (side_to_move == WHITE) ? BLACK : WHITE;
    }

    void GameState::decrease_half_move() {
        assert(half_moves != 0);
        --half_moves;
        if(side_to_move == WHITE) {
            --full_moves;
        }
        side_to_move = (side_to_move == WHITE) ? BLACK : WHITE;
    }

    void Board::remove_piece_from_square(Square square) {
        Piece piece = pieces[square];
        assert(piece != NO_PIECE);
        Color piece_color = color_from_piece(piece);
        bb_remove_square(all_pieces_bb, square);
        bb_remove_square(color_bbs[piece_color], square);
        bb_remove_square(piece_bbs[piece], square);
        pieces[square] = NO_PIECE;
    }

    void Board::place_piece_on_square(Piece piece, Square square) {
        Color piece_color = color_from_piece(piece);
        bb_add_square(all_pieces_bb, square);
        bb_add_square(color_bbs[piece_color], square);
        bb_add_square(piece_bbs[piece], square);
        pieces[square] = piece;
    }

    void Board::set_position(const FEN &fen) {
        // TODO: cleanup.
        game_state = GameState(fen);
        castle_right_mask = fen.castle_right_mask;
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

    std::optional<CastleBits> Board::get_move_castle_type(const Move &move) {
        // better way?
        Piece src_piece = pieces[move.source];
        if(src_piece == W_KING) {
            if(move.source == E1 && move.dest == G1 && pieces[H1] == W_ROOK) {
                return WHITE_KS;
            } else if(move.source == E1 && move.dest == C1 && pieces[A1] == W_ROOK) {
                return WHITE_QS;
            }
        }
        if(src_piece == B_KING) {
            if(move.source == E8 && move.dest == G8 && pieces[H8] == B_ROOK) {
                return BLACK_KS;
            } else if(move.source == E8 && move.dest == C8 && pieces[A8] == B_ROOK) {
                return BLACK_QS;
            }
        }
        return std::nullopt;
    }

    std::optional<Square> Board::get_new_enp_square(Move const& move) {
        // better way?
        if(pieces[move.source] == W_PAWN) {
            if(move.dest - move.source == 2 * UP) {
                return static_cast<Square>(move.source + UP);
            }
        } else if(pieces[move.source] == B_PAWN) {
            ;if(move.dest - move.source == 2 * DOWN) {
                return static_cast<Square>(move.source + DOWN);
            }
        }
        return std::nullopt;
    }

    void Board::make_castle_move(CastleBits castle) {
        Square src_king, src_rook, dest_king, dest_rook;
        Piece king = (castle == WHITE_KS || castle == WHITE_QS) ? W_KING : B_KING;
        Piece rook = (castle == WHITE_KS || castle == WHITE_QS) ? W_ROOK : B_ROOK;
        // better way?
        switch(castle) {
            case WHITE_KS:
                src_king = E1, dest_king = G1;
                src_rook = H1, dest_rook = F1;
                castle_right_mask &= ~WHITE_KS;
                break;
            case WHITE_QS:
                src_king = E1, dest_king = C1;
                src_rook = A1, dest_rook = D1;
                castle_right_mask &= ~WHITE_QS;
                break;
            case BLACK_KS:
                src_king = E8, dest_king = G8;
                src_rook = H8, dest_rook  = F8;
                castle_right_mask &= ~BLACK_KS;
                break;
            case BLACK_QS:
                src_king = E8, dest_king = C8;
                src_rook = A8, dest_rook = D8;
                castle_right_mask &= ~BLACK_QS;
                break;
        }
        remove_piece_from_square(src_king);
        remove_piece_from_square(src_rook);
        place_piece_on_square(king, dest_king);
        place_piece_on_square(rook, dest_rook);
        enp_square = std::nullopt;
    }

    UnMove Board::get_regular_unmove(Move const& move) {
        UnMove unmove {castle_right_mask, enp_square};
        unmove.add_place_piece(move.source, pieces[move.source]);
        if(pieces[move.dest] != NO_PIECE) {
            unmove.add_place_piece(move.dest, pieces[move.dest]);
        }
        unmove.add_clear_square(move.dest);
        return unmove;
    }

    void Board::make_regular_move(Move const& move) {
        Piece src_piece = pieces[move.source], dest_piece = pieces[move.dest];
        Piece target_piece = move.promotion != NO_PIECE ? move.promotion : src_piece;
        remove_piece_from_square(move.source);
        if(dest_piece != NO_PIECE) {
            remove_piece_from_square(move.dest);
        }
        place_piece_on_square(target_piece, move.dest);
        enp_square = get_new_enp_square(move);
    }

    bool Board::is_enp_capture(jchess::Move const& move) {
        if(!enp_square) {
            return false;
        }
        Piece src_piece = pieces[move.source];
        // a pawn can't move forwards into the enp square so this is a good enough check.
        if(src_piece == W_PAWN || src_piece == B_PAWN && enp_square.value() == move.dest) {
            return true;
        }
        return false;
    }

    UnMove Board::get_enp_unmove(Move const& move) {
        UnMove unmove {castle_right_mask, enp_square};
        Direction capture_dir = (game_state.side_to_move == WHITE) ? DOWN : UP;
        auto capture_square = static_cast<Square>(enp_square.value() + capture_dir);
        unmove.add_place_piece(capture_square, pieces[capture_square]);
        unmove.add_place_piece(move.source, pieces[move.source]);
        unmove.add_clear_square(enp_square.value());
        return unmove;
    }

    void Board::make_enp_move(jchess::Move const& move) {
        assert(enp_square.has_value());
        Direction capture_dir = (game_state.side_to_move == WHITE) ? DOWN : UP;
        Piece source_pawn = pieces[move.source];
        auto capture_square = static_cast<Square>(enp_square.value() + capture_dir);
        remove_piece_from_square(capture_square);
        remove_piece_from_square(move.source);
        place_piece_on_square(pieces[source_pawn], enp_square.value());

        enp_square = std::nullopt;
    }

    void Board::make_move(jchess::Move const& move) {
        auto castle_type = get_move_castle_type(move);
        if(move.is_null_move) {
            throw std::invalid_argument("null moves not supported");
        } else if(castle_type) {
            // this is a bit cryptic.
            unmoves.emplace(castle_right_mask, enp_square, castle_type.value());
            make_castle_move(castle_type.value());
        } else if(is_enp_capture(move)) {
            unmoves.push(get_enp_unmove(move));
            make_enp_move(move);
        } else {
            unmoves.push(get_regular_unmove(move));
            make_regular_move(move);
        }

        game_state.increase_half_move();
    }

    void Board::apply_unmove(jchess::UnMove const& unmove) {
        for(int i=0; i<unmove.num_clear; ++i) {
            remove_piece_from_square(unmove.clear_squares[i]);
        }
        for(int i=0; i<unmove.num_place; ++i) {
            place_piece_on_square(unmove.place_pieces[i], unmove.place_squares[i]);
        }
        castle_right_mask = unmove.castle_right_mask;
        enp_square = unmove.enp_state;

        game_state.decrease_half_move();
    }

    bool Board::unmake_move() {
        if(unmoves.empty()) {
            return false;
        }
        UnMove unmove = unmoves.top();
        unmoves.pop();
        apply_unmove(unmove);
        return true;
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