#pragma once

#include "core.h"

#include <cstdint>
#include <array>
#include <vector>
#include <string>

namespace jchess {
    const Bitboard notAFile = 0xfefefefefefefefe; // ~0x0101010101010101
    const Bitboard notHFile = 0x7f7f7f7f7f7f7f7f; // ~0x8080808080808080
    const Bitboard FILE_A_BB = 0x101010101010101;
    const Bitboard RANK_1_BB = 0xFF;

    std::string bb_to_string(Bitboard bb);

    Bitboard get_king_attacks(Bitboard kingSet);

    Bitboard bb_from_squares(std::vector<Square> squares);

    void bb_add_square(Bitboard& bb, Square square);
    void bb_remove_square(Bitboard& bb, Square square);
    bool bb_get_square(Bitboard& bb, Square square);

    Bitboard bb_south_one (Bitboard b);
    Bitboard bb_north_one (Bitboard b);
    Bitboard bb_east_one (Bitboard b);
    Bitboard bb_neast_one (Bitboard b);
    Bitboard bb_seast_one (Bitboard b);
    Bitboard bb_west_one (Bitboard b);
    Bitboard bb_swest_one (Bitboard b);
    Bitboard bb_nwest_one (Bitboard b);

    // don't implement own iterator here as this is only used for non performance
    // critical pre-computation ( <= 4096 possible subsets )
    std::vector<Bitboard> get_subsets_of_mask(Bitboard mask);
    std::vector<Square> bb_get_squares(Bitboard bb);

    Bitboard get_rook_attacks_empty_board(Square square);
    Bitboard get_rook_blocker_mask(Square square);

    Bitboard get_bishop_attacks_empty_board(Square square);
    Bitboard get_bishop_blocker_mask(Square square);

    // get index of LSB (forward) or MSB (reverse)
    int bit_scan(Bitboard bb, bool reverse);
    Bitboard get_ray_attacks(Bitboard occupied, Direction direction, Square square);
    Bitboard get_bishop_attacks(Bitboard occupied, Square square);
    Bitboard get_rook_attacks(Bitboard occupied, Square square);

    constexpr std::array<Bitboard, 8> initialise_file_bbs() {
        std::array<Bitboard, 8> init{};
        for(int i=0; i<8; ++i) {
            init[i] = FILE_A_BB << i;
        }
        return init;
    }

    constexpr std::array<Bitboard, 8> initialise_rank_bbs() {
        std::array<Bitboard, 8> init{};
        for(int i=0; i<8; ++i) {
            init[i] = RANK_1_BB << 8 * i;
        }
        return init;
    }

    constexpr std::array<Bitboard, 15> initialise_diag_bbs() {
        std::array<Bitboard, 15> init{};
        for(int i=0; i<64; ++i) {
            int d = diagonal_from_square(i);
            bb_add_square(init[d], i);
        }
        return init;
    }

    constexpr std::array<Bitboard, 15> initialise_anti_diag_bbs() {
        std::array<Bitboard, 15> init{};
        for(int i=0; i<64; ++i) {
            int d = antidiag_from_square(i);
            bb_add_square(init[d], i);
        }
        return init;
    }

    // do this in a slow but correct way
    constexpr std::array<std::array<Bitboard, 64>, 8> initialise_ray_bbs() {
        std::array<std::array<Bitboard, 64>, 8> init{};
        for(Square sq=0; sq<64; ++sq) {
            for(Direction d : all_directions) {
                const auto [dx, dy] = offsets_of_dir[d];
                const auto [x, y] = rank_file_from_square(sq);
                Bitboard bb = 0ull;
                for(int i=1; check_rank_file(x + i*dx, y + i*dy); ++i) {
                    bb_add_square(bb, square_from_rank_file(x + i*dx, y + i*dy));
                }
                init[d][sq] = bb;
            }
        }
        return init;
    }

    const Bitboard FULL_BB = 0xFFFFFFFFFFFFFFFF;
    // diagonal is up-left, anti-diagonal is down-right
    const std::array<Bitboard, 8> FILE_BBS {initialise_file_bbs()};
    const std::array<Bitboard, 8> RANK_BBS {initialise_rank_bbs()};
    const std::array<Bitboard, 15> DIAG_BBS {initialise_diag_bbs()};
    const std::array<Bitboard, 15> ANTI_DIAG_BBS {initialise_anti_diag_bbs()};
    // usage RAY_BBS[WEST][A3]
    const std::array<std::array<Bitboard, 64>, 8> RAY_BBS {initialise_ray_bbs()};

    std::array<Bitboard, 64> compute_all_king_attacks();
    std::array<Bitboard, 64> compute_all_knight_attacks();
    std::array<Bitboard, 64> compute_all_pawn_attacks(Color color);
}