#include "evaluation.hpp"
#include "move.hpp"

Evaluation* Evaluation::instanceptr=nullptr;

Evaluation* Evaluation::get_instance(){
    if(instanceptr == nullptr)
        instanceptr = new Evaluation();
    return instanceptr;
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
        unsigned int change = captured_piece * piece_values[captured_piece];

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