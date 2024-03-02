cmake_minimum_required(VERSION 3.26)
project(untitled4)

set(CMAKE_CXX_STANDARD 20)

add_executable(magics src/misc/magic_finder.c)
add_executable(zobrist src/misc/zobrist_constant_gen.cpp)

Include(FetchContent)

add_library(chess_lib STATIC
    src/jchess/core.cpp
    src/jchess/uci.cpp
    src/jchess/bitboard.cpp
    src/jchess/board.cpp
    src/jchess/movegen.cpp
    src/jchess/board_state.cpp
    src/jchess/search.cpp
    src/jchess/eval.cpp
    src/jchess/moves.cpp
    src/jchess/temp_eval.cpp
    src/jchess/engine.cpp
    src/jchess/polyglot/pg_reader.cpp
)
target_include_directories(chess_lib PUBLIC src)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # hit constexpr limit for the rook table, this is necessary
    target_compile_options(chess_lib PUBLIC -fconstexpr-steps=104857600)
endif ()

set(BOOST_INCLUDE_LIBRARIES container)
set(BOOST_ENABLE_CMAKE ON)

include(FetchContent)
FetchContent_Declare(
    Boost
    GIT_REPOSITORY https://github.com/boostorg/boost.git
    GIT_TAG boost-1.84.0
)
FetchContent_MakeAvailable(Boost)
target_link_libraries(chess_lib PUBLIC Boost::container)

FetchContent_Declare(
    Mio
    GIT_REPOSITORY https://github.com/vimpunk/mio
)
FetchContent_MakeAvailable(Mio)
target_link_libraries(chess_lib PUBLIC mio::mio)

add_executable(untitled4 src/main.cpp)
target_link_libraries(untitled4 PRIVATE chess_lib)

# attempt to use Catch2 for tests

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
    test/search.cpp
    test/movegen.cpp
    test/polyglot_util.cpp
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

