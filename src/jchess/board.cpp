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

    std::optional<CastleBits> get_move_castle_type(BoardState const& state, Move const& move) {
        // better way?
        Piece src_piece = state.pieces[move.source];
        if(src_piece == W_KING) {
            if(move.source == E1 && move.dest == G1 && state.pieces[H1] == W_ROOK) {
                return WHITE_KS;
            } else if(move.source == E1 && move.dest == C1 && state.pieces[A1] == W_ROOK) {
                return WHITE_QS;
            }
        }
        if(src_piece == B_KING) {
            if(move.source == E8 && move.dest == G8 && state.pieces[H8] == B_ROOK) {
                return BLACK_KS;
            } else if(move.source == E8 && move.dest == C8 && state.pieces[A8] == B_ROOK) {
                return BLACK_QS;
            }
        }
        return std::nullopt;
    }

    BoardState::BoardState(FEN const& fen) {
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

    // it may be worth decomposing this into private helpers.
    BoardState get_state_after_move(BoardState const& current, Move const& move) {
        BoardState next_state = current;
        next_state.enp_square = std::nullopt; // no enp square unless a double pawn push.
        Piece src_piece = current.pieces[move.source];
        Color side_to_move = color_from_piece(src_piece);

        // moves that change the enp state or castle rights from the previous
        if(type_from_piece(src_piece) == PAWN && move.dest == current.enp_square) {
            // enp capture - involves a capture not at move.dest
            Square enp_capture_square = move.dest + (side_to_move == WHITE) ? DOWN : UP;
            next_state.remove_piece_from_square(enp_capture_square);
        } else if(type_from_piece(src_piece) == PAWN && vertical_distance(move.source, move.dest) == 2) {
            // double pawn push - creates enp square
            next_state.enp_square = move.source + (side_to_move == WHITE) ? UP : DOWN;
        } else if(type_from_piece(src_piece) == KING) {
            // king moves - king can no longer castle + castling involves moving associated rook
            next_state.castle_right_mask = 0;
            auto castle_type = get_move_castle_type(current, move);
            if(castle_type) {
                switch(castle_type.value()) {
                    case WHITE_KS:
                        next_state.remove_piece_from_square(A8);
                        next_state.place_piece_on_square(W_ROOK, A6);
                        break;
                    case WHITE_QS:
                        next_state.remove_piece_from_square(A1);
                        next_state.place_piece_on_square(W_ROOK, A4);
                        break;
                    case BLACK_KS:
                        next_state.remove_piece_from_square(H8);
                        next_state.place_piece_on_square(B_ROOK, H6);
                        break;
                    case BLACK_QS:
                        next_state.remove_piece_from_square(H1);
                        next_state.place_piece_on_square(B_ROOK, H4);
                        break;
                }
            }
        } else if(type_from_piece(src_piece) == ROOK && is_corner_square(move.source)) {
            // moving a rook from a corner will change the castling rights.
            if(move.source == A1) {
                next_state.castle_right_mask &= ~WHITE_QS;
            } else if(move.source == A8) {
                next_state.castle_right_mask &= ~WHITE_KS;
            } else if(move.source == H1) {
                next_state.castle_right_mask &= ~BLACK_QS;
            } else if(move.source == H8) {
                next_state.castle_right_mask &= ~BLACK_KS;
            }
        }

        // handle moving the piece and possible promotions.
        Piece dest_piece = move.promotion == NO_PIECE ? src_piece : move.promotion;
        next_state.remove_piece_from_square(move.source);
        next_state.place_piece_on_square(dest_piece, move.dest);
        return next_state;
    }

    void BoardState::remove_piece_from_square(Square square) {
        Piece piece = pieces[square];
        if(piece != NO_PIECE) {
            Color piece_color = color_from_piece(piece);
            bb_remove_square(all_pieces_bb, square);
            bb_remove_square(color_bbs[piece_color], square);
            bb_remove_square(piece_bbs[piece], square);
            pieces[square] = NO_PIECE;
        }
    }

    void BoardState::place_piece_on_square(Piece piece, Square square) {
        Color piece_color = color_from_piece(piece);
        bb_add_square(all_pieces_bb, square);
        bb_add_square(color_bbs[piece_color], square);
        bb_add_square(piece_bbs[piece], square);
        pieces[square] = piece;
    }

    void Board::set_position(const FEN &fen) {
        game_state = GameState(fen);
        board_state = BoardState(fen);
    }

    void Board::make_move(jchess::Move const& move) {
        prev_board_states.push(board_state);
        board_state = get_state_after_move(board_state, move);
        game_state.increase_half_move();
    }

    bool Board::unmake_move() {
        if(prev_board_states.empty()) {
            return false;
        }
        board_state = prev_board_states.top();
        prev_board_states.pop();
        game_state.decrease_half_move();
        return true;
    }

    std::string Board::to_string() {
        std::ostringstream oss;
        for(int rank = 7; rank >= 0; --rank) {
            for(int file = 0; file < 8; ++ file) {
                Piece piece = board_state.pieces[square_from_rank_file(rank, file)];
                oss << char_from_piece(piece);
            }
            oss << std::endl;
        }
        return oss.str();
    }
}