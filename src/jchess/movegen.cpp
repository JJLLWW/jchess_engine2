#include "movegen.h"
#include <cassert>

namespace jchess {
    MoveGenerator::MoveGenerator() {
        king_attacks_tbl = compute_all_king_attacks();
        knight_attacks_tbl = compute_all_knight_attacks();
        pawn_attacks_tbl[WHITE] = compute_all_pawn_attacks(WHITE);
        pawn_attacks_tbl[BLACK] = compute_all_pawn_attacks(BLACK);
        initialise_rectangle_between_tbl();
    }

    std::vector<Move> MoveGenerator::get_legal_moves(BoardState const& state, Color color) {
        // TODO: board state should store where the kings are
        Piece own_king_piece = piece_from(KING, color);
        Bitboard own_king_bb = state.piece_bbs[own_king_piece];
        Square king_sq = bit_scan(own_king_bb, false);

        compute_pin_masks(state, color, king_sq);

        std::vector<Move> moves;
        // detect if in check + double check somehow
        // if double check, just get the king moves
        // if single check, also get moves that capture the checker (HOW?!?!)

        // if not in check:
        // king castles.
        // pawns (hard special case)

        // knights + sliders
        get_all_piece_moves(moves, KNIGHT, state, color);
        get_all_piece_moves(moves, BISHOP, state, color);
        get_all_piece_moves(moves, ROOK, state, color);
        get_all_piece_moves(moves, QUEEN, state, color);

        return moves;
    }

    void MoveGenerator::get_all_piece_moves(std::vector<Move>& moves, PieceType type, BoardState const& state, Color color) {
        assert(type != KING && type != PAWN);
        Bitboard src_bb = state.piece_bbs[piece_from(type, color)];
        Bitboard own = state.color_bbs[color], enemy = state.color_bbs[!color];
        while(src_bb) {
            Square src = bit_scan(src_bb, false);
            Bitboard dests_bb = get_slider_and_knight_moves(type, src, own, enemy) & pin_masks[src];
            auto dests = bb_get_squares(dests_bb);
            for(auto dest : dests) {
                moves.emplace_back(src, dest);
            }
            src_bb &= src_bb - 1;
        }
    }

