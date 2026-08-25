#!/usr/bin/env bash

FEN_STR=$(cat "$1")
DEPTH=$2

# Shift off the first two arguments ($1: file, $2: depth)
shift 2

# Check if any setup moves remain
MOVES_STR=""
if [ $# -gt 0 ]; then
    MOVES_STR=" moves $*"
fi

echo -e "position fen ${FEN_STR}${MOVES_STR}\ngo perft ${DEPTH}" | stockfish