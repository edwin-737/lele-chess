#include "const.hpp"
#include "board_squares.hpp"
#include "move.hpp"
#include "utils.hpp"

#include <climits>
void BoardSquares::init_squares()
{
    for (size_t i = 0; i < 64; i++) squares[i] = 1ULL << i;
    for (size_t i = 0 ; i < NUM_SQUARES ; i ++){
        for(size_t j = 0 ; j < NUM_SQUARES ; j ++){
            // from_to_table[i][j] = get_square_bitboard(i) ^ get_square_bitboard(j);
        }
    }
}
void BoardSquares::init_files()
{
    uint64 file_val = 0x0101010101010101ULL;
    for (int i = 0; i < 8; i++)
    {
        files[i] = file_val;
        clear_files[i] = ~ file_val;
        file_val = file_val << 1;
    }
}
void BoardSquares::init_ranks()
{
    uint64 rank_val = 0xFFULL;
    for(int i = 0 ; i < 8 ; i ++){
        ranks[i] = rank_val;
        rank_val = rank_val << 8;
    }
}
void BoardSquares::init_blocks()
{
    uint64 accumulator = 0ULL;
    for(unsigned int from = 0 ; from < NUM_RANKS ; from ++){
        accumulator |= ranks[from];
        for(unsigned int to = from; to < NUM_RANKS ; to ++){
            rank_blocks[from][to] = accumulator;
            rank_blocks[to][NUM_RANKS - 1] = ULONG_LONG_MAX ^ accumulator;
        }
    }
    accumulator = 0;
    for(unsigned int from = 0 ; from < NUM_FILES ; from ++){
        accumulator |= files[from];
        for(unsigned int to = from; to < NUM_FILES; to ++){
            file_blocks[from][to] = accumulator;
            file_blocks[to][NUM_RANKS - 1] = ULONG_LONG_MAX ^ accumulator;
        }
    }
}
direction_e BoardSquares::get_direction(unsigned int from, unsigned int to)
{
    int rank_from = get_rank(from);
    int rank_to = get_rank(to);
    int file_from = get_file(from);
    int file_to = get_file(to);
    if(from == to){
        return INVALID_DIRECTION;
    } else if(rank_from == rank_to){
        if(file_from < file_to)
            return EAST;
        else
            return WEST;
    }
    else if(file_from == file_to){
        if(rank_from < rank_to)
            return NORTH;
        else
            return SOUTH;
    } else if(rank_from - rank_to == -(file_from - file_to)){
        if(file_from < file_to)
            return SOUTHEAST;
        else
            return NORTHWEST;
    }
    else if(rank_from - rank_to == file_from - file_to){
        if(file_from < file_to)
            return NORTHEAST;
        else 
            return SOUTHWEST;
    }
    return INVALID_DIRECTION;
}

uint64 BoardSquares::get_block(unsigned int from, unsigned int to, direction_e dir)
{
    // if(dir == NORTH){
        
    // }
    return 0;
}
void BoardSquares::init_ray_mask()
{
    for(unsigned int sq = 0 ; sq < NUM_SQUARES ; sq ++){
        unsigned int rk = sq / 8, fl = sq % 8, r, f;
        for (r = rk, f = fl; r < NUM_RANKS && f < NUM_FILES; r++, f++)
            ray_mask[NORTHEAST][sq] |= (1ULL << (f + r * 8));
        for (r = rk, f = fl; r < NUM_RANKS && f >= 0; r++, f--)
            ray_mask[NORTHEAST][sq] |= (1ULL << (f + r * 8));
        for (r = rk, f = fl; r >= 0 && f < NUM_FILES; r--, f++)
            ray_mask[SOUTHEAST][sq] |= (1ULL << (f + r * 8));
        for (r = rk, f = fl; r >= 0 && f >= 0; r--, f--)
            ray_mask[SOUTHWEST][sq] |= (1ULL << (f + r * 8));
    }
    for(unsigned int sq = 0 ; sq < NUM_SQUARES ; sq ++){
        unsigned int rk = sq / 8, fl = sq % 8, r, f;
        for (r = 0; r < NUM_RANKS; r++)
            ray_mask[NORTH][sq] |= (1ULL << (fl + r * 8));
        for (r = rk; r >= 0; r--)
            ray_mask[SOUTH][sq] |= (1ULL << (fl + r * 8));
        for (f = fl; f < NUM_FILES; f++)
            ray_mask[EAST][sq] |= (1ULL << (f + rk * 8));
        for (f = fl; f >= 0; f--)
            ray_mask[WEST][sq] |= (1ULL << (f + rk * 8));
    }
}

uint64 BoardSquares::get_from_to(int from, int to){
    return (1ULL << from) | (1ULL << to);
}
string BoardSquares::get_square(int sq)
{
    string algebraic_sq = "";
    char fi = sq % 8 + 'a';
    char rank = int(sq / 8) + '1';
    algebraic_sq += fi;
    algebraic_sq += rank;
    return algebraic_sq;
}
uint64 BoardSquares::get_square_bitboard(int idx)
{
    return 1ULL << idx;
}
uint64& BoardSquares::get_file_bitboard(int idx)
{
    return files[idx];
}
uint64& BoardSquares::get_rank_bitboard(int idx)
{
    return ranks[idx];
}

uint64& BoardSquares::get_clear_file_bitboard(int idx)
{
    return clear_files[idx];
}
