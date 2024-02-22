#include "bitboard.h"

// many implementations of these helpers taken from https://www.chessprogramming.org/Bitboards


namespace jchess {
    namespace {
        // should these be public in case some other bitboard code needs to change them.
        const Bitboard notAFile = 0xfefefefefefefefe; // ~0x0101010101010101
        const Bitboard notHFile = 0x7f7f7f7f7f7f7f7f; // ~0x8080808080808080

        Bitboard get_pawn_attacks(Bitboard b, Color color) {
            if(color == WHITE) {
                return bb_neast_one(b) | bb_nwest_one(b);
            } else {
                return bb_seast_one(b) | bb_swest_one(b);
            }
        }
    }
    void bb_add_square(Bitboard& bb, Square square) {
        bb |= (1 << square);
    }
    void bb_remove_square(Bitboard& bb, Square square) {
        bb &= ~(1 << square);
    }
    bool bb_get_square(Bitboard& bb, Square square) {
        return bb & (1 << square);
    }

    Bitboard bb_south_one (Bitboard b) {return  b >> 8;}
    Bitboard bb_north_one (Bitboard b) {return  b << 8;}
    Bitboard bb_east_one (Bitboard b) {return (b << 1) & notAFile;}
    Bitboard bb_neast_one (Bitboard b) {return (b << 9) & notAFile;}
    Bitboard bb_seast_one (Bitboard b) {return (b >> 7) & notAFile;}
    Bitboard bb_west_one (Bitboard b) {return (b >> 1) & notHFile;}
    Bitboard bb_swest_one (Bitboard b) {return (b >> 9) & notHFile;}
    Bitboard bb_nwest_one (Bitboard b) {return (b << 7) & notHFile;}

    Bitboard get_king_attacks(Bitboard kingSet) {
        Bitboard attacks = bb_east_one(kingSet) | bb_west_one(kingSet);
        kingSet    |= attacks;
        attacks    |= bb_north_one(kingSet) | bb_south_one(kingSet);
        return attacks;
    }

    // implement the pre-computation functions in stupid/unoptimised way as only called once.
    std::array<Bitboard, 64> precompute_king_attacks() {
        std::array<Bitboard, 64> attacks{};
        for(int i=0; i<64; ++i) {
            attacks[i] = get_king_attacks(1 << i);
        }
        return attacks;
    }

    std::array<Bitboard, 64> precompute_knight_attacks() {
        std::array<Bitboard, 64> attacks{};
        std::fill(attacks.begin(), attacks.end(), 0);
        int knight_jumps[8] = { 10, 17, 15, 6, -10, -17, -15, -6 };
        for(int i=0; i<64; ++i) {
            for(int jump : knight_jumps) {
                int dest = i + jump;
                if(0 <= dest && dest < 64) {
                    attacks[i] |= (1 << dest);
                }
            }
        }
        return attacks;
    }

    std::array<Bitboard, 64> precompute_pawn_attacks(Color color) {
        std::array<Bitboard, 64> attacks{};
        for(int i=0; i<64; ++i) {
            attacks[i] = get_pawn_attacks((1 << i), color);
        }
        return attacks;
    }
}
