#include "fathom_third_party/tbprobe.h"
#include "sz_wrapper.h"

namespace jchess::syzgy {
    namespace {
        unsigned to_fathom_castling(int castle_rights) {
            unsigned fathom = 0;
            if(castle_rights & WHITE_QS) {
                fathom |= TB_CASTLING_Q;
            }
            if(castle_rights & WHITE_KS) {
                fathom |= TB_CASTLING_K;
            }
            if(castle_rights & BLACK_QS) {
                fathom |= TB_CASTLING_q;
            }
            if(castle_rights & BLACK_KS) {
                fathom |= TB_CASTLING_k;
            }
            return fathom;
        }

        WDL wdl_from_fathom(unsigned fathom_wdl_res) {
            return static_cast<WDL>(fathom_wdl_res);
        }

        std::optional<PieceType> piece_type_from_fathom(unsigned fathom_promotion, Color color) {
            switch (fathom_promotion) {
                case TB_PROMOTES_NONE:
                    return std::nullopt;
                case TB_PROMOTES_QUEEN:
                    return QUEEN;
                case TB_PROMOTES_ROOK:
                    return ROOK;
                case TB_PROMOTES_BISHOP:
                    return BISHOP;
                case TB_PROMOTES_KNIGHT:
                    return KNIGHT;
                default:
                    assert(false && "corrupted fathom dtz table promotion piece");
            }
        }

        struct FathomArguments {
            Bitboard white;
            Bitboard black;
            Bitboard kings;
            Bitboard queens;
            Bitboard rooks;
            Bitboard bishops;
            Bitboard knights;
            Bitboard pawns;
            unsigned rule50;
            unsigned castling;
            unsigned ep;
            unsigned turn;
            static FathomArguments from_board(Board const& board) {
                FathomArguments args {};
                auto bstate = board.get_board_state();
                auto gstate = board.get_game_state();

                args.white = bstate.color_bbs[WHITE];
                args.black = bstate.color_bbs[BLACK];
                args.kings = bstate.piece_bbs[KING | WHITE] | bstate.piece_bbs[KING | BLACK];
                args.queens = bstate.piece_bbs[QUEEN | WHITE] | bstate.piece_bbs[QUEEN | BLACK];
                args.rooks = bstate.piece_bbs[ROOK | WHITE] | bstate.piece_bbs[ROOK | BLACK];
                args.bishops = bstate.piece_bbs[BISHOP | WHITE] | bstate.piece_bbs[BISHOP | BLACK];
                args.knights = bstate.piece_bbs[KNIGHT | WHITE] | bstate.piece_bbs[KNIGHT | BLACK];
                args.pawns = bstate.piece_bbs[PAWN | WHITE] | bstate.piece_bbs[PAWN | BLACK];
                args.rule50 = gstate.half_moves;
                args.castling = to_fathom_castling(bstate.castle_right_mask);
                args.ep = bstate.enp_square.has_value() ? bstate.enp_square.value() : 0;
                args.turn = (gstate.side_to_move == WHITE) ? 1 : 0;
                return args;
            }
        };

        std::optional<DTZEntry> dtz_entry_from_fathom(unsigned result, Color color) {
            if(result == TB_RESULT_FAILED) {
                return std::nullopt;
            }
            DTZEntry entry {};
            if(result == TB_RESULT_CHECKMATE) {
                entry.checkmate = true;
                return entry;
            } else if(result == TB_RESULT_STALEMATE) {
                entry.stalemate = true;
                return entry;
            }
            entry.wdl = wdl_from_fathom(TB_GET_WDL(result));
            entry.move.is_null_move = false;
            entry.move.source = static_cast<Square>(TB_GET_FROM(result));
            entry.move.dest = static_cast<Square>(TB_GET_TO(result));
            entry.move.promotion_type = piece_type_from_fathom(TB_GET_PROMOTES(result), color);
            entry.dtz = TB_GET_DTZ(result);
            return entry;
        }
    }

    SZEndgameTables::SZEndgameTables(std::string const& tables_root) {
        bool success = tb_init(tables_root.c_str());
        if(!success) {
            throw std::runtime_error("failed to load endgame tables");
        }
    }

    SZEndgameTables::~SZEndgameTables() {
        tb_free();
    }

    std::optional<WDL> SZEndgameTables::probe_wdl_tables(Board const& board) {
        FathomArguments args = FathomArguments::from_board(board);

        unsigned result = tb_probe_wdl(
            args.white,
            args.black,
            args.kings,
            args.queens,
            args.rooks,
            args.bishops,
            args.knights,
            args.pawns,
            args.rule50,
            args.castling,
            args.ep,
            args.turn
            );

        if(result == TB_RESULT_FAILED) {
            return std::nullopt;
        }

        return wdl_from_fathom(result);
    }

    std::optional<DTZEntry> SZEndgameTables::probe_dtz_tables(Board const& board) {
        FathomArguments args = FathomArguments::from_board(board);

        unsigned result = tb_probe_root(
            args.white,
            args.black,
            args.kings,
            args.queens,
            args.rooks,
            args.bishops,
            args.knights,
            args.pawns,
            args.rule50,
            args.castling,
            args.ep,
            args.turn,
            nullptr
        );

        return dtz_entry_from_fathom(result, board.get_side_to_move());
    }
}