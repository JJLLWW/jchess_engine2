#pragma once

#include "core.h"
#include "bitboard.h"
#include <vector>
#include <array>

namespace jchess {
    namespace detail {
        constexpr size_t MAX_BISHOP_TBL_SZ = 512;
        constexpr size_t MAX_ROOK_TBL_SZ = 4096;

        // https://www.chessprogramming.org/Looking_for_Magics
        constexpr uint64_t rook_magics[64] = {0xa8002c000108020ULL, 0x6c00049b0002001ULL, 0x100200010090040ULL, 0x2480041000800801ULL, 0x280028004000800ULL, 0x900410008040022ULL, 0x280020001001080ULL, 0x2880002041000080ULL, 0xa000800080400034ULL, 0x4808020004000ULL, 0x2290802004801000ULL, 0x411000d00100020ULL, 0x402800800040080ULL, 0xb000401004208ULL, 0x2409000100040200ULL, 0x1002100004082ULL, 0x22878001e24000ULL, 0x1090810021004010ULL, 0x801030040200012ULL, 0x500808008001000ULL, 0xa08018014000880ULL, 0x8000808004000200ULL, 0x201008080010200ULL, 0x801020000441091ULL, 0x800080204005ULL, 0x1040200040100048ULL, 0x120200402082ULL, 0xd14880480100080ULL, 0x12040280080080ULL, 0x100040080020080ULL, 0x9020010080800200ULL, 0x813241200148449ULL, 0x491604001800080ULL, 0x100401000402001ULL, 0x4820010021001040ULL, 0x400402202000812ULL, 0x209009005000802ULL, 0x810800601800400ULL, 0x4301083214000150ULL, 0x204026458e001401ULL, 0x40204000808000ULL, 0x8001008040010020ULL, 0x8410820820420010ULL, 0x1003001000090020ULL, 0x804040008008080ULL, 0x12000810020004ULL, 0x1000100200040208ULL, 0x430000a044020001ULL, 0x280009023410300ULL, 0xe0100040002240ULL, 0x200100401700ULL, 0x2244100408008080ULL, 0x8000400801980ULL, 0x2000810040200ULL, 0x8010100228810400ULL, 0x2000009044210200ULL, 0x4080008040102101ULL, 0x40002080411d01ULL, 0x2005524060000901ULL, 0x502001008400422ULL, 0x489a000810200402ULL, 0x1004400080a13ULL, 0x4000011008020084ULL, 0x26002114058042ULL};
        constexpr uint64_t bishop_magics[64] = {0x89a1121896040240ULL, 0x2004844802002010ULL, 0x2068080051921000ULL, 0x62880a0220200808ULL, 0x4042004000000ULL, 0x100822020200011ULL, 0xc00444222012000aULL, 0x28808801216001ULL, 0x400492088408100ULL, 0x201c401040c0084ULL, 0x840800910a0010ULL, 0x82080240060ULL, 0x2000840504006000ULL, 0x30010c4108405004ULL, 0x1008005410080802ULL, 0x8144042209100900ULL, 0x208081020014400ULL, 0x4800201208ca00ULL, 0xf18140408012008ULL, 0x1004002802102001ULL, 0x841000820080811ULL, 0x40200200a42008ULL, 0x800054042000ULL, 0x88010400410c9000ULL, 0x520040470104290ULL, 0x1004040051500081ULL, 0x2002081833080021ULL, 0x400c00c010142ULL, 0x941408200c002000ULL, 0x658810000806011ULL, 0x188071040440a00ULL, 0x4800404002011c00ULL, 0x104442040404200ULL, 0x511080202091021ULL, 0x4022401120400ULL, 0x80c0040400080120ULL, 0x8040010040820802ULL, 0x480810700020090ULL, 0x102008e00040242ULL, 0x809005202050100ULL, 0x8002024220104080ULL, 0x431008804142000ULL, 0x19001802081400ULL, 0x200014208040080ULL, 0x3308082008200100ULL, 0x41010500040c020ULL, 0x4012020c04210308ULL, 0x208220a202004080ULL, 0x111040120082000ULL, 0x6803040141280a00ULL, 0x2101004202410000ULL, 0x8200000041108022ULL, 0x21082088000ULL, 0x2410204010040ULL, 0x40100400809000ULL, 0x822088220820214ULL, 0x40808090012004ULL, 0x910224040218c9ULL, 0x402814422015008ULL, 0x90014004842410ULL, 0x1000042304105ULL, 0x10008830412a00ULL, 0x2520081090008908ULL, 0x40102000a0a60140ULL};

        using BishopAttacks = std::array<std::array<Bitboard, MAX_BISHOP_TBL_SZ>, 64>;
        using RookAttacks = std::array<std::array<Bitboard, MAX_ROOK_TBL_SZ>, 64>;

        constexpr int RBits[64] = {
            12, 11, 11, 11, 11, 11, 11, 12,
            11, 10, 10, 10, 10, 10, 10, 11,
            11, 10, 10, 10, 10, 10, 10, 11,
            11, 10, 10, 10, 10, 10, 10, 11,
            11, 10, 10, 10, 10, 10, 10, 11,
            11, 10, 10, 10, 10, 10, 10, 11,
            11, 10, 10, 10, 10, 10, 10, 11,
            12, 11, 11, 11, 11, 11, 11, 12
        };

