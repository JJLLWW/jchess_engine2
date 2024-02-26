#include "movegen.h"
#include "bitboard.h"

#include <cassert>

namespace jchess {
    // how to handle the obscure enp check move? either K..Pp..(ORTH) or (ORTH)..pP..
    // removing p and P this becomes K.....(ORTH) or (ORTH)....K (can check)
    void MoveGenerator::append_moves_from_source_bb(std::vector<Move>& moves, Square dest, Bitboard source_bb) {
        auto srcs = bb_get_squares(source_bb);
        for(auto src: srcs) {
            assert(cur_state.pieces[src] != NO_PIECE);
            moves.emplace_back(src, dest);
        }
    }
    void MoveGenerator::append_moves_from_dest_bb(std::vector<Move> &moves, Square source, Bitboard dest_bb) {
        auto dests = bb_get_squares(dest_bb);
        assert(cur_state.pieces[source] != NO_PIECE);
        for (auto dest: dests) {
            moves.emplace_back(source, dest);
        }
    }

    void append_king_castle_moves(std::vector<Move>& moves, BoardState const &state, Bitboard attacked, Color color) {
        Square king_sq = (color == WHITE) ? E1 : E8;
        if (can_castle(state, color, true, attacked)) { // queenside
            Square king_dest = king_sq + 2*offset_of_dir[WEST];
            moves.emplace_back(king_sq, king_dest);
        }

        if (can_castle(state, color, false, attacked)) { // kingside
            Square king_dest = king_sq + 2*offset_of_dir[EAST];
            moves.emplace_back(king_sq, king_dest);
        }
    }

    Square get_king_square(BoardState const& state, Color color) {
        Piece own_king_piece = piece_from(KING, color);
        Bitboard own_king_bb = state.piece_bbs[own_king_piece];
        return bit_scan(own_king_bb, false);
    }

    MoveGenerator::MoveGenerator() {
        king_attacks_tbl = compute_all_king_attacks();
        knight_attacks_tbl = compute_all_knight_attacks();
        pawn_attacks_tbl[WHITE] = compute_all_pawn_attacks(WHITE);
        pawn_attacks_tbl[BLACK] = compute_all_pawn_attacks(BLACK);
        initialise_rectangle_between_tbl();
    }

    bool MoveGenerator::in_check_after_enp(Square src, Square other_pawn, BoardState const& state, Color color) {
        Bitboard blockers = state.all_pieces_bb;
        // WRONG
        blockers &= ~(bb_from_square(src) | bb_from_square(other_pawn));
        Bitboard king = state.piece_bbs[piece_from(KING, color)];
        Square king_sq = bit_scan(king, false);
        Bitboard checkers = get_rook_moves(king_sq, 0ull, blockers) & state.orth_slider_bb[other_color(color)];
        return static_cast<bool>(checkers);
    }

    Bitboard MoveGenerator::get_ray_between(Square sq1, Square sq2) {
        const auto [r1, f1] = rank_file_from_square(sq1);
        const auto [r2, f2] = rank_file_from_square(sq2);
        if(r1 == r2) {
            return rectangle_between_tbl[sq1][sq2] & RANK_BBS[r1];
        } else if(f1 == f2) {
            return rectangle_between_tbl[sq1][sq2] & FILE_BBS[f1];
        } else if(diagonal_from_square(sq1) == diagonal_from_square(sq2)) {
            return rectangle_between_tbl[sq1][sq2] & DIAG_BBS[diagonal_from_square(sq1)];
        } else if(antidiag_from_square(sq1) == antidiag_from_square(sq2)) {
            return rectangle_between_tbl[sq1][sq2] & ANTI_DIAG_BBS[antidiag_from_square(sq1)];
        }
        assert(false);
    }


