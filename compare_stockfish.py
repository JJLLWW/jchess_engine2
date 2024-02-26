#!/usr/bin/python3

from subprocess import Popen, PIPE, run
import argparse
import re


def parse_args() -> tuple[int, str, list[str]]:
    parser = argparse.ArgumentParser()
    parser.add_argument('--depth', nargs=1, type=int, required=True)
    parser.add_argument('--fen', nargs=1, type=str, required=False)
    parser.add_argument('--moves', nargs='*', required=False, type=str)
    start_fen = 'rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1'
    args = parser.parse_args()
    fen = args.fen[0] if args.fen else start_fen
    return args.depth[0], fen, args.moves


def sort_output(lines: list[str]):
    output_line_re = re.compile(r"[a-h][1-8]")
    lines = filter(lambda line : output_line_re.match(line), lines)
    return '\n'.join(sorted(lines))


def run_stockfish(depth: int, moves: list[str], fen: str) -> str:
    if moves is not None:
        format_moves = 'moves ' + ' '.join(moves)
    else:
        format_moves = ''
    p = Popen("stockfish", stdin=PIPE, stdout=PIPE, stderr=PIPE, text=True)
    input_str = f'position fen {fen} {format_moves}\n go perft {depth}'
    output = p.communicate(input=input_str)[0]
    return sort_output(output.splitlines())


def run_perft(depth: int, moves: list[str], fen: str) -> str:
    args = ['./cmake-build-debug/perft', f'{depth}', fen]
    if moves is not None:
        args.extend(moves)
    complete = run(args=args, text=True, capture_output=True, check=True)
    output = complete.stdout
    return sort_output(output.splitlines())


def compare_output(sf_out: str, perf_out: str):
    sf_lines = sf_out.splitlines()
    perf_lines = perf_out.splitlines()
    if len(perf_lines) != len(sf_lines):
        perf_moves = set(map(lambda line : line.split()[0], perf_lines))
        sf_moves = set(map(lambda line : line.split()[0], sf_lines))
        print(f"perf unique moves: {perf_moves - sf_moves}")
        print(f"stockfish unique moves: {sf_moves - perf_moves}")

    else:
        print("stockfish | perf")
        for sf_line, perf_line in zip(sf_lines, perf_lines):
            if sf_line != perf_line:
                print(sf_line + " | " + perf_line)


def main():
    depth, fen, moves = parse_args()
    sf_out = run_stockfish(depth, moves, fen)
    perf_out = run_perft(depth, moves, fen)
    compare_output(sf_out, perf_out)


if __name__ == "__main__":
    main()