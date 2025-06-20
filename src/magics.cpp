#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "magics.hpp"
#include "utils.hpp"
using namespace std;
uint64 random_uint64()
{
    uint64 u1, u2, u3, u4;
    u1 = (uint64)(random()) & 0xFFFF;
    u2 = (uint64)(random()) & 0xFFFF;
    u3 = (uint64)(random()) & 0xFFFF;
    u4 = (uint64)(random()) & 0xFFFF;
    return u1 | (u2 << 16) | (u3 << 32) | (u4 << 48);
}

uint64 random_uint64_fewbits()
{
    return random_uint64() & random_uint64() & random_uint64();
}


int pop_1st_bit(uint64 *bb)
{
    uint64 b = *bb ^ (*bb - 1);
    unsigned int fold = (unsigned)((b & 0xffffffff) ^ (b >> 32));
    *bb &= (*bb - 1);
    return BitTable[(fold * 0x783a9b23) >> 26];
}

uint64 index_to_uint64(int index, int bits, uint64 m)
{
    int i, j;
    uint64 result = 0ULL;
    for (i = 0; i < bits; i++)
    {
        j = pop_1st_bit(&m);
        if (index & (1 << i))
            result |= (1ULL << j);
    }
    return result;
}

uint64 rmask(int sq)
{
    uint64 result = 0ULL;
    int rk = sq / 8, fl = sq % 8, r, f;
    for (r = rk + 1; r <= 6; r++)
        result |= (1ULL << (fl + r * 8));
    for (r = rk - 1; r >= 1; r--)
        result |= (1ULL << (fl + r * 8));
    for (f = fl + 1; f <= 6; f++)
        result |= (1ULL << (f + rk * 8));
    for (f = fl - 1; f >= 1; f--)
        result |= (1ULL << (f + rk * 8));
    return result;
}

uint64 bmask(int sq)
{
    uint64 result = 0ULL;
    int rk = sq / 8, fl = sq % 8, r, f;
    for (r = rk + 1, f = fl + 1; r <= 6 && f <= 6; r++, f++)
        result |= (1ULL << (f + r * 8));
    for (r = rk + 1, f = fl - 1; r <= 6 && f >= 1; r++, f--)
        result |= (1ULL << (f + r * 8));
    for (r = rk - 1, f = fl + 1; r >= 1 && f <= 6; r--, f++)
        result |= (1ULL << (f + r * 8));
    for (r = rk - 1, f = fl - 1; r >= 1 && f >= 1; r--, f--)
        result |= (1ULL << (f + r * 8));
    return result;
}

uint64 ratt(int sq, uint64 block)
{
    uint64 result = 0ULL;
    int rk = sq / 8, fl = sq % 8, r, f;
    for (r = rk + 1; r <= 7; r++)
    {
        result |= (1ULL << (fl + r * 8));
        if (block & (1ULL << (fl + r * 8)))
            break;
    }
    for (r = rk - 1; r >= 0; r--)
    {
        result |= (1ULL << (fl + r * 8));
        if (block & (1ULL << (fl + r * 8)))
            break;
    }
    for (f = fl + 1; f <= 7; f++)
    {
        result |= (1ULL << (f + rk * 8));
        if (block & (1ULL << (f + rk * 8)))
            break;
    }
    for (f = fl - 1; f >= 0; f--)
    {
        result |= (1ULL << (f + rk * 8));
        if (block & (1ULL << (f + rk * 8)))
            break;
    }
    return result;
}

uint64 batt(int sq, uint64 block)
{
    uint64 result = 0ULL;
    int rk = sq / 8, fl = sq % 8, r, f;
    for (r = rk + 1, f = fl + 1; r <= 7 && f <= 7; r++, f++)
    {
        result |= (1ULL << (f + r * 8));
        if (block & (1ULL << (f + r * 8)))
            break;
    }
    for (r = rk + 1, f = fl - 1; r <= 7 && f >= 0; r++, f--)
    {
        result |= (1ULL << (f + r * 8));
        if (block & (1ULL << (f + r * 8)))
            break;
    }
    for (r = rk - 1, f = fl + 1; r >= 0 && f <= 7; r--, f++)
    {
        result |= (1ULL << (f + r * 8));
        if (block & (1ULL << (f + r * 8)))
            break;
    }
    for (r = rk - 1, f = fl - 1; r >= 0 && f >= 0; r--, f--)
    {
        result |= (1ULL << (f + r * 8));
        if (block & (1ULL << (f + r * 8)))
            break;
    }
    return result;
}

int transform(uint64 b, uint64 magic, int bits)
{
    return (unsigned)((int)b * (int)magic ^ (int)(b >> 32) * (int)(magic >> 32)) >> (32 - bits);
}
void print_board(uint64 cur_val)
{
    int sq[8][8];
    for(int r = 0 ; r < 8 ; r++){
        for(int f = 0 ; f < 8 ; f++){
            sq[r][f] = cur_val % 2;
            cur_val >>= 1;  
            cout<<sq[r][f]<<",";
        }
        cout<<"\n";
    }
}
uint64 find_magic(int sq, int cnt_bits, int bishop)
{
    uint64 mask, b[4096], a[4096], used[4096], magic;
    int i, j, k, n, fail;

    mask = bishop ? bmask(sq) : rmask(sq);
    n = count_1s(mask);
    // check all possible combinations of attack mask"
    for (i = 0; i < (1 << n); i++)
    {
        b[i] = index_to_uint64(i, n, mask);

        a[i] = bishop ? batt(sq, b[i]) : ratt(sq, b[i]);
    }
    for (k = 0; k < 100000000; k++)
    {
        magic = random_uint64_fewbits();
        if (count_1s((mask * magic) & 0xFF00000000000000ULL) < 6)
            continue;
        for (i = 0; i < 4096; i++)
            used[i] = 0ULL;
        for (i = 0, fail = 0; !fail && i < (1 << n); i++)
        {
            j = transform(b[i], magic, cnt_bits);
            if (used[j] == 0ULL)
                used[j] = a[i];
            else if (used[j] != a[i])
                fail = 1;
        }
        if (!fail)
            return magic;
    }
    printf("***Failed***\n");
    return 0ULL;
}
void Magics::find_all_magics()
{
    // find magic number for each square
    for(int sq = 0; sq < NUM_SQUARES; sq ++){
        rook_magics[sq] = find_magic(sq, RBits[sq], 0);
        bishop_magics[sq] = find_magic(sq, BBits[sq], 1);
    }
}