#include <iostream>
#include <cstdlib>
#include <cmath>
#include "move_gen.hpp"
#include "board_squares.hpp"
#include "direction_map.hpp"
#include "utils.hpp"
#include "move.hpp"
#include "move_set.hpp"
#include <immintrin.h>
using namespace std;
using namespace BoardSquares;
using namespace DirectionMap;

inline int lsb(uint64_t bb) {
    #if defined(__GNUC__) || defined(__clang__)
        return __builtin_ctzll(bb);
    #elif defined(_MSC_VER)
        unsigned long index;
        _BitScanForward64(&index, bb);
        return index;
    #endif
}

inline uint64_t pop_lsb(uint64_t& bb) {
    int s = lsb(bb);
    bb &= bb - 1; // Clears the lowest set bit in 1 CPU instruction
    return s;
}

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
                // if(gen_type == ALL_MOVES) 
                return get_special_move();
                /*TODO: account for ep capture when gen_type == ONLY_CAPTURES
                        account for king and queen side castle when gen_type == ONLY_QUIET
                */
                // else
                //     return NO_MOVES_LEFT;
            }
        }
    }
    // unsigned int captured_piece = bb->get_piece_on_square(side ^ 1, to);
    unsigned int captured_piece = bb->piece_on_square[to];
    unsigned int additional_info = QUIET_MOVE;

    if(captured_piece != NO_PIECE && captured_piece != pKING){
        additional_info = CAPTURE;
    } else if(piece == pPAWN && abs((int)from - (int)to) == 16){
        additional_info = DOUBLE_PAWN_PUSH;
    } 
    if(piece == pPAWN && MoveUtils::is_final_rank(to)){
        if(captured_piece != NO_PIECE && captured_piece != pKING){
            if(promoted_piece == pKNIGHT)
                additional_info = KNIGHT_CAPTURE_PROMOTION;
            else if(promoted_piece == pBISHOP)
                additional_info = BISHOP_CAPTURE_PROMOTION;
            else if(promoted_piece == pROOK)
                additional_info = ROOK_CAPTURE_PROMOTION;
            else if(promoted_piece == pQUEEN)
                additional_info = QUEEN_CAPTURE_PROMOTION;
            promoted_piece += 1;
        } else {
            if(promoted_piece == pKNIGHT)
                additional_info = KNIGHT_PROMOTION;
            else if(promoted_piece == pBISHOP)
                additional_info = BISHOP_PROMOTION;
            else if(promoted_piece == pROOK)
                additional_info = ROOK_PROMOTION;
            else if(promoted_piece == pQUEEN)
                additional_info = QUEEN_PROMOTION;
            promoted_piece += 1;
        }
        promotion_counter += 1;
        if(promoted_piece <= pQUEEN){
            if(!(move_set & get_square_bitboard(to)))
                move_set ^= get_square_bitboard(to);
        }
        if(promoted_piece > pQUEEN && promotion_counter < 12){
            promoted_piece = pKNIGHT;
        }
    }
    unsigned int move = MoveUtils::create_move(from, to, side, piece, additional_info, captured_piece);
    return move;
}

unsigned int MoveGen::get_special_move(){
    unsigned int move = NO_MOVES_LEFT;
    if(move_type == mQUIET){
        move = INCREMENTING_MOVE_TYPE;
        if(gen_type != ONLY_CAPTURES){
            move_type ++;
        } else {
            move_type = mEP_CAPTURE;
        }
    } else if(move_type == mKING_CASTLE){

        // cout<<"checking castle kingside\n";
        if(gen_type != ONLY_CAPTURES){
            if(can_castle_kingside(side)){
                move = side == WHITE ? MoveUtils::create_move(e1, g1, side, pKING, KING_CASTLE) : MoveUtils::create_move(e8, g8, side, pKING, KING_CASTLE);
            } else {
                move = INCREMENTING_MOVE_TYPE;
            }
        } else {
            move = INCREMENTING_MOVE_TYPE;
        }
        move_type ++;
    } else if(move_type == mQUEEN_CASTLE){
        // cout<<"checking castle queenside\n";
        if(gen_type != ONLY_CAPTURES){
            if(can_castle_queenside(side)){
                // cout<<"can_castle_queenside\n";
                move = side == WHITE ? MoveUtils::create_move(e1, c1, side, pKING, QUEEN_CASTLE) : MoveUtils::create_move(e8, c8, side, pKING, QUEEN_CASTLE);
            } else {
                move = INCREMENTING_MOVE_TYPE;
            }
        } else {
            move = INCREMENTING_MOVE_TYPE;
        }
        move_type ++;
    } else if(move_type == mEP_CAPTURE){
        if(gen_type != ONLY_QUIET){
            move = get_ep_capture();
        }
    }
    return move;
}


