#ifndef evaluation_h
#define evaluation_h
#include <vector>
#include <queue>
#include "const.hpp"
#include "bitboard.hpp"
#include "bidirectional_map.hpp"
typedef unsigned long long uint64;

class Evaluation{
private:
    Evaluation(): attack_table(NUM_SQUARES){
        init_piece_keys();
    }
    int material = 0;
    int piece_square_value = 0;
    int king_safety_value = 0;

    BidirectionalMap<unsigned int, unsigned int, NUM_PIECE_KEYS, NUM_SQUARES> piece_to_square;
    std::vector<int> attack_table;
    unsigned int square_blocking[NUM_SQUARES][NUM_DIRECTIONS], square_blocked[NUM_SQUARES][NUM_DIRECTIONS]; // each square can be blocked by at most 8 
    std::queue<unsigned int> blocking_piece_queue; // stack of piece keys whose blocking state has changed since the last move
    unsigned int piece_keys[NUM_SIDES][NUM_PIECE_TYPES][8]; // unique key for each piece, each piece from each side has a unique key assigned from starting position, used for keeping track of piece location
    const unsigned int directions[NUM_DIRECTIONS] = {NORTH, SOUTH, WEST, EAST, NORTHEAST, NORTHWEST, SOUTHEAST, SOUTHWEST};
public:
    static Evaluation* instanceptr;
    Evaluation(const Evaluation& obj) = delete;
    static Evaluation* get_instance();

    unsigned int get_piece_key(unsigned int side, unsigned int piece, unsigned int idx);
    unsigned int get_side_from_piece_key(unsigned int piece_key);
    unsigned int get_piece_from_piece_key(unsigned int piece_key);
    
    int get_material();
    int get_piece_square_value();
    int get_evaluation();
    unsigned int get_piece_key_on_square(unsigned int square);
    unsigned int get_square_of_piece_key(unsigned int piece_key);

    bool is_sliding_piece(unsigned int piece_key);

    void init_material();
    void init_piece_square_value();
    void init_piece_locations();
    void init_piece_keys();

    void update_material(unsigned int move, bool reverse=false);
    void update_piece_square_value(unsigned int move, bool reverse=false);
    void update_piece_locations(unsigned int move, bool reverse=false);

    static unsigned int get_chebyshev_distance(unsigned int from, unsigned int to);
    Bitboard* bb = Bitboard::get_instance();
};
#endif