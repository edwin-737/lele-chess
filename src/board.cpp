#include <iostream>
#include <string>
#include <cassert>
#include "board.hpp"
#include "board_info.hpp"
#include "board_squares.hpp"
#include "const.hpp"
#include "move.hpp"
#include "pesto.hpp"
using namespace BoardSquares;

bool Board::apply_promotion_move(
    unsigned int move,
    unsigned int from, 
    unsigned int to,
    unsigned int side,
    unsigned int promoted_piece, 
    unsigned int king_location, 
    uint64 piece_square_hash_val
){

    unsigned int castle_rights = bi->peek_castle_right();

    uint64 from_to = get_from_to(from, to);
    uint64 from_bitboard = get_square_bitboard(from);
    uint64 to_bitboard = get_square_bitboard(to);
    bb->piece_boards[side][pPAWN] ^= from_bitboard;
    bb->collective_piece_boards[side] ^= from_to;
    bb->piece_boards[side][promoted_piece] ^= to_bitboard;
    bb->all = bb->collective_piece_boards[WHITE] | bb->collective_piece_boards[BLACK];

    if(bb->attacked(side, get_king_location(side))){
        bb->piece_boards[side][pPAWN] ^= from_bitboard;
        bb->collective_piece_boards[side] ^= from_to;        
        bb->all = bb->collective_piece_boards[WHITE] | bb->collective_piece_boards[BLACK];
        bb->piece_boards[side][promoted_piece] ^= to_bitboard;

        tt.update_hash_val_piece_square(move, piece_square_hash_val);
        tt.update_hash_val_side_to_move(move);
        return false;
    }
    bi->add_board_info(castle_rights, NO_EP_RIGHTS);

    bb->piece_on_square[from] = NO_PIECE;
    bb->piece_on_square[to] = promoted_piece;
    return true;
}

bool Board::apply_capture_promotion_move(
    unsigned int move, 
    unsigned int from,
    unsigned int to,
    unsigned int side,
    unsigned int promoted_piece, 
    unsigned int captured_piece,
    unsigned int king_location, 
    uint64 piece_square_hash_val
) {

    unsigned int castle_rights = bi->peek_castle_right();

    uint64 from_to = get_from_to(from, to);
    uint64 from_bitboard = get_square_bitboard(from);
    uint64 to_bitboard = get_square_bitboard(to);
    bb->piece_boards[side][pPAWN] ^= from_bitboard;
    bb->collective_piece_boards[side] ^= from_to;
    bb->all = bb->collective_piece_boards[WHITE] | bb->collective_piece_boards[BLACK];

    bb->piece_boards[side][promoted_piece] ^= to_bitboard;
    bb->piece_boards[side ^ 1][captured_piece] ^= to_bitboard;
    bb->collective_piece_boards[side ^ 1] ^= to_bitboard;
    if(bb->attacked(side, king_location)){
        bb->piece_boards[side][pPAWN] ^= from_bitboard;
        bb->collective_piece_boards[side] ^= from_to;
        bb->piece_boards[side][promoted_piece] ^= to_bitboard;
        bb->piece_boards[side ^ 1][captured_piece] ^= to_bitboard;
        bb->collective_piece_boards[side ^ 1] ^= to_bitboard;
        bb->all = bb->collective_piece_boards[WHITE] | bb->collective_piece_boards[BLACK];

        tt.update_hash_val_piece_square(move, piece_square_hash_val);
        tt.update_hash_val_side_to_move(move);
        return false;
    }
    bi->add_board_info(castle_rights, NO_EP_RIGHTS);
    bb->piece_on_square[from] = NO_PIECE;
    bb->piece_on_square[to] = promoted_piece;
    return true;
}

