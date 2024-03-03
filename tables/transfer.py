import argparse
import subprocess


def read_cmdline() -> str:
    parser = argparse.ArgumentParser()
    parser.add_argument("--url-file", type=str, required=True, nargs=1)
    args = parser.parse_args()
    return args.url_file[0]


def transfer_url(url: str):
    file_name = url.rsplit(sep='/', maxsplit=1)[1]
    subprocess.run(args=["curl", "-o", file_name, url], check=True)


def main():
    url_path = read_cmdline()
    with open(url_path) as url_file:
        for url in url_file:
            url = url.strip()
            transfer_url(url)


if __name__ == "__main__":
    main()