#include <random>
#include "const.hpp"
#include "move.hpp"
#include "transposition_table.hpp"
TranspositionTable* TranspositionTable::instanceptr=nullptr;

TranspositionTable* TranspositionTable::get_instance() {
    if (instanceptr == nullptr) {
        instanceptr = new TranspositionTable();
    }
    return instanceptr;
}

void TranspositionTable::generate_zobrist_values(){
    std::mt19937_64 gen(0);           // 64-bit Mersenne Twister RNG
    std::uniform_int_distribution<uint64> dist;
    for(int i = 0 ; i < NUM_ZOBRIST_VALS ; i ++)
        zobrist_vals[i] = dist(gen);
}
uint64 TranspositionTable::get_zobrist_value(unsigned int zobrist_offset, unsigned int idx){
    return zobrist_vals[zobrist_offset + idx];
}
unsigned int TranspositionTable::calculate_zobrist_idx_side_to_move(unsigned int side){
    return ZOBRIST_OFFSET_SIDE_TO_MOVE + side;
}
unsigned int TranspositionTable::calculate_zobrist_idx_piece_square(unsigned int side, unsigned int piece, unsigned int square){
    // 0 - 377 white
        // 0 - 63 pawns 
        // 64 - 127 knights
        //...
    // 378 - 767 black
        // 378 - 441 pawns
        // 441 - 504 knights
        // ...
    return (square + (NUM_SQUARES * piece)) + (NUM_SQUARES * NUM_PIECE_TYPES * side);
}
unsigned int TranspositionTable::calculate_zobrist_idx_castle_rights(unsigned int castle_right){
    return  ZOBRIST_OFFSET_CASTLE_RIGHTS + castle_right;
}
unsigned int TranspositionTable::calculate_zobrist_idx_ep_rights(unsigned int ep_right){
    return ZOBRIST_OFFSET_EP_RIGHTS + ep_right;
}
void TranspositionTable::initialise_hash_val(unsigned int side_to_move, Bitboard* bb, BoardInfo* bi){
    hash_val = 0;
    for(int side = 0 ; side < NUM_SIDES ; side ++){
        for(int piece = 0 ; piece < NUM_PIECE_TYPES ; piece ++){
            for(int sq = 0 ; sq < NUM_SQUARES ; sq ++){
                if(bb->get_piece_on_square(side, sq) == piece)
                    hash_val ^= get_zobrist_value(ZOBRIST_OFFSET_PIECE_ON_SQUARE, calculate_zobrist_idx_piece_square(side, piece, sq));
            }
        }
    }
    hash_val ^= get_zobrist_value(ZOBRIST_OFFSET_SIDE_TO_MOVE, calculate_zobrist_idx_side_to_move(side_to_move));
    hash_val ^= get_zobrist_value(ZOBRIST_OFFSET_CASTLE_RIGHTS, calculate_zobrist_idx_castle_rights(bi->peek_castle_right()));
    hash_val ^= get_zobrist_value(ZOBRIST_OFFSET_EP_RIGHTS, calculate_zobrist_idx_ep_rights(bi->peek_ep_right()));
}
void TranspositionTable::update_hash_val_side_to_move(unsigned int move){
    hash_val ^= get_zobrist_value(ZOBRIST_OFFSET_SIDE_TO_MOVE, calculate_zobrist_idx_side_to_move(WHITE));
    hash_val ^= get_zobrist_value(ZOBRIST_OFFSET_SIDE_TO_MOVE, calculate_zobrist_idx_side_to_move(BLACK));
}
void TranspositionTable::update_hash_val_piece_square(unsigned int move){

    unsigned int side = MoveUtils::get_side(move);
    unsigned int from = MoveUtils::get_from(move);
    unsigned int to = MoveUtils::get_to(move);
    unsigned int piece = MoveUtils::get_piece(move);
    unsigned int captured_piece = MoveUtils::get_captured_piece(move);

    uint64 from_zobrist_val = get_zobrist_value(ZOBRIST_OFFSET_PIECE_ON_SQUARE, calculate_zobrist_idx_piece_square(side, piece, from));
    uint64 to_zobrist_val = get_zobrist_value(ZOBRIST_OFFSET_PIECE_ON_SQUARE, calculate_zobrist_idx_piece_square(side, piece, to));

    if(MoveUtils::is_quiet(move)){
        hash_val ^= from_zobrist_val;
        hash_val ^= to_zobrist_val;
    } else if(MoveUtils::is_castle(move)){
        if(MoveUtils::is_king_castle(move) && side == WHITE){
            hash_val ^= get_zobrist_value(ZOBRIST_OFFSET_PIECE_ON_SQUARE, calculate_zobrist_idx_piece_square(side, pROOK, h1));
            hash_val ^= get_zobrist_value(ZOBRIST_OFFSET_PIECE_ON_SQUARE, calculate_zobrist_idx_piece_square(side, pROOK, f1));
            hash_val ^= get_zobrist_value(ZOBRIST_OFFSET_PIECE_ON_SQUARE, calculate_zobrist_idx_piece_square(side, pKING, e1));
            hash_val ^= get_zobrist_value(ZOBRIST_OFFSET_PIECE_ON_SQUARE, calculate_zobrist_idx_piece_square(side, pKING, g1));
        } else if(MoveUtils::is_queen_castle(move) && side == WHITE){
            hash_val ^= get_zobrist_value(ZOBRIST_OFFSET_PIECE_ON_SQUARE, calculate_zobrist_idx_piece_square(side, pROOK, a1));
            hash_val ^= get_zobrist_value(ZOBRIST_OFFSET_PIECE_ON_SQUARE, calculate_zobrist_idx_piece_square(side, pROOK, d1));
            hash_val ^= get_zobrist_value(ZOBRIST_OFFSET_PIECE_ON_SQUARE, calculate_zobrist_idx_piece_square(side, pKING, e1));
            hash_val ^= get_zobrist_value(ZOBRIST_OFFSET_PIECE_ON_SQUARE, calculate_zobrist_idx_piece_square(side, pKING, c1));
        } else if(MoveUtils::is_king_castle(move) && side == BLACK){
            hash_val ^= get_zobrist_value(ZOBRIST_OFFSET_PIECE_ON_SQUARE, calculate_zobrist_idx_piece_square(side, pROOK, h8));
            hash_val ^= get_zobrist_value(ZOBRIST_OFFSET_PIECE_ON_SQUARE, calculate_zobrist_idx_piece_square(side, pROOK, f8));
            hash_val ^= get_zobrist_value(ZOBRIST_OFFSET_PIECE_ON_SQUARE, calculate_zobrist_idx_piece_square(side, pKING, e8));
            hash_val ^= get_zobrist_value(ZOBRIST_OFFSET_PIECE_ON_SQUARE, calculate_zobrist_idx_piece_square(side, pKING, g8));
        } else if(MoveUtils::is_queen_castle(move) && side == BLACK){
            hash_val ^= get_zobrist_value(ZOBRIST_OFFSET_PIECE_ON_SQUARE, calculate_zobrist_idx_piece_square(side, pROOK, a8));
            hash_val ^= get_zobrist_value(ZOBRIST_OFFSET_PIECE_ON_SQUARE, calculate_zobrist_idx_piece_square(side, pROOK, d8));
            hash_val ^= get_zobrist_value(ZOBRIST_OFFSET_PIECE_ON_SQUARE, calculate_zobrist_idx_piece_square(side, pKING, e8));
            hash_val ^= get_zobrist_value(ZOBRIST_OFFSET_PIECE_ON_SQUARE, calculate_zobrist_idx_piece_square(side, pKING, c8));
        }
    } else if(MoveUtils::is_capture(move)){
        uint64 captured_piece_zobrist_val = get_zobrist_value(ZOBRIST_OFFSET_PIECE_ON_SQUARE, calculate_zobrist_idx_piece_square(side ^ 1, captured_piece, to));

        hash_val ^= from_zobrist_val;
        hash_val ^= to_zobrist_val;
        hash_val ^= captured_piece_zobrist_val;
    } else if(MoveUtils::is_ep_capture(move)){
        unsigned int captured_file = MoveUtils::get_ep_capture_file(move);
        unsigned int captured_sq =  side == WHITE ? a5 + captured_file : a4 + captured_file;

        hash_val ^= from_zobrist_val;
        hash_val ^= to_zobrist_val;
        hash_val ^= get_zobrist_value(ZOBRIST_OFFSET_PIECE_ON_SQUARE, calculate_zobrist_idx_piece_square(side ^ 1, pPAWN, captured_sq));
    } else if(MoveUtils::is_double_pawn_push(move)){
        hash_val ^= from_zobrist_val;
        hash_val ^= to_zobrist_val;
    } else if(MoveUtils::is_promotion(move)){
        hash_val ^= from_zobrist_val;
        if(MoveUtils::is_queen_promotion(move))
            to_zobrist_val = get_zobrist_value(ZOBRIST_OFFSET_PIECE_ON_SQUARE, calculate_zobrist_idx_piece_square(side, pQUEEN, to));
        else if(MoveUtils::is_rook_promotion(move))
            to_zobrist_val = get_zobrist_value(ZOBRIST_OFFSET_PIECE_ON_SQUARE, calculate_zobrist_idx_piece_square(side, pROOK, to));
        else if(MoveUtils::is_bishop_promotion(move))
            to_zobrist_val = get_zobrist_value(ZOBRIST_OFFSET_PIECE_ON_SQUARE, calculate_zobrist_idx_piece_square(side, pBISHOP, to));
        else
            to_zobrist_val = get_zobrist_value(ZOBRIST_OFFSET_PIECE_ON_SQUARE, calculate_zobrist_idx_piece_square(side, pKNIGHT, to));
        hash_val ^= to_zobrist_val;
    } else if(MoveUtils::is_capture_promotion(move)){
        hash_val ^= from_zobrist_val;
        uint64 captured_piece_zobrist_val = get_zobrist_value(ZOBRIST_OFFSET_PIECE_ON_SQUARE, calculate_zobrist_idx_piece_square(side ^ 1, captured_piece, to));
        hash_val ^= captured_piece_zobrist_val;

        if(MoveUtils::is_queen_promotion(move))
            to_zobrist_val = get_zobrist_value(ZOBRIST_OFFSET_PIECE_ON_SQUARE, calculate_zobrist_idx_piece_square(side, pQUEEN, to));
        else if(MoveUtils::is_rook_promotion(move))
            to_zobrist_val = get_zobrist_value(ZOBRIST_OFFSET_PIECE_ON_SQUARE, calculate_zobrist_idx_piece_square(side, pROOK, to));
        else if(MoveUtils::is_bishop_promotion(move))
            to_zobrist_val = get_zobrist_value(ZOBRIST_OFFSET_PIECE_ON_SQUARE, calculate_zobrist_idx_piece_square(side, pBISHOP, to));
        else
            to_zobrist_val = get_zobrist_value(ZOBRIST_OFFSET_PIECE_ON_SQUARE, calculate_zobrist_idx_piece_square(side, pKNIGHT, to));
        hash_val ^= to_zobrist_val;
    }
}
void TranspositionTable::update_hash_val_castle_rights(unsigned int prev_castle_right, unsigned int next_castle_right){
    hash_val ^= get_zobrist_value(ZOBRIST_OFFSET_CASTLE_RIGHTS, calculate_zobrist_idx_castle_rights(prev_castle_right));
    hash_val ^= get_zobrist_value(ZOBRIST_OFFSET_CASTLE_RIGHTS, calculate_zobrist_idx_castle_rights(next_castle_right));
}
void TranspositionTable::update_hash_val_ep_rights(unsigned int prev_ep_right, unsigned int next_ep_right){
    hash_val ^= get_zobrist_value(ZOBRIST_OFFSET_EP_RIGHTS, calculate_zobrist_idx_ep_rights(prev_ep_right));
    hash_val ^= get_zobrist_value(ZOBRIST_OFFSET_EP_RIGHTS, calculate_zobrist_idx_ep_rights(next_ep_right));
}
void TranspositionTable::not_equal(){
    return;
}
unsigned int TranspositionTable::get_value_perft(int depth_searched){
    auto val = perft_table[depth_searched].find(hash_val);
    if(val != perft_table[depth_searched].end()){
        found_value();
        return val->second;
    }
    return 0;
}
int TranspositionTable::get_value_eval(int depth_searched){
    auto val = eval_table[depth_searched].find(hash_val);
    if(val != eval_table[depth_searched].end()){
        found_value();
        return val->second;
    }
    return DEFAULT_EVAL;
}
void TranspositionTable::found_value(){
    return;
}
void TranspositionTable::add_value_perft(int depth_searched, unsigned int val){
    perft_table[depth_searched].insert({hash_val, val});
}
void TranspositionTable::add_value_eval(int depth_searched, int val){
    eval_table[depth_searched].try_emplace(hash_val & 0xFFFFFFULL, val);
}

