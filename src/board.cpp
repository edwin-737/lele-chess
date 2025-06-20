#include <iostream>
#include "board.hpp"
#include "board_squares.hpp"
using namespace BoardSquares;
void Board::apply_move(unsigned int move, int update_num_moves){
    unsigned int side = MoveUtils::get_side(move);
    unsigned int piece = MoveUtils::get_piece(move);
    unsigned int from = MoveUtils::get_from(move);
    unsigned int to = MoveUtils::get_to(move);        
    int castle_rights = bi->peek_castle_right();

    if(MoveUtils::is_castle(move)){
        // bi->num_castles += update_num_moves;
        if(MoveUtils::is_king_castle(move) && side == WHITE){
            uint64 king_from_to = get_from_to(e1, g1);
            bb->piece_boards[side][pKING] ^= king_from_to;
            bb->collective_piece_boards[side] ^= king_from_to;

            uint64 rook_from_to = get_from_to(h1, f1);
            bb->piece_boards[side][pROOK] ^= rook_from_to;
            bb->collective_piece_boards[side] ^= rook_from_to;

            castle_rights &= 0x3;
            update_king_location(side, g1);
        } else if(MoveUtils::is_queen_castle(move) && side == WHITE){
            uint64 king_from_to = get_from_to(e1, c1);
            bb->piece_boards[side][pKING] ^= king_from_to;
            bb->collective_piece_boards[side] ^= king_from_to;

            uint64 rook_from_to = get_from_to(a1, d1);
            bb->piece_boards[side][pROOK] ^= rook_from_to;
            bb->collective_piece_boards[side] ^= rook_from_to;

            castle_rights &= 0x3;
            update_king_location(side, c1);
        } else if(MoveUtils::is_king_castle(move) && side == BLACK){
            uint64 king_from_to = get_from_to(e8, g8);
            bb->piece_boards[side][pKING] ^= king_from_to;
            bb->collective_piece_boards[side] ^= king_from_to;

            uint64 rook_from_to = get_from_to(h8, f8);
            bb->piece_boards[side][pROOK] ^= rook_from_to;
            bb->collective_piece_boards[side] ^= rook_from_to;

            castle_rights &= 0xc;
            update_king_location(side, g8);
        } else if(MoveUtils::is_queen_castle(move) && side == BLACK){
            uint64 king_from_to = get_from_to(e8, c8);
            bb->piece_boards[side][pKING] ^= king_from_to;
            bb->collective_piece_boards[side] ^= king_from_to;

            uint64 rook_from_to = get_from_to(a8, d8);
            bb->piece_boards[side][pROOK] ^= rook_from_to;
            bb->collective_piece_boards[side] ^= rook_from_to;

            castle_rights &= 0xc;
            update_king_location(side, c8);
        } 
        bi->add_board_info(castle_rights, NO_EP_RIGHTS);
    } else if(MoveUtils::is_capture(move)){    
        uint64 from_to = get_from_to(from, to);
        bb->piece_boards[side][piece] ^= from_to;
        bb->collective_piece_boards[side] ^= from_to;
        unsigned int captured_piece = MoveUtils::get_captured_piece(move);
        uint64 sq_bitboard = get_square_bitboard(to);
        bb->piece_boards[side ^ 1][captured_piece] ^= sq_bitboard;
        bb->collective_piece_boards[side ^ 1] ^= sq_bitboard;

        if(piece == pKING){
            if(side == WHITE){
                castle_rights &= 0x3;
            } else{
                castle_rights &= 0xc;
            }
            update_king_location(side, to);
            // cout<<"move is capture king move\n";
            bi->add_board_info(castle_rights, NO_EP_RIGHTS);
        } else {
            bi->add_board_info(castle_rights, NO_EP_RIGHTS);
        }
    } else if(MoveUtils::is_ep_capture(move)){
        // bi->num_ep_captures+=update_num_moves;
        // bi->num_captures+=update_num_moves;
        uint64 from_to = get_from_to(from, to);
        bb->piece_boards[side][piece] ^= from_to;
        bb->collective_piece_boards[side] ^= from_to;
        int captured_file = MoveUtils::get_ep_capture_file(move);
        int captured_sq =  side == WHITE ? a5 + captured_file : a4 + captured_file;

        uint64 sq_bitboard = get_square_bitboard(captured_sq);
        bb->piece_boards[side ^ 1][pPAWN] ^= sq_bitboard;
        bb->collective_piece_boards[side ^ 1] ^= sq_bitboard;

        bi->add_board_info(castle_rights, NO_EP_RIGHTS);
    } else {
        uint64 from_to = get_from_to(from, to);
        bb->piece_boards[side][piece] ^= from_to;
        bb->collective_piece_boards[side] ^= from_to;

        unsigned int additional_info = MoveUtils::get_additional_info(move);
        if(piece == pKING){
            if(side == WHITE){
                castle_rights &= 0x3;
            } else{
                castle_rights &= 0xc;
            }
            update_king_location(side, to);
            // cout<<"move is quiet king move\n";
            bi->add_board_info(castle_rights, NO_EP_RIGHTS);
        } else if(piece == pROOK){
            if(side == WHITE){
                if(from == a1){
                    castle_rights &= (0b1111 ^ 0b0100);
                } else if(from == h1){
                    castle_rights &= (0b1111 ^ 0b1000);
                } 
            } else{
                if(from == a8){
                    castle_rights &= (0b1111 ^ 0b0001);
                } else if(from == h8 ){
                    castle_rights &= (0b1111 ^ 0b0010);
                }
            } 
            // cout<<"move is quiet rook move\n";
            bi->add_board_info(castle_rights, NO_EP_RIGHTS);
        } else if(piece == pPAWN && additional_info == DOUBLE_PAWN_PUSH){
            int ep_rights = bi->peek_ep_right();
            if(side == WHITE){
                ep_rights = to - a4;
            } else {
                ep_rights = to - a5;
            }
            bi->add_board_info(castle_rights, ep_rights);
        } else {
            bi->add_board_info(castle_rights, NO_EP_RIGHTS);
        }
        // update_piece_locations(side, piece, from, to);
    }
    // eval->update_material(move, false);
    bb->all = bb->collective_piece_boards[WHITE] | bb->collective_piece_boards[BLACK];
    // bb->update();
}

