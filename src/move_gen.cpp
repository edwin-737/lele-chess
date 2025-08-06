#include <iostream>
#include <cstdlib>
#include <cmath>
#include "move_gen.hpp"
#include "board_squares.hpp"
#include "direction_map.hpp"
#include "utils.hpp"
#include "move.hpp"
#include "move_set.hpp"
using namespace std;
using namespace BoardSquares;
using namespace DirectionMap;

unsigned int MoveGen::get_move(){
    if(!initialised){
        if(!initialise_piece()){
            return NO_MOVES_LEFT;
        }
    } else if(move_type != mQUIET){
        return get_special_move();
    } else if(!update_to()){
        if(!update_from()){
            if(!update_piece()){
                if(gen_type == ALL_MOVES)
                    return get_special_move();
                /*TODO: account for ep capture when gen_type == ONLY_CAPTURES
                        account for king and queen side castle when gen_type == ONLY_QUIET
                */
                else
                    return NO_MOVES_LEFT;
            }
        }
    }
    unsigned int captured_piece = bb->get_piece_on_square(side ^ 1, to);
    unsigned int additional_info = QUIET_MOVE;

    if(captured_piece != NO_PIECE && captured_piece != pKING){
        additional_info = CAPTURE;
    } else if(piece == pPAWN && abs((int)from - (int)to) == 16){
        additional_info = DOUBLE_PAWN_PUSH;
    } else if(piece == pPAWN && is_final_rank(to)){
        if(captured_piece != NO_PIECE && captured_piece != pKING){
            // if(promoted_piece == pKNIGHT)
            //     additional_info = KNIGHT_CAPTURE_PROMOTION;
            // else if(promoted_piece == pBISHOP)
            //     additional_info = BISHOP_CAPTURE_PROMOTION;
            // else if(promoted_piece == pROOK)
            //     additional_info = ROOK_CAPTURE_PROMOTION;
            // else if(promoted_piece == pQUEEN)
            //     additional_info = QUEEN_CAPTURE_PROMOTION;
            // promoted_piece = (promoted_piece - 1) + 1 % 4 + 1;
            additional_info = QUEEN_CAPTURE_PROMOTION;
        } else {
            // if(promoted_piece == pKNIGHT)
            //     additional_info = KNIGHT_PROMOTION;
            // else if(promoted_piece == pBISHOP)
            //     additional_info = BISHOP_PROMOTION;
            // else if(promoted_piece == pROOK)
            //     additional_info = ROOK_PROMOTION;
            // else if(promoted_piece == pQUEEN)
            //     additional_info = QUEEN_PROMOTION;
            // promoted_piece = (promoted_piece - 1) + 1 % 4 + 1;
            additional_info = QUEEN_PROMOTION;
        }
    }
    unsigned int move = MoveUtils::create_move(from, to, side, piece, captured_piece, additional_info);
    return move;
}

unsigned int MoveGen::get_special_move(){
    unsigned int move = 0;
    if(move_type == mQUIET){
        move = INCREMENTING_MOVE_TYPE;
        move_type ++;
    }
    else if(move_type == mKING_CASTLE){
        // cout<<"move_type is mKING_CASTLE\n";
        if(can_castle_kingside(side)){
            // cout<<"getting king castle move\n";
            move = side == WHITE ? MoveUtils::create_move(e1, g1, side, 0, 0, KING_CASTLE) : MoveUtils::create_move(e8, g8, side, 0, 0, KING_CASTLE);
            num_special_moves[mKING_CASTLE] ++;
        } else {
            move = INCREMENTING_MOVE_TYPE;
        }
        move_type ++;
    } else if(move_type == mQUEEN_CASTLE){
        if(can_castle_queenside(side)){
            move = side == WHITE ? MoveUtils::create_move(e1, c1, side, 0, 0, QUEEN_CASTLE) : MoveUtils::create_move(e8, c8, side, 0, 0, QUEEN_CASTLE);
            num_special_moves[mQUEEN_CASTLE] ++;
        } else {
            move = INCREMENTING_MOVE_TYPE;
        }
        move_type ++;
    } else if(move_type == mEP_CAPTURE){
        move = get_ep_capture(side);
    }
    // cout<<"special_move_type: "<<special_move_type<<"\n";
    // cout<<"ep_from: "<<ep_from<<"\n";
    return move;
}


unsigned int MoveGen::get_capture(){
    return 0ULL;
}

int MoveGen::get_special_move_type(){
    return move_type;
}
bool MoveGen::initialise_piece(){

    if(initialised)
        return true;
    bool found_piece = false;
    while(!found_piece && piece < NUM_PIECE_TYPES){
        piece_board = bb->piece_boards[side][piece];
        if(piece_board){
            found_piece = update_from();
            if(found_piece){
                initialised = true;
                return true;
            }
        }
        piece ++;
    }
    return false;
}
bool MoveGen::update_piece(){

    bool found_piece = false;
    while(!found_piece && piece < NUM_PIECE_TYPES){
        if(initialised){
            piece += 1;
        }
        piece_board = bb->piece_boards[side][piece];
        if(piece_board){
            found_piece = update_from();
            if(found_piece)
                return true;
        }
    }
    return false;
}