bool Board::apply_move(unsigned int move){
    unsigned int side = MoveUtils::get_side(move);
    unsigned int piece = MoveUtils::get_piece(move);
    unsigned int from = MoveUtils::get_from(move);
    unsigned int to = MoveUtils::get_to(move);        

    unsigned int castle_rights = bi->peek_castle_right();
    unsigned int ep_rights = bi->peek_ep_right();
    unsigned int captured_piece = MoveUtils::get_captured_piece(move);

    unsigned int king_location = get_king_location(side);
    // for zobrist hash val update
    unsigned int prev_castle_rights = bi->peek_castle_right();
    unsigned int prev_ep_rights = bi->peek_ep_right();
    uint64 piece_square_hash_val = tt.get_hash_val_change_piece_square(move);
    tt.update_hash_val_piece_square(move, piece_square_hash_val);
    tt.update_hash_val_side_to_move(move);
    switch(MoveUtils::get_additional_info(move)){
        case QUIET_MOVE: {
            uint64 from_to = get_from_to(from, to);
            uint64 old_pb = bb->piece_boards[side][piece];
            uint64 old_cb = bb->collective_piece_boards[side];
            bb->piece_boards[side][piece] ^= from_to;
            bb->collective_piece_boards[side] ^= from_to;
            bb->all = bb->collective_piece_boards[WHITE] | bb->collective_piece_boards[BLACK];

            if(piece == pKING){
                if(bb->attacked(side, to)){
                    bb->piece_boards[side][piece] ^= from_to;
                    bb->collective_piece_boards[side] ^= from_to;
                    bb->all = bb->collective_piece_boards[WHITE] | bb->collective_piece_boards[BLACK];

                    tt.update_hash_val_piece_square(move, piece_square_hash_val);
                    tt.update_hash_val_side_to_move(move);
                    return false;
                }
            } else {
                if(bb->attacked(side, king_location)){
                    bb->piece_boards[side][piece] ^= from_to;
                    bb->collective_piece_boards[side] ^= from_to;
                    bb->all = bb->collective_piece_boards[WHITE] | bb->collective_piece_boards[BLACK];

                    tt.update_hash_val_piece_square(move, piece_square_hash_val);
                    tt.update_hash_val_side_to_move(move);
                    return false;
                }
            }
            if(piece == pKING){
                castle_rights &= castle_rights_king_mask[side];
                update_king_location(side, to);
                bi->add_board_info(castle_rights, NO_EP_RIGHTS);
            } else if(piece == pROOK){
                castle_rights &= castle_rights_rook_mask[side][from];
                bi->add_board_info(castle_rights, NO_EP_RIGHTS);
            } else {
                bi->add_board_info(castle_rights, NO_EP_RIGHTS);
            }
            bb->piece_on_square[from] = NO_PIECE;
            bb->piece_on_square[to] = piece;
            break;
        } case CAPTURE: {
            uint64 from_to = get_from_to(from, to);
            bb->piece_boards[side][piece] ^= from_to;
            bb->collective_piece_boards[side] ^= from_to;
            uint64 sq_bitboard = get_square_bitboard(to);
            bb->piece_boards[side ^ 1][captured_piece] ^= sq_bitboard;
            bb->collective_piece_boards[side ^ 1] ^= sq_bitboard;
            
            bb->all = bb->collective_piece_boards[WHITE] | bb->collective_piece_boards[BLACK];
            if(piece == pKING){

                if(bb->attacked(side, to)){
                    bb->piece_boards[side][piece] ^= from_to;
                    bb->collective_piece_boards[side] ^= from_to;
                    bb->piece_boards[side ^ 1][captured_piece] ^= sq_bitboard;
                    bb->collective_piece_boards[side ^ 1] ^= sq_bitboard;
                    bb->all = bb->collective_piece_boards[WHITE] | bb->collective_piece_boards[BLACK];

                    tt.update_hash_val_piece_square(move, piece_square_hash_val);
                    tt.update_hash_val_side_to_move(move);
                    return false;
                }
            } else {

                if(bb->attacked(side, king_location)){
                    bb->piece_boards[side][piece] ^= from_to;
                    bb->collective_piece_boards[side] ^= from_to;
                    bb->piece_boards[side ^ 1][captured_piece] ^= sq_bitboard;
                    bb->collective_piece_boards[side ^ 1] ^= sq_bitboard;
                    bb->all = bb->collective_piece_boards[WHITE] | bb->collective_piece_boards[BLACK];

                    tt.update_hash_val_piece_square(move);
                    tt.update_hash_val_side_to_move(move);
                    return false;
                }
            }
            if(piece == pKING){
                castle_rights &= castle_rights_king_mask[side];
                update_king_location(side, to);
                // cout<<"move is capture king move\n";
                bi->add_board_info(castle_rights, NO_EP_RIGHTS);
            } else if(piece == pROOK){
                castle_rights &= castle_rights_rook_mask[side][from];
                bi->add_board_info(castle_rights, NO_EP_RIGHTS);
            } else {
                bi->add_board_info(castle_rights, NO_EP_RIGHTS);
            }
            bb->piece_on_square[from] = NO_PIECE;
            bb->piece_on_square[to] = piece;
            break;
        } case DOUBLE_PAWN_PUSH: {
            uint64 from_to = get_from_to(from, to);
            bb->piece_boards[side][piece] ^= from_to;
            bb->collective_piece_boards[side] ^= from_to;
            bb->all = bb->collective_piece_boards[WHITE] | bb->collective_piece_boards[BLACK];
            if(bb->attacked(side, king_location)){
                bb->piece_boards[side][piece] ^= from_to;
                bb->collective_piece_boards[side] ^= from_to;
                bb->all = bb->collective_piece_boards[WHITE] | bb->collective_piece_boards[BLACK];

                tt.update_hash_val_piece_square(move, piece_square_hash_val);
                tt.update_hash_val_side_to_move(move);
                return false;
            }
            int ep_rights = bi->peek_ep_right();
            if(side == WHITE){
                ep_rights = to - a4;
            } else {
                ep_rights = to - a5;
            }
            bi->add_board_info(castle_rights, ep_rights);
            bb->piece_on_square[from] = NO_PIECE;
            bb->piece_on_square[to] = pPAWN;
            break;
        } case EP_CAPTURE: {
            uint64 from_to = get_from_to(from, to);
            bb->piece_boards[side][piece] ^= from_to;
            bb->collective_piece_boards[side] ^= from_to;
            int captured_file = MoveUtils::get_ep_capture_file(move);
            int captured_sq =  side == WHITE ? a5 + captured_file : a4 + captured_file;

            uint64 sq_bitboard = get_square_bitboard(captured_sq);
            bb->piece_boards[side ^ 1][pPAWN] ^= sq_bitboard;
            bb->collective_piece_boards[side ^ 1] ^= sq_bitboard;
            bb->all = bb->collective_piece_boards[WHITE] | bb->collective_piece_boards[BLACK];
            if(bb->attacked(side, king_location)){
                bb->piece_boards[side][piece] ^= from_to;
                bb->collective_piece_boards[side] ^= from_to;
                bb->piece_boards[side ^ 1][pPAWN] ^= sq_bitboard;
                bb->collective_piece_boards[side ^ 1] ^= sq_bitboard;
                bb->all = bb->collective_piece_boards[WHITE] | bb->collective_piece_boards[BLACK];
                tt.update_hash_val_piece_square(move, piece_square_hash_val);
                tt.update_hash_val_side_to_move(move);
                return false;
            }
            bi->add_board_info(castle_rights, NO_EP_RIGHTS);
            bb->piece_on_square[from] = NO_PIECE;
            bb->piece_on_square[to] = pPAWN;
            bb->piece_on_square[captured_sq] = NO_PIECE;
            break;
        } case KING_CASTLE: {
            bb->piece_boards[side][pKING] ^= king_from_to_bb[side][KING_CASTLE];
            bb->collective_piece_boards[side] ^= king_from_to_bb[side][KING_CASTLE];

            bb->piece_boards[side][pROOK] ^= rook_from_to_bb[side][KING_CASTLE];
            bb->collective_piece_boards[side] ^= rook_from_to_bb[side][KING_CASTLE];
            castle_rights &= castle_rights_king_mask[side];
            update_king_location(side, castle_king_to[side][KING_CASTLE]);

            bb->piece_on_square[castle_king_from[side][KING_CASTLE]] = NO_PIECE;
            bb->piece_on_square[castle_rook_from[side][KING_CASTLE]] = NO_PIECE;
            bb->piece_on_square[castle_king_to[side][KING_CASTLE]] = pKING;
            bb->piece_on_square[castle_rook_to[side][KING_CASTLE]] = pROOK;

            bi->add_board_info(castle_rights, NO_EP_RIGHTS);
            break;
        } case QUEEN_CASTLE: {

            bb->piece_boards[side][pKING] ^= king_from_to_bb[side][QUEEN_CASTLE];
            bb->collective_piece_boards[side] ^= king_from_to_bb[side][QUEEN_CASTLE];
            bb->piece_boards[side][pROOK] ^= rook_from_to_bb[side][QUEEN_CASTLE];
            bb->collective_piece_boards[side] ^= rook_from_to_bb[side][QUEEN_CASTLE];

            castle_rights &= castle_rights_king_mask[side];
            update_king_location(side, castle_king_to[side][QUEEN_CASTLE]);

            bb->piece_on_square[castle_king_from[side][QUEEN_CASTLE]] = NO_PIECE;
            bb->piece_on_square[castle_rook_from[side][QUEEN_CASTLE]] = NO_PIECE;
            bb->piece_on_square[castle_king_to[side][QUEEN_CASTLE]] = pKING;
            bb->piece_on_square[castle_rook_to[side][QUEEN_CASTLE]] = pROOK;

            bi->add_board_info(castle_rights, NO_EP_RIGHTS);
            break;
        } 
        case KNIGHT_PROMOTION: {
            return apply_promotion_move(move, from, to, side, pKNIGHT, king_location, piece_square_hash_val);
        } case BISHOP_PROMOTION: {
            return apply_promotion_move(move, from, to, side, pBISHOP, king_location, piece_square_hash_val);
        } case ROOK_PROMOTION: {
            return apply_promotion_move(move, from, to, side, pROOK, king_location, piece_square_hash_val);
        } case QUEEN_PROMOTION: {
            return apply_promotion_move(move, from, to, side, pQUEEN, king_location, piece_square_hash_val);
        } 
        case KNIGHT_CAPTURE_PROMOTION: {
            return apply_capture_promotion_move(move, from, to, side, pKNIGHT, captured_piece, king_location, piece_square_hash_val);
        } case BISHOP_CAPTURE_PROMOTION: {
            return apply_capture_promotion_move(move, from, to, side, pBISHOP, captured_piece, king_location, piece_square_hash_val);
        } case ROOK_CAPTURE_PROMOTION: {
            return apply_capture_promotion_move(move, from, to, side, pROOK, captured_piece, king_location, piece_square_hash_val);
        } case QUEEN_CAPTURE_PROMOTION: {
            return apply_capture_promotion_move(move, from, to, side, pQUEEN, captured_piece, king_location, piece_square_hash_val);
        }
    }


    unsigned int next_castle_rights = bi->peek_castle_right();
    unsigned int next_ep_rights = bi->peek_ep_right();
    tt.update_hash_val_castle_rights(prev_castle_rights, next_castle_rights);
    tt.update_hash_val_ep_rights(prev_ep_rights, next_ep_rights);
    return true;
}

