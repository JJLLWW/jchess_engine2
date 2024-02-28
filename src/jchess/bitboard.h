#pragma once

#include "core.h"

#include <cstdint>
#include <array>
#include <vector>
#include <string>
#include <cassert>

namespace jchess {
    const Bitboard notAFile = 0xfefefefefefefefe; // ~0x0101010101010101
    const Bitboard notHFile = 0x7f7f7f7f7f7f7f7f; // ~0x8080808080808080
    const Bitboard FILE_A_BB = 0x101010101010101;
    const Bitboard RANK_1_BB = 0xFF;

    const Bitboard WHITE_QS_BB = 0x1F;
    const Bitboard WHITE_KS_BB = 0xF0;
    const Bitboard BLACK_QS_BB = WHITE_QS_BB << 7*8;
    const Bitboard BLACK_KS_BB = WHITE_KS_BB << 7*8;

    using RectTable = std::array<std::array<Bitboard, 64>, 64>;

    constexpr void bb_add_square(Bitboard& bb, Square square) { bb |= (1ull << square); }
    constexpr void bb_remove_square(Bitboard& bb, Square square) { bb &= ~(1ull << square); }

    // can't use an array here as these enum values are not contiguous
    constexpr Bitboard castle_square_bb(CastleBits castle_type) {
        switch (castle_type) {
            case WHITE_KS:
                return WHITE_KS_BB;
            case WHITE_QS:
                return WHITE_QS_BB;
            case BLACK_KS:
                return BLACK_KS_BB;
            case BLACK_QS:
                return BLACK_QS_BB;
        }
    }

    std::string bb_to_string(Bitboard bb);

    constexpr Bitboard bb_from_square(Square square) { return 1ull << square; }

    constexpr Bitboard bb_south_one (Bitboard b) {return  b >> 8;}
    constexpr Bitboard bb_north_one (Bitboard b) {return  b << 8;}
    constexpr Bitboard bb_east_one (Bitboard b) {return (b << 1) & notAFile;}
    constexpr Bitboard bb_neast_one (Bitboard b) {return (b << 9) & notAFile;}
    constexpr Bitboard bb_seast_one (Bitboard b) {return (b >> 7) & notAFile;}
    constexpr Bitboard bb_west_one (Bitboard b) {return (b >> 1) & notHFile;}
    constexpr Bitboard bb_swest_one (Bitboard b) {return (b >> 9) & notHFile;}
    constexpr Bitboard bb_nwest_one (Bitboard b) {return (b << 7) & notHFile;}

    constexpr Bitboard compute_king_attacks(Bitboard kingSet) {
        Bitboard attacks = bb_east_one(kingSet) | bb_west_one(kingSet);
        kingSet    |= attacks;
        attacks    |= bb_north_one(kingSet) | bb_south_one(kingSet);
        return attacks;
    }

    constexpr Bitboard compute_ray_attack(Square sq, Direction d) {
        // in (rank, file) order
        const int offsets_of_dir[8][2] = {
            {0, -1}, {0, 1}, {1, 0}, {-1, 0},
            {1, -1}, {1, 1}, {-1,-1}, {-1, 1}
        };
        const auto [dx, dy] = offsets_of_dir[d];
        const auto [x, y] = rank_file_from_square(sq);
        Bitboard bb = 0ull;
        for(int i=1; check_rank_file(x + i*dx, y + i*dy); ++i) {
            bb_add_square(bb, square_from_rank_file(x + i*dx, y + i*dy));
        }
        return bb;
    }

    namespace detail {
        constexpr std::array<Bitboard, 8> initialise_file_bbs() {
            std::array<Bitboard, 8> init{};
            for (int i = 0; i < 8; ++i) {
                init[i] = FILE_A_BB << i;
            }
            return init;
        }

        constexpr std::array<Bitboard, 8> initialise_rank_bbs() {
            std::array<Bitboard, 8> init{};
            for (int i = 0; i < 8; ++i) {
                init[i] = RANK_1_BB << 8 * i;
            }
            return init;
        }

        constexpr std::array<Bitboard, 15> initialise_diag_bbs() {
            std::array<Bitboard, 15> init{};
            for (Square i = A1; i < NUM_SQUARES; ++i) {
                int d = diagonal_from_square(i);
                bb_add_square(init[d], i);
            }
            return init;
        }

        constexpr std::array<Bitboard, 15> initialise_anti_diag_bbs() {
            std::array<Bitboard, 15> init{};
            for (Square i = A1; i < NUM_SQUARES; ++i) {
                int d = antidiag_from_square(i);
                bb_add_square(init[d], i);
            }
            return init;
        }

        constexpr std::array<std::array<Bitboard, 64>, 8> initialise_ray_bbs() {
            std::array<std::array<Bitboard, 64>, 8> init{};
            for(Square sq=A1; sq<NUM_SQUARES; ++sq) {
                for(Direction d : all_directions) {
                    init[d][sq] = compute_ray_attack(sq, d);
                }
            }
            return init;
        }

