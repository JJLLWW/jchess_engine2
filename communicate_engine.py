import chess
import chess.engine
from chessboard import display

import asyncio

async def main():
    game_board = display.start()

    # eng_path = '/opt/homebrew/bin/stockfish'
    eng_path = './cmake-build-debug/untitled4'
    _, engine = await chess.engine.popen_uci(eng_path)
    board = chess.Board()
    while not board.is_game_over():
        result = await engine.play(board, chess.engine.Limit(time=1))
        board.push(result.move)
        display.update(board.fen(), game_board)
        display.check_for_quit()

    await engine.quit()
    display.terminate()

if __name__ == "__main__":
    asyncio.run(main())