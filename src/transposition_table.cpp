#include <iostream>
#include "board_squares.hpp"
#include "transposition_table.hpp"
#include "move.hpp"
using namespace BoardSquares;

unsigned int TranspositionTable::get_zobrist_key(unsigned int side, unsigned int piece, unsigned int sq){
    // range (side * 384 + piece * 64 + sq): [0, 767]
    // range (piece * 64 + sq): [0, 383]
    // range (sq): [0, 63]
    return side * (NUM_PIECE_TYPES * NUM_SQUARES) + piece * NUM_SQUARES + sq;
}
unsigned int TranspositionTable::get_zobrist_key(ZOBRIST_KEYS info_type, unsigned int idx){
    return info_type + idx;
}
uint64 TranspositionTable::get_zobrist_val(unsigned int key){
    try{
        return zobrist_map.at(key);
    } catch(std::out_of_range e){
        return 0;
    }
}

void TranspositionTable::init_zobrist_map(){
    for(int side = 0 ; side < NUM_SIDES ; side ++){
        for(int piece = 0 ; piece < NUM_PIECE_TYPES ; piece ++){
            for(int sq = 0 ; sq < NUM_SQUARES ; sq ++){
                unsigned int key = get_zobrist_key(side, piece, sq);
                zobrist_map[key] = rng_state.next();
            }
        }
    }
    zobrist_map[zBLACK_TO_MOVE] = rng_state.next();
    for(int i = zCASTLE_RIGHTS ; i < zCASTLE_RIGHTS + NUM_CASTLE_RIGHTS; i ++){
        zobrist_map[i] = rng_state.next();
    }
    for(int i = zEP_RIGHTS ; i < zEP_RIGHTS + NUM_EP_RIGHTS ; i ++){
        zobrist_map[i] = rng_state.next();
    }
}
void TranspositionTable::init_zobrist_hash_val(){
    for(int side = 0 ; side < NUM_SIDES ; side ++){
        for(int piece = 0; piece < NUM_SQUARES ; piece ++){
            for(int sq = 0 ; sq < NUM_SQUARES ; sq ++){
                if(get_square_bitboard(sq) & bb->piece_boards[side][piece])
                    zobrist_hash_val ^= zobrist_map[get_zobrist_key(side, piece, sq)];
            }
        }
    }
}
uint64 TranspositionTable::get_current_hash_val(){
    return zobrist_hash_val;
}
void TranspositionTable::update_zobrist_hash_val(unsigned int move, unsigned int prev_ep_right, unsigned int next_ep_right, unsigned int prev_castle_right, unsigned int next_castle_right){
    /**
     * same update logic both when 
     * 1. applying move
     * 2. reversing move
     * due to the way xor works
     */
    unsigned int side = MoveUtils::get_side(move);
    unsigned int from = MoveUtils::get_from(move);
    unsigned int to = MoveUtils::get_to(move);
    unsigned int piece = MoveUtils::get_piece(move);
    unsigned int captured_piece = MoveUtils::get_captured_piece(move);
    /**
     * CASE 0: QUIET
     * xor piece on from, piece on to
     * CASE 1: CAPTURE
     * xor piece on from, piece on to, captured_piece on to 
     * CAS# 2: DOUBLE_PAWN_PUSH
     * xor piece on from, piece on to, and xor ep rights
     * CASE 2: EP_CAPTURE
     * xor pawn on from, pawn on to, captured piece on ep_capture_sq, and xor ep rights
     * CASE 3: KING_CASTLE, QUEEN_CASTLE
     * xor king on from, king on to, rook on from, rook on to
     * CASE 4: PROMOTION
     * xor pawn on from, pawn on to, promoted piece on to
     * CASE 5: CAPTURE PROMOTION
     * xor pawn on from, pawn on to, captured piece on to, promoted piece on to
     */
    if(MoveUtils::is_quiet(move)){
        unsigned int piece_on_from_key = get_zobrist_key(side, piece, from);
        unsigned int piece_on_to_key = get_zobrist_key(side, piece, to);
        zobrist_hash_val ^= get_zobrist_val(piece_on_from_key);
        zobrist_hash_val ^= get_zobrist_val(piece_on_to_key);
        if(piece == pKING || piece == pROOK){
            unsigned int prev_castle_right_key = get_zobrist_key(zCASTLE_RIGHTS, prev_castle_right);
            unsigned int next_castle_right_key = get_zobrist_key(zCASTLE_RIGHTS, next_castle_right);
            zobrist_hash_val ^= get_zobrist_val(prev_castle_right_key);
            zobrist_hash_val ^= get_zobrist_val(next_castle_right_key);
        } 
    } else if(MoveUtils::is_capture(move)){
        unsigned int piece_on_from_key = get_zobrist_key(side, piece, from);
        unsigned int piece_on_to_key = get_zobrist_key(side, piece, to);
        unsigned int captured_piece_on_to_key = get_zobrist_key(side ^ 1, captured_piece, to);
        zobrist_hash_val ^= get_zobrist_val(piece_on_from_key);
        zobrist_hash_val ^= get_zobrist_val(piece_on_to_key);  
        zobrist_hash_val ^= get_zobrist_val(captured_piece_on_to_key);
        if(piece == pKING || piece == pROOK){
            unsigned int prev_castle_right_key = get_zobrist_key(zCASTLE_RIGHTS, prev_castle_right);
            unsigned int next_castle_right_key = get_zobrist_key(zCASTLE_RIGHTS, next_castle_right);
            zobrist_hash_val ^= get_zobrist_val(prev_castle_right_key);
            zobrist_hash_val ^= get_zobrist_val(next_castle_right_key);
        }
    } else if(MoveUtils::is_double_pawn_push(move)){
        unsigned int piece_on_from_key = get_zobrist_key(side, piece, from);
        unsigned int piece_on_to_key = get_zobrist_key(side, piece, to);
        zobrist_hash_val ^= get_zobrist_val(piece_on_from_key);
        zobrist_hash_val ^= get_zobrist_val(piece_on_to_key);
        unsigned int prev_ep_right_key = get_zobrist_key(zEP_RIGHTS, prev_ep_right);
        unsigned int next_ep_right_key = get_zobrist_key(zEP_RIGHTS, next_ep_right);
        zobrist_hash_val ^= get_zobrist_val(prev_ep_right_key);
        zobrist_hash_val ^= get_zobrist_val(next_ep_right_key);
    } else if(MoveUtils::is_promotion(move)){
        unsigned int piece_on_from_key = get_zobrist_key(side, piece, from);
        unsigned int piece_on_to_key;
        if(MoveUtils::is_knight_promotion(move)){
            piece_on_to_key = get_zobrist_key(side, pKNIGHT, to);
        } else if(MoveUtils::is_bishop_promotion(move)){
            piece_on_to_key = get_zobrist_key(side, pBISHOP, to);
        } else if(MoveUtils::is_rook_promotion(move)){
            piece_on_to_key = get_zobrist_key(side, pBISHOP, to);
        } else if(MoveUtils::is_queen_promotion(move)){
            piece_on_to_key = get_zobrist_key(side, pQUEEN, to);
        }
        zobrist_hash_val ^= get_zobrist_val(piece_on_from_key);
        zobrist_hash_val ^= get_zobrist_val(piece_on_to_key);  
    } else if(MoveUtils::is_capture_promotion(move)){
        unsigned int piece_on_from_key = get_zobrist_key(side, piece, from);
        unsigned int piece_on_to_key;
        unsigned int captured_piece_on_to_key = get_zobrist_key(side, captured_piece, to);
        if(MoveUtils::is_knight_capture_promotion(move)){
            piece_on_to_key = get_zobrist_key(side, pKNIGHT, to);
        } else if(MoveUtils::is_bishop_capture_promotion(move)){
            piece_on_to_key = get_zobrist_key(side, pBISHOP, to);
        } else if(MoveUtils::is_rook_capture_promotion(move)){
            piece_on_to_key = get_zobrist_key(side, pROOK, to);
        } else if(MoveUtils::is_queen_capture_promotion(move)){
            piece_on_to_key = get_zobrist_key(side, pQUEEN, to);
        }
        zobrist_hash_val ^= get_zobrist_val(piece_on_from_key);
        zobrist_hash_val ^= get_zobrist_val(piece_on_to_key);
        zobrist_hash_val ^= get_zobrist_val(captured_piece_on_to_key);
    } else if(MoveUtils::is_ep_capture(move)){
        unsigned int ep_capture_file = MoveUtils::get_ep_capture_file(move);
        unsigned int ep_capture_sq = MoveUtils::get_ep_capture_sq(side, ep_capture_file);
        unsigned int piece_on_from_key = get_zobrist_key(side, piece, from);
        unsigned int piece_on_to_key = get_zobrist_key(side, piece, to);
        unsigned int ep_capture_sq_key = get_zobrist_key(side ^ 1, pPAWN, ep_capture_sq);
        unsigned int prev_ep_right_key = get_zobrist_key(zEP_RIGHTS, prev_ep_right);
        unsigned int next_ep_right_key = get_zobrist_key(zEP_RIGHTS, next_ep_right);
        zobrist_hash_val ^= get_zobrist_val(piece_on_from_key);
        zobrist_hash_val ^= get_zobrist_val(piece_on_to_key);
        zobrist_hash_val ^= get_zobrist_val(ep_capture_sq_key);
        zobrist_hash_val ^= get_zobrist_val(prev_ep_right_key);
        zobrist_hash_val ^= get_zobrist_val(next_ep_right_key);
    } else if(MoveUtils::is_castle(move)){
        unsigned int king_on_from_key, king_on_to_key, rook_on_from_key, rook_on_to_key;
        unsigned int prev_castle_right_key = get_zobrist_key(zCASTLE_RIGHTS, prev_castle_right);
        unsigned int next_castle_right_key = get_zobrist_key(zCASTLE_RIGHTS, next_castle_right);
        if(side == WHITE && MoveUtils::is_king_castle(move)){
            king_on_from_key = get_zobrist_key(side, pKING, e1);
            king_on_to_key = get_zobrist_key(side, pKING, g1);
            rook_on_from_key = get_zobrist_key(side, pROOK, h1);
            rook_on_to_key = get_zobrist_key(side, pROOK, f1);
        } else if(side == WHITE && MoveUtils::is_queen_castle(move)){
            king_on_from_key = get_zobrist_key(side, pKING, e1);
            king_on_to_key = get_zobrist_key(side, pKING, c1);
            rook_on_from_key = get_zobrist_key(side, pROOK, a1);
            rook_on_to_key = get_zobrist_key(side, pROOK, d1);
        } else if(side == BLACK && MoveUtils::is_king_castle(move)){
            king_on_from_key = get_zobrist_key(side, pKING, e8);
            king_on_to_key = get_zobrist_key(side, pKING, g8);
            rook_on_from_key = get_zobrist_key(side, pROOK, h8);
            rook_on_to_key = get_zobrist_key(side, pROOK, f8);
        } else if(side == BLACK && MoveUtils::is_queen_castle(move)){
            king_on_from_key = get_zobrist_key(side, pKING, e8);
            king_on_to_key = get_zobrist_key(side, pKING, c8);
            rook_on_from_key = get_zobrist_key(side, pROOK, a8);
            rook_on_to_key = get_zobrist_key(side, pROOK, d8);
        }
        zobrist_hash_val ^= get_zobrist_val(king_on_from_key);
        zobrist_hash_val ^= get_zobrist_val(king_on_to_key);
        zobrist_hash_val ^= get_zobrist_val(rook_on_from_key);
        zobrist_hash_val ^= get_zobrist_val(rook_on_to_key);
        zobrist_hash_val ^= get_zobrist_val(prev_castle_right_key);
        zobrist_hash_val ^= get_zobrist_val(next_castle_right_key);
    }
}
map<unsigned int, uint64> TranspositionTable::get_zobrist_map(){
    return zobrist_map;
}
void TranspositionTable::update_transposition_table(uint64 key, uint64 val, int depth_left){
    perft_transposition_table[depth_left][key] = val;
}
uint64 TranspositionTable::get_transposition_table_value(uint64 key, int depth_left){
    if(perft_transposition_table[depth_left].find(key) != perft_transposition_table[depth_left].end()){
        // cout<<"found perft value in transposition table: "<<perft_transposition_table[key]<<endl;
        return perft_transposition_table[depth_left][key];
    }
    else return 0ULL;
}