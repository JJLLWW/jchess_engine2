cmake_minimum_required(VERSION 3.26)
project(jchess_engine)

set(CMAKE_CXX_STANDARD 20)

add_executable(magics src/misc/magic_finder.c)
add_executable(zobrist src/misc/zobrist_constant_gen.cpp)

Include(FetchContent)

set(FATHOM_SRC_ROOT src/jchess/syzygy/fathom_third_party)

# third party in tree dependencies
add_library(fathom_lib STATIC
    ${FATHOM_SRC_ROOT}/tbprobe.c
)
target_include_directories(fathom_lib PRIVATE ${FATHOM_SRC_ROOT})

add_library(jdart_nnue INTERFACE)
target_include_directories(jdart_nnue INTERFACE third_party/nnue)
message(${CMAKE_HOST_SYSTEM_PROCESSOR})
# TODO: check this works for non arm64 processor
if(${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL arm64)
    target_compile_definitions(jdart_nnue INTERFACE SIMD NEON)
    target_compile_options(jdart_nnue INTERFACE -mcpu=apple-m1)
else()
    target_compile_definitions(jdart_nnue INTERFACE SIMD AVX2)
    target_compile_options(jdart_nnue INTERFACE -mavx2 -mbmi2 -msse2)
endif()
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
    src/jchess/syzygy/sz_wrapper.cpp
    src/jchess/nnue/wrap_nnue.cpp
    src/jchess/search_limits.cpp
)
target_link_libraries(chess_lib PRIVATE jdart_nnue)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(chess_lib PUBLIC MOVE_DEBUG)
endif()
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
FetchContent_Declare(
    Mio
    GIT_REPOSITORY https://github.com/vimpunk/mio
)
FetchContent_MakeAvailable(Mio)
FetchContent_Declare(
    SpdLog
    GIT_REPOSITORY https://github.com/gabime/spdlog
    GIT_TAG v1.13.0
)
FetchContent_MakeAvailable(SpdLog)

target_link_libraries(chess_lib PUBLIC Boost::container mio::mio)
target_link_libraries(chess_lib PRIVATE spdlog::spdlog)

add_executable(jchess_engine src/main.cpp)
target_link_libraries(jchess_engine PRIVATE chess_lib fathom_lib)

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
    test/polyglot_util.cpp
    test/syzgy.cpp
    test/nnue.cpp
    test/engine.cpp
    test/search_time.cpp
)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain chess_lib fathom_lib)
target_include_directories(tests PRIVATE src)

add_executable(perft test/perft.cpp)
target_link_libraries(perft PRIVATE chess_lib)
target_include_directories(perft PRIVATE src)

add_executable(perft2 test/perft2.cpp)
target_link_libraries(perft2 PRIVATE chess_lib nlohmann_json::nlohmann_json)
target_include_directories(perft2 PRIVATE src)

add_executable(search_stress test/search_stress.cpp)
target_link_libraries(search_stress PRIVATE chess_lib)
target_include_directories(search_stress PRIVATE src)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(tests)

