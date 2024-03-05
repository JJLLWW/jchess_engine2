import chess
import chess.engine
from chessboard import display
import argparse

import asyncio


async def main(time_secs: int, eng_path: str, fen: str):
    game_board = display.start()

    _, engine = await chess.engine.popen_uci(eng_path)
    board = chess.Board(fen)
    while not board.is_game_over():
        result = await engine.play(board, chess.engine.Limit(time=time_secs))
        board.push(result.move)
        display.update(board.fen(), game_board)
        display.check_for_quit()

    await engine.quit()
    display.terminate()


if __name__ == "__main__":
    start_fen = 'rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1'
    parser = argparse.ArgumentParser()
    parser.add_argument('--time', nargs=1, type=int, required=True)
    parser.add_argument('--engine', nargs=1, type=str, required=True)
    parser.add_argument('--fen', nargs=1, type=str, required=False)
    args = parser.parse_args()
    fen = start_fen if args.fen is None else args.fen[0]
    asyncio.run(main(args.time[0], args.engine[0], fen))