        constexpr int BBits[64] = {
            6, 5, 5, 5, 5, 5, 5, 6,
            5, 5, 5, 5, 5, 5, 5, 5,
            5, 5, 7, 7, 7, 7, 5, 5,
            5, 5, 7, 9, 9, 7, 5, 5,
            5, 5, 7, 9, 9, 7, 5, 5,
            5, 5, 7, 7, 7, 7, 5, 5,
            5, 5, 5, 5, 5, 5, 5, 5,
            6, 5, 5, 5, 5, 5, 5, 6
        };

        constexpr void blocker_mask_bb_edges(Bitboard &bb, Square square) {
            auto [rank, file] = rank_file_from_square(square);
            if (rank != RANK_1) {
                bb &= ~RANK_BBS[RANK_1];
            }
            if (rank != RANK_8) {
                bb &= ~RANK_BBS[RANK_8];
            }
            if (file != A) {
                bb &= ~FILE_BBS[A];
            }
            if (file != H) {
                bb &= ~FILE_BBS[H];
            }
        }

        constexpr Bitboard get_rook_attacks_empty_board(Square square) {
            auto [rank, file] = rank_file_from_square(square);
            return (RANK_BBS[rank] | FILE_BBS[file]) & ~bb_from_square(square);
        }

        constexpr Bitboard get_rook_blocker_mask(Square square) {
            Bitboard mask = get_rook_attacks_empty_board(square);
            blocker_mask_bb_edges(mask, square);
            return mask;
        }


        constexpr Bitboard get_bishop_attacks_empty_board(Square square) {
            int diag = diagonal_from_square(square);
            int anti_diag = antidiag_from_square(square);
            return (DIAG_BBS[diag] | ANTI_DIAG_BBS[anti_diag]) & ~bb_from_square(square);
        }

        constexpr Bitboard get_bishop_blocker_mask(Square square) {
            Bitboard mask = get_bishop_attacks_empty_board(square);
            blocker_mask_bb_edges(mask, square);
            return mask;
        }

        constexpr Bitboard compute_blocked_ray_attacks(Bitboard occupied, Direction direction, Square square) {
            Bitboard attacks = RAY_BBS[direction][square];
            Bitboard blocker = attacks & occupied;
            if (blocker) {
                square = static_cast<Square>(bit_scan(blocker, is_negative_dir(direction)));
                attacks ^= RAY_BBS[direction][square];
            }
            return attacks;
        }

        constexpr Bitboard compute_blocked_bishop_attacks(Bitboard occupied, Square square) {
            Bitboard attacks = 0ull;
            attacks |= compute_blocked_ray_attacks(occupied, NWEST, square);
            attacks |= compute_blocked_ray_attacks(occupied, NEAST, square);
            attacks |= compute_blocked_ray_attacks(occupied, SWEST, square);
            attacks |= compute_blocked_ray_attacks(occupied, SEAST, square);
            return attacks;
        }

        constexpr Bitboard compute_blocked_rook_attacks(Bitboard occupied, Square square) {
            Bitboard attacks = 0ull;
            attacks |= compute_blocked_ray_attacks(occupied, WEST, square);
            attacks |= compute_blocked_ray_attacks(occupied, EAST, square);
            attacks |= compute_blocked_ray_attacks(occupied, SOUTH, square);
            attacks |= compute_blocked_ray_attacks(occupied, NORTH, square);
            return attacks;
        }

        constexpr uint64_t get_bishop_index(Bitboard blockers, Square square) {
            uint64_t index = blockers * bishop_magics[square];
            int n = BBits[square];
            index >>= (64 - n);
            return index;
        }

        constexpr uint64_t get_rook_index(Bitboard blockers, Square square) {
            uint64_t index = blockers * rook_magics[square];
            int n = RBits[square];
            index >>= (64 - n);
            return index;
        }

        constexpr BishopAttacks initialise_bishop_attacks() {
            BishopAttacks bishop_attacks{};
            for (Square square = A1; square < NUM_SQUARES; ++square) {
                Bitboard blockers_mask = get_bishop_blocker_mask(square);
                // https://www.chessprogramming.org/Traversing_Subsets_of_a_Set
                Bitboard blockers = 0ull;
                do {
                    uint64_t index = get_bishop_index(blockers, square);
                    bishop_attacks[square][index] = compute_blocked_bishop_attacks(blockers, square);
                    blockers = (blockers - blockers_mask) & blockers_mask;
                } while (blockers);
            }
            return bishop_attacks;
        }

        constexpr RookAttacks initialise_rook_attacks() {
            RookAttacks rook_attacks{};
            for (Square square = A1; square < NUM_SQUARES; ++square) {
                Bitboard blockers_mask = get_rook_blocker_mask(square);
                Bitboard blockers = 0ull;
                do {
                    uint64_t index = get_rook_index(blockers, square);
                    rook_attacks[square][index] = compute_blocked_rook_attacks(blockers, square);
                    blockers = (blockers - blockers_mask) & blockers_mask;
                } while (blockers);
            }
            return rook_attacks;
        }

        inline constexpr BishopAttacks BISHOP_ATTACKS{initialise_bishop_attacks()};
        inline constexpr RookAttacks ROOK_ATTACKS{initialise_rook_attacks()};
    } // namespace detail

    constexpr Bitboard get_bishop_attacks(Square source, Bitboard blockers) {
        blockers = blockers & detail::get_bishop_blocker_mask(source);
        return detail::BISHOP_ATTACKS[source][detail::get_bishop_index(blockers, source)];
    }

    constexpr Bitboard get_rook_attacks(Square source, Bitboard blockers) {
        blockers = blockers & detail::get_rook_blocker_mask(source);
        return detail::ROOK_ATTACKS[source][detail::get_rook_index(blockers, source)];
    }
}