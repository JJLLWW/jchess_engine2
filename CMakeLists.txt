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