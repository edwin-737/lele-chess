#ifndef TRANSPOSITION_TABLE_H
#define TRANSPOSITION_TABLE_H
#include <map>
#include "hash_generator.hpp"
#include "bitboard.hpp"
using namespace std;
#define NUM_EP_RIGHTS 8
#define NUM_CASTLE_RIGHTS 16
class TranspositionTable{
public:
    TranspositionTable(){}
    unsigned int get_zobrist_key(unsigned int side, unsigned int piece, unsigned int sq);
    unsigned int get_zobrist_key(ZOBRIST_KEYS info_type, unsigned int idx);
    uint64 get_zobrist_val(unsigned int key);
    void init_zobrist_map();
    void init_zobrist_hash_val();
    uint64 get_current_hash_val();
    void update_zobrist_hash_val(unsigned int move, unsigned int prev_ep_right, unsigned int next_ep_right, unsigned int prev_castle_right, unsigned int next_castle_right);
    map<unsigned int, uint64> get_zobrist_map();

    void update_transposition_table(uint64 key, uint64 val, int depth_left=0);
    uint64 get_transposition_table_value(uint64 key, int depth_left=0);
private:
    Bitboard* bb = Bitboard::get_instance();
    map<unsigned int, uint64> zobrist_map;
    map<uint64, uint64> perft_transposition_table[10];
    RngState rng_state = RngState();
    uint64 zobrist_hash_val = 0ULL;
};
#endif