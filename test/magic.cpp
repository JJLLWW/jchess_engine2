#include <catch2/catch_test_macros.hpp>

#include "jchess/magic_bitboard.h"
#include <vector>

using namespace jchess;

TEST_CASE("magics test 1") {
    MagicDatabase db;
    auto blocker_cross = (FILE_BBS[D] | RANK_BBS[RANK_4]);
    std::vector<std::pair<Bitboard, Square>> test_cases {
        {0ull, A1}, {0ull, H1}, {0ull, A8}, {0ull, H8},
        {0ull, A4}, {0ull, D1}, {0ull, D4}, {0ull, D5},
        {blocker_cross, A1}, {blocker_cross, H1},
        {blocker_cross, A8}, {blocker_cross, H8}
    };
    for(const auto [blockers, square] : test_cases) {
        auto rook_mask = get_rook_blocker_mask(square);
        auto expected_rook = get_rook_attacks(blockers & rook_mask, square);
        auto actual_rook = db.get_rook_attacks(blockers & rook_mask, square);
        REQUIRE(actual_rook == expected_rook);
        auto bishop_mask = get_bishop_blocker_mask(square);
        auto expected_bishop = get_bishop_attacks(blockers & bishop_mask, square);
        auto actual_bishop = db.get_bishop_attacks(blockers & bishop_mask, square);
        REQUIRE(expected_bishop == actual_bishop);
    }
}