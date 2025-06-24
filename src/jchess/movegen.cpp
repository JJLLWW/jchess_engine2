#include "movegen.h"
#include "bitboard.h"
#include "moves.h"

#include <bit>
#include <cassert>

namespace jchess {
    namespace {
        void append_king_castle_moves(MoveVector &moves, BoardState const &state, Bitboard attacked, Color color) {
            Square king_sq = (color == WHITE) ? E1 : E8;
            if (can_castle(state, color, true, attacked)) { // queenside
                Square king_dest = king_sq + WEST + WEST;
                moves.emplace_back(king_sq, king_dest);
            }

            if (can_castle(state, color, false, attacked)) { // kingside
                Square king_dest = king_sq + EAST + EAST;
                moves.emplace_back(king_sq, king_dest);
            }
        }

        void append_moves_from_dest_bb(MoveVector &moves, Square source, Bitboard dest_bb) {
            Square dest;
            while(pop_lsb_square(dest_bb, dest)) {
                moves.emplace_back(source, dest);
            }
        }

        Bitboard get_slider_and_knight_moves(
            PieceType piece_type,
            Square source,
            Bitboard own_pieces,
            Bitboard enemy_pieces
            ) {
            assert(piece_type != KING && piece_type != PAWN);
            switch (piece_type) {
                case KNIGHT:
                    return get_knight_moves(source, own_pieces);
                case ROOK:
                    return get_rook_moves(source, own_pieces, enemy_pieces);
                case BISHOP:
                    return get_bishop_moves(source, own_pieces, enemy_pieces);
                case QUEEN:
                    return get_queen_moves(source, own_pieces, enemy_pieces);
                default:
                    assert(false);
            }
        }

        bool in_check_after_enp(Square src, Square other_pawn, BoardState const &state, Color color) {
            Bitboard blockers = state.all_pieces_bb;
            blockers &= ~(bb_from_square(src) | bb_from_square(other_pawn));
            Square king_sq = state.king_sq[color];
            Bitboard checkers = get_rook_moves(king_sq, 0ull, blockers) & state.orth_slider_bb[other_color(color)];
            return static_cast<bool>(checkers);
        }

        bool can_enp_capture(BoardState const& state, Square checker_sq, Color color) {
            return (type_from_piece(state.pieces[checker_sq]) == PAWN) &&
                state.enp_square.has_value() &&
                state.enp_square.value() + (color == WHITE ? SOUTH : NORTH) == checker_sq;
        }
    } // anonymous namespace end

    void MoveGenerator::get_legal_moves(MoveVector& moves, BoardState const& state, Color color, GenPolicy policy) {
        Bitboard checkers = get_attackers_of(state.king_sq[color], state, !color);
        int num_checkers = std::popcount(checkers);

        compute_dest_masks(state, color, checkers, policy); // handle all information about pins here

        if(policy != GenPolicy::ONLY_CAPTURES) {
            get_king_non_castle_moves(moves, state, color);
        }

        if (num_checkers >= 2) {
            return; //. if in double check can only move the king.
        }

        if (num_checkers == 0 && policy != GenPolicy::ONLY_CAPTURES) {
            // castling only possible if not in check
            Bitboard attacked = get_all_attacked_squares(state, !color);
            append_king_castle_moves(moves, state, attacked, color);
        }

        get_all_piece_moves(moves, KNIGHT, state, color);
        get_all_piece_moves(moves, BISHOP, state, color);
        get_all_piece_moves(moves, ROOK, state, color);
        get_all_piece_moves(moves, QUEEN, state, color);
        get_all_pawn_moves(moves, state, color);
    }

    void MoveGenerator::get_all_pawn_moves(MoveVector &moves, BoardState const &state, Color color) {
        Bitboard pawns_bb = state.piece_bbs[PAWN | color];
        while (pawns_bb) {
            Square src = lsb_square_from_bb(pawns_bb);
            Bitboard dests = get_pawn_moves(src, state, color);
            Bitboard promote = back_rank_bb[color] & dests;
            append_moves_from_dest_bb(moves, src, dests & ~promote);
            Square dest;
            while(pop_lsb_square(promote, dest)) {
                PieceType promotions[4] {KNIGHT, BISHOP, ROOK, QUEEN};
                for (PieceType promotion: promotions) {
                    moves.emplace_back(src, dest, promotion);
                }
            }
            pawns_bb &= pawns_bb - 1;
        }
    }

    Bitboard MoveGenerator::get_pawn_moves(Square square, BoardState const &state, Color color) {
        Direction push_dir = (color == WHITE) ? NORTH : SOUTH;
        Bitboard attacks = PAWN_ATTACKS[color][square] & state.color_bbs[!color];
        Bitboard enp = 0ull;
        if (state.enp_square.has_value()) {
            Bitboard potential_enp = PAWN_ATTACKS[color][square] & bb_from_square(state.enp_square.value());
            Square other_pawn = state.enp_square.value() + ((color == WHITE) ? SOUTH : NORTH);
            // some obscure positions can move into check after enp
            if (potential_enp && !in_check_after_enp(square, other_pawn, state, color)) {
                enp = potential_enp;
            }
        }
        Bitboard push_one = bb_from_square(square + push_dir) & ~state.all_pieces_bb;
        Bitboard push_two = 0ull;
        if (push_one && (bb_from_square(square) & pawn_start_bb[color])) {
            push_two = bb_from_square(square + push_dir + push_dir) & ~state.all_pieces_bb;
        }
        return (attacks | enp | push_one | push_two) & allowed_dest_mask[square];
    }

