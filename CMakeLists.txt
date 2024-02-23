cmake_minimum_required(VERSION 3.26)
project(untitled4)

set(CMAKE_CXX_STANDARD 20)

add_library(chess_lib STATIC
    src/jchess/core.cpp
    src/jchess/uci.cpp
    src/jchess/bitboard.cpp
    src/jchess/board.cpp
    src/jchess/movegen.cpp
)
target_include_directories(chess_lib PUBLIC src)

add_executable(untitled4 src/main.cpp)
target_link_libraries(untitled4 PRIVATE chess_lib)

# attempt to use Catch2 for tests
Include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.4.0 # or a later release
)

FetchContent_MakeAvailable(Catch2)
add_executable(tests test/core.cpp test/board.cpp test/bitboard.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain chess_lib)
target_include_directories(tests PRIVATE src)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(tests)

