#ifndef TRANSPOSITION_TABLE_H
#define TRANSPOSITION_TABLE_H
#include "const.hpp"
#include "board_info.hpp"
#include "board_squares.hpp"
#include <unordered_map>
#define NUM_ZOBRIST_VALS 1600
#define ZOBRIST_OFFSET_PIECE_ON_SQUARE 0
#define ZOBRIST_OFFSET_SIDE_TO_MOVE 768
#define ZOBRIST_OFFSET_CASTLE_RIGHTS 769
#define ZOBRIST_OFFSET_EP_RIGHTS 785 
#define MAX_DEPTH 10
#define DEFAULT_EVAL -1000000000

using namespace BoardSquares;

class TranspositionTable{
private:
    TranspositionTable(){
        generate_zobrist_values();
    };
    void generate_zobrist_values();
    uint64 get_zobrist_value(unsigned int zobrist_offset, unsigned int idx);
    unsigned int calculate_zobrist_idx_piece_square(unsigned int side, unsigned int piece, unsigned int square);
    unsigned int calculate_zobrist_idx_side_to_move(unsigned int side);
    unsigned int calculate_zobrist_idx_castle_rights(unsigned int castle_right);
    unsigned int calculate_zobrist_idx_ep_rights(unsigned int ep_right);
public: 
    static TranspositionTable* instanceptr;
    TranspositionTable(const TranspositionTable& obj) = delete;
    static TranspositionTable* get_instance();
    void initialise_hash_val(unsigned int side_to_move, Bitboard* bb, BoardInfo* bi);
    void update_table(uint64 val);
    void update_hash_val_side_to_move(unsigned int move);
    void update_hash_val_piece_square(unsigned int move);
    void update_hash_val_castle_rights(unsigned int prev_castle_right, unsigned int next_castle_right);
    void update_hash_val_ep_rights(unsigned int prev_ep_right, unsigned int next_ep_right);
    unsigned int get_value_perft(int depth);
    int get_value_eval(int depth);
    unsigned int get_value_threefold();
    void found_value();
    void not_equal();
    void add_value_perft(int depth, unsigned int val);
    void add_value_eval(int depth, int val);
    void add_value_threefold(int val=1);
    void increment_value_threefold();
    void decrement_value_threefold();
    unordered_map<uint64, unsigned int> perft_table[MAX_DEPTH];
    unordered_map<uint64, int> eval_table[MAX_DEPTH];
    unordered_map<uint64, unsigned int> threefold_table;
    uint64 zobrist_vals[NUM_ZOBRIST_VALS];
    uint64 hash_val = 0;
};
#endif