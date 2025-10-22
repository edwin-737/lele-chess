#ifndef moveset_h
#define moveset_h
#include <map>
#include "const.hpp"
#include "bitboard.hpp"
#include "magics.hpp"
typedef unsigned long long uint64;
using namespace std;
namespace MoveSet{
    uint64 get_king_attack_mask(int sq);
    uint64 get_knight_attack_mask(int sq);
    uint64 get_white_pawn_attack_mask(int sq);
    uint64 get_black_pawn_attack_mask(int sq);
    uint64 get_white_pawn_forward_mask(Bitboard* bb, int sq);
    uint64 get_black_pawn_forward_mask(Bitboard* bb, int sq);
    uint64 get_pawn_forward_mask(Bitboard* bb, int sq, int side);
    void init_attack_masks();
    void set_rook_attack_set(int sq);
    void set_bishop_attack_set(int sq);
    void set_attack_sets();
    uint64 get_white_pawn_attack_set(Bitboard* bb, int sq);
    uint64 get_black_pawn_attack_set(Bitboard* bb, int sq);
    uint64 get_pawn_attack_set(Bitboard* bb, int sq, int side);
    uint64 get_knight_attack_set(Bitboard* bb, int sq, int side);
    uint64 get_king_attack_set(Bitboard* bb, int sq, int side);
    uint64 get_bishop_attack_set(Bitboard* bb, int sq);
    uint64 get_rook_attack_set(Bitboard* bb, int sq);
    uint64 get_bishop_attack_set(Bitboard* bb, int sq, int side);
    uint64 get_rook_attack_set(Bitboard* bb, int sq, int side);
    uint64 get_all_move_set(Bitboard* bb, int piece, int sq, int side);
    uint64 get_capture_move_set(Bitboard* bb, int piece, int sq, int side);
    uint64 get_quiet_move_set(Bitboard* bb, int piece, int sq, int side);
    bool king_attacked_by_move(Bitboard* bb, int piece, int sq, int side);
    // inline map<int, uint64>  rook_attack_set[NUM_SQUARES], bishop_attack_set[NUM_SQUARES];
    inline uint64 rook_attack_set[NUM_SQUARES][8192], bishop_attack_set[NUM_SQUARES][8192];
    inline uint64 rook_attack_mask[NUM_SQUARES], bishop_attack_mask[NUM_SQUARES];
    inline Magics mg = Magics();
    inline uint64 king_attack_mask[NUM_SQUARES], knight_attack_mask[NUM_SQUARES], white_pawn_attack_mask[NUM_SQUARES], black_pawn_attack_mask[NUM_SQUARES];

};
#endif