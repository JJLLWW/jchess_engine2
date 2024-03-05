// ideally would want the include to look like nnue/nnue.h
#include "nnue.h"
#include "wrap_nnue.h"
#include <fstream>

namespace {
    nnue::Piece convert_piece(jchess::Piece piece) {
        // my ordering PAWN, ROOK, KNIGHT, BISHOP, KING, QUEEN
        // white offset 0, black offset 9, [pawn, knight, bishop, rook, queen, king]
        auto type = jchess::type_from_piece(piece);
        auto color = jchess::color_from_piece(piece);
        int color_offset = (color == jchess::WHITE) ? 0 : 8;
        int type_offsets[6] = {1, 4, 2, 3, 6, 5};
        return static_cast<nnue::Piece>(color_offset + type_offsets[type]);
    }

    // see third_party/nnue/interface for the expected/possible methods of this class
    class NNUEChessInterface {
    public:
        NNUEChessInterface(jchess::Board const& board) : board{board} {
            auto state = board.get_board_state();
            for(int sq=0; sq<64; ++sq) {
                if(state.pieces[sq] != jchess::NO_PIECE) {
                    locs[sq] = convert_piece(state.pieces[sq]);
                }
            }
        }
        nnue::Color sideToMove() const noexcept {
            return board.get_side_to_move() == jchess::WHITE ? nnue::White : nnue::Black;
        }

        nnue::Square kingSquare(nnue::Color side) const noexcept {
            jchess::Color color = (side == nnue::White) ? jchess::WHITE : jchess::BLACK;
            return static_cast<nnue::Square>(board.get_board_state().king_sq[color]);
        }

        unsigned pieceCount() const noexcept {
            return board.get_num_pieces();
        }

        auto begin() const noexcept { return locs.begin(); }

        auto end() const noexcept { return locs.end(); }

    private:
        jchess::Board const& board;
        std::unordered_map<nnue::Square, nnue::Piece> locs;
    };
}


namespace jchess::nnue_eval {
    // TODO: the eval methods here don't use the api in the most efficient way, revisit this
    class NNUEEvaluator::impl {
    public:
        impl(std::string const& network_file) {
            std::ifstream file {network_file};
            file >> network;
            if(file.bad()) {
                throw std::runtime_error("nnue: could not read network file");
            }
        }
        int32_t nnue_eval_fen(std::string const& fen) {
            NNUEChessInterface intf{jchess::Board{fen}};
            return nnue::Evaluator<NNUEChessInterface>::fullEvaluate(network, intf);
        }
        int32_t nnue_eval_board(jchess::Board const& board) {
            NNUEChessInterface intf{board};
            return nnue::Evaluator<NNUEChessInterface>::fullEvaluate(network, intf);
        }
    private:
        nnue::Network network;
    };

    NNUEEvaluator::NNUEEvaluator(std::string const& network_file) {
        pimpl = std::make_unique<impl>(network_file);
    }

    // stop compiler generating a destructor which complains about incomplete type
    NNUEEvaluator::~NNUEEvaluator() {}

    int32_t NNUEEvaluator::nnue_eval_fen(std::string const& fen) {
        return pimpl->nnue_eval_fen(fen);
    }

    int32_t NNUEEvaluator::nnue_eval_board(jchess::Board const& board) {
        return pimpl->nnue_eval_board(board);
    }
}