unsigned int MoveGen::get_capture(){
    return 0ULL;
}

int MoveGen::get_special_move_type(){
    return move_type;
}

unsigned int MoveGen::get_move(move_gen_state_t cur_state){
    unsigned int move = NO_MOVES_LEFT;
    // cout<<"MoveGen::get_move initial\n";
    // cout<<"MoveGen::get_move initial piece: "<<cur_state.piece<<"\n";
    if(cur_state.piece == NO_PIECE && !cur_state.piece_board && !cur_state.move_set){
        // cout<<"MoveGen::get_move NO_MOVES_LEFT initial\n";
        move = NO_MOVES_LEFT;
        return move;
    } else if(cur_state.move_gen_stage == NORMAL_STAGE){
        unsigned int cur_piece = cur_state.piece;
        uint64 piece_board = cur_state.piece_board;
        uint64 move_set = cur_state.move_set;

        unsigned int cur_from = get_next_from(piece_board);
        unsigned int cur_to = get_next_to(move_set);

        // cout<<"MoveGen::get_move cur_from: "<<MoveUtils::square_as_string(cur_from)<<"\n";
        // cout<<"MoveGen::get_move cur_to: "<<MoveUtils::square_as_string(cur_to)<<"\n";
        // cout<<"MoveGen::get_move cur_piece: "<<MoveUtils::piece_as_string(cur_piece)<<"\n";
        // cout<<"MoveGen::get_move cur_side: "<<MoveUtils::side_as_string(side)<<"\n";
        
        // unsigned int captured_piece = bb->get_piece_on_square(side ^ 1, cur_to);
        unsigned int captured_piece = bb->piece_on_square[cur_to];
        unsigned int additional_info = QUIET_MOVE;

        if(captured_piece != NO_PIECE && captured_piece != pKING){
            additional_info = CAPTURE;
        } 
        if(cur_piece == pPAWN && abs((int)cur_from - (int)cur_to) == 16){
            additional_info = DOUBLE_PAWN_PUSH;
        } 
        if(cur_piece == pPAWN && MoveUtils::is_final_rank(cur_to)){

            if(captured_piece != NO_PIECE && captured_piece != pKING){
                if(cur_state.promoted_piece == pKNIGHT)
                    additional_info = KNIGHT_CAPTURE_PROMOTION;
                else if(cur_state.promoted_piece == pBISHOP)
                    additional_info = BISHOP_CAPTURE_PROMOTION;
                else if(cur_state.promoted_piece == pROOK)
                    additional_info = ROOK_CAPTURE_PROMOTION;
                else if(cur_state.promoted_piece == pQUEEN)
                    additional_info = QUEEN_CAPTURE_PROMOTION;
            } else {
                if(cur_state.promoted_piece == pKNIGHT)
                    additional_info = KNIGHT_PROMOTION;
                else if(cur_state.promoted_piece == pBISHOP)
                    additional_info = BISHOP_PROMOTION;
                else if(cur_state.promoted_piece == pROOK)
                    additional_info = ROOK_PROMOTION;
                else if(cur_state.promoted_piece == pQUEEN)
                    additional_info = QUEEN_PROMOTION;
            }
        }
        move = MoveUtils::create_move(cur_from, cur_to, side, cur_piece, additional_info, captured_piece);

        return move;
    } else {
        return get_special_move();
    }
}
bool MoveGen::should_update_move_set(move_gen_state_t cur_state){

    unsigned int _cur_piece = cur_state.piece;
    uint64 _piece_board = cur_state.piece_board;
    uint64 _move_set = cur_state.move_set;

    unsigned int cur_from = get_next_from(_piece_board);
    unsigned int cur_to = get_next_to(_move_set);
    // unsigned int captured_piece = bb->get_piece_on_square(side ^ 1, cur_to);
    unsigned int captured_piece = bb->piece_on_square[cur_to];
    if(_cur_piece == pPAWN && MoveUtils::is_final_rank(cur_to)){            
        if(cur_state.promoted_piece < pQUEEN){
            return false;
        }
    }
    return true;
}
move_gen_state_t MoveGen::initialise(unsigned int starting_piece){
    move_gen_state_t move_gen_state = {
        .piece = NO_PIECE,
        .piece_board = 0,
        .move_set = 0,
        .promoted_piece = pKNIGHT,
        .move_gen_stage = NORMAL_STAGE
    };
    for(unsigned int cur_piece = starting_piece ; cur_piece < NUM_PIECE_TYPES ; cur_piece ++){
        uint64 cur_piece_board = bb->piece_boards[side][cur_piece];
        unsigned int cur_from = INVALID_LOCATION;
        unsigned int cur_to = INVALID_LOCATION;
        while(cur_piece_board){
            cur_from = get_next_from(cur_piece_board);

            // uint64 cur_move_set = MoveSet::get_all_move_set(bb, cur_piece, cur_from, side);
            uint64 cur_move_set = get_move_set(cur_piece, cur_from);
            while(cur_move_set){
                cur_to = get_next_to(cur_move_set);
                if(cur_to != INVALID_LOCATION){
                    move_gen_state = {
                        .piece = cur_piece,
                        .piece_board = cur_piece_board,
                        .move_set = cur_move_set,
                        .promoted_piece = pKNIGHT,
                        .move_gen_stage = NORMAL_STAGE
                    };
                    return move_gen_state;
                } else {
                    cur_move_set ^= get_square_bitboard(cur_to);
                }
            }
            cur_piece_board ^= get_square_bitboard(cur_from);
        }
    }
    move_gen_state = {
        .piece=pKING,
        .piece_board=bb->piece_boards[side][pKING],
        .move_set=0,
        .promoted_piece=pKNIGHT,
        .move_gen_stage=SPECIAL_STAGE
    };
    
    return move_gen_state;
}

