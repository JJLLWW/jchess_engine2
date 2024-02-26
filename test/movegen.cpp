#include <catch2/catch_test_macros.hpp>

#include "jchess/movegen.h"
#include "jchess/bitboard.h"
#include "jchess/board.h" // DEBUG ONLY

#include <string>

using namespace jchess;

TEST_CASE("DEBUG ONLY") {
    // pos_3, depth 5, b4f4 h4g3 a5b4 c7c5 (stockfish finds b5c6 enp out of check.)
    std::string fen = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1";
    Board board{fen};
    std::vector<std::string> move_strs {"b4f4", "h4g3", "a5b4", "c7c5"};
    for(const auto& move_str : move_strs) {
        Move move {move_str};
        board.make_move(move);
    }
    auto moves = board.generate_legal_moves();
    int i = 2;
}

TEST_CASE("xray primitives") {
    MoveGenerator gen;
    auto moves = gen.xray_rook_moves(bb_from_squares({C1, G1}), bb_from_squares({C1, G1}), A1);
    auto str = bb_to_string(moves);
    REQUIRE(true);
}

TEST_CASE("attackers of") {
    MoveGenerator gen;
    // TODO: test this
}

TEST_CASE("bishop") {
    BoardState state{"rnbqkbnr/pppppppp/8/8/6P1/8/PPPPPP1P/RNBQKBNR w KQkq - 0 1"};

}

TEST_CASE("move generation simple 1") {
    BoardState state{starting_fen};
    MoveGenerator mg;
    auto moves = mg.get_legal_moves(state, WHITE);
    // 4 knight moves, 16 pawn moves
    REQUIRE(moves.size() == 20);
}

TEST_CASE("move generation simple 2") {
    std::string pin_fen = "8/8/8/8/8/8/8/KQ1r4 w KQkq - 0 1";
    BoardState state{pin_fen};
    MoveGenerator mg;
    auto moves = mg.get_legal_moves(state, WHITE);
    // can't create a full test until king moves are implemented, LGTM
    // for just the queen though.
    REQUIRE(true);
}

TEST_CASE("castling 1") {
    std::string castle_fen = "4k3/8/8/8/8/8/8/R3K2R w KQkq - 0 1";
    BoardState state{castle_fen};
    MoveGenerator mg;
    auto w_moves = mg.get_legal_moves(state, WHITE);
    auto b_moves = mg.get_legal_moves(state, BLACK);
    auto end = std::remove_if(w_moves.begin(), w_moves.end(), [](Move move){ return (move.source == A1) || (move.source == H1); });
    w_moves.erase(end, w_moves.end());
    REQUIRE(w_moves.size() == 7); // 5 king steps and 2 castles
}

TEST_CASE("enp basic") {
    std::string fen = "K7/8/8/8/8/8/8/Pp6 w KQkq b2 0 1";
    BoardState state{fen};
    MoveGenerator mg;
    auto moves = mg.get_legal_moves(state, WHITE);
    auto new_end = std::remove_if(moves.begin(), moves.end(), [](Move move){ return move.source != A1 || move.dest != B2; });
    moves.erase(new_end, moves.end());
    REQUIRE(moves.size() == 1);
}

TEST_CASE("check 1") {
    std::string fen = "4r3/8/8/8/8/8/8/B3K2B w KQkq - 0 1";
    BoardState state{fen};
    MoveGenerator mg;
    auto moves = mg.get_legal_moves(state, WHITE);
    // king can step out of the way in 4 ways, bishops can move in front in 2 ways.
    REQUIRE(moves.size() == 6);
}

TEST_CASE("check 2") {
    std::string fen = "r7/8/8/8/8/8/8/KB5r w KQkq - 0 1";
    BoardState state{fen};
    MoveGenerator mg;
    auto moves = mg.get_legal_moves(state, WHITE);
    // the bishop is pinned so the only way out of check is to move the king to b2.
    REQUIRE((moves.size() == 1 && moves[0].dest == B2));
}

TEST_CASE("check 3") {
    std::string fen = "8/8/8/8/8/8/8/KPpr4 w KQkq c2 0 1";
    BoardState state{fen};
    MoveGenerator mg;
    auto moves = mg.get_legal_moves(state, WHITE);
    // should only be 3 moves (2 king + push pawn once) as the enp capture will put the king in check.
    REQUIRE(moves.size() == 3);
}