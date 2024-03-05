#pragma once

#include "../board.h"
#include <memory>

namespace jchess::nnue_eval {
    class NNUEEvaluator {
    public:
        NNUEEvaluator(std::string const& network_filename);
        ~NNUEEvaluator();
        int32_t nnue_eval_fen(std::string const& fen);
        int32_t nnue_eval_board(jchess::Board const& board);
    private:
        // avoid the jdart nnue headers being part of the library's public interface
        class impl;
        std::unique_ptr<impl> pimpl;
    };
}