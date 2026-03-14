#ifndef bitboard_h
#define bitboard_h
#include "const.hpp"
typedef unsigned long long uint64;
class Bitboard{
private:
    Bitboard(){
        reset();
    }

    bool initialised = false;
public:
    static Bitboard* instanceptr;
    Bitboard(const Bitboard& obj) = delete;
    static Bitboard* get_instance();
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
    uint64 piece_boards[NUM_SIDES][NUM_PIECE_TYPES];
    uint64 all, white, black;
    uint64 collective_piece_boards[NUM_SIDES];
};
#endif