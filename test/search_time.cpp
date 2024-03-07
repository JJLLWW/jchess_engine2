#include <catch2/catch_test_macros.hpp>

#include "jchess/search_limits.h"

using namespace jchess;

TEST_CASE("basic search limits") {
    auto time1 = compute_time_to_search_msec(1000, 10, 1, 1);
    auto time2 = compute_time_to_search_msec(1000, 100, 1, 1);
    REQUIRE(time2 <= time1);
}