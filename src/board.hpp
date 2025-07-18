#ifndef board_h
#define board_h
#include <stack>
#include <set>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <cctype>
#include "const.hpp"
#include "board_squares.hpp"
#include "move.hpp"
#include "bitboard.hpp"
#include "board_info.hpp"
#include "evaluation.hpp"
#include "hash_generator.hpp"
#include "transposition_table.hpp"
using namespace std;
using namespace BoardSquares;
namespace fs = filesystem;


typedef enum fen_state{
    POSITION,
    SIDE_TO_MOVE,
    CASTLE_RIGHTS,
    EP_TARGET_SQUARE,
    FINISHED
} FEN_STATE;


class Board{
public:
    Board(): side_to_move(WHITE){
        king_location[WHITE] = e1;
        king_location[BLACK] = e8;
    }
    void apply_move(unsigned int move);
    void reverse_move(unsigned int move);
    bool apply_move_if_legal(unsigned int move);
    bool can_castle_kingside(unsigned int side);
    bool can_castle_queenside(unsigned int side);
    void parse_fen(fs::path path);
    int get_side_to_move();
    void change_side_to_move();
    int get_piece_location(int side, int piece);
    int get_king_location(int side);
    unsigned int get_initial_ep_rights();
    unsigned int get_initial_castle_rights();
    BoardInfo* get_board_info();
    Bitboard* get_bitboard();
    unsigned int get_zobrist_key(unsigned int side, unsigned int piece, unsigned int sq);
    unsigned long long get_zobrist_val(unsigned int key);
    void init_zobrist_map();
    void init_zobrist_hash_val();
    void update_zobrist_hash_val(unsigned int move, unsigned int prev_ep_right, unsigned int next_ep_right, unsigned int prev_castle_right, unsigned int next_castle_right);
    uint64 get_current_hash_val();
    set<int> piece_locations[NUM_SIDES][NUM_PIECE_TYPES];
    int king_location[NUM_SIDES];
    map<unsigned int, unsigned long long> get_zobrist_map();
    void update_transposition_table(unsigned long long key, unsigned long long val, int depth_left=0);
    uint64 get_transposition_table_value(unsigned long long key, int depth_left=0);

private:
    void update_piece_locations(int side, int piece, int from, int to);
    void update_king_location(int side, int square);
    void init_piece_locations();
    BoardInfo* bi = BoardInfo::get_instance();
    Bitboard* bb = Bitboard::get_instance();
    Evaluation* eval = Evaluation::get_instance();
    TranspositionTable* tt = new TranspositionTable();
    int side_to_move;
    unsigned int initial_ep_rights = 0, initial_castle_rights = 0;
    map<unsigned int, unsigned long long> zobrist_map;
    RngState rng_state = RngState();
    unsigned long long zobrist_hash_val = 0ULL;
};
#endif