move_gen_state_t MoveGen::update(move_gen_state_t cur_state){
    move_gen_state_t new_state = {};
    if(cur_state.move_gen_stage == NORMAL_STAGE){
        // cout<<"MoveGen::update: cur_state.move_gen_stage == NORMAL_STAGE\n";
        unsigned int cur_piece = cur_state.piece;
        uint64 cur_piece_board = cur_state.piece_board;
        uint64 cur_move_set = cur_state.move_set;
        // try update to
        unsigned int next_to = get_next_to(cur_move_set);
        if(next_to != INVALID_LOCATION){
            if(should_update_move_set(cur_state)){
                cur_move_set ^= get_square_bitboard(next_to);
                new_state.promoted_piece = pKNIGHT;
            } else {
                new_state.promoted_piece = cur_state.promoted_piece + 1;
            }
            new_state.piece = cur_piece;
            new_state.piece_board = cur_piece_board;
            new_state.move_set = cur_move_set;
            new_state.move_gen_stage = NORMAL_STAGE;
            if(new_state.move_set) { // still have moves left, dont update piece_board
                return new_state;
            }
            else {
                new_state.piece_board ^= get_square_bitboard(get_next_from(new_state.piece_board));
                new_state = update_piece_board(new_state);
                return new_state;
            }
        } else {
            new_state = update_piece_board(new_state);
            return new_state;
        }
    } 
    return new_state;
}

// move_gen_state_t MoveGen::update_piece_board(move_gen_state_t cur_state){
//     unsigned int cur_piece = cur_state.piece;
//     uint64 cur_piece_board = cur_state.piece_board;
//     uint64 cur_move_set = cur_state.move_set;
   
//     // try update from
//     unsigned int next_from;
//     unsigned int next_to = INVALID_LOCATION;
//     uint64 next_piece_board = cur_piece_board;
//     uint64 next_move_set = 0;
//     // keep updating next_from for cur_piece
//     while(next_to == INVALID_LOCATION && next_from != INVALID_LOCATION){
//         next_from = get_next_from(next_piece_board);
//         next_move_set = get_move_set(cur_piece, next_from);

