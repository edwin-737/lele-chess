# Lele
A chess engine written in c++. The goal is to produce an engine with a playing strength of about 2000 elo.

## Current Features: 
- [board.cpp] FEN string parsing
- [board.cpp] Board representation
- [move_gen.cpp] Move generation (lazy model). Includes:
  * castling
  * en passant captures
  * captures
- [search.cpp] PERFT, correct results up to depth = 7 (takes 10 mins) 

## TODO:
- Pawn promotion
- Minimax with alpha-beta pruning
  
## Build The project
`bash build.sh`

## Run the project

### Engine
TODO

### Perft
run the `engine` executable produced.

### Flags:
-f: path of fen string
-d: depth for perft to search
-s: side to move

### example:
`./engine > output.txt -f ./positions/starting_position.txt -d  6 -s w`

In this example, the fen string of the default starting position is used, and perft is run to a depth of 6, with a side of white.
