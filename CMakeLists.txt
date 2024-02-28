cmake_minimum_required(VERSION 3.26)
project(untitled4)

set(CMAKE_CXX_STANDARD 20)

add_executable(magics src/misc/magic_finder.c)

add_library(chess_lib STATIC
    src/jchess/core.cpp
    src/jchess/uci.cpp
    src/jchess/bitboard.cpp
    src/jchess/board.cpp
    src/jchess/movegen.cpp
    src/jchess/board_state.cpp
)
target_include_directories(chess_lib PUBLIC src)

add_executable(untitled4 src/main.cpp)
target_link_libraries(untitled4 PRIVATE chess_lib)
# TODO: do the equivalent of this for other compilers
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # hit constexpr limit for the rook table, this is necessary
    target_compile_options(chess_lib PUBLIC -fconstexpr-steps=104857600)
endif ()

# attempt to use Catch2 for tests
Include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.4.0 # or a later release
)
FetchContent_MakeAvailable(Catch2)

FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(json)

add_executable(tests
    test/core.cpp
    test/board.cpp
    test/bitboard.cpp
    test/movegen.cpp
)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain chess_lib)
target_include_directories(tests PRIVATE src)

add_executable(perft test/perft.cpp)
target_link_libraries(perft PRIVATE chess_lib)
target_include_directories(perft PRIVATE src)

add_executable(perft2 test/perft2.cpp)
target_link_libraries(perft2 PRIVATE chess_lib nlohmann_json::nlohmann_json)
target_include_directories(perft2 PRIVATE src)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(tests)

