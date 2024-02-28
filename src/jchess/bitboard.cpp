#include "bitboard.h"

#include <cassert>
// many implementations of these helpers taken from https://www.chessprogramming.org/Bitboards


namespace jchess {
    std::string bb_to_string(Bitboard bb) {
        std::string str;
        for(int i=0; i<64; ++i) {
            int lsb = bb % 2;
            str += lsb ? 'x' : '.';
            if(i % 8 == 7) {
                str += '\n';
            }
            bb >>= 1;
        }
        return str;
    }

    Bitboard bb_from_squares(std::vector<Square> squares) {
        Bitboard bb = 0;
        for(const Square square : squares) {
            bb_add_square(bb, square);
        }
        return bb;
    }

    std::vector<Bitboard> get_subsets_of_mask(Bitboard mask) {
        // black magic implementation: https://www.chessprogramming.org/Traversing_Subsets_of_a_Set
        std::vector<Bitboard> subsets;
        Bitboard subset = 0ull;
        do {
            subsets.push_back(subset);
            subset = (subset - mask) & mask;
        } while(subset);
        return subsets;
    }

    Bitboard get_ray_between(Square sq1, Square sq2) {
        const auto [r1, f1] = rank_file_from_square(sq1);
        const auto [r2, f2] = rank_file_from_square(sq2);
        if (r1 == r2) {
            return RECTANGLE_BETWEEN[sq1][sq2] & RANK_BBS[r1];
        } else if (f1 == f2) {
            return RECTANGLE_BETWEEN[sq1][sq2] & FILE_BBS[f1];
        } else if (diagonal_from_square(sq1) == diagonal_from_square(sq2)) {
            return RECTANGLE_BETWEEN[sq1][sq2] & DIAG_BBS[diagonal_from_square(sq1)];
        } else if (antidiag_from_square(sq1) == antidiag_from_square(sq2)) {
            return RECTANGLE_BETWEEN[sq1][sq2] & ANTI_DIAG_BBS[antidiag_from_square(sq1)];
        }
        assert(false);
    }

    Square lsb_square_from_bb(Bitboard bb) {
        return static_cast<Square>(bit_scan(bb, false));
    }

    bool pop_lsb_square(Bitboard& bb, Square& sq) {
        if(bb == 0) {
            return false;
        }
        sq = lsb_square_from_bb(bb);
        bb &= (bb - 1);
        return true;
    }

    Bitboard segment_between(Square sq1, Square sq2) {
        if(horizontal_distance(sq1, sq2) == 0) {
            return RECTANGLE_BETWEEN[sq1][sq2]  & FILE_BBS[file_of(sq1)];
        } else if(vertical_distance(sq1, sq2) == 0) {
            return RECTANGLE_BETWEEN[sq1][sq2] & RANK_BBS[rank_of(sq1)];
        } else if(diagonal_from_square(sq1) == diagonal_from_square(sq2)) {
            return RECTANGLE_BETWEEN[sq1][sq2] & DIAG_BBS[diagonal_from_square(sq1)];
        } else if(antidiag_from_square(sq1) == antidiag_from_square(sq2)) {
            return RECTANGLE_BETWEEN[sq1][sq2] & ANTI_DIAG_BBS[antidiag_from_square(sq2)];
        }
        assert(false);
    }
}
