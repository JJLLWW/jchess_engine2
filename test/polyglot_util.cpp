#include <catch2/catch_test_macros.hpp>

#include "jchess/polyglot/pg_reader.h"

using namespace jchess;
using namespace jchess::polyglot;

TEST_CASE("hasher") {
    PGZobristHasher hasher;
    std::vector<std::string> input {
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1",
        "rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq d6 0 2",
        "rnbqkbnr/ppp1pppp/8/3pP3/8/8/PPPP1PPP/RNBQKBNR b KQkq - 0 2",
        "rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPP1PPP/RNBQKBNR w KQkq f6 0 3",
        "rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPPKPPP/RNBQ1BNR b kq - 0 3",
        "rnbq1bnr/ppp1pkpp/8/3pPp2/8/8/PPPPKPPP/RNBQ1BNR w - - 0 4",
        "rnbqkbnr/p1pppppp/8/8/PpP4P/8/1P1PPPP1/RNBQKBNR b KQkq c3 0 3",
        "rnbqkbnr/p1pppppp/8/8/P6P/R1p5/1P1PPPP1/1NBQKBNR b Kkq - 0 4"
    };
    std::vector<uint64_t> expected {
        0x463b96181691fc9cull,
        0x823c9b50fd114196ull,
        0x0756b94461c50fb0ull,
        0x662fafb965db29d4ull,
        0x22a48b5a8e47ff78ull,
        0x652a607ca3f242c1ull,
        0x00fdd303c946bdd9ull,
        0x3c8123ea7b067637ull,
        0x5c3f9b829b279560ull
    };
    for(int i=0; i<input.size(); ++i) {
        uint64_t output = hasher.hash_board(Board(input[i]));
        REQUIRE(output == expected[i]);
    }
}

TEST_CASE("file_mapper") {
    std::string book_path = "../data/baron30.bin";
    PGMappedBook book{book_path};
    for(int i=1; i<=100; ++i) { // unlikely this would happen by chance.
        REQUIRE(book[i-1].key <= book[i].key);
    }
}

TEST_CASE("getting_move") {
    std::string book_path = "../data/baron30.bin";
    PGMappedBook book{book_path};
    Board board{starting_fen};
    std::optional<Move> move = book.get_random_book_move(board);
    REQUIRE(move.has_value()); // the opening book had better have a move for the starting position
    board.make_move(move.value()); // did the move actually make sense in the position?
}