    void MoveGenerator::get_king_non_castle_moves(MoveVector& moves, BoardState const&state, Color color) {
        Square king_sq = state.king_sq[color];
        Bitboard potential_moves = KING_ATTACKS[king_sq];
        Bitboard in_check = get_all_attacked_squares(state, !color);
        Bitboard dests = potential_moves & ~(in_check | state.color_bbs[color]);
        append_moves_from_dest_bb(moves, king_sq, dests);
    }

    Bitboard get_all_attacked_squares(BoardState const &state, Color color) {
        Bitboard all_attacked = 0ull;

        all_attacked |= KING_ATTACKS[state.king_sq[color]];
        Bitboard pawns = state.piece_bbs[PAWN | color];
        Bitboard knights = state.piece_bbs[KNIGHT | color];
        Bitboard rooks = state.piece_bbs[ROOK | color];
        Bitboard bishops = state.piece_bbs[BISHOP | color];
        Bitboard queens = state.piece_bbs[QUEEN | color];
        Square sq;

        // xray through the opponents king for purpose of correctly determining squares that are in check
        Bitboard pieces_ignoring_king = state.all_pieces_bb & ~bb_from_square(state.king_sq[!color]);
        while(pop_lsb_square(pawns, sq)) {
            all_attacked |= PAWN_ATTACKS[color][sq];
        }
        while(pop_lsb_square(rooks, sq)) {
            all_attacked |= get_rook_moves(sq, 0ull, pieces_ignoring_king);
        }
        while(pop_lsb_square(knights, sq)) {
            all_attacked |= get_knight_moves(sq, 0ull);
        }
        while(pop_lsb_square(bishops, sq)) {
            all_attacked |= get_bishop_moves(sq, 0ull, pieces_ignoring_king);
        }
        while(pop_lsb_square(queens, sq)) {
            all_attacked |= get_queen_moves(sq, 0ull, pieces_ignoring_king);
        }
        return all_attacked;
    }

    void MoveGenerator::compute_dest_masks(BoardState const &state, Color color, Bitboard checker, GenPolicy policy) {
        Square king_sq = state.king_sq[color];
        Bitboard own_pieces = state.color_bbs[color];

        Square checker_sq = A1;
        Piece checker_piece = NO_PIECE;

        // if we're in check, every piece can only capture the checker or move "in front" of it.
        Bitboard default_mask = -1ull;
        if(checker != 0ull) {
            checker_sq = lsb_square_from_bb(checker);
            checker_piece = state.pieces[checker_sq];
            default_mask = bb_from_square(checker_sq);
            if(is_slider(checker_piece)) {
                default_mask |= segment_between(king_sq, checker_sq);
            }
        }

        if(policy == GenPolicy::ONLY_CAPTURES) {
            default_mask &= state.color_bbs[!color];
        }

        std::fill(allowed_dest_mask.begin(), allowed_dest_mask.end(), default_mask);

        // case where a pawn can enp capture the checker (inefficient but rare)
        if(checker != 0ull && can_enp_capture(state, checker_sq, color)) {
            Bitboard cap_pawns = PAWN_ATTACKS[!color][state.enp_square.value()] & state.piece_bbs[PAWN | color];
            Square sq;
            while (pop_lsb_square(cap_pawns, sq)) {
                allowed_dest_mask[sq] |= bb_from_square(state.enp_square.value());
            }
        }

        // op -> opponent
        Bitboard opRQ = state.orth_slider_bb[!color];
        Bitboard opBQ = state.diag_slider_bb[!color];
        Bitboard potential_pin_squares = xray_queen_moves(state.all_pieces_bb, own_pieces, king_sq);

        // pinned pieces.
        for (Direction dir: all_directions) {
            Bitboard dir_ray = RAY_BBS[dir][king_sq];
            Bitboard ray_pin_squares = potential_pin_squares & dir_ray;
            Bitboard op = is_orthogonal_dir(dir) ? opRQ : opBQ;
            Bitboard pinner = ray_pin_squares & op;
            if (pinner) {
                Square pinner_sq = lsb_square_from_bb(pinner);
                Bitboard pinned = dir_ray & RECTANGLE_BETWEEN[king_sq][pinner_sq] & own_pieces;
                if (pinned) {
                    Square pinned_sq = lsb_square_from_bb(pinned);
                    allowed_dest_mask[pinned_sq] &= dir_ray;// should be &= in case of double pin.
                }
            }
        }
    }

    void MoveGenerator::get_all_piece_moves(MoveVector& moves, PieceType type, BoardState const &state, Color color) {
        assert(type != KING && type != PAWN);
        Bitboard src_bb = state.piece_bbs[type | color];
        Bitboard own = state.color_bbs[color], enemy = state.color_bbs[!color];
        while (src_bb) {
            Square src = lsb_square_from_bb(src_bb);
            Bitboard dests_bb = get_slider_and_knight_moves(type, src, own, enemy) & allowed_dest_mask[src];
            append_moves_from_dest_bb(moves, src, dests_bb);
            src_bb &= src_bb - 1;
        }
    }
}