bool MoveGen::update_from(){

    bool found_from = false;
    int counter = 0;
    while(!found_from && counter < 7){
        int next_from = bit_scan_forward(piece_board);
        if(next_from == -1){
            return false;
        }
        piece_board ^= get_square_bitboard(next_from);
        from = next_from;

        if(gen_type == ALL_MOVES){
            move_set = MoveSet::get_all_move_set(bb, piece, from, side);
        } else if(gen_type == ONLY_CAPTURES) {
            move_set = MoveSet::get_capture_move_set(bb, piece, from, side);
        } else if(gen_type == ONLY_QUIET) {
            move_set = MoveSet::get_quiet_move_set(bb, piece, from, side);
        }

        found_from = move_set > 0;
        if(found_from){
            return update_to();
        }
        counter += 1;
    }
    return false;

}

bool MoveGen::update_to(){
    int next_to = bit_scan_forward(move_set);
    if(next_to == -1){
        return false;
    }
    move_set ^= get_square_bitboard(next_to);
    to = next_to;
    return true;
}

bool MoveGen::can_castle_kingside(int side){
    if(side == WHITE){
        return (bi->peek_castle_right() & 0b1000) && 
        !(bb->all & WHITE_KING_CASTLE_SQUARES) &&
        !bb->attacked(side, e1) && !bb->attacked(side, f1) && !bb->attacked(side, g1);

    } else {
        return (bi->peek_castle_right() & 0b0010) && 
        !(bb->all & BLACK_KING_CASTLE_SQUARES) &&
        !bb->attacked(side, e8) && !bb->attacked(side, f8) && !bb->attacked(side, g8);
    }
}

bool MoveGen::can_castle_queenside(int side){
    if(side == WHITE){
        return (bi->peek_castle_right() & 0b0100) &&
        !(bb->all & WHITE_QUEEN_CASTLE_SQUARES) &&
        !bb->attacked(side, e1) && !bb->attacked(side, b1) && !bb->attacked(side, c1);
    } else {
        return (bi->peek_castle_right() & 0b0001) &&
        !(bb->all & BLACK_QUEEN_CASTLE_SQUARES) &&
        !bb->attacked(side, e8) && !bb->attacked(side, b8) && !bb->attacked(side, c8) && !bb->attacked(side, d8);
    }
}

unsigned int MoveGen::get_ep_capture(int side){
    const int ep_rights = bi->peek_ep_right();

    if(ep_rights == NO_EP_RIGHTS) {
        ep_from = EP_FINISHED;
        return 0;
    };

    int ep_from_centre = ep_rights + (a5 - (side * 8));
    int left_ep_from = ep_from_centre - 1;
    int right_ep_from = ep_from_centre + 1;

    if(ep_from == EP_START) {
        if(floor(ep_from_centre / 8) == floor(left_ep_from / 8) && bb->piece_boards[side][pPAWN] & get_square_bitboard(left_ep_from))
            ep_from = left_ep_from;
        else if(floor(ep_from_centre / 8) == floor(right_ep_from / 8) && (bb->piece_boards[side][pPAWN] & get_square_bitboard(right_ep_from)))
            ep_from = right_ep_from;
        else{
            ep_from = EP_FINISHED;
            return NO_MOVES_LEFT;
        }
    } else if(ep_from == left_ep_from) {
        if(floor(ep_from_centre / 8) == floor(right_ep_from / 8) && bb->piece_boards[side][pPAWN] & get_square_bitboard(right_ep_from))
            ep_from = right_ep_from;
        else{
            ep_from = EP_FINISHED;
            return NO_MOVES_LEFT;
        }
    } else if(ep_from == right_ep_from) {
        ep_from = EP_FINISHED;
        return NO_MOVES_LEFT;
    }
    unsigned int ep_to = side == WHITE ? ep_from_centre + 8 : ep_from_centre - 8;
    unsigned int ep_capture_file = ep_rights & 0xf;
    unsigned int move = MoveUtils::create_move(ep_from, ep_to, side, pPAWN, pPAWN, EP_CAPTURE, ep_capture_file);
    return move;
}
bool MoveGen::is_final_rank(int square){
    return (get_rank_bitboard(7) & get_square_bitboard(square)) || (get_rank_bitboard(0) & get_square_bitboard(square));
}
void MoveGen::set_gen_type(int _gen_type){
    gen_type = _gen_type;
}
void MoveGen::set_move_type(int _move_type){
    move_type = _move_type;
}