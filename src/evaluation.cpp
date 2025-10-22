#include "evaluation.hpp"
#include "move.hpp"
#include "utils.hpp"
#include "board_squares.hpp"
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
            int set_bit = 0;
            // cout<<"side: "<<side<<endl;
            // cout<<"piece: "<<piece<<endl;
            while(set_bit != -1){ // bit scan the board and invert every set bit until its empty
                set_bit = bit_scan_forward(b);
                if (set_bit != -1) {
                    count++;
                    b ^= get_square_bitboard(set_bit);  // safe clearing
                } else {
                    break;
                }
            }
            //     if(set_bit != -1){
            //         cout<<"not breaking\n";
            //         count++;
            //     } else {
            //         cout<<"breaking loop\n";
            //         break;
            //     }
            //     b &= ~get_square_bitboard(set_bit);
            // }
            // cout<<"finished while loop\n";
            // material = side == WHITE ? material + (count * piece_values[piece]) : material - (count * piece_values[piece]);
        }
    }
}
int Evaluation::get_material(){
    return material;
}
int Evaluation::get_piece_square_value(){
    return piece_square_value;
}
int Evaluation::get_evaluation(){
    // cout<<"piece_square_value: "<<piece_square_value<<endl;
    // return 100 * material + piece_square_value;
    // return 30 * material + piece_square_value;
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