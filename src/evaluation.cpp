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
int Evaluation::get_piece_square_values(){
    return piece_square_value;
}
int Evaluation::get_evaluation(){
    return 0;
}
void Evaluation::update_material(unsigned int move, bool reverse){
    if(MoveUtils::is_capture(move)){
        unsigned int captured_piece = MoveUtils::get_captured_piece(move);
        unsigned int side = MoveUtils::get_side(move);
        unsigned int change = piece_values[captured_piece];

        // if(MoveUtils::get_from(move) == f2 && MoveUtils::get_to(move) == e3){
        //     cout<<"reverse: "<<reverse<<endl;
        //     cout<<"f2e3 material before: "<<get_material()<<endl;
        //     cout<<"captured piece: "<<captured_piece<<endl;
        //     cout<<"piece value: "<<change<<endl;
        // }
        if(reverse)
            change *= -1;

        if(side == WHITE){
            material += change;
        } else {
            material -= change;
        }

        // if(MoveUtils::get_from(move) == f2 && MoveUtils::get_to(move) == e3)
        //     cout<<"f2e3 material after: "<<get_material()<<endl;
    }
}
void Evaluation::update_piece_square_value(unsigned int move, bool reverse)
{
    unsigned int side = MoveUtils::get_side(move);
    unsigned int from = MoveUtils::get_from(move);
    unsigned int to = MoveUtils::get_to(move);
    unsigned int piece = MoveUtils::get_piece(move);

    int change = PIECE_SQUARE_TABLE[side][piece][to] - PIECE_SQUARE_TABLE[side][piece][from];
    if(side == BLACK)
        change *= -1;
    piece_square_value += change;

    if(MoveUtils::is_capture(move)){
        unsigned int captured_piece = MoveUtils::get_captured_piece(move);
        int captured_piece_change = -PIECE_SQUARE_TABLE[side][captured_piece][to];
        if(side == BLACK)
            captured_piece_change *= -1;
        piece_square_value += captured_piece_change;
    }
}