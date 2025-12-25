#ifndef bitboard_h
#define bitboard_h
#include <set>
#include "const.hpp"
typedef unsigned long long uint64;
class Bitboard{
private:
    Bitboard(){
        init_piece_boards();
        update();
        initialised = true;
    }

    bool initialised = false;
public:
    static Bitboard* instanceptr;
    Bitboard(const Bitboard& obj) = delete;
    static Bitboard* get_instance();
    bool is_initialised();
    void init_piece_boards();
    void update();
    bool any_piece_on_square(int sq);
    bool any_piece_on_square(int side, int sq);
    bool attacked(unsigned int defending_side, unsigned int sq);
    unsigned int get_lowest_value_attacker_to(unsigned int defending_side, unsigned int sq);
    unsigned int get_piece_on_square(unsigned int side, unsigned int square);
    unsigned int get_captured_piece(unsigned int move);
    unsigned int get_piece_key(unsigned int side, unsigned int piece, unsigned int idx);
    unsigned int get_side_from_piece_key(unsigned int piece_key);
    unsigned int get_piece_from_piece_key(unsigned int piece_key);
    void update_blocking_pieces();
    void display();
    void display_bitboard(uint64 b);
    unsigned int piece_location_arr[NUM_PIECE_TYPES * 8];
    uint64 piece_boards[NUM_SIDES][NUM_PIECE_TYPES];
    std::set<unsigned int> piece_locations[NUM_SIDES][NUM_PIECE_TYPES];
    uint64 all, white, black;
    uint64 collective_piece_boards[NUM_SIDES];
};
#endif