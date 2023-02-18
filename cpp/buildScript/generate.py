#!/usr/bin/python3

import sys
from amalgamation.amalgamation import run
if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python main.py config.json", file=sys.stderr)
        exit(1)

    file = sys.argv[1]
    run(file)
