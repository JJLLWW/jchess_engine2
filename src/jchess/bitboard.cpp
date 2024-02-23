#include "bitboard.h"

#include <cassert>
// many implementations of these helpers taken from https://www.chessprogramming.org/Bitboards


namespace jchess {
    namespace {
        // should these be public in case some other bitboard code needs to use them.
        Bitboard get_pawn_attacks(Bitboard b, Color color) {
            if(color == WHITE) {
                return bb_neast_one(b) | bb_nwest_one(b);
            } else {
                return bb_seast_one(b) | bb_swest_one(b);
            }
        }

        void blocker_mask_bb_edges(Bitboard& bb, Square square) {
            auto [rank, file] = rank_file_from_square(square);
            if(rank != RANK_1) {
                bb &= ~RANK_BBS[RANK_1];
            }
            if(rank != RANK_8) {
                bb &= ~RANK_BBS[RANK_8];
            }
            if(file != A) {
                bb &= ~FILE_BBS[A];
            }
            if(file != H) {
                bb &= ~FILE_BBS[H];
            }
        }
    }

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

    void bb_add_square(Bitboard& bb, Square square) {
        bb |= (1ull << square);
    }
    void bb_remove_square(Bitboard& bb, Square square) {
        bb &= ~(1ull << square);
    }
    bool bb_get_square(Bitboard& bb, Square square) {
        return bb & (1ull << square);
    }

    Bitboard bb_from_squares(std::vector<Square> squares) {
        Bitboard bb = 0;
        for(const Square square : squares) {
            bb_add_square(bb, square);
        }
        return bb;
    }
    Bitboard bb_south_one (Bitboard b) {return  b >> 8;}
    Bitboard bb_north_one (Bitboard b) {return  b << 8;}
    Bitboard bb_east_one (Bitboard b) {return (b << 1) & notAFile;}
    Bitboard bb_neast_one (Bitboard b) {return (b << 9) & notAFile;}
    Bitboard bb_seast_one (Bitboard b) {return (b >> 7) & notAFile;}
    Bitboard bb_west_one (Bitboard b) {return (b >> 1) & notHFile;}
    Bitboard bb_swest_one (Bitboard b) {return (b >> 9) & notHFile;}
    Bitboard bb_nwest_one (Bitboard b) {return (b << 7) & notHFile;}

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

    Bitboard get_king_attacks(Bitboard kingSet) {
        Bitboard attacks = bb_east_one(kingSet) | bb_west_one(kingSet);
        kingSet    |= attacks;
        attacks    |= bb_north_one(kingSet) | bb_south_one(kingSet);
        return attacks;
    }

    // implement the pre-computation functions in stupid/unoptimised way as only called once.
    std::array<Bitboard, 64> compute_all_king_attacks() {
        std::array<Bitboard, 64> attacks{};
        for(int i=0; i<64; ++i) {
            attacks[i] = get_king_attacks(1ull << i);
        }
        return attacks;
    }

    std::array<Bitboard, 64> compute_all_knight_attacks() {
        std::array<Bitboard, 64> attacks{};
        std::fill(attacks.begin(), attacks.end(), 0);
        int knight_jumps[8][2] = {
            {-2, 1}, {-1, 2}, {1, 2}, {2, 1},
            {-2,-1}, {-1,-2}, {1,-2}, {2,-1}
        };
        for(int i=0; i<64; ++i) {
            for(auto [dx, dy] : knight_jumps) {
                auto [x, y] = rank_file_from_square(i);
                int x_dest = x + dx, y_dest = y + dy;
                if(!check_rank_file(x_dest, y_dest)) {
                    continue;
                }
                Square dest = square_from_rank_file(x_dest, y_dest);
                attacks[i] |= (1ull << dest);
            }
        }
        return attacks;
    }

    std::array<Bitboard, 64> compute_all_pawn_attacks(Color color) {
        std::array<Bitboard, 64> attacks{};
        for(int i=0; i<64; ++i) {
            attacks[i] = get_pawn_attacks((1ull << i), color);
        }
        return attacks;
    }

