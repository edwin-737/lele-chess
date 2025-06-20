#include "const.hpp"
#include "board_squares.hpp"
#include "move.hpp"

void BoardSquares::init_squares()
{
    for (uint64 i = 0; i < 64; i++) squares[i] = 1ULL << i;
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
uint64 BoardSquares::get_from_to(int from, int to){
    return get_square_bitboard(from) ^ get_square_bitboard(to);
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
uint64& BoardSquares::get_square_bitboard(int idx)
{
    return squares[idx];
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