void Board::reverse_move(unsigned int move){
    unsigned int side = MoveUtils::get_side(move);
    unsigned int piece = MoveUtils::get_piece(move);
    unsigned int from = MoveUtils::get_from(move);
    unsigned int to = MoveUtils::get_to(move);

    // for zobrist hash val update
    unsigned int prev_castle_rights = bi->peek_castle_right();
    unsigned int prev_ep_rights = bi->peek_ep_right();
    tt.update_hash_val_piece_square(move);
    tt.update_hash_val_side_to_move(move);
    if(MoveUtils::is_quiet(move)){
        uint64 from_to = get_from_to(from, to);
        bb->piece_boards[side][piece] ^= from_to;
        bb->collective_piece_boards[side] ^= from_to; 

        int additional_info = MoveUtils::get_additional_info(move);
        if(piece == pKING)
            update_king_location(side, from);
        bb->piece_on_square[from] = piece;
        bb->piece_on_square[to] = NO_PIECE;
    } else if (MoveUtils::is_capture(move)){ // capture
        // bi->num_captures -= update_num_moves;
        uint64 from_to = get_from_to(from, to);
        bb->piece_boards[side][piece] ^= from_to;
        bb->collective_piece_boards[side] ^= from_to;
        int captured_piece = MoveUtils::get_captured_piece(move);
        uint64 sq_bitboard = get_square_bitboard(to);
        bb->piece_boards[side ^ 1][captured_piece] ^= sq_bitboard;
        bb->collective_piece_boards[side ^ 1] ^= sq_bitboard;
        if(piece == pKING)
            update_king_location(side, from);
        bb->piece_on_square[from] = piece;
        bb->piece_on_square[to] = captured_piece;
    } else if(MoveUtils::is_double_pawn_push(move)){
        uint64 from_to = get_from_to(from, to);
        bb->piece_boards[side][piece] ^= from_to;
        bb->collective_piece_boards[side] ^= from_to; 

        bb->piece_on_square[from] = piece;
        bb->piece_on_square[to] = NO_PIECE;
    } else if(MoveUtils::is_ep_capture(move)){
        uint64 from_to = get_from_to(from, to);
        bb->piece_boards[side][piece] ^= from_to;
        bb->collective_piece_boards[side] ^= from_to;
        int captured_file = MoveUtils::get_ep_capture_file(move);
        int captured_sq = side == WHITE ? a5 + captured_file: a4 + captured_file;

        uint64 sq_bitboard = get_square_bitboard(captured_sq);
        bb->piece_boards[side ^ 1][pPAWN] ^= sq_bitboard;
        bb->collective_piece_boards[side ^ 1] ^= sq_bitboard;

        bb->piece_on_square[from] = pPAWN;
        bb->piece_on_square[to] = NO_PIECE;
        bb->piece_on_square[captured_sq] = pPAWN;
        
    } else if(MoveUtils::get_additional_info(move) == KING_CASTLE) {
        bb->piece_boards[side][pKING] ^= king_from_to_bb[side][KING_CASTLE];
        bb->collective_piece_boards[side] ^= king_from_to_bb[side][KING_CASTLE];

        bb->piece_boards[side][pROOK] ^= rook_from_to_bb[side][KING_CASTLE];
        bb->collective_piece_boards[side] ^= rook_from_to_bb[side][KING_CASTLE];

        update_king_location(side, castle_king_from[side][KING_CASTLE]);

        bb->piece_on_square[castle_king_to[side][KING_CASTLE]] = NO_PIECE;
        bb->piece_on_square[castle_rook_to[side][KING_CASTLE]] = NO_PIECE;
        bb->piece_on_square[castle_king_from[side][KING_CASTLE]] = pKING;
        bb->piece_on_square[castle_rook_from[side][KING_CASTLE]] = pROOK;

    } else if(MoveUtils::get_additional_info(move) == QUEEN_CASTLE) {

        bb->piece_boards[side][pKING] ^= king_from_to_bb[side][QUEEN_CASTLE];
        bb->collective_piece_boards[side] ^= king_from_to_bb[side][QUEEN_CASTLE];
        bb->piece_boards[side][pROOK] ^= rook_from_to_bb[side][QUEEN_CASTLE];
        bb->collective_piece_boards[side] ^= rook_from_to_bb[side][QUEEN_CASTLE];

        update_king_location(side, castle_king_from[side][QUEEN_CASTLE]);

        bb->piece_on_square[castle_king_to[side][QUEEN_CASTLE]] = NO_PIECE;
        bb->piece_on_square[castle_rook_to[side][QUEEN_CASTLE]] = NO_PIECE;
        bb->piece_on_square[castle_king_from[side][QUEEN_CASTLE]] = pKING;
        bb->piece_on_square[castle_rook_from[side][QUEEN_CASTLE]] = pROOK;

    } else if(MoveUtils::is_promotion(move)){
        uint64 from_to = get_from_to(from, to);
        uint64 from_bitboard = get_square_bitboard(from);
        uint64 to_bitboard = get_square_bitboard(to);
        bb->piece_boards[side][pPAWN] ^= from_bitboard;
        bb->collective_piece_boards[side] ^= from_to;

        if(MoveUtils::is_knight_promotion(move)){
            bb->piece_boards[side][pKNIGHT] ^= to_bitboard;
        } else if(MoveUtils::is_bishop_promotion(move)){
            bb->piece_boards[side][pBISHOP] ^= to_bitboard;
        } else if(MoveUtils::is_rook_promotion(move)){
            bb->piece_boards[side][pROOK] ^= to_bitboard;
        } else if(MoveUtils::is_queen_promotion(move)){
            bb->piece_boards[side][pQUEEN] ^= to_bitboard;
        }
        bb->piece_on_square[from] = pPAWN;
            bb->piece_on_square[to] = NO_PIECE;
    } else if(MoveUtils::is_capture_promotion(move)){

        uint64 from_to = get_from_to(from, to);
        uint64 from_bitboard = get_square_bitboard(from);
        uint64 to_bitboard = get_square_bitboard(to);
        bb->piece_boards[side][pPAWN] ^= from_bitboard;
        bb->collective_piece_boards[side] ^= from_to;

        unsigned int captured_piece = MoveUtils::get_captured_piece(move);
        if(MoveUtils::is_knight_capture_promotion(move)){
            bb->piece_boards[side][pKNIGHT] ^= to_bitboard;
            bb->piece_boards[side ^ 1][captured_piece] ^= to_bitboard;
            bb->collective_piece_boards[side ^ 1] ^= to_bitboard;
        } else if(MoveUtils::is_bishop_capture_promotion(move)){
            bb->piece_boards[side][pBISHOP] ^= to_bitboard;
            bb->piece_boards[side ^ 1][captured_piece] ^= to_bitboard;
            bb->collective_piece_boards[side ^ 1] ^= to_bitboard;
        } else if(MoveUtils::is_rook_capture_promotion(move)){
            bb->piece_boards[side][pROOK] ^= to_bitboard;
            bb->piece_boards[side ^ 1][captured_piece] ^= to_bitboard;
            bb->collective_piece_boards[side ^ 1] ^= to_bitboard;
        } else if(MoveUtils::is_queen_capture_promotion(move)){
            bb->piece_boards[side][pQUEEN] ^= to_bitboard;
            bb->piece_boards[side ^ 1][captured_piece] ^= to_bitboard;
            bb->collective_piece_boards[side ^ 1] ^= to_bitboard;
        }
        bb->piece_on_square[from] = pPAWN;
        bb->piece_on_square[to] = captured_piece;

    }

    bb->all = bb->collective_piece_boards[WHITE] | bb->collective_piece_boards[BLACK];
    bi->remove_board_info();

    unsigned int next_castle_rights = bi->peek_castle_right();
    unsigned int next_ep_rights = bi->peek_ep_right();
    tt.update_hash_val_castle_rights(prev_castle_rights, next_castle_rights);
    tt.update_hash_val_ep_rights(prev_ep_rights, next_ep_rights);
}
bool Board::apply_move_if_legal(unsigned int move)
{
    unsigned int defending_side = MoveUtils::get_side(move);
    return apply_move(move);
}
bool Board::can_castle_kingside(unsigned int side){
    if(MoveUtils::get_side(side) == WHITE){
        return bi->peek_castle_right() & 0b1000;
    } else {
        return bi->peek_castle_right()  & 0x0010;
    }
}

