#include "board.h"
#include "movegen.h"

#include <sstream>
#include <numeric>
#include <cassert>
#include <bit> // C++20 popcount

namespace jchess {
    namespace {
        void update_castle_rights_from_corner(BoardState& state, Square corner) {
            if(corner == A1) {
                state.castle_right_mask &= ~WHITE_QS;
            } else if(corner == A8) {
                state.castle_right_mask &= ~BLACK_QS;
            } else if(corner == H1) {
                state.castle_right_mask &= ~WHITE_KS;
            } else if(corner == H8) {
                state.castle_right_mask &= ~BLACK_KS;
            }
        }
    }

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

    std::vector<Move> Board::generate_legal_moves() {
        return movegen.get_legal_moves(board_state, game_state.side_to_move);
    }

    // it may be worth decomposing this into private helpers.
    // does capturing a rook update the castle rights?
    BoardState get_state_after_move(BoardState const& current, Move const& move) {
        BoardState next_state = current;
        next_state.enp_square = std::nullopt; // no enp square unless a double pawn push.
        Piece src_piece = current.pieces[move.source];
        Color side_to_move = color_from_piece(src_piece);

        // moves that change the enp state or castle rights from the previous
        if(type_from_piece(src_piece) == PAWN && move.dest == current.enp_square) {
            // enp capture - involves a capture not at move.dest
            Square enp_capture_square = move.dest + ((side_to_move == WHITE) ? SOUTH : NORTH);
            next_state.remove_piece_from_square(enp_capture_square);
        } else if(type_from_piece(src_piece) == PAWN && vertical_distance(move.source, move.dest) == 2) {
            // double pawn push - creates enp square
            next_state.enp_square = move.source + ((side_to_move == WHITE) ? NORTH : SOUTH);
        } else if(type_from_piece(src_piece) == KING) {
            // king moves - king can no longer castle + castling involves moving associated rook
            auto side_castle_flags = (side_to_move == WHITE) ? (WHITE_QS | WHITE_KS) : (BLACK_QS | BLACK_KS);
            next_state.castle_right_mask &= ~side_castle_flags;
            auto castle_type = get_move_castle_type(current, move);
            if(castle_type) {
                switch(castle_type.value()) {
                    case WHITE_KS:
                        next_state.remove_piece_from_square(H1);
                        next_state.place_piece_on_square(W_ROOK, F1);
                        break;
                    case WHITE_QS:
                        next_state.remove_piece_from_square(A1);
                        next_state.place_piece_on_square(W_ROOK, D1);
                        break;
                    case BLACK_KS:
                        next_state.remove_piece_from_square(H8);
                        next_state.place_piece_on_square(B_ROOK, F8);
                        break;
                    case BLACK_QS:
                        next_state.remove_piece_from_square(A8);
                        next_state.place_piece_on_square(B_ROOK, D8);
                        break;
                }
            }
        } else if(type_from_piece(src_piece) == ROOK && is_corner_square(move.source)) {
            // moving a rook from a corner will change the castling rights.
            update_castle_rights_from_corner(next_state, move.source);
        }

        // update castle rights when potentially capturing an unmoved rook
        if(is_corner_square(move.dest)) {
            update_castle_rights_from_corner(next_state, move.dest);
        }

        // handle moving the piece and possible promotions.
        Piece dest_piece = move.promotion == NO_PIECE ? src_piece : move.promotion;
        next_state.remove_piece_from_square(move.source);
        next_state.remove_piece_from_square(move.dest); // cryptic
        next_state.place_piece_on_square(dest_piece, move.dest);
        return next_state;
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