//         next_to = get_next_to(next_move_set);
//         if(next_from != INVALID_LOCATION && next_to != INVALID_LOCATION){
//             return {
//                 .piece=cur_piece,
//                 .piece_board=next_piece_board,
//                 .move_set=next_move_set,
//                 .move_gen_stage=NORMAL_STAGE
//             };
//         } else {
//             next_piece_board ^= get_square_bitboard(next_from);
//         }
//     }
//     // try update piece
//     unsigned int next_piece = cur_piece;
//     while(next_from == INVALID_LOCATION && next_piece < NUM_PIECE_TYPES){
//         next_piece ++;
//         next_piece_board = bb->piece_boards[side][next_piece];
//         next_from = get_next_from(next_piece_board);
//         next_move_set = get_move_set(next_piece, next_from);
//         next_to = get_next_to(next_move_set);
//         if(next_from != INVALID_LOCATION && next_to != INVALID_LOCATION){
//             return {
//                 .piece = next_piece,
//                 .piece_board = next_piece_board,
//                 .move_set = next_move_set,
//                 .promoted_piece = pKNIGHT,
//                 .move_gen_stage = NORMAL_STAGE
//             };
//         } 
//         // keep updating next_from for next_piece
//         while(next_to == INVALID_LOCATION && next_from != INVALID_LOCATION){
//             next_from = get_next_from(next_piece_board);
//             next_move_set = get_move_set(next_piece, next_from);
//             next_to = get_next_to(next_move_set);
//             if(next_from != INVALID_LOCATION && next_to  != INVALID_LOCATION){
//                 return {
//                     .piece = next_piece,
//                     .piece_board = next_piece_board,
//                     .move_set = next_move_set,
//                     .promoted_piece = pKNIGHT,
//                     .move_gen_stage = NORMAL_STAGE
//                 };
//             } else {
//                 next_piece_board ^= get_square_bitboard(next_from);
//             }
//         }
//     }
//     return {
//         .piece = pKING,
//         .piece_board = bb->piece_boards[side][pKING],
//         .move_set = 0,
//         .promoted_piece = pKNIGHT,
//         .move_gen_stage = SPECIAL_STAGE
//     };
// }
move_gen_state_t MoveGen::update_piece_board(move_gen_state_t cur_state) {
    unsigned int cur_piece = cur_state.piece;
    uint64_t cur_piece_board = cur_state.piece_board;

    // 1. Process remaining pieces of the current piece type
    while (cur_piece_board) {
        // Pop the lowest bit (square index) and update bitboard in-place
        unsigned int from = get_next_from(cur_piece_board);
        // cur_piece_board &= cur_piece_board - 1; 

        uint64_t move_set = get_move_set(cur_piece, from);
        unsigned int to = get_next_to(move_set);
        if (from != INVALID_LOCATION && to != INVALID_LOCATION) {
            return {
                .piece = cur_piece,
                .piece_board = cur_piece_board,
                .move_set = move_set,
                .promoted_piece = pKNIGHT,
                .move_gen_stage = NORMAL_STAGE
            };
        } else {
            cur_piece_board &= cur_piece_board - 1; 
        }
    }

    // 2. Advance to subsequent piece types
    for (unsigned int next_piece = cur_piece + 1; next_piece < NUM_PIECE_TYPES; ++next_piece) {
        uint64_t piece_board = bb->piece_boards[side][next_piece];

        while (piece_board) {
            unsigned int from = get_next_from(piece_board);
            // piece_board &= piece_board - 1;

            uint64_t move_set = get_move_set(next_piece, from);
            unsigned int to = get_next_to(move_set);
            if (from != INVALID_LOCATION && to != INVALID_LOCATION) {
                return {
                    .piece = next_piece,
                    .piece_board = piece_board,
                    .move_set = move_set,
                    .promoted_piece = pKNIGHT,
                    .move_gen_stage = NORMAL_STAGE
                };
            } else {
                piece_board &= piece_board - 1; 
            }
        }
    }

    // 3. Fallback to Special Stage (e.g., Castling/King special moves)
    return {
        .piece = pKING,
        .piece_board = bb->piece_boards[side][pKING],
        .move_set = 0,
        .promoted_piece = pKNIGHT,
        .move_gen_stage = SPECIAL_STAGE
    };
}
unsigned int MoveGen::get_next_from(uint64 cur_piece_board){
    // if(!cur_piece_board)
    //     return INVALID_LOCATION;
    int from = lsb(cur_piece_board);
    // if(from == -1)
    //     return INVALID_LOCATION;
    return from;
}
unsigned int MoveGen::get_next_to(uint64 cur_move_set){
    // if(!cur_move_set)
    //     return INVALID_LOCATION;
    int to = lsb(cur_move_set);
    // if(to == -1)
    //     return INVALID_LOCATION;
    return to;
}

