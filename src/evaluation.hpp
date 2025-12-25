#ifndef evaluation_h
#define evaluation_h
#include <vector>
#include "const.hpp"
#include "bitboard.hpp"
#include "bidirectional_map.hpp"
typedef unsigned long long uint64;

class Evaluation{
private:
    Evaluation():piece_to_blocking_piece(NUM_PIECE_KEYS), blocking_piece_to_piece(NUM_PIECE_KEYS), attack_table(NUM_SQUARES){
    }
    unsigned int get_piece_key(unsigned int side, unsigned int piece, unsigned int idx);
    unsigned int get_side_from_piece_key(unsigned int piece_key);
    unsigned int get_piece_from_piece_key(unsigned int piece_key);
    int material = 0;
    int piece_square_value = 0;
    int king_safety_value = 0;

    BidirectionalMap<unsigned int, unsigned int, NUM_PIECE_KEYS, NUM_SQUARES> piece_to_square;
    std::vector<int> attack_table;
    std::vector<std::vector<unsigned int>> piece_to_blocking_piece;
    std::vector<std::vector<unsigned int>> blocking_piece_to_piece; // key is blockking piece, vals are pieces blocked by it

    unsigned int piece_keys[NUM_SIDES][NUM_PIECE_TYPES][8]; // unique key for each piece, each piece from each side has a unique key assigned from starting position, used for keeping track of piece location

public:
    static Evaluation* instanceptr;
    Evaluation(const Evaluation& obj) = delete;
    static Evaluation* get_instance();

    int get_material();
    int get_piece_square_value();
    int get_evaluation();
    unsigned int get_piece_key_on_square(unsigned int square);
    unsigned int get_square_of_piece_key(unsigned int piece_key);
    std::vector<unsigned int> get_blocking_piece_keys(unsigned int piece_key);
    std::vector<unsigned int> get_piece_keys_blocked(unsigned int blocking_piece_key);

    void update_material(unsigned int move, bool reverse=false);
    void update_piece_square_value(unsigned int move, bool reverse=false);
    void update_attack_table(unsigned int move, bool reverse=false);
    void update_piece_locations(unsigned int move, bool reverse=false);
    void update_blocking_pieces(unsigned int move, bool reverse=false);

    int get_attack_table_val(unsigned int sq);

    void init_material();
    void init_piece_square_value();
    void init_attack_table();
    void init_piece_locations();
    void init_piece_keys();
    void init_blocking_pieces();

    static unsigned int get_chebyshev_distance(unsigned int from, unsigned int to);
    Bitboard* bb = Bitboard::get_instance();
};
#endif