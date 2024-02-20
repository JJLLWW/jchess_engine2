cmake_minimum_required(VERSION 3.26)
project(untitled4)

set(CMAKE_CXX_STANDARD 20)

add_executable(untitled4 src/main.cpp
    src/jchess/core.cpp
    src/jchess/core.h
    src/jchess/uci.cpp
    src/jchess/uci.h)