    Bitboard get_rook_attacks_empty_board(Square square) {
        Bitboard attacks = 0ull;
        auto [rank, file] = rank_file_from_square(square);
        attacks |= (RANK_BBS[rank] | FILE_BBS[file]);
        bb_remove_square(attacks, square);
        return attacks;
    }

    Bitboard get_rook_blocker_mask(Square square) {
        Bitboard mask = get_rook_attacks_empty_board(square);
        blocker_mask_bb_edges(mask, square);
        return mask;
    }

    Bitboard get_bishop_attacks_empty_board(Square square) {
        Bitboard attacks = 0ull;
        int diag = diagonal_from_square(square);
        int anti_diag = antidiag_from_square(square);
        attacks |= (DIAG_BBS[diag] | ANTI_DIAG_BBS[anti_diag]);
        bb_remove_square(attacks, square);
        return attacks;
    }

    Bitboard get_bishop_blocker_mask(Square square) {
        Bitboard mask = get_bishop_attacks_empty_board(square);
        blocker_mask_bb_edges(mask, square);
        return mask;
    }

    // https://www.chessprogramming.org/BitScan#GeneralizedBitscan
    const int index64[64] = {
        0, 47,  1, 56, 48, 27,  2, 60,
        57, 49, 41, 37, 28, 16,  3, 61,
        54, 58, 35, 52, 50, 42, 21, 44,
        38, 32, 29, 23, 17, 11,  4, 62,
        46, 55, 26, 59, 40, 36, 15, 53,
        34, 51, 20, 43, 31, 22, 10, 45,
        25, 39, 14, 33, 19, 30,  9, 24,
        13, 18,  8, 12,  7,  6,  5, 63
    };

    /**
     * bitScanReverse
     * @authors Kim Walisch, Mark Dickinson
     * @param bb bitboard to scan
     * @precondition bb != 0
     * @return index (0..63) of most significant one bit
     */
    int bitScanReverse(uint64_t bb) {
        const uint64_t debruijn64 = 0x03f79d71b4cb0a89ull;
        assert (bb != 0);
        bb |= bb >> 1;
        bb |= bb >> 2;
        bb |= bb >> 4;
        bb |= bb >> 8;
        bb |= bb >> 16;
        bb |= bb >> 32;
        return index64[(bb * debruijn64) >> 58];
    }

    // https://www.chessprogramming.org/BitScan#GeneralizedBitscan
    int bit_scan(Bitboard bb, bool reverse) {
        uint64_t rMask;
        assert (bb != 0);
        rMask = -(uint64_t)reverse;
        bb &= -bb | rMask;
        return bitScanReverse(bb);
    }

    // https://www.chessprogramming.org/Classical_Approach
    Bitboard get_ray_attacks(Bitboard occupied, Direction direction, Square square) {
        Bitboard attacks = RAY_BBS[direction][square];
        Bitboard blocker = attacks & occupied;
        if ( blocker ) {
            square = bit_scan(blocker, is_negative_dir(direction));
            attacks ^= RAY_BBS[direction][square];
        }
        return attacks;
    }

    Bitboard get_bishop_attacks(Bitboard occupied, Square square) {
        Bitboard attacks = 0ull;
        attacks |= get_ray_attacks(occupied, NWEST, square);
        attacks |= get_ray_attacks(occupied, NEAST, square);
        attacks |= get_ray_attacks(occupied, SWEST, square);
        attacks |= get_ray_attacks(occupied, SEAST, square);
        return attacks;
    }

    Bitboard get_rook_attacks(Bitboard occupied, Square square) {
        Bitboard attacks = 0ull;
        attacks |= get_ray_attacks(occupied, WEST, square);
        attacks |= get_ray_attacks(occupied, EAST, square);
        attacks |= get_ray_attacks(occupied, SOUTH, square);
        attacks |= get_ray_attacks(occupied, NORTH, square);
        return attacks;
    }
}
