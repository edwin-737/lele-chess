#ifndef bitboard_h
#define bitboard_h
#include "const.hpp"
#include <array>
typedef unsigned long long uint64;


class Bitboard{
public:
    Bitboard(){
        reset();
    }
    bool initialised = false;
    void reset();
    bool is_initialised();
    void init_piece_boards();
    void update();
    bool any_piece_on_square(int sq);
    bool any_piece_on_square(int side, int sq);
    bool attacked(unsigned int defending_side, unsigned int sq);
    unsigned int get_lowest_value_attacker_to(unsigned int defending_side, unsigned int sq);
    unsigned int get_piece_on_square(unsigned int side, unsigned int square);
    unsigned int get_captured_piece(unsigned int move);
    void display();
    void display_bitboard(uint64 b);
    inline static uint64 piece_boards[NUM_SIDES][NUM_PIECE_TYPES];
    inline static uint64 all, white, black;
    inline static uint64 collective_piece_boards[NUM_SIDES];
    inline static unsigned int piece_on_square[NUM_SQUARES];

};
// calculate ray between each pair of squares
// at each move gen stage
//      pinning_bishops_piece_board = get_bishop_attack_set(bb, king_location[side], side) & (piece_boards[side ^ 1][pBISHOP] | piece_boards[side ^ 1][pQUEEN]);
//      .........
//      ..b......
//      ...,.....
//      ....B....
//      .....,...
//      ......K..
//      .........
//      .........
//      uint64 full_pin = 0; // piece cant move at all (if blocking two pinners)
//      ATTACK_DIRECTIONS = {DIAG_UL_DR, HORIZ, VERT}
//      for(from in pinning_bishop_piece_board)
//        uint64 possible_pinned_pieces = ray_bishops[from][king[side]] & coll_piece_boards[side];
//        uint64 blocking_pieces = ray_bishops[from][king[side]] & coll_piece_boards[side ^ 1];
//        if(pop_cnt(possible_pinned_pieces) > 1)
//             continue;
//        if(blocking_pieces)
//             continue;
//        if(possible_pinned_pieces & pinned)
//             full_pins |= possible_pinned_pieces;
//        else if(piece == pKNIGHT)
//             full_pins |= possible_pinned_pieces;
//        attack_direction = get_attack_direction(from, to)
//        pinned[attack_direction] |= possible_pinned_pieces;
//        
//      for(from in piece_boards[side][piece]){
//      }
//      if(ray_bishop[])
#endif