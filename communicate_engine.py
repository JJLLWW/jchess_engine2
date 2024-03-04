import chess
import chess.engine
from chessboard import display
import argparse

import asyncio


async def main(time_secs: int):
    game_board = display.start()

    eng_path = './cmake-build-release/untitled4'
    _, engine = await chess.engine.popen_uci(eng_path)
    board = chess.Board()
    while not board.is_game_over():
        result = await engine.play(board, chess.engine.Limit(time=time_secs))
        board.push(result.move)
        display.update(board.fen(), game_board)
        display.check_for_quit()

    await engine.quit()
    display.terminate()


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--time', nargs=1, type=int, required=True)
    args = parser.parse_args()
    asyncio.run(main(args.time[0]))