uint64 MoveGen::get_move_set(unsigned int _piece, unsigned int _from){
    if(gen_type == ALL_MOVES)
        return MoveSet::get_all_move_set(bb, _piece, _from, side);
    else if(gen_type == ONLY_QUIET)
        return MoveSet::get_quiet_move_set(bb, _piece, _from, side);
    else if(gen_type == ONLY_CAPTURES)
        return MoveSet::get_capture_move_set(bb, _piece, _from, side);
    return 0;
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
                // if(gen_type==ONLY_CAPTURES)
                //      cout<<"[MoveGen] ONLY_CAPTURES initialised piece: "<<piece<<"\n";
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
            // if(piece == pKING){
            //     bb->display_bitboard(piece_board);
            // }
            found_piece = update_from();
            if(found_piece){
                return true;
            } else {
                // if(piece == pKING)
                //     cout<<"[MoveGen] from not found for king\n";
            }
        } else {
            // if(piece == pKING)
            //     cout<<"[MoveGen] piece_board empty for king\n";
        }
    }
    return false;
}

bool MoveGen::update_from(){

    bool found_from = false;
    int counter = 0;
    while(!found_from && counter < 8){
    // while(!found_from && counter < 7){
        int next_from = bit_scan_forward(piece_board);
        if(next_from == -1){
            // if(gen_type==ONLY_CAPTURES)
            //     cout<<"[MoveGen] ONLY_CAPTURES next_from not found for: "<<MoveUtils::piece_as_string(piece)<<"\n";
            return false;
        }
        // cout<<"[MoveGen] ONLY_CAPTURES piece_board before xor next_from\n";
        // bb->display_bitboard(piece_board);
        piece_board ^= get_square_bitboard(next_from);

        // cout<<"[MoveGen] ONLY_CAPTURES piece_board after xor next_from\n";
        // bb->display_bitboard(piece_board);
        from = next_from;

        if(gen_type == ALL_MOVES){
            move_set = MoveSet::get_all_move_set(bb, piece, from, side);
            if(piece == pKING && move_set==0){
                // cout<<"[MoveGen] ONLY_CAPTURES move_set empty \n";
            }
        } else if(gen_type == ONLY_CAPTURES) {
            move_set = MoveSet::get_capture_move_set(bb, piece, from, side);
            // cout<<"[MoveGen] ONLY_CAPTURES move_set for: "<<MoveUtils::piece_as_string(piece)<<"\n";
            // cout<<"[MoveGen] ONLY_CAPTURES piece: "<<MoveUtils::piece_as_string(piece)<<"\n";
            // cout<<"[MoveGen] ONLY_CAPTURES from: "<<MoveUtils::square_as_string(from)<<"\n";
            if(piece == pKING && move_set > 0){
                // cout<<"[MoveGen] ONLY_CAPTURES move_set not empty \n";
            }
        } else if(gen_type == ONLY_QUIET) {
            move_set = MoveSet::get_quiet_move_set(bb, piece, from, side);
        }

        found_from = move_set > 0;
        if(found_from){
            if(gen_type==ONLY_CAPTURES){
                // cout<<"[MoveGen] ONLY_CAPTURES found_from for: "<<MoveUtils::piece_as_string(piece)<<"\n";
                // cout<<"[MoveGen] ONLY_CAPTURES move_set for: \n";
                // bb->display_bitboard(move_set);
            }
            return update_to();
        }
        counter += 1;
    }

    // if(gen_type==ONLY_CAPTURES)
    //     cout<<"[MoveGen] ONLY_CAPTURES ending next_from not found for: "<<MoveUtils::piece_as_string(piece)<<"\n";
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
        if(!(bb->piece_boards[WHITE][pROOK] & get_square_bitboard(h1)))
            return false;
        else if(!(bi->peek_castle_right() & 0b1000))
            return false;
        else if((bb->all & WHITE_KING_CASTLE_SQUARES))
            return false;
        else if(bb->attacked(side, e1))
            return false;
        else if(bb->attacked(side, f1))
            return false;
        else if(bb->attacked(side, g1))
            return false;
        return true;

    } else {
        if(!(bb->piece_boards[BLACK][pROOK] & get_square_bitboard(h8)))
            return false;
        else if(!(bi->peek_castle_right() & 0b0010))
            return false;
        else if((bb->all & BLACK_KING_CASTLE_SQUARES))
            return false;
        else if(bb->attacked(side, e8))
            return false;
        else if(bb->attacked(side, f8))
            return false;
        else if(bb->attacked(side, g8))
            return false;
        return true;
    }
}