    Bitboard MoveGenerator::get_slider_and_knight_moves(PieceType piece_type, Square source, Bitboard own_pieces, Bitboard enemy_pieces) {
        assert(piece_type != KING && piece_type != PAWN);
        switch(piece_type) {
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

    Bitboard MoveGenerator::get_king_moves(Square source, BoardState const& state, Color color) {
        Color other_color = (color == WHITE) ? BLACK : WHITE;
        Bitboard potential_moves = king_attacks_tbl[source];
        Bitboard in_check = get_all_attacked_squares(state, other_color);
        return potential_moves & ~(in_check | state.all_pieces_bb);
    }

    Bitboard MoveGenerator::get_knight_moves(Square source, Bitboard own_pieces) {
        return knight_attacks_tbl[source] & ~own_pieces;
    }

    Bitboard MoveGenerator::get_bishop_moves(Square source, Bitboard own_pieces, Bitboard enemy_pieces) {
        Bitboard source_bb = bb_from_squares({source});
        Bitboard blockers_all = (own_pieces | enemy_pieces) & ~source_bb;
        Bitboard blockers = blockers_all & get_bishop_blocker_mask(source);
        Bitboard with_capture_own = magic_db.get_bishop_attacks(blockers, source);
        return with_capture_own & ~own_pieces;
    }

    Bitboard MoveGenerator::get_rook_moves(Square source, Bitboard own_pieces, Bitboard enemy_pieces) {
        Bitboard source_bb = bb_from_squares({source});
        Bitboard blockers_all = (own_pieces | enemy_pieces) & ~source_bb;
        Bitboard blockers = blockers_all & get_rook_blocker_mask(source);
        Bitboard with_capture_own = magic_db.get_rook_attacks(blockers, source);
        return with_capture_own & ~own_pieces;
    }

    Bitboard MoveGenerator::get_queen_moves(Square source, Bitboard own_pieces, Bitboard enemy_pieces) {
        Bitboard bishop_moves = get_bishop_moves(source, own_pieces, enemy_pieces);
        Bitboard rook_moves = get_rook_moves(source, own_pieces, enemy_pieces);
        return bishop_moves | rook_moves;
    }

    Bitboard MoveGenerator::get_all_attacked_squares(BoardState const& state, Color color) {
        Bitboard all_attacked = 0ull;
        Color other_color = (color == WHITE) ? BLACK : WHITE;
        // inefficient
        for(Square square=0; square<64; ++square) {
            Piece piece = state.pieces[square];
            if(piece == NO_PIECE || color_from_piece(piece) != color) {
                continue;
            }
            switch(type_from_piece(piece)) {
                case PAWN:
                    all_attacked |= pawn_attacks_tbl[color][square];
                    break;
                case ROOK:
                    all_attacked |= get_rook_moves(square, state.color_bbs[color], state.color_bbs[other_color]);
                    break;
                case KNIGHT:
                    all_attacked |= get_knight_moves(square, state.color_bbs[color]);
                    break;
                case BISHOP:
                    all_attacked |= get_bishop_moves(square, state.color_bbs[color], state.color_bbs[other_color]);
                    break;
                case QUEEN:
                    all_attacked |= get_queen_moves(square, state.color_bbs[color], state.color_bbs[other_color]);
                    break;
                case KING:
                    all_attacked |= king_attacks_tbl[square];
                    break;
            }
        }
        return all_attacked;
    }

    void MoveGenerator::initialise_rectangle_between_tbl() {
        for(Square sq1=0; sq1<64; ++sq1) {
            for(Square sq2=0; sq2<64; ++sq2) {
                rectangle_between_tbl[sq1][sq2] = detail::get_rectangle_between(sq1, sq2);
            }
        }
    }

    Bitboard MoveGenerator::xray_rook_moves(Bitboard all_pieces, Bitboard blockers, Square rook_sq) {
        // https://www.chessprogramming.org/X-ray_Attacks_(Bitboards)#ModifyingOccupancy
        Bitboard attacks = get_rook_moves(rook_sq, 0ull, all_pieces);
        blockers &= attacks;
        return attacks ^ get_rook_moves(rook_sq, 0ull, all_pieces ^ blockers);
    }

    Bitboard MoveGenerator::xray_bishop_moves(Bitboard all_pieces, Bitboard blockers, Square bishop_sq) {
        Bitboard attacks = get_bishop_moves(bishop_sq, 0ull, all_pieces);
        blockers &= attacks;
        return attacks ^ get_bishop_moves(bishop_sq, 0ull, all_pieces ^ blockers);
    }

    Bitboard MoveGenerator::xray_queen_moves(Bitboard all_pieces, Bitboard blockers, Square queen_sq) {
        return xray_rook_moves(all_pieces, blockers, queen_sq) | xray_bishop_moves(all_pieces, blockers, queen_sq);
    }

    // this pin masks approach may not be a great idea for testability
    void MoveGenerator::compute_pin_masks(BoardState const& state, Color color, Square king_sq) {
        std::fill(pin_masks.begin(), pin_masks.end(), -1ull);

        auto [rank, file] = rank_file_from_square(king_sq);
        Bitboard own_pieces = state.color_bbs[color];
        // op -> opponent
        Piece op_rook = piece_from(ROOK, static_cast<Color>(!color));
        Piece op_bishop = piece_from(BISHOP, static_cast<Color>(!color));
        Piece op_queen = piece_from(QUEEN, static_cast<Color>(!color));
        Bitboard opRQ = state.piece_bbs[op_rook] | state.piece_bbs[op_queen];
        Bitboard opBQ = state.piece_bbs[op_bishop] | state.piece_bbs[op_queen];

        Bitboard potential_pin_squares = xray_queen_moves(state.all_pieces_bb, own_pieces, king_sq);
        for(Direction dir : all_directions) {
            Bitboard dir_ray = RAY_BBS[dir][king_sq];
            Bitboard ray_pin_squares = potential_pin_squares & dir_ray;
            Bitboard op = is_orthogonal_dir(dir) ? opRQ : opBQ;
            Bitboard pinner = ray_pin_squares & op;
            if(pinner) {
                Square pinner_sq = bit_scan(pinner, false);
                Bitboard pinned = dir_ray & rectangle_between_tbl[king_sq][pinner_sq] & own_pieces;
                Square pinned_sq = bit_scan(pinned, false);
                pin_masks[pinned_sq] = dir_ray;
            }
        }
    }

    Bitboard detail::get_rectangle_between(Square sq1, Square sq2) {
        // black magic: https://www.chessprogramming.org/Square_Attacked_By#Obstructed
        const uint64_t m1   = -1ull;
        const uint64_t a2a7 = 0x0001010101010100ull;
        const uint64_t b2g7 = 0x0040201008040200ull;
        const uint64_t h1b7 = 0x0002040810204080ull; /* Thanks Dustin, g2b7 did not work for c1-a3 */
        uint64_t btwn, line, rank, file;

        btwn  = (m1 << sq1) ^ (m1 << sq2);
        file  =   (sq2 & 7) - (sq1   & 7);
        rank  =  ((sq2 | 7) -  sq1) >> 3 ;
        line  =      (   (file  &  7) - 1) & a2a7; /* a2a7 if same file */
        line += 2 * ((   (rank  &  7) - 1) >> 58); /* b1g1 if same rank */
        line += (((rank - file) & 15) - 1) & b2g7; /* b2g7 if same diagonal */
        line += (((rank + file) & 15) - 1) & h1b7; /* h1b7 if same antidiag */
        line *= btwn & -btwn; /* mul acts like shift by smaller square */
        return line & btwn;   /* return the bits on that line in-between */
    }
}