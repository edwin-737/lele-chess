#include "evaluation.hpp"
#include "board_squares.hpp"
#include "const.hpp"
#include "utils.hpp"
#include "move.hpp"
#include "move_set.hpp"
using namespace BoardSquares;

Evaluation* Evaluation::instanceptr=nullptr;

Evaluation* Evaluation::get_instance(){
    if(instanceptr == nullptr)
        instanceptr = new Evaluation();
    return instanceptr;
}

int Evaluation::get_material(){
    return material;
}

int Evaluation::get_piece_square_value(){
    return piece_square_value;
}

int Evaluation::get_evaluation(){
    return 100 * material + piece_square_value;
}

unsigned int Evaluation::get_piece_key(unsigned int side, unsigned int piece, unsigned int idx){
    return (side * NUM_PIECE_TYPES * MAX_NUM_PIECES) + (piece * MAX_NUM_PIECES) + idx;
}

unsigned int Evaluation::get_piece_key_on_square(unsigned int square){
    return piece_to_square.vk[square];
}

unsigned int Evaluation::get_square_of_piece_key(unsigned int piece_key){
    return piece_to_square.kv[piece_key];
}

unsigned int Evaluation::get_side_from_piece_key(unsigned int piece_key){
    constexpr unsigned int BLOCK_SIDE  = NUM_PIECE_TYPES * MAX_NUM_PIECES;
    constexpr unsigned int BLOCK_PIECE = MAX_NUM_PIECES;
    unsigned int side = piece_key / BLOCK_SIDE;
    return side;
}

unsigned int Evaluation::get_piece_from_piece_key(unsigned int piece_key){
    constexpr unsigned int BLOCK_SIDE  = NUM_PIECE_TYPES * MAX_NUM_PIECES;
    constexpr unsigned int BLOCK_PIECE = MAX_NUM_PIECES;
    unsigned int piece = (piece_key % BLOCK_SIDE) / BLOCK_PIECE;
    return piece;
}

bool Evaluation::is_sliding_piece(unsigned int piece_key){
    unsigned int piece = get_piece_from_piece_key(piece_key);
    return piece == pBISHOP || piece == pROOK || piece == pQUEEN;
}


void Evaluation::init_material(){
    for(int side = 0 ; side < NUM_SIDES ; side ++){
        for(int piece = 0 ; piece < NUM_PIECE_TYPES ; piece ++){
            uint64 b = bb->piece_boards[side][piece];
            int count = 0;
            int set_bit = 0;
            while(set_bit != -1){ // bit scan the board and invert every set bit until its empty
                set_bit = bit_scan_forward(b);
                if (set_bit != -1) {
                    count++;
                    b ^= get_square_bitboard(set_bit);  // safe clearing
                } else {
                    break;
                }
            }
            material = side == WHITE ? material + (count * piece_values[piece]) : material - (count * piece_values[piece]);
        }
    }
}

void Evaluation::init_piece_square_value(){
    for(int side = 0 ; side < NUM_SIDES ; side ++){
        for(int piece = 0 ; piece < NUM_PIECE_TYPES ; piece ++){
            for(int sq = 0 ; sq < NUM_SQUARES ; sq ++){
                piece_square_value += PIECE_SQUARE_TABLE[side][piece][sq];
            }
        }
    }
}

void Evaluation::init_piece_locations(){
    for(unsigned int piece_key = 0 ; piece_key <  NUM_SIDES * NUM_PIECE_TYPES * 8 ; piece_key ++){
        piece_to_square.kv[piece_key] = INVALID_LOCATION; // initialise to an invalid square
    }
    for(unsigned int sq = 0 ; sq <  NUM_SQUARES ; sq ++){
        piece_to_square.vk[sq] = NO_PIECE_KEY; // initialise to an invalid square
    }
    for(unsigned int side = 0 ; side < NUM_SIDES ; side ++){
        for(unsigned int piece = 0 ; piece < NUM_PIECE_TYPES ; piece ++){
            unsigned int idx = 0;
            for(unsigned int sq = 0 ; sq < NUM_SQUARES ; sq ++){
                if(bb->piece_boards[side][piece] & get_square_bitboard(sq)){
                    unsigned int piece_key = piece_keys[side][piece][idx];
                    piece_to_square.kv[piece_key] = sq;
                    piece_to_square.vk[sq] = piece_key;
                    // increase idx only once we find a piece
                    idx ++;
                }
            }
        }
    }
}

void Evaluation::init_piece_keys(){
    // depending on the starting position, 
    // each piece will have its own key, so we can keep track of multiple of the same pieces
    // 0 - 47 white
    //      0 - 7 pawns
    //      8 - 15 knights
    // 48 - 95 black
    //      56 - 63 pawns
    //      64 - 71 knight
    for(unsigned int side = 0 ; side < NUM_SIDES ; side ++){
        for(unsigned int piece = 0 ; piece < NUM_PIECE_TYPES ; piece ++){
            for(unsigned int idx = 0 ; idx < MAX_NUM_PIECES ; idx ++){
                unsigned int piece_key = get_piece_key(side, piece, idx);
                piece_keys[side][piece][idx] = piece_key;
            }
        }
    }
}

void Evaluation::update_material(unsigned int move, bool reverse){
    if(MoveUtils::is_capture(move)){
        unsigned int captured_piece = MoveUtils::get_captured_piece(move);
        unsigned int side = MoveUtils::get_side(move);
        unsigned int change = piece_values[captured_piece];

        if(reverse)
            change *= -1;

        if(side == WHITE){
            material += change;
        } else {
            material -= change;
        }
    }
}
void Evaluation::update_piece_locations(unsigned int move, bool reverse)
{
    
}
void Evaluation::update_piece_square_value(unsigned int move, bool reverse)
{
    unsigned int side = MoveUtils::get_side(move);
    unsigned int from = MoveUtils::get_from(move);
    unsigned int to = MoveUtils::get_to(move);
    unsigned int piece = MoveUtils::get_piece(move);

    int change = PIECE_SQUARE_TABLE[side][piece][to] - PIECE_SQUARE_TABLE[side][piece][from];
    if(reverse)
        change *= -1;
    piece_square_value += change;

    if(MoveUtils::is_capture(move)){
        unsigned int captured_piece = MoveUtils::get_captured_piece(move);
        int captured_piece_change = -PIECE_SQUARE_TABLE[side][captured_piece][to];
        if(reverse){
            captured_piece_change *= -1;
        }
        piece_square_value += captured_piece_change;
    }
}

unsigned int Evaluation::get_chebyshev_distance(unsigned int from, unsigned int to){
    int from_file = (int) get_file(from);
    int to_file = (int) get_file(to);

    int from_rank = (int) get_rank(from);
    int to_rank = (int) get_rank(to);
    
    int rank_dist = abs(from_rank - to_rank);
    int file_dist = abs(from_file - to_file);

    return max(rank_dist, file_dist);
}
