#ifndef utils_h
#define utils_h
#include <iostream>
#include <cassert>
#include "const.hpp"
using namespace std;
typedef unsigned long long uint64;

const uint64 DE_BRUIJN = 0x03f79d71b4cb0a89ULL;
const int index64[64] = {
    0,  1, 48,  2, 57, 49, 28,  3,
   61, 58, 50, 42, 38, 29, 17,  4,
   62, 55, 59, 36, 53, 51, 43, 22,
   45, 39, 33, 30, 24, 18, 12,  5,
   63, 47, 56, 27, 60, 41, 37, 16,
   54, 35, 52, 21, 44, 32, 23, 11,
   46, 26, 40, 15, 34, 20, 31, 10,
   25, 14, 19,  9, 13,  8,  7,  6
};
inline void get_set_bit_indices(uint64 n, int* set_bits){

    for(int i = 0 ; i < NUM_SQUARES ; i ++)
        set_bits[i] = 0;
    int set_bits_idx = 0;
    int index = 1;
    while (n) {
        if(n & 1)
            set_bits[set_bits_idx] = index;
        set_bits_idx += n & 1;
        n >>= 1;
        index += 1;
    }
}
inline int bit_scan_forward(uint64 bb){
    if(bb > 0){
        // cout<<"bit scan not empty\n";
        return index64[((bb & -bb) * DE_BRUIJN) >> 58];
    }
    else{
        // cout<<"bit scan empty\n";
        return -1;
    }
}
inline uint64 OR_mult(uint64 others[], int len)
{
    uint64 board_union = 0ULL;
    for(int i = 0 ; i < len ; i ++)
        board_union |= others[i];
    return board_union;
}
inline uint64 AND_mult(uint64 others[], int len)
{
    uint64 board_union = others[0];
    for(int i = 0 ; i < len ; i ++)
        board_union &= others[i];
    return board_union;
}

inline int count_1s(uint64 b)
{
    int r;
    for (r = 0; b; r++, b &= b - 1)
        ;
    return r;
}
inline void print_board(char* board)
{
    for(int rank = 7 ; rank >= 0 ; rank --){
        for(int file = 0 ; file < 8 ; file ++){
            int sq = rank * 8 + file;
            cout<<board[sq]<<" ";
            if((sq + 1) % 8 == 0 && sq > 0)
                cout<<"\n";
        }
    }
}
#endif