bool MoveGen::can_castle_queenside(int side){
    if(side == WHITE){
        if(!(bb->piece_boards[WHITE][pROOK] & get_square_bitboard(a1)))
            return false;
        else if(!(bi->peek_castle_right() & 0b0100))
            return false;
        else if((bb->all & WHITE_QUEEN_CASTLE_SQUARES))
            return false;
        else if(bb->attacked(side, e1))
            return false;
        else if(bb->attacked(side, d1))
            return false;
        else if(bb->attacked(side, c1))
            return false;
        return true;
    } else {
        if(!(bb->piece_boards[BLACK][pROOK] & get_square_bitboard(a8)))
            return false;
        else if(!(bi->peek_castle_right() & 0b0001))
            return false;
        else if((bb->all & BLACK_QUEEN_CASTLE_SQUARES))
            return false;
        else if(bb->attacked(side, e8))
            return false;
        else if(bb->attacked(side, d8))
            return false;
        else if(bb->attacked(side, c8))
            return false;
        return true;
    }
}

unsigned int MoveGen::get_ep_capture(){
    const unsigned int ep_rights = bi->peek_ep_right();
    if(ep_rights > 7) {
        ep_from = EP_FINISHED;
        // cout<<"NO EP RIGHT\n";
        return NO_MOVES_LEFT;
    };

    unsigned int ep_from_centre = ep_rights + (a5 - (side * 8));
    unsigned int left_ep_from = ep_from_centre - 1;
    unsigned int right_ep_from = ep_from_centre + 1;
    if(ep_from == EP_FINISHED){
        return NO_MOVES_LEFT;
    } else if(ep_from == EP_START) {
        if(floor(ep_from_centre / 8) == floor(left_ep_from / 8) && (bb->piece_boards[side][pPAWN] & get_square_bitboard(left_ep_from))){
            ep_from = left_ep_from;
        } else if(floor(ep_from_centre / 8) == floor(right_ep_from / 8) && (bb->piece_boards[side][pPAWN] & get_square_bitboard(right_ep_from))){
            ep_from = right_ep_from;
        } else{
            ep_from = EP_FINISHED;
            return NO_MOVES_LEFT;
        }
    } else if(ep_from == left_ep_from) {
        if(floor(ep_from_centre / 8) == floor(right_ep_from / 8) && (bb->piece_boards[side][pPAWN] & get_square_bitboard(right_ep_from))){
            ep_from = right_ep_from;
        } else{
            ep_from = EP_FINISHED;
            return NO_MOVES_LEFT;
        }
    } else if(ep_from == right_ep_from) {
        ep_from = EP_FINISHED;
        return NO_MOVES_LEFT;
    }
    unsigned int ep_to = side == WHITE ? ep_from_centre + 8 : ep_from_centre - 8;
    unsigned int ep_capture_file = ep_rights & 0xf;
    unsigned int move = MoveUtils::create_move(ep_from, ep_to, side, pPAWN, EP_CAPTURE, pPAWN, ep_capture_file);
    return move;
}
void MoveGen::set_gen_type(int _gen_type){
    gen_type = _gen_type;
}
void MoveGen::set_move_type(int _move_type){
    move_type = _move_type;
}