        constexpr std::array<Bitboard, 64> compute_all_king_attacks() {
            std::array<Bitboard, 64> attacks{};
            for(int i=0; i<64; ++i) {
                attacks[i] = compute_king_attacks(1ull << i);
            }
            return attacks;
        }

        constexpr Bitboard compute_pawn_attacks(Bitboard b, Color color) {
            if(color == WHITE) {
                return bb_neast_one(b) | bb_nwest_one(b);
            } else {
                return bb_seast_one(b) | bb_swest_one(b);
            }
        }

        constexpr std::array<Bitboard, 64> compute_all_pawn_attacks(Color color) {
            std::array<Bitboard, 64> attacks{};
            for(int i=0; i<64; ++i) {
                attacks[i] = compute_pawn_attacks((1ull << i), color);
            }
            return attacks;
        }

        constexpr std::array<Bitboard, 64> compute_all_knight_attacks() {
            std::array<Bitboard, 64> attacks{};
            int knight_jumps[8][2] = {
                {-2, 1}, {-1, 2}, {1, 2}, {2, 1},
                {-2,-1}, {-1,-2}, {1,-2}, {2,-1}
            };
            for(Square i=A1; i<NUM_SQUARES; ++i) {
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

        // https://www.chessprogramming.org/BitScan#GeneralizedBitscan
        constexpr int index64[64] = {
            0, 47,  1, 56, 48, 27,  2, 60,
            57, 49, 41, 37, 28, 16,  3, 61,
            54, 58, 35, 52, 50, 42, 21, 44,
            38, 32, 29, 23, 17, 11,  4, 62,
            46, 55, 26, 59, 40, 36, 15, 53,
            34, 51, 20, 43, 31, 22, 10, 45,
            25, 39, 14, 33, 19, 30,  9, 24,
            13, 18,  8, 12,  7,  6,  5, 63
        };

        // bitScanReverse: Kim Walisch, Mark Dickinson. gets MSB of the bitboard
        constexpr int bit_scan_reverse(uint64_t bb) {
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

        constexpr Bitboard get_rectangle_between(Square sq1, Square sq2) {
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

        constexpr RectTable initialise_rectangle_between() {
            RectTable table;
            for(Square sq1=A1; sq1<NUM_SQUARES; ++sq1) {
                for(Square sq2=A1; sq2<NUM_SQUARES; ++sq2) {
                   table[sq1][sq2] = get_rectangle_between(sq1, sq2);
                }
            }
            return table;
        }
    } // namespace detail

    // https://www.chessprogramming.org/BitScan#GeneralizedBitscan
    constexpr int bit_scan(Bitboard bb, bool reverse) {
        uint64_t rMask;
        assert (bb != 0);
        rMask = -(uint64_t)reverse;
        bb &= -bb | rMask;
        return detail::bit_scan_reverse(bb);
    }

    // diagonal is up-right, anti-diagonal is down-right
    inline constexpr std::array<Bitboard, 8> FILE_BBS {detail::initialise_file_bbs()};
    inline constexpr std::array<Bitboard, 8> RANK_BBS {detail::initialise_rank_bbs()};
    inline constexpr std::array<Bitboard, 15> DIAG_BBS {detail::initialise_diag_bbs()};
    inline constexpr std::array<Bitboard, 15> ANTI_DIAG_BBS {detail::initialise_anti_diag_bbs()};

    inline constexpr Bitboard pawn_start_bb[2] {RANK_BBS[RANK_2], RANK_BBS[RANK_7]}; // PST_WHITE, PST_BLACK
    inline constexpr Bitboard back_rank_bb[2] {RANK_BBS[RANK_8], RANK_BBS[RANK_1]}; // PST_WHITE, PST_BLACK

    // usage RAY_BBS[WEST][A3]
    inline constexpr std::array<std::array<Bitboard, 64>, 8> RAY_BBS {detail::initialise_ray_bbs()};

    // precompute all attack squares that do not depend on blockers.
    constexpr inline std::array<Bitboard, 64> KING_ATTACKS {detail::compute_all_king_attacks()};
    constexpr inline std::array<Bitboard, 64> KNIGHT_ATTACKS {detail::compute_all_knight_attacks()};
    constexpr inline std::array<Bitboard, 64> PAWN_ATTACKS[2] {
        detail::compute_all_pawn_attacks(WHITE),
        detail::compute_all_pawn_attacks(BLACK)
    };

    constexpr inline RectTable RECTANGLE_BETWEEN {detail::initialise_rectangle_between()};

    // helpers that don't have to be used at compile time to initialise magic tables:

    std::vector<Bitboard> get_subsets_of_mask(Bitboard mask); // bottleneck
    Bitboard bb_from_squares(std::vector<Square> squares);
    Bitboard get_ray_between(Square sq1, Square sq2);
    Square lsb_square_from_bb(Bitboard bb);
    Bitboard segment_between(Square sq1, Square sq2); // not including endpoints

    bool pop_lsb_square(Bitboard& bb, Square& sq);
}