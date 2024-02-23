#include "magic_bitboard.h"
#include <bit> // std::popcount C++20
#include <algorithm> // TEMPORARY

namespace jchess {
    namespace {
        // THESE CONSTANTS ARE WRONG
        constexpr size_t MAX_BISHOP_TBL_SZ = 2048;
        constexpr size_t MAX_ROOK_TBL_SZ = 8196;
    }

    int get_bishop_index(Bitboard blockers, Square square) {
        uint64_t index = blockers * bishop_magics[square];
        int n = std::popcount(index);
        index >>= (64 - n);
        return index;
    }

    int get_rook_index(Bitboard blockers, Square square) {
        uint64_t index = blockers * rook_magics[square];
        int n = std::popcount(index);
        index >>= (64 - n);
        return index;
    }

    Bitboard MagicDatabase::get_bishop_attacks(Bitboard blockers, Square square) {
        return bishop_attacks[get_bishop_index(blockers, square)];
    }

    Bitboard MagicDatabase::get_rook_attacks(Bitboard blockers, Square square) {
        return rook_attacks[get_rook_index(blockers, square)];
    }

    MagicDatabase::MagicDatabase() {
        bishop_attacks.assign(MAX_BISHOP_TBL_SZ, 0);
        rook_attacks.assign(MAX_ROOK_TBL_SZ, 0);
        int max_bindex = 0;
        int max_rindex = 0;
        for(Square square = 0; square < 64; ++square) {
            // bishop table fill
            Bitboard blockers_mask = get_bishop_blocker_mask(square);
            for(Bitboard blockers : get_subsets_of_mask(blockers_mask)) {
                int index = get_bishop_index(blockers, square);
                max_bindex = std::max(max_bindex, index);
                // bishop_attacks[index] = get_bishop_attacks(blockers, square);
            }
            // rook table fill
            blockers_mask = get_rook_blocker_mask(square);
            for(Bitboard blockers : get_subsets_of_mask(blockers_mask)) {
                int index = get_rook_index(blockers, square);
                max_rindex = std::max(max_rindex, index);
                // rook_attacks[index] = get_rook_attacks(blockers, square);
            }
        }
        // max bindex: 4,398,046,509,043 : 2,126,507,072 <- 4 orders of magnitude smaller but still seems insanely large
        // (yeah this is approx 2 TB)
        // max rindex: 4,397,031,371,967 : 2,147,229,353
    }
}