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

void Evaluation::init_piece_square_value(){
    for(int side = 0 ; side < NUM_SIDES ; side ++){
        for(int piece = 0 ; piece < NUM_PIECE_TYPES ; piece ++){
            for(int sq = 0 ; sq < NUM_SQUARES ; sq ++){
                piece_square_value += PIECE_SQUARE_TABLE[side][piece][sq];
            }
        }
    }
}

void Evaluation::init_material(){
    for(int side = 0 ; side < NUM_SIDES ; side ++){
        for(int piece = 0 ; piece < NUM_PIECE_TYPES ; piece ++){
            uint64 b = bb->piece_boards[side][piece];
            int count = 0;
            unsigned int set_bit = 0;
            while(set_bit != INVALID_LOCATION){ // bit scan the board and invert every set bit until its empty
                set_bit = bit_scan_forward(b);
                if (set_bit != INVALID_LOCATION) {
                    count++;
                    b ^= get_square_bitboard(set_bit);  // safe clearing
                } else {
                    break;
                }
            }
        }
    }
}
void Evaluation::update_attack_table(unsigned int move){

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

void Evaluation::init_attack_table(){

    for(unsigned int side = 0 ; side < NUM_SIDES ; side ++){
        for(unsigned int piece = 0 ; piece < NUM_PIECE_TYPES ; piece ++){
            for(unsigned int idx = 0 ; idx < MAX_NUM_PIECES ; idx ++){
                unsigned int piece_key = get_piece_key(side, piece, idx);
                unsigned int location = get_square_of_piece_key(piece_key);
                if(location == INVALID_LOCATION)
                    continue;
                uint64 attack_set = 0;
                if(piece == pKNIGHT){
                    attack_set = MoveSet::get_knight_attack_mask(location);
                } else if(piece == pKING){
                    attack_set = MoveSet::get_king_attack_mask(location);
                } else if(piece == pPAWN){
                    if(side == WHITE)
                        attack_set = MoveSet::get_white_pawn_attack_mask(location);
                    else
                        attack_set = MoveSet::get_black_pawn_attack_mask(location);
                } else if(piece == pBISHOP){
                    attack_set = MoveSet::get_bishop_move_set(bb, location);
                } else if(piece == pROOK){
                    attack_set = MoveSet::get_rook_move_set(bb, location);
                } else if(piece == pQUEEN){
                    attack_set = MoveSet::get_bishop_move_set(bb, location) | MoveSet::get_rook_move_set(bb, location);
                }
                for(unsigned int sq = 0 ; sq < NUM_SQUARES ; sq ++){
                    if(attack_set & get_square_bitboard(sq)){
                        int piece_val = min(MAX_CAPTURE_VALUE, piece_values[piece]);
                        if(side == WHITE){
                            attack_table[sq] += (MAX_CAPTURE_VALUE + 1 - piece_val); // if piece worth is lower, attack table value increases for this side
                        }
                        else {
                            attack_table[sq] -= (MAX_CAPTURE_VALUE + 1 - piece_val); 
                        }
                    }
                }
            }
        }
    }
}
int Evaluation::get_attack_table_val(unsigned int sq){
    return attack_table[sq];
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


unsigned int Evaluation::get_piece_key(unsigned int side, unsigned int piece, unsigned int idx){
    return (side * NUM_PIECE_TYPES * MAX_NUM_PIECES) + (piece * MAX_NUM_PIECES) + idx;
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
std::vector<unsigned int> Evaluation::get_blocking_piece_keys(unsigned int piece_key){
    return piece_to_blocking_piece[piece_key];
}
std::vector<unsigned int> Evaluation::get_piece_keys_blocked(unsigned int blocking_piece_key){
    return blocking_piece_to_piece[blocking_piece_key];
}
unsigned int Evaluation::get_piece_key_on_square(unsigned int square){
    return piece_to_square.vk[square];
}
unsigned int Evaluation::get_square_of_piece_key(unsigned int piece_key){
    return piece_to_square.kv[piece_key];
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
void Evaluation::init_blocking_pieces(){
    for(unsigned int sq = 0 ; sq < NUM_SQUARES ; sq ++){
        unsigned int piece_key = piece_to_square.vk[sq];
        unsigned int piece = get_piece_from_piece_key(piece_key);
        unsigned int side = get_side_from_piece_key(piece_key);
        uint64 same_blocking_piece_set = 0;
        uint64 opp_blocking_piece_set = 0;
        if(piece == pBISHOP){
            same_blocking_piece_set = MoveSet::get_bishop_blocking_set(bb, sq, side);
            opp_blocking_piece_set = MoveSet::get_capture_move_set(bb, pBISHOP, sq, side);
        } else if(piece == pROOK){
            same_blocking_piece_set = MoveSet::get_rook_blocking_set(bb, sq, side);
            opp_blocking_piece_set = MoveSet::get_capture_move_set(bb, pROOK, sq, side);
        } else if(piece == pQUEEN){
            same_blocking_piece_set = MoveSet::get_rook_blocking_set(bb, sq, side) | MoveSet::get_bishop_blocking_set(bb, sq, side);
            opp_blocking_piece_set = MoveSet::get_capture_move_set(bb, pBISHOP, sq, side) | MoveSet::get_capture_move_set(bb, pROOK, sq, side);
        }
        if(same_blocking_piece_set > 0){
            int blocking_piece_location = 0;
            unsigned int blocking_piece_key = 0;
            while((blocking_piece_location = bit_scan_forward(same_blocking_piece_set)) != INVALID_LOCATION){
                same_blocking_piece_set ^= get_square_bitboard(blocking_piece_location);
                blocking_piece_key = piece_to_square.vk[blocking_piece_location];
                piece_to_blocking_piece[piece_key].push_back(blocking_piece_location);
                blocking_piece_to_piece[blocking_piece_key].push_back(piece_key);
            }
        }
        if(opp_blocking_piece_set > 0){
            int blocking_piece_location = 0;
            unsigned int blocking_piece_key = 0;
            while((blocking_piece_location = bit_scan_forward(opp_blocking_piece_set)) != INVALID_LOCATION){
                opp_blocking_piece_set ^= get_square_bitboard(blocking_piece_location);
                blocking_piece_key = piece_to_square.vk[blocking_piece_location];
                piece_to_blocking_piece[piece_key].push_back(blocking_piece_location);
                blocking_piece_to_piece[blocking_piece_key].push_back(piece_key);
            }
        }
    }
}
void Evaluation::update_blocking_pieces(unsigned int move, bool reverse){
    unsigned int to = MoveUtils::get_from(move);
    unsigned int piece_key = get_piece_key_on_square(to);
    unsigned int piece = get_piece_from_piece_key(piece_key);

    std::vector<unsigned int> pieces_blocked = get_piece_keys_blocked(piece_key);
    std::vector<unsigned int> blocking_pieces = get_blocking_piece_keys(piece_key);

    for(const unsigned int& blocked_piece_key : pieces_blocked){
        
    }
}