    std::vector<Move> MoveGenerator::get_legal_moves(BoardState& state, Color color) {
        // TODO: board state should store where the kings are
        king_sq = get_king_square(state, color);

        compute_pin_info(state, color, king_sq);
        cur_state = state; // not really used yet - future refactor

        std::vector<Move> moves;

        Bitboard checkers = get_attackers_of(king_sq, state, static_cast<Color>(!color));
        int num_checkers = std::popcount(checkers);
        bool in_check = (num_checkers > 0);
        bool in_double_check = (num_checkers > 1);
        Square checker_sq = in_check ? bit_scan(checkers, false) : 0;

        Bitboard king_dests = get_king_non_castle_moves(king_sq, state, color);
        append_moves_from_dest_bb(moves, king_sq, king_dests);

        if(!in_double_check) {
            if(!in_check) {
                // castling only possible if not in check
                Bitboard attacked = get_all_attacked_squares(state, static_cast<Color>(!color));
                append_king_castle_moves(moves, state, attacked, color);
            } else {
                // update all pieces pin squares from the single checker. (should be function)
                Bitboard checker_squares = bb_from_square(checker_sq);
                if(type_from_piece(state.pieces[checker_sq]) != KNIGHT) {
                    checker_squares |= get_ray_between(king_sq, checker_sq);
                }
                // complicated due to special case of enp capturing a checker
                bool can_enp_capture =
                    (type_from_piece(state.pieces[checker_sq]) == PAWN) &&
                    state.enp_square.has_value() &&
                    state.enp_square.value() + (color == WHITE ? offset_of_dir[SOUTH] : offset_of_dir[NORTH]) == checker_sq;
                for(Square sq=0; sq<64; ++sq) {
                    pin_masks[sq] &= checker_squares;
                    if(can_enp_capture && state.pieces[sq] == piece_from(PAWN, color)) {
                        pin_masks[sq] |= bb_from_square(state.enp_square.value());
                    }
                }
            }

            get_all_piece_moves(moves, KNIGHT, state, color);
            get_all_piece_moves(moves, BISHOP, state, color);
            get_all_piece_moves(moves, ROOK, state, color);
            get_all_piece_moves(moves, QUEEN, state, color);
            get_all_pawn_moves(moves, state, color);

        }
        return moves;
    }

    void MoveGenerator::get_all_pawn_moves(std::vector<Move>& moves, BoardState const& state, Color color) {
        Bitboard pawns_bb = state.piece_bbs[piece_from(PAWN, color)];
        while(pawns_bb) {
            Square src = bit_scan(pawns_bb, false);
            Bitboard dests = get_pawn_moves(src, state, color);
            Bitboard promote = back_rank_bb[color] & dests;
            append_moves_from_dest_bb(moves, src, dests & ~promote);
            for(Square dest : bb_get_squares(promote)) {
                for(Piece promotion : all_promotion[color]) {
                    moves.emplace_back(src, dest, promotion);
                }
            }
            pawns_bb &= pawns_bb - 1;
        }
    }

    Bitboard MoveGenerator::get_pawn_moves(Square square, BoardState const& state, Color color) {
        // attacks (enp attack not handled)
        Color other_color = static_cast<Color>(!color);
        Direction push_dir = (color == WHITE) ? NORTH : SOUTH;
        Bitboard attacks = pawn_attacks_tbl[color][square] & state.color_bbs[other_color];
        Bitboard enp = 0ull;
        if(state.enp_square.has_value()) {
            Bitboard potential_enp = pawn_attacks_tbl[color][square] & bb_from_square(state.enp_square.value());
            Square other_pawn = state.enp_square.value() + offset_of_dir[(color == WHITE) ? SOUTH : NORTH];
            // some obscure positions can move into check after enp
            if(potential_enp && !in_check_after_enp(square, other_pawn, state, color)) {
                enp = potential_enp;
            }
        }
        Bitboard push_one = bb_from_square(square + offset_of_dir[push_dir]) & ~state.all_pieces_bb;
        Bitboard push_two = 0ull;
        if(push_one && (bb_from_square(square) & pawn_start_bb[color])) {
            push_two = bb_from_square(square + 2*offset_of_dir[push_dir]) & ~state.all_pieces_bb;
        }
        return (attacks | enp | push_one | push_two) & pin_masks[square];
    }

