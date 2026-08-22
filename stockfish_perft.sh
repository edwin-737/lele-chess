FEN_STR=$(cat $1)
DEPTH=$2
echo -e "position fen $FEN_STR\ngo perft $DEPTH" | stockfish