void Board::reverse_move(unsigned int move, int update_num_moves){
    unsigned int side = MoveUtils::get_side(move);
    unsigned int piece = MoveUtils::get_piece(move);
    unsigned int from = MoveUtils::get_from(move);
    unsigned int to = MoveUtils::get_to(move);
    if (MoveUtils::is_capture(move)){ // capture
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
    } else if(MoveUtils::is_ep_capture(move)){
        // bi->num_ep_captures -= update_num_moves;
        // bi->num_captures -= update_num_moves;
        uint64 from_to = get_from_to(from, to);
        bb->piece_boards[side][piece] ^= from_to;
        bb->collective_piece_boards[side] ^= from_to;
        // int captured_piece = MoveUtils::get_captured_piece(move);
        int captured_file = MoveUtils::get_ep_capture_file(move);
        int captured_sq = side == WHITE ? a5 + captured_file: a4 + captured_file;

        uint64 sq_bitboard = get_square_bitboard(captured_sq);
        bb->piece_boards[side ^ 1][pPAWN] ^= sq_bitboard;
        bb->collective_piece_boards[side ^ 1] ^= sq_bitboard;
    } else if(MoveUtils::is_castle(move)){ // castle, reverse location for both king and rook
        // bi->num_castles -= update_num_moves;
        if(MoveUtils::is_king_castle(move) && side == WHITE){
            bb->piece_boards[side][pKING] ^= get_from_to(e1, g1);
            bb->collective_piece_boards[side] ^= get_from_to(e1, g1); 
            bb->piece_boards[side][pROOK] ^= get_from_to(h1, f1);
            bb->collective_piece_boards[side] ^= get_from_to(h1, f1); 

            // update_piece_locations(side, pROOK, f1, h1);
            update_king_location(side, e1);
        } else if(MoveUtils::is_queen_castle(move) && side == WHITE){
            bb->piece_boards[side][pKING] ^= get_from_to(e1, c1);
            bb->collective_piece_boards[side] ^= get_from_to(e1, c1); 
            bb->piece_boards[side][pROOK] ^= get_from_to(a1, d1);
            bb->collective_piece_boards[side] ^= get_from_to(a1, d1); 

            // update_piece_locations(side, pROOK, d1, a1);
            update_king_location(side, e1);
        } else if(MoveUtils::is_king_castle(move) && side == BLACK){
            bb->piece_boards[side][pKING] ^= get_from_to(e8, g8);
            bb->collective_piece_boards[side] ^= get_from_to(e8, g8); 
            bb->piece_boards[side][pROOK] ^= get_from_to(h8, f8);
            bb->collective_piece_boards[side] ^= get_from_to(h8, f8); 

            // update_piece_locations(side, pROOK, f8, h8);
            update_king_location(side, e8);
        } else if(MoveUtils::is_queen_castle(move) && side == BLACK){
            bb->piece_boards[side][pKING] ^= get_from_to(e8, c8);
            bb->collective_piece_boards[side] ^= get_from_to(e8, c8); 
            bb->piece_boards[side][pROOK] ^= get_from_to(a8, d8);
            bb->collective_piece_boards[side] ^= get_from_to(a8, d8); 

            // update_piece_locations(side, pROOK, d8, a8);
            update_king_location(side, e8);
        }
    } else{ // quiet move, reverse location of the piece
        uint64 from_to = get_from_to(from, to);
        bb->piece_boards[side][piece] ^= from_to;
        bb->collective_piece_boards[side] ^= from_to; 

        int additional_info = MoveUtils::get_additional_info(move);
        if(piece == pKING)
            update_king_location(side, from);
    }

    // eval->update_material(move, true);
    bb->all = bb->collective_piece_boards[WHITE] | bb->collective_piece_boards[BLACK];
    bi->remove_board_info();
    // bb->update();
}
bool Board::apply_move_if_legal(unsigned int move, int update_num_moves)
{
    // cout<<"apply_move_if_legal\n";
    unsigned int defending_isde = MoveUtils::get_side(move);
    // cout<<"side: "<<side<<endl;
    apply_move(move, update_num_moves);
    // int king_location = get_piece_location(side, pKING);
    int king_location = get_king_location(defending_isde);
    if(bb->attacked(defending_isde, king_location)){
        // cout<<"king attacked, reversing move\n";
        reverse_move(move, update_num_moves);
        return false;
    }
    return true;
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
void Board::update_king_location(int side, int square){
    king_location[side] = square;
}
int Board::get_piece_location(int side, int piece){
    auto beginning = piece_locations[side][piece].begin();
    if(beginning != piece_locations[side][piece].end())
        return *beginning;
    return -1;

}
int Board::get_king_location(int side){
    return king_location[side];
}

int Board::get_side_to_move(){
    return side_to_move;
}

void Board::change_side_to_move(){
    side_to_move ^= 1;
}

void Board::parse_fen(fs::path path){

    std::ifstream file(path);

    if(file.is_open()){
        char ch;
        int row = 7;
        int pos = row * 8 - 1;
        FEN_STATE fen_state = POSITION;
        int castle_rights = 0;
        while(file.get(ch) && fen_state != FINISHED){
            if(ch == ' '){
                if(fen_state == POSITION){
                    fen_state = SIDE_TO_MOVE;
                } else if(fen_state == SIDE_TO_MOVE){
                    fen_state = CASTLE_RIGHTS;
                } else if(fen_state == CASTLE_RIGHTS){
                    fen_state = EP_TARGET_SQUARE;
                } else if(fen_state == EP_TARGET_SQUARE){
                    fen_state = FINISHED;
                }
            }
            if(fen_state == SIDE_TO_MOVE){
                side_to_move = WHITE ? ch == 'w' : ch == 'b';
            } else if(fen_state == CASTLE_RIGHTS){
                // cout<<"fen state is castle rights\n";
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
                // cout<<"initial_castle_rights: "<<initial_castle_rights<<endl;

                // bi->add_castle_right(castle_rights);
                // initial_castle_rights = castle_rights;
            } else if(fen_state == EP_TARGET_SQUARE){
                if(!isalpha(ch))
                    continue;
                // cout<<"state is ep target square\n";
                initial_ep_rights = ch - 'a';
                // cout<<"parse fen inital ep rights: "<<initial_ep_rights<<endl;
            } else if(fen_state == POSITION){
                if(isalpha(ch)){
                    pos += 1;
                    uint64 sq = BoardSquares::get_square_bitboard(pos);
                    int side;
                    if(ch > 'Z'){
                        side = BLACK;
                    } else {
                        side = WHITE;
                    }
                    char piece_type = tolower(ch);
                    if(piece_type == 'p'){
                        bb->piece_boards[side][pPAWN] |= sq;
                    } else if(piece_type == 'n'){
                        bb->piece_boards[side][pKNIGHT] |= sq;
                    } else if(piece_type == 'b'){
                        bb->piece_boards[side][pBISHOP] |= sq;
                    } else if(piece_type == 'q'){
                        bb->piece_boards[side][pQUEEN] |= sq;
                    } else if(piece_type == 'k'){
                        bb->piece_boards[side][pKING] |= sq;
                    } else if(piece_type =='r'){
                        bb->piece_boards[side][pROOK] |= sq;
                    } 
                } else if(isnumber(ch)){
                    int stride = ch - '0';
                    pos += stride;
                } else if(ch =='/'){
                    row -= 1;
                    pos = row * 8 - 1;
                    continue;
                } else {
                    break;
                }
            }
            
        }
    }
    init_piece_locations();
    bb->update();
}
void Board::init_piece_locations(){
    for(int sq = 0 ; sq < NUM_SQUARES ; sq ++){
        for(int side = 0 ; side < NUM_SIDES ; side ++){
            for(int piece = 0 ; piece < NUM_PIECE_TYPES ; piece ++){
                if(bb->piece_boards[side][piece] & get_square_bitboard(sq)){
                    piece_locations[side][piece].insert(sq);
                }
            }
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