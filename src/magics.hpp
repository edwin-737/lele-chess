#ifndef magic_h
#define magic_h
#include "const.hpp"
typedef unsigned long long uint64;
uint64 random_uint64();
uint64 random_uint64_fewbits();
int pop_1st_bit(uint64 *bb);
uint64 index_to_uint64(int index, int bits, uint64 m);
uint64 rmask(int sq);
uint64 rmask_all(int sq);
uint64 bmask(int sq);
uint64 bmask_all(int sq);
uint64 ratt(int sq, uint64 block);
uint64 batt(int sq, uint64 block);
int transform(uint64 b, uint64 magic, int bits);
void print_board(uint64 cur_val);
uint64 find_magic(int sq, int cnt_bits, int bishop);
inline int RBits[64] = {
    12, 11, 11, 11, 11, 11, 11, 12,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    12, 11, 11, 11, 11, 11, 11, 12
};

inline int BBits[64] = {
    6, 5, 5, 5, 5, 5, 5, 6,
    5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 5, 5, 5, 5, 5, 5, 6
};
inline int BitTable[64] = {
    63, 30, 3, 32, 25, 41, 22, 33, 15, 50, 42, 13, 11, 53, 19, 34, 61, 29, 2,
    51, 21, 43, 45, 10, 18, 47, 1, 54, 9, 57, 0, 35, 62, 31, 40, 4, 49, 5, 52,
    26, 60, 6, 23, 44, 46, 27, 56, 16, 7, 39, 48, 24, 59, 14, 12, 55, 38, 28,
    58, 20, 37, 17, 36, 8
};


class Magics{
public:
    Magics(){find_all_magics();};
    void find_all_magics();
    uint64 rook_magics[NUM_SQUARES], bishop_magics[NUM_SQUARES];
};

#endif