    void MoveGenerator::get_all_piece_moves(std::vector<Move>& moves, PieceType type, BoardState const& state, Color color) {
        assert(type != KING && type != PAWN);
        Bitboard src_bb = state.piece_bbs[piece_from(type, color)];
        Bitboard own = state.color_bbs[color], enemy = state.color_bbs[!color];
        while(src_bb) {
            Square src = bit_scan(src_bb, false);
            Bitboard dests_bb = get_slider_and_knight_moves(type, src, own, enemy) & pin_masks[src];
            append_moves_from_dest_bb(moves, src, dests_bb);
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

    Bitboard MoveGenerator::get_king_non_castle_moves(Square source, BoardState& state, Color color) {
        Color other_color = (color == WHITE) ? BLACK : WHITE;
        Bitboard potential_moves = king_attacks_tbl[source];
        // TEMPORARY (to get squares attacked once the king has moved, remove it from the board)
        state.remove_piece_from_square(king_sq);
        // does this take into account if the other side has one of their own pieces on the square?
        Bitboard in_check = get_all_attacked_squares(state, other_color);
        state.place_piece_on_square(piece_from(KING, color), king_sq);
        // END TEMPORARY
        // it may be possible to capture with the king
        return potential_moves & ~(in_check | state.color_bbs[color]);
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

    // this is only used for detecting if the king is in check or if it can castle, should allow capturing
    // own pieces for these purposes
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
                    all_attacked |= get_rook_moves(square, 0ull, state.all_pieces_bb);
                    break;
                case KNIGHT:
                    all_attacked |= get_knight_moves(square, 0ull);
                    break;
                case BISHOP:
                    all_attacked |= get_bishop_moves(square, 0ull, state.all_pieces_bb);
                    break;
                case QUEEN:
                    all_attacked |= get_queen_moves(square, 0ull, state.all_pieces_bb);
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

    void MoveGenerator::compute_pin_info(BoardState const& state, Color color, Square king_sq) {
        std::fill(pin_masks.begin(), pin_masks.end(), -1ull);
        pinned_bb = 0ull;

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
                pinned_bb |= pinned;
                Square pinned_sq = bit_scan(pinned, false);
                pin_masks[pinned_sq] &= dir_ray; // should be &= in case of double pin.
            }
        }
    }

    // does this need to be a seperate function?
    Bitboard MoveGenerator::get_unpinned_attackers_of(Square square, BoardState const& state, Color color) {
        Bitboard attackers = get_attackers_of(square, state, color);
        return attackers & !pinned_bb;
    }

    Bitboard MoveGenerator::get_attackers_of(Square square, BoardState const& state, Color color) {
        Color other_color = static_cast<Color>(!color);
        Bitboard diag_sliders = state.piece_bbs[piece_from(BISHOP, color)] | state.piece_bbs[piece_from(QUEEN, color)];
        Bitboard orth_sliders = state.piece_bbs[piece_from(ROOK, color)] | state.piece_bbs[piece_from(QUEEN, color)];
        Bitboard knights = knight_attacks_tbl[square] & state.piece_bbs[piece_from(KNIGHT, color)];
        // these are wrong, own_pieces used only to avoid capture squares
        Bitboard ortho = get_rook_moves(square, 0ull, state.all_pieces_bb) & orth_sliders;
        Bitboard diag = get_bishop_moves(square, 0ull, state.all_pieces_bb) & diag_sliders;
        Bitboard pawn = pawn_attacks_tbl[other_color][square] & state.piece_bbs[piece_from(PAWN, color)];
        return knights | ortho | diag | pawn;
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