bool Board::can_castle_queenside(unsigned int side){
    if(MoveUtils::get_side(side) == BLACK){
        return bi->peek_castle_right()  & 0b0100;
    } else{
        return bi->peek_castle_right()  & 0b0001;
    }
}

void Board::update_piece_locations(int side, int piece, int from, int to){
    piece_locations[side][piece].erase(from);
    piece_locations[side][piece].insert(to);
}
void Board::update_king_location(unsigned int side, unsigned int square){
    king_location[side] = square;
}
int Board::get_piece_location(unsigned int side, unsigned int piece){
    auto beginning = piece_locations[side][piece].begin();
    if(beginning != piece_locations[side][piece].end())
        return *beginning;
    return -1;

}
unsigned int Board::get_king_location(unsigned int side){
    return king_location[side];
}

unsigned int Board::get_side_to_move(){
    return side_to_move;
}

void Board::change_side_to_move(){
    side_to_move ^= 1;
}

unsigned int Board::create_move_using_pgn(unsigned int from, unsigned int to, unsigned int promoted_piece){
    unsigned int side = side_to_move;
    // unsigned int piece = bb->get_piece_on_square(side, from);
    unsigned int piece = bb->piece_on_square[from];
    // unsigned int captured_piece =  bb->get_piece_on_square(side ^ 1, to);
    unsigned int captured_piece = bb->piece_on_square[to];
    unsigned int additional_info = QUIET_MOVE;
    unsigned int ep_target_file = 0;

    uint64 from_bb = get_square_bitboard(from);
    uint64 to_bb = get_square_bitboard(to);

    if(piece == pKING){
        int from_to_difference = (int)from - (int)to;
        if(from_to_difference == 2){
            additional_info = QUEEN_CASTLE;
        } else if(from_to_difference == -2){
            additional_info = KING_CASTLE;
        }
        else if(captured_piece != NO_PIECE){
            additional_info = CAPTURE;
        }
    } else if(piece == pPAWN){
        int from_to_difference = abs((int)from - (int)to);
        if(from_to_difference == 16){
            additional_info = DOUBLE_PAWN_PUSH;
        } else if((promoted_piece != NO_PIECE || MoveUtils::is_final_rank(to)) && from_to_difference == 8){
            if(promoted_piece == pKNIGHT){
                additional_info = KNIGHT_PROMOTION;
            } else if(promoted_piece == pBISHOP){
                additional_info = BISHOP_PROMOTION;
            } else if(promoted_piece == pROOK){
                additional_info = ROOK_PROMOTION;
            } else {
                additional_info = QUEEN_PROMOTION;
            }
        } else if(promoted_piece != NO_PIECE || MoveUtils::is_final_rank(to) && from_to_difference != 8){
            if(promoted_piece == pKNIGHT){
                additional_info = KNIGHT_CAPTURE_PROMOTION;
            } else if(promoted_piece == pBISHOP){
                additional_info = BISHOP_CAPTURE_PROMOTION;
            } else if(promoted_piece == pROOK){
                additional_info = ROOK_CAPTURE_PROMOTION;
            } else {
                additional_info = QUEEN_CAPTURE_PROMOTION;
            }
        } else if(from_to_difference == 7 || from_to_difference == 9){
            if(captured_piece != NO_PIECE){
                additional_info = CAPTURE;
            } else {
                additional_info = EP_CAPTURE;
            }
        }
    } else {
        if(captured_piece != NO_PIECE)
            additional_info = CAPTURE;
    }
    if(additional_info == QUIET_MOVE){
        // cout<<"move is quiet\n";
        return MoveUtils::create_move(from, to, side, piece, QUIET_MOVE);
    } else if(additional_info == DOUBLE_PAWN_PUSH){
        // cout<<"move is double pawn push\n";
        return MoveUtils::create_move(from, to, side, pPAWN, DOUBLE_PAWN_PUSH);
    } else if(additional_info == CAPTURE){
        // cout<<"move is quiet\n";
        return MoveUtils::create_move(from, to, side, piece, CAPTURE, captured_piece);
    } else if(additional_info == EP_CAPTURE){
        // cout<<"move is en passant\n";
        unsigned int ep_capture_file = 0;
        if(side == WHITE){
            ep_capture_file = to - a4;
        } else {
            ep_capture_file = to - a5;
        }
        return MoveUtils::create_move(from, to, side, pPAWN, EP_CAPTURE, pPAWN, ep_capture_file);
    } else if(additional_info == KING_CASTLE){
        // cout<<"move is kingside castle\n";
        return MoveUtils::create_move(from, to, side, pKING, KING_CASTLE);
    } else if(additional_info == QUEEN_CASTLE){
        // cout<<"move is queenside castle\n";
        return MoveUtils::create_move(from, to, side, pKING, QUEEN_CASTLE);
    } else if(additional_info == KNIGHT_PROMOTION){
        // cout<<"move is knight promotion\n";
        return MoveUtils::create_move(from, to, side, piece, KNIGHT_PROMOTION);
    } else if(additional_info == BISHOP_PROMOTION){
        // cout<<"move is bishop promotion\n";
        return MoveUtils::create_move(from, to, side, piece, BISHOP_PROMOTION);
    } else if(additional_info == ROOK_PROMOTION){
        // cout<<"move is rook promotion\n";
        return MoveUtils::create_move(from, to, side, piece, ROOK_PROMOTION);
    } else if(additional_info == QUEEN_PROMOTION){
        // cout<<"move is queen promotion\n";
        return MoveUtils::create_move(from, to, side, piece, QUEEN_PROMOTION);
    } else if(additional_info == KNIGHT_CAPTURE_PROMOTION){
        // cout<<"move is knight capture promotion\n";
        return MoveUtils::create_move(from, to, side, piece, KNIGHT_CAPTURE_PROMOTION, captured_piece);
    } else if(additional_info == BISHOP_CAPTURE_PROMOTION){
        // cout<<"move is bishop capture promotion\n";
        return MoveUtils::create_move(from, to, side, piece, BISHOP_CAPTURE_PROMOTION, captured_piece);
    } else if(additional_info == ROOK_CAPTURE_PROMOTION){
        // cout<<"move is rook capture promotion\n";
        return MoveUtils::create_move(from, to, side, piece, ROOK_CAPTURE_PROMOTION, captured_piece);
    } else if(additional_info == QUEEN_CAPTURE_PROMOTION){
        // cout<<"move is queen capture promotion\n";
        return MoveUtils::create_move(from, to, side, piece, QUEEN_CAPTURE_PROMOTION, captured_piece);
    }
    cout<<"no move match\n";
    return 0;
}
void Board::parse_fen(fs::path path){
    for(unsigned int side = 0 ; side < NUM_SIDES ; side ++){
        for(unsigned int piece = 0; piece < NUM_PIECE_TYPES ; piece ++){
            bb->piece_boards[side][piece] = 0;
        }
    }
    for(unsigned int sq = 0 ; sq < NUM_SQUARES ;sq ++)
        bb->piece_on_square[sq] = NO_PIECE;

    std::ifstream file(path);
    unsigned int initial_side_to_move = WHITE;
    if(file.is_open()){
        string word;
        FEN_STATE fen_state = POSITION;
        while(file >> word && fen_state != FINISHED){
            if(fen_state == POSITION){

                fen_state = SIDE_TO_MOVE;

                int row = 7;
                int pos = row * 8 - 1;
                for(size_t i = 0 ; i < word.length() ; i ++){
                    char ch = word[i];
                    if(isalpha(ch)){
                        pos += 1;
                        uint64 square_bitboard = get_square_bitboard(pos);
                        int side;
                        if(ch > 'Z'){
                            side = BLACK;
                        } else {
                            side = WHITE;
                        }
                        char piece_type = tolower(ch);
                        if(piece_type == 'p'){
                            bb->piece_boards[side][pPAWN] |= square_bitboard;
                            bb->piece_on_square[pos] = pPAWN;
                        } else if(piece_type == 'n'){
                            bb->piece_boards[side][pKNIGHT] |= square_bitboard;
                            bb->piece_on_square[pos] = pKNIGHT;
                        } else if(piece_type == 'b'){
                            bb->piece_boards[side][pBISHOP] |= square_bitboard;
                            bb->piece_on_square[pos] = pBISHOP;
                        } else if(piece_type == 'q'){
                            bb->piece_boards[side][pQUEEN] |= square_bitboard;
                            bb->piece_on_square[pos] = pQUEEN;
                        } else if(piece_type == 'k'){
                            bb->piece_boards[side][pKING] |= square_bitboard;
                            bb->piece_on_square[pos] = pKING;
                            update_king_location(side, pos);
                        } else if(piece_type =='r'){
                            bb->piece_boards[side][pROOK] |= square_bitboard;
                            bb->piece_on_square[pos] = pROOK;
                        } 
                    } else if(isdigit(ch)){

                        int stride = ch - '0';
                        pos += stride;
                    } else if(ch == '/'){
                        row -= 1;
                        pos = row * 8 - 1;
                        continue;
                    } 
                }
            } else if(fen_state == SIDE_TO_MOVE){

                fen_state = CASTLE_RIGHTS;

                char ch = word[0];
                side_to_move = !(ch == 'w');
            } else if(fen_state == CASTLE_RIGHTS){

                fen_state = EP_TARGET_SQUARE;

                for(size_t i = 0 ; i < word.length() ; i ++){
                    char ch = word[i];
                    if(ch == 'K'){
                        initial_castle_rights |= 0b1000;
                    } else if(ch == 'Q'){
                        initial_castle_rights |= 0b0100;
                    }
                    if(ch == 'k'){
                        initial_castle_rights |= 0b0010;
                    } else if(ch == 'q'){
                        initial_castle_rights |= 0b0001;
                    } 
                }
            } else if(fen_state == EP_TARGET_SQUARE){

                fen_state = FINISHED;
                char ch = word[0];
                cout<<"ep target square: "<<ch<<endl;
                if(!isalpha(ch))
                    continue;
                initial_ep_rights = ch - 'a';                
                cout<<"initial_ep_rights: "<<initial_ep_rights<<endl;

            } 
            /*TODO: Halfmove clock*/
        }
    }
    bi->set_board_info(initial_castle_rights, initial_ep_rights);
    bb->update();
    // init_piece_locations();
    tt.initialise_hash_val(side_to_move, bb, bi);
}
unsigned int Board::parse_single_move(string move_string, bool verbose){
    unsigned int move = 0;
    if(move_string.length() < 4){
        return 0;
    } else if(move_string.length() == 4){
        std::string from_string = move_string.substr(0, 2);
        std::string to_string = move_string.substr(2, 2);

        unsigned int from = MoveUtils::square_as_uint(from_string);
        unsigned int to = MoveUtils::square_as_uint(to_string);
        move = create_move_using_pgn(from, to);
    } else {
        std::string from_string = move_string.substr(0, 2);
        std::string to_string = move_string.substr(2, 2);
        std::string promoted_piece_string = move_string.substr(4, 1);

        unsigned int from = MoveUtils::square_as_uint(from_string);
        unsigned int to = MoveUtils::square_as_uint(to_string);
        unsigned int promoted_piece = MoveUtils::promoted_piece_as_uint(promoted_piece_string);
        move = create_move_using_pgn(from, to, promoted_piece);
    } 
    move_count++;
    if(verbose && side_to_move == WHITE)
        cout<<move_count<<"\n";
    if(verbose)
        MoveUtils::display(move);
    if(!apply_move_if_legal(move) || move == 0){
        bb->display();
        throw std::runtime_error("[parse_pgn] apply_move_if_legal, move not legal");
    };
    tt.increment_value_threefold();
    if(verbose){
        cout<<"threefold repition: "<< tt.get_value_threefold()<<"\n";
        cout<<"transposition hash val: "<<tt.hash_val<<"\n";
    }
    unsigned int value_threefold = tt.get_value_threefold();
    // cout<<"get_value_threefold(): "<<value_threefold<<"\n";
    if(value_threefold == 3){  
        threefold_draw = true;
        return 0;
    } 
    change_side_to_move();
    return move;
}
void Board::parse_uci_pgn(fs::path path, int last_move, bool verbose){

    bi->set_board_info(initial_castle_rights, initial_ep_rights);
    bb->update();

    std::ifstream file(path);
    if(file.is_open()){
        std::string move_string;
        while(move_count < last_move && file >> move_string){
            unsigned int move = 0;
            if(move_string.length() < 4){
                return;
            } else if(move_string.length() == 4){
                std::string from_string = move_string.substr(0, 2);
                std::string to_string = move_string.substr(2, 2);

                unsigned int from = MoveUtils::square_as_uint(from_string);
                unsigned int to = MoveUtils::square_as_uint(to_string);
                move = create_move_using_pgn(from, to);
            } else {
                std::string from_string = move_string.substr(0, 2);
                std::string to_string = move_string.substr(2, 2);
                std::string promoted_piece_string = move_string.substr(4, 1);

                unsigned int from = MoveUtils::square_as_uint(from_string);
                unsigned int to = MoveUtils::square_as_uint(to_string);
                unsigned int promoted_piece = MoveUtils::promoted_piece_as_uint(promoted_piece_string);
                move = create_move_using_pgn(from, to, promoted_piece);
            } 
            move_count++;
            if(verbose && side_to_move == WHITE)
                cout<<move_count<<"\n";
            if(verbose)
                MoveUtils::display(move);
            if(!apply_move_if_legal(move) || move == 0){
                bb->display();
                throw std::runtime_error("[parse_pgn] apply_move_if_legal, move not legal");
            };
            tt.increment_value_threefold();
            if(verbose){
                cout<<"threefold repition: "<< tt.get_value_threefold()<<"\n";
                cout<<"transposition hash val: "<<tt.hash_val<<"\n";
            }
            unsigned int value_threefold = tt.get_value_threefold();
            // cout<<"get_value_threefold(): "<<value_threefold<<"\n";
            if(value_threefold == 3){  
                threefold_draw = true;
                return;
            } 
            change_side_to_move();
        }
    }
}
BoardInfo* Board::get_board_info(){
    return bi;
}
Bitboard* Board::get_bitboard(){
    return bb;
}
int Board::get_initial_ep_rights(){
    return initial_ep_rights;
}
int Board::get_initial_castle_rights(){
    return initial_castle_rights;
}