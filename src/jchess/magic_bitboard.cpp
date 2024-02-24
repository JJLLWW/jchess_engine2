#include "magic_bitboard.h"

namespace jchess {
    namespace {
        // max sizes of a single square's lookup table
        constexpr size_t MAX_BISHOP_TBL_SZ = 512;
        constexpr size_t MAX_ROOK_TBL_SZ = 4096;
    }

    uint64_t get_bishop_index(Bitboard blockers, Square square) {
        uint64_t index = blockers * bishop_magics[square];
        int n = BBits[square];
        index >>= (64 - n);
        return index;
    }

    uint64_t get_rook_index(Bitboard blockers, Square square) {
        uint64_t index = blockers * rook_magics[square];
        int n = RBits[square];
        index >>= (64 - n);
        return index;
    }

    Bitboard MagicDatabase::get_bishop_attacks(Bitboard blockers, Square square) {
        return bishop_attacks[square][get_bishop_index(blockers, square)];
    }

    Bitboard MagicDatabase::get_rook_attacks(Bitboard blockers, Square square) {
        return rook_attacks[square][get_rook_index(blockers, square)];
    }

    MagicDatabase::MagicDatabase() {
        for(auto& bishop_table : bishop_attacks) {
            bishop_table.resize(MAX_BISHOP_TBL_SZ);
        }
        for(auto& rook_table : rook_attacks) {
            rook_table.resize(MAX_ROOK_TBL_SZ);
        }
        // TEMPORARY
        for(Square square = 0; square < 64; ++square) {
            // bishop table fill
            Bitboard blockers_mask = get_bishop_blocker_mask(square);
            for(Bitboard blockers : get_subsets_of_mask(blockers_mask)) {
                uint64_t index = get_bishop_index(blockers, square);
                bishop_attacks[square][index] = jchess::get_bishop_attacks(blockers, square);
            }
            // rook table fill
            blockers_mask = get_rook_blocker_mask(square);
            for(Bitboard blockers : get_subsets_of_mask(blockers_mask)) {
                uint64_t index = get_rook_index(blockers, square);
                rook_attacks[square][index] = jchess::get_rook_attacks(blockers, square);
            }
        }
    }
}