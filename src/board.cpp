#include <iostream>
#include "board.hpp"
#include "board_squares.hpp"
using namespace BoardSquares;
void Board::apply_move(unsigned int move){
    unsigned int side = MoveUtils::get_side(move);
    unsigned int piece = MoveUtils::get_piece(move);
    unsigned int from = MoveUtils::get_from(move);
    unsigned int to = MoveUtils::get_to(move);        
    unsigned int castle_rights = bi->peek_castle_right();
    unsigned int ep_rights = bi->peek_ep_right();
    if(MoveUtils::is_quiet(move)){
        uint64 from_to = get_from_to(from, to);
        bb->piece_boards[side][piece] ^= from_to;
        bb->collective_piece_boards[side] ^= from_to;
        unsigned int new_castle_rights = castle_rights;
        unsigned int additional_info = MoveUtils::get_additional_info(move);
        if(piece == pKING){
            if(side == WHITE){
                new_castle_rights &= 0x3;
            } else{
                new_castle_rights &= 0xc;
            }
            update_king_location(side, to);
        } else if(piece == pROOK){
            if(side == WHITE){
                if(from == a1){
                    new_castle_rights &= 0b1011;
                } else if(from == h1){
                    new_castle_rights &= 0b0111;
                } 
            } else{
                if(from == a8){
                    new_castle_rights &= 0b1110;
                } else if(from == h8 ){
                    new_castle_rights &= 0b1101;
                }
            } 
        }            
        bi->add_board_info(new_castle_rights, NO_EP_RIGHTS);
        update_zobrist_hash_val(move, ep_rights, NO_EP_RIGHTS, castle_rights, new_castle_rights);
    } else if(MoveUtils::is_double_pawn_push(move)){
        uint64 from_to = get_from_to(from, to);
        bb->piece_boards[side][pPAWN] ^= from_to;
        bb->collective_piece_boards[side] ^= from_to;
        int new_ep_rights = ep_rights;
        if(side == WHITE){
            new_ep_rights = to - a4;
        } else {
            new_ep_rights = to - a5;
        }
        bi->add_board_info(castle_rights, new_ep_rights);
        update_zobrist_hash_val(move, ep_rights, new_ep_rights, castle_rights, castle_rights);
    } else if(MoveUtils::is_castle(move)){
        unsigned int new_castle_rights = castle_rights;
        if(MoveUtils::is_king_castle(move) && side == WHITE){
            uint64 king_from_to = get_from_to(e1, g1);
            bb->piece_boards[side][pKING] ^= king_from_to;
            bb->collective_piece_boards[side] ^= king_from_to;

            uint64 rook_from_to = get_from_to(h1, f1);
            bb->piece_boards[side][pROOK] ^= rook_from_to;
            bb->collective_piece_boards[side] ^= rook_from_to;

            new_castle_rights &= 0x3;
            update_king_location(side, g1);
        } else if(MoveUtils::is_queen_castle(move) && side == WHITE){
            uint64 king_from_to = get_from_to(e1, c1);
            bb->piece_boards[side][pKING] ^= king_from_to;
            bb->collective_piece_boards[side] ^= king_from_to;

            uint64 rook_from_to = get_from_to(a1, d1);
            bb->piece_boards[side][pROOK] ^= rook_from_to;
            bb->collective_piece_boards[side] ^= rook_from_to;

            new_castle_rights &= 0x3;
            update_king_location(side, c1);
        } else if(MoveUtils::is_king_castle(move) && side == BLACK){
            uint64 king_from_to = get_from_to(e8, g8);
            bb->piece_boards[side][pKING] ^= king_from_to;
            bb->collective_piece_boards[side] ^= king_from_to;

            uint64 rook_from_to = get_from_to(h8, f8);
            bb->piece_boards[side][pROOK] ^= rook_from_to;
            bb->collective_piece_boards[side] ^= rook_from_to;

            new_castle_rights &= 0xc;
            update_king_location(side, g8);
        } else if(MoveUtils::is_queen_castle(move) && side == BLACK){
            uint64 king_from_to = get_from_to(e8, c8);
            bb->piece_boards[side][pKING] ^= king_from_to;
            bb->collective_piece_boards[side] ^= king_from_to;

            uint64 rook_from_to = get_from_to(a8, d8);
            bb->piece_boards[side][pROOK] ^= rook_from_to;
            bb->collective_piece_boards[side] ^= rook_from_to;

            new_castle_rights &= 0xc;
            update_king_location(side, c8);
        } 
        bi->add_board_info(new_castle_rights, NO_EP_RIGHTS);
        update_zobrist_hash_val(move, ep_rights, NO_EP_RIGHTS, castle_rights, new_castle_rights);

    } else if(MoveUtils::is_ep_capture(move)){
        uint64 from_to = get_from_to(from, to);
        bb->piece_boards[side][piece] ^= from_to;
        bb->collective_piece_boards[side] ^= from_to;
        int captured_file = MoveUtils::get_ep_capture_file(move);
        int captured_sq =  MoveUtils::get_ep_capture_sq(side, captured_file);

        uint64 sq_bitboard = get_square_bitboard(captured_sq);
        bb->piece_boards[side ^ 1][pPAWN] ^= sq_bitboard;
        bb->collective_piece_boards[side ^ 1] ^= sq_bitboard;
        bi->add_board_info(castle_rights, NO_EP_RIGHTS);

        update_zobrist_hash_val(move, ep_rights, NO_EP_RIGHTS, castle_rights, castle_rights);
    } else if(MoveUtils::is_capture(move)){    
        unsigned int new_castle_rights = castle_rights;
        uint64 from_to = get_from_to(from, to);
        bb->piece_boards[side][piece] ^= from_to;
        bb->collective_piece_boards[side] ^= from_to;
        unsigned int captured_piece = MoveUtils::get_captured_piece(move);
        uint64 sq_bitboard = get_square_bitboard(to);
        bb->piece_boards[side ^ 1][captured_piece] ^= sq_bitboard;
        bb->collective_piece_boards[side ^ 1] ^= sq_bitboard;

        if(piece == pKING){
            if(side == WHITE){
                new_castle_rights &= 0x3;
            } else{
                new_castle_rights &= 0xc;
            }
            update_king_location(side, to);
            bi->add_board_info(new_castle_rights, NO_EP_RIGHTS);
            update_zobrist_hash_val(move, ep_rights, NO_EP_RIGHTS, castle_rights, new_castle_rights);
        } else if(piece == pROOK){
            if(side == WHITE){
                if(from == a1){
                    new_castle_rights &= 0b1011;
                } else if(from == h1){
                    new_castle_rights &= 0b0111;
                } 
            } else{
                if(from == a8){
                    new_castle_rights &= 0b1110;
                } else if(from == h8 ){
                    new_castle_rights &= 0b1101;
                }
            } 
            bi->add_board_info(new_castle_rights, NO_EP_RIGHTS);
            update_zobrist_hash_val(move, ep_rights, NO_EP_RIGHTS, castle_rights, new_castle_rights);
        } else {
            bi->add_board_info(castle_rights, NO_EP_RIGHTS);
            update_zobrist_hash_val(move, ep_rights, NO_EP_RIGHTS, castle_rights, castle_rights);
        }

    } else if(MoveUtils::is_promotion(move)){
        bb->piece_boards[side][piece] ^= get_square_bitboard(from);
        uint64 to_bitboard = get_square_bitboard(to);
        if(MoveUtils::is_queen_promotion(move)){
            bb->piece_boards[side][pQUEEN] ^= to_bitboard;
        } else if(MoveUtils::is_knight_promotion(move)){
            bb->piece_boards[side][pKNIGHT] ^= to_bitboard;
        } else if(MoveUtils::is_bishop_promotion(move)){
            bb->piece_boards[side][pBISHOP] ^= to_bitboard;
        } else if(MoveUtils::is_rook_promotion(move)){
            bb->piece_boards[side][pROOK] ^= to_bitboard;
        }
        update_zobrist_hash_val(move, ep_rights, ep_rights, castle_rights, castle_rights);
    } else if(MoveUtils::is_capture_promotion(move)){
        unsigned int captured_piece = MoveUtils::get_captured_piece(move);
        bb->piece_boards[side][piece] ^= get_square_bitboard(from);
        uint64 to_bitboard = get_square_bitboard(to);
        if(MoveUtils::is_queen_promotion(move)){
            bb->piece_boards[side][pQUEEN] ^= to_bitboard;
        } else if(MoveUtils::is_knight_promotion(move)){
            bb->piece_boards[side][pKNIGHT] ^= to_bitboard;
        } else if(MoveUtils::is_bishop_promotion(move)){
            bb->piece_boards[side][pBISHOP] ^= to_bitboard;
        } else if(MoveUtils::is_rook_promotion(move)){
            bb->piece_boards[side][pROOK] ^= to_bitboard;
        }
        bb->piece_boards[side ^ 1][captured_piece] ^= to_bitboard;
        update_zobrist_hash_val(move, ep_rights, ep_rights, castle_rights, castle_rights);
    } 
    eval->update_material(move, false);
    bb->all = bb->collective_piece_boards[WHITE] | bb->collective_piece_boards[BLACK];
}

void Board::reverse_move(unsigned int move){
    unsigned int side = MoveUtils::get_side(move);
    unsigned int piece = MoveUtils::get_piece(move);
    unsigned int from = MoveUtils::get_from(move);
    unsigned int to = MoveUtils::get_to(move);
    unsigned int ep_right = bi->peek_ep_right();
    unsigned int castle_right = bi->peek_castle_right();
    bi->remove_board_info();  
    if(MoveUtils::is_quiet(move)){
        uint64 from_to = get_from_to(from, to);
        bb->piece_boards[side][piece] ^= from_to;
        bb->collective_piece_boards[side] ^= from_to;         
        if(piece == pKING)
            update_king_location(side, from);
        unsigned int new_castle_right = bi->peek_castle_right();
        update_zobrist_hash_val(move, ep_right, ep_right, castle_right, new_castle_right);
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
        unsigned int new_castle_right = bi->peek_castle_right();
        update_zobrist_hash_val(move, ep_right, ep_right, castle_right, new_castle_right);
    } else if(MoveUtils::is_ep_capture(move)){
        uint64 from_to = get_from_to(from, to);
        bb->piece_boards[side][piece] ^= from_to;
        bb->collective_piece_boards[side] ^= from_to;

        int ep_capture_file = MoveUtils::get_ep_capture_file(move);
        int ep_capture_sq = MoveUtils::get_ep_capture_sq(side, ep_capture_file);

        uint64 sq_bitboard = get_square_bitboard(ep_capture_sq);
        bb->piece_boards[side ^ 1][pPAWN] ^= sq_bitboard;
        bb->collective_piece_boards[side ^ 1] ^= sq_bitboard;

        unsigned int new_ep_right = bi->peek_ep_right();
        update_zobrist_hash_val(move, ep_right, new_ep_right, castle_right, castle_right);
    } else if(MoveUtils::is_castle(move)){ // castle, reverse location for both king and rook
        if(MoveUtils::is_king_castle(move) && side == WHITE){
            bb->piece_boards[side][pKING] ^= get_from_to(e1, g1);
            bb->collective_piece_boards[side] ^= get_from_to(e1, g1); 
            bb->piece_boards[side][pROOK] ^= get_from_to(h1, f1);
            bb->collective_piece_boards[side] ^= get_from_to(h1, f1); 

            update_king_location(side, e1);
        } else if(MoveUtils::is_queen_castle(move) && side == WHITE){
            bb->piece_boards[side][pKING] ^= get_from_to(e1, c1);
            bb->collective_piece_boards[side] ^= get_from_to(e1, c1); 
            bb->piece_boards[side][pROOK] ^= get_from_to(a1, d1);
            bb->collective_piece_boards[side] ^= get_from_to(a1, d1); 

            update_king_location(side, e1);
        } else if(MoveUtils::is_king_castle(move) && side == BLACK){
            bb->piece_boards[side][pKING] ^= get_from_to(e8, g8);
            bb->collective_piece_boards[side] ^= get_from_to(e8, g8); 
            bb->piece_boards[side][pROOK] ^= get_from_to(h8, f8);
            bb->collective_piece_boards[side] ^= get_from_to(h8, f8); 

            update_king_location(side, e8);
        } else if(MoveUtils::is_queen_castle(move) && side == BLACK){
            bb->piece_boards[side][pKING] ^= get_from_to(e8, c8);
            bb->collective_piece_boards[side] ^= get_from_to(e8, c8); 
            bb->piece_boards[side][pROOK] ^= get_from_to(a8, d8);
            bb->collective_piece_boards[side] ^= get_from_to(a8, d8); 

            update_king_location(side, e8);
        }

        unsigned int new_castle_right = bi->peek_castle_right();
        update_zobrist_hash_val(move, ep_right, ep_right, castle_right, new_castle_right);

    } else if(MoveUtils::is_promotion(move)){
        uint64 from_bitboard = get_square_bitboard(from);
        uint64 to_bitboard = get_square_bitboard(to);
        bb->piece_boards[side][pPAWN] ^= from_bitboard;
        if(MoveUtils::is_queen_promotion(move)){
            bb->piece_boards[side][pQUEEN] ^= to_bitboard;
        } else if(MoveUtils::is_knight_promotion(move)){
            bb->piece_boards[side][pKNIGHT] ^= to_bitboard;
        } else if(MoveUtils::is_bishop_promotion(move)){
            bb->piece_boards[side][pBISHOP] ^= to_bitboard;
        } else if(MoveUtils::is_rook_promotion(move)){
            bb->piece_boards[side][pROOK] ^= to_bitboard;
        }
        update_zobrist_hash_val(move, ep_right, ep_right, castle_right, castle_right);
    } else if(MoveUtils::is_capture_promotion(move)){ 
        unsigned int captured_piece = MoveUtils::get_captured_piece(move);
        uint64 from_bitboard = get_square_bitboard(from);
        uint64 to_bitboard = get_square_bitboard(to);
        bb->piece_boards[side][pPAWN] ^= from_bitboard;
        if(MoveUtils::is_queen_promotion(move)){
            bb->piece_boards[side][pQUEEN] ^= to_bitboard;
        } else if(MoveUtils::is_knight_promotion(move)){
            bb->piece_boards[side][pKNIGHT] ^= to_bitboard;
        } else if(MoveUtils::is_bishop_promotion(move)){
            bb->piece_boards[side][pBISHOP] ^= to_bitboard;
        } else if(MoveUtils::is_rook_promotion(move)){
            bb->piece_boards[side][pROOK] ^= to_bitboard;
        }
        bb->piece_boards[side ^ 1][captured_piece] ^= to_bitboard;
        update_zobrist_hash_val(move, ep_right, ep_right, castle_right, castle_right);

    } else if(MoveUtils::is_double_pawn_push(move)){ // quiet move, reverse location of the piece
        uint64 from_to = get_from_to(from, to);
        bb->piece_boards[side][piece] ^= from_to;
        bb->collective_piece_boards[side] ^= from_to; 
        update_zobrist_hash_val(move, ep_right, ep_right, castle_right, castle_right);

    }

    eval->update_material(move, true);
    bb->all = bb->collective_piece_boards[WHITE] | bb->collective_piece_boards[BLACK];
}
bool Board::apply_move_if_legal(unsigned int move)
{
    unsigned int defending_side = MoveUtils::get_side(move);
    apply_move(move);
    int king_location = get_king_location(defending_side);
    if(bb->attacked(defending_side, king_location)){
        reverse_move(move);
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
            } else if(fen_state == EP_TARGET_SQUARE){
                if(!isalpha(ch)) // ep rights will be b3, g6, etc, so just ignore the character if its a number
                    continue;
                initial_ep_rights = ch - 'a';
            } else if(fen_state == POSITION){
                if(isalpha(ch)){
                    pos += 1;
                    uint64 sq = BoardSquares::get_square_bitboard(pos);
                    int side = BLACK ? ch > 'Z' : WHITE;
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
    // init_piece_locations();
    bb->update();
    eval->init_material();
    init_zobrist_map();
    init_zobrist_hash_val();
    // cout<<"finished init material\n";
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
unsigned int Board::get_initial_ep_rights(){
    return initial_ep_rights;
}
unsigned int Board::get_initial_castle_rights(){
    return initial_castle_rights;
}
unsigned int Board::get_zobrist_key(unsigned int side, unsigned int piece, unsigned int sq){
    return tt->get_zobrist_key(side, piece, sq);
}
unsigned long long Board::get_zobrist_val(unsigned int key){
    return tt->get_zobrist_val(key);
}
void Board::init_zobrist_map(){
    tt->init_zobrist_map();
}
void Board::init_zobrist_hash_val(){
    tt->init_zobrist_hash_val();
}
void Board::update_zobrist_hash_val(unsigned int move, unsigned int prev_ep_right, unsigned int next_ep_right, unsigned int prev_castle_right, unsigned int next_castle_right){
    tt->update_zobrist_hash_val(move, prev_ep_right, next_ep_right, prev_castle_right, next_castle_right);
}
map<unsigned int, unsigned long long> Board::get_zobrist_map(){
    return tt->get_zobrist_map();
}
void Board::update_transposition_table(unsigned long long key, unsigned long long val, int depth_left){
    return tt->update_transposition_table(key, val, depth_left);
}
uint64 Board::get_transposition_table_value(unsigned long long key, int depth_left){
    return tt->get_transposition_table_value(key, depth_left);
}
uint64 Board::get_current_hash_val(){
    return tt->get_current_hash_val();
}