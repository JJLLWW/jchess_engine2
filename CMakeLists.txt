cmake_minimum_required(VERSION 3.26)
project(jchess_engine_2)

set(CMAKE_CXX_STANDARD 20)

set(TARGET_SOURCES src/main.cpp
    src/uci/uci.h
    src/uci/fen.h
    src/uci/CommandParser.cpp
    src/uci/CommandParser.h)

add_executable(jchess_engine ${TARGET_SOURCES})

target_include_directories(jchess_engine
    PRIVATE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)