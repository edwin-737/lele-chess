#include <exception>
#include <iostream>
#include <string>
#include "board.hpp"
#include "board_squares.hpp"
#include "const.hpp"
#include "move.hpp"
#include "move_set.hpp"
#include "pesto.hpp"
#include "transposition_table.hpp"
#include "utils.hpp"
using namespace BoardSquares;
void Board::apply_move(unsigned int move){
    unsigned int side = MoveUtils::get_side(move);
    unsigned int piece = MoveUtils::get_piece(move);
    unsigned int from = MoveUtils::get_from(move);
    unsigned int to = MoveUtils::get_to(move);        

    unsigned int castle_rights = bi->peek_castle_right();
    unsigned int ep_rights = bi->peek_ep_right();

    // for zobrist hash val update
    unsigned int prev_castle_rights = bi->peek_castle_right();
    unsigned int prev_ep_rights = bi->peek_ep_right();
    tt->update_hash_val_piece_square(move);
    tt->update_hash_val_side_to_move(move);
    if(MoveUtils::is_quiet(move)){
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
                    castle_rights &= (0b1011);
                } else if(from == h1){
                    castle_rights &= (0b0111);
                } 
            } else{
                if(from == a8){
                    castle_rights &= (0b1110);
                } else if(from == h8 ){
                    castle_rights &= (0b1101);
                }
            } 
            // cout<<"move is quiet rook move\n";
            bi->add_board_info(castle_rights, NO_EP_RIGHTS);
        } else {
            bi->add_board_info(castle_rights, NO_EP_RIGHTS);
        }
    } else if(MoveUtils::is_castle(move)){
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
        } else if(piece == pROOK){
            if(side == WHITE){
                if(from == a1){
                    castle_rights &= (0b1011);
                } else if(from == h1){
                    castle_rights &= (0b0111);
                } 
            } else{
                if(from == a8){
                    castle_rights &= (0b1110);
                } else if(from == h8 ){
                    castle_rights &= (0b1101);
                }
            } 
            // cout<<"move is quiet rook move\n";
            bi->add_board_info(castle_rights, NO_EP_RIGHTS);
        } else {
            bi->add_board_info(castle_rights, NO_EP_RIGHTS);
        }
    } else if(MoveUtils::is_ep_capture(move)){
        uint64 from_to = get_from_to(from, to);
        bb->piece_boards[side][piece] ^= from_to;
        bb->collective_piece_boards[side] ^= from_to;
        int captured_file = MoveUtils::get_ep_capture_file(move);
        int captured_sq =  side == WHITE ? a5 + captured_file : a4 + captured_file;

        uint64 sq_bitboard = get_square_bitboard(captured_sq);
        bb->piece_boards[side ^ 1][pPAWN] ^= sq_bitboard;
        bb->collective_piece_boards[side ^ 1] ^= sq_bitboard;

        bi->add_board_info(castle_rights, NO_EP_RIGHTS);
    } else if(MoveUtils::is_double_pawn_push(move)){
        uint64 from_to = get_from_to(from, to);
        bb->piece_boards[side][piece] ^= from_to;
        bb->collective_piece_boards[side] ^= from_to;

        int ep_rights = bi->peek_ep_right();
        if(side == WHITE){
            ep_rights = to - a4;
        } else {
            ep_rights = to - a5;
        }
        bi->add_board_info(castle_rights, ep_rights);
    } else if(MoveUtils::is_promotion(move)){
        uint64 from_to = get_from_to(from, to);
        uint64 from_bitboard = get_square_bitboard(from);
        uint64 to_bitboard = get_square_bitboard(to);
        bb->piece_boards[side][piece] ^= from_bitboard;
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
        bi->add_board_info(castle_rights, NO_EP_RIGHTS);
    } else if(MoveUtils::is_capture_promotion(move)){

        uint64 from_to = get_from_to(from, to);
        uint64 from_bitboard = get_square_bitboard(from);
        uint64 to_bitboard = get_square_bitboard(to);
        bb->piece_boards[side][piece] ^= from_bitboard;
        bb->collective_piece_boards[side] ^= from_to;

        unsigned int captured_piece = MoveUtils::get_captured_piece(move);
        bb->piece_boards[side ^ 1][captured_piece] ^= to_bitboard;
        bb->collective_piece_boards[side ^ 1] ^= to_bitboard;

        if(MoveUtils::is_knight_promotion(move)){
            bb->piece_boards[side][pKNIGHT] ^= to_bitboard;
        } else if(MoveUtils::is_bishop_promotion(move)){
            bb->piece_boards[side][pBISHOP] ^= to_bitboard;
        } else if(MoveUtils::is_rook_promotion(move)){
            bb->piece_boards[side][pROOK] ^= to_bitboard;
        } else if(MoveUtils::is_queen_promotion(move)){
            bb->piece_boards[side][pQUEEN] ^= to_bitboard;
        }
        bi->add_board_info(castle_rights, NO_EP_RIGHTS);
    }
    bb->all = bb->collective_piece_boards[WHITE] | bb->collective_piece_boards[BLACK];

    unsigned int next_castle_rights = bi->peek_castle_right();
    unsigned int next_ep_rights = bi->peek_ep_right();
    tt->update_hash_val_castle_rights(prev_castle_rights, next_castle_rights);
    tt->update_hash_val_ep_rights(prev_ep_rights, next_ep_rights);
}

void Board::reverse_move(unsigned int move){
    unsigned int side = MoveUtils::get_side(move);
    unsigned int piece = MoveUtils::get_piece(move);
    unsigned int from = MoveUtils::get_from(move);
    unsigned int to = MoveUtils::get_to(move);

    // for zobrist hash val update
    unsigned int prev_castle_rights = bi->peek_castle_right();
    unsigned int prev_ep_rights = bi->peek_ep_right();
    tt->update_hash_val_piece_square(move);
    tt->update_hash_val_side_to_move(move);
    if(MoveUtils::is_quiet(move)){
        uint64 from_to = get_from_to(from, to);
        bb->piece_boards[side][piece] ^= from_to;
        bb->collective_piece_boards[side] ^= from_to; 

        int additional_info = MoveUtils::get_additional_info(move);
        if(piece == pKING)
            update_king_location(side, from);
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
    } else if(MoveUtils::is_double_pawn_push(move)){
        uint64 from_to = get_from_to(from, to);
        bb->piece_boards[side][piece] ^= from_to;
        bb->collective_piece_boards[side] ^= from_to; 
    } else if(MoveUtils::is_promotion(move)){
        uint64 from_to = get_from_to(from, to);
        uint64 from_bitboard = get_square_bitboard(from);
        uint64 to_bitboard = get_square_bitboard(to);
        bb->piece_boards[side][piece] ^= from_bitboard;
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
    } else if(MoveUtils::is_capture_promotion(move)){

        uint64 from_to = get_from_to(from, to);
        uint64 from_bitboard = get_square_bitboard(from);
        uint64 to_bitboard = get_square_bitboard(to);
        bb->piece_boards[side][piece] ^= from_bitboard;
        bb->collective_piece_boards[side] ^= from_to;

        unsigned int captured_piece = MoveUtils::get_captured_piece(move);
        bb->piece_boards[side ^ 1][captured_piece] ^= to_bitboard;
        bb->collective_piece_boards[side ^ 1] ^= to_bitboard;

        if(MoveUtils::is_knight_promotion(move)){
            bb->piece_boards[side][pKNIGHT] ^= to_bitboard;
        } else if(MoveUtils::is_bishop_promotion(move)){
            bb->piece_boards[side][pBISHOP] ^= to_bitboard;
        } else if(MoveUtils::is_rook_promotion(move)){
            bb->piece_boards[side][pROOK] ^= to_bitboard;
        } else if(MoveUtils::is_queen_promotion(move)){
            bb->piece_boards[side][pQUEEN] ^= to_bitboard;
        }
    }

    bb->all = bb->collective_piece_boards[WHITE] | bb->collective_piece_boards[BLACK];
    bi->remove_board_info();

    unsigned int next_castle_rights = bi->peek_castle_right();
    unsigned int next_ep_rights = bi->peek_ep_right();
    tt->update_hash_val_castle_rights(prev_castle_rights, next_castle_rights);
    tt->update_hash_val_ep_rights(prev_ep_rights, next_ep_rights);
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
int Board::get_piece_location(unsigned int side, unsigned int piece){
    auto beginning = piece_locations[side][piece].begin();
    if(beginning != piece_locations[side][piece].end())
        return *beginning;
    return -1;

}
int Board::get_king_location(unsigned int side){
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
    unsigned int piece = bb->get_piece_on_square(side, from);
    unsigned int captured_piece =  bb->get_piece_on_square(side ^ 1, to);
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
            if(bb->collective_piece_boards[side ^ 1] & to_bb){
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
                        uint64 square_bitboard = BoardSquares::get_square_bitboard(pos);
                        int side;
                        if(ch > 'Z'){
                            side = BLACK;
                        } else {
                            side = WHITE;
                        }
                        char piece_type = tolower(ch);
                        if(piece_type == 'p'){
                            bb->piece_boards[side][pPAWN] |= square_bitboard;
                        } else if(piece_type == 'n'){
                            bb->piece_boards[side][pKNIGHT] |= square_bitboard;
                        } else if(piece_type == 'b'){
                            bb->piece_boards[side][pBISHOP] |= square_bitboard;
                        } else if(piece_type == 'q'){
                            bb->piece_boards[side][pQUEEN] |= square_bitboard;
                        } else if(piece_type == 'k'){
                            bb->piece_boards[side][pKING] |= square_bitboard;
                            update_king_location(side, pos);
                        } else if(piece_type =='r'){
                            bb->piece_boards[side][pROOK] |= square_bitboard;
                        } 
                    } else if(isnumber(ch)){

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
    init_piece_locations();
    tt->initialise_hash_val(side_to_move, bb, bi);
}
void Board::parse_pgn(fs::path path){

    bi->set_board_info(initial_castle_rights, initial_ep_rights);
    bb->update();

    std::ifstream file(path);
    if(file.is_open()){
        std::string str;
        unsigned int pgn_state = PGN_STATE::NUMBER;
        while(file >> str){
            // cout<<"side_to_move: "<<side_to_move<<endl;
            if(pgn_state == PGN_STATE::NUMBER){
                cout<<"string: "<<str<<endl;
                pgn_state = PGN_STATE::MOVE;
            } else if(pgn_state == PGN_STATE::MOVE) {
                unsigned int move = 0;
                unsigned int additional_info = QUIET_MOVE;
                unsigned int piece = MoveUtils::get_piece(str);
                unsigned int to = MoveUtils::get_to(str);
                unsigned int from_rank = INVALID_LOCATION, from_file = INVALID_LOCATION;
                // cout<<"piece: "<<MoveUtils::piece_as_string(piece)<<"\n";
                bool has_from_rank = MoveUtils::has_from_rank(str);
                bool has_from_file = MoveUtils::has_from_file(str);
                if(has_from_rank)
                    from_rank = MoveUtils::get_from_rank(str);
                else if(has_from_file)
                    from_file = MoveUtils::get_from_file(str);
                bool is_castle = MoveUtils::is_castle(str);
                bool is_capture = MoveUtils::is_capture(str);
                if(is_castle){
                    move = MoveUtils::get_castle_move(side_to_move, str);
                } else if(is_capture){
                    if(piece == pPAWN){
                        unsigned int captured_piece = bb->get_piece_on_square(side_to_move ^ 1, to);
                        if(bb->get_piece_on_square(side_to_move ^ 1, to) == INVALID_LOCATION){
                            additional_info = EP_CAPTURE;
                            unsigned int pawn_from_file = MoveUtils::get_pawn_from_file(str);
                            unsigned int pawn_from = 32 + pawn_from_file;
                            if(side_to_move == BLACK)
                                pawn_from = 24 + pawn_from_file;
                            move = MoveUtils::create_move(pawn_from, to, side_to_move, pPAWN, additional_info, pPAWN);
                        } else if(MoveUtils::is_capture_promotion(str)){
                            unsigned int promoted_piece = MoveUtils::get_promoted_piece(str);
                            if(promoted_piece == pQUEEN)
                                additional_info = QUEEN_CAPTURE_PROMOTION;
                            else if(promoted_piece == pKNIGHT)
                                additional_info = KNIGHT_CAPTURE_PROMOTION;
                            else if(promoted_piece == pBISHOP)
                                additional_info = BISHOP_CAPTURE_PROMOTION;
                            else if(promoted_piece == pROOK)
                                additional_info = ROOK_CAPTURE_PROMOTION;
                            unsigned int pawn_from_file = MoveUtils::get_pawn_from_file(str);
                            unsigned int pawn_from = 48 + pawn_from_file;
                            unsigned int piece_on_square = bb->get_piece_on_square(side_to_move, pawn_from);
                            if(piece_on_square == INVALID_LOCATION){
                                bb->display();
                                throw std::runtime_error("[parse_pgn] CAPTURE_PROMOTION, INVALID_PIECE, no pawn on from");
                            }
                            move = MoveUtils::create_move(pawn_from, to, side_to_move, pPAWN, additional_info, captured_piece);
                        } else {
                            additional_info = CAPTURE;
                            unsigned int pawn_from_file = MoveUtils::get_pawn_from_file(str);
                            uint64 file_bb = get_file_bitboard(pawn_from_file);
                            uint64 pawn_bb = bb->piece_boards[side_to_move][piece] & file_bb;
                            if(pawn_bb){
                                int from = bit_scan_forward(pawn_bb);
                                if(from != -1)
                                    move = MoveUtils::create_move(from, to, side_to_move, piece, additional_info, captured_piece);
                            } else {
                                bb->display();
                                throw std::runtime_error("[parse_pgn] CAPTURE, invalid pawn, no piece on from");
                            }
                        }
                    } else { // other piece capture
                        additional_info = CAPTURE;
                        unsigned int captured_piece = bb->get_piece_on_square(side_to_move ^ 1, to);
                        if(from_rank != INVALID_LOCATION){
                            uint64 rank_bb = get_rank_bitboard(from_rank);
                            uint64 piece_bb = bb->piece_boards[side_to_move][piece] & rank_bb;
                            if(piece_bb){
                                int from = bit_scan_forward(piece_bb);
                                if(from != -1)
                                    move = MoveUtils::create_move(from, to, side_to_move, piece, additional_info, captured_piece);
                            } else {
                                bb->display();
                                throw std::runtime_error("[parse_pgn] CAPTURE, invalid piece, no piece on from_rank");
                            }
                        } else if(from_file != INVALID_LOCATION){
                            uint64 file_bb = get_file_bitboard(from_file);
                            uint64 piece_bb = bb->piece_boards[side_to_move][piece] & file_bb;
                            if(piece_bb){
                                int from = bit_scan_forward(piece_bb);
                                if(from != -1)
                                    move = MoveUtils::create_move(from, to, side_to_move, piece, additional_info, captured_piece);
                            } else {
                                bb->display();
                                throw std::runtime_error("[parse_pgn] CAPTURE, invalid piece, no piece on from_file");
                            }
                        } else {
                            // cout<<MoveUtils::piece_as_string(piece)<<"\n";
                            // cout<<MoveUtils::square_as_string(to)<<"\n";
                            uint64 move_set = MoveSet::get_all_attack_mask(piece, to, side_to_move);
                            // bool set = false;
                            // if((bb->piece_boards[side_to_move ^ 1][piece] & get_square_bitboard(to)) == 0){
                            //     bb->piece_boards[side_to_move ^ 1][piece] ^= get_square_bitboard(to);
                            //     set = true;
                            // }
                            // uint64 move_set = MoveSet::get_capture_move_set(bb, piece, to, side_to_move ^ 1);
                            // if(set){
                            //     bb->piece_boards[side_to_move ^ 1][piece] ^= get_square_bitboard(to);
                            //     set = false;
                            // }
                            // cout<<"move_set\n";
                            // bb->display_bitboard(move_set);
                            uint64 piece_bb = bb->piece_boards[side_to_move][piece] & move_set;
                            // cout<<"piece_board\n";
                            // bb->display_bitboard(bb->piece_boards[side_to_move][piece]);
                            if(piece_bb){
                                int from = bit_scan_forward(piece_bb);
                                if(from != -1)
                                    move = MoveUtils::create_move(from, to, side_to_move, piece, additional_info, captured_piece);
                            } else {
                                bb->display();
                                throw std::runtime_error("[parse_pgn] CAPTURE, invalid piece, no piece can get to to");
                            }
                        }
                    }
                } else {
                    additional_info = QUIET_MOVE;
                    if(piece == pPAWN){
                        
                        unsigned int pawn_from_file = MoveUtils::get_pawn_from_file(str);
                        uint64 file_bb = get_file_bitboard(pawn_from_file);
                        uint64 pawn_bb = bb->piece_boards[side_to_move][piece] & file_bb;
                        // cout<<"pawn_from_file: "<<pawn_from_file<<"\n";
                        // cout<<"file_bb: "<<"\n";
                        // bb->display_bitboard(file_bb);
                        // cout<<"pawn_bb: "<<"\n";
                        // bb->display_bitboard(pawn_bb);
                        if(pawn_bb){
                            unsigned int from = bit_scan_forward(pawn_bb);
                            if((side_to_move == WHITE && to - from == 16) || (side_to_move == BLACK && from - to == 16)){
                                additional_info = DOUBLE_PAWN_PUSH;
                            }
                            if(from != -1)
                                move = MoveUtils::create_move(from, to, side_to_move, piece, additional_info);
                        } else {
                            bb->display();
                            throw std::runtime_error("[parse_pgn] QUIET, invalid pawn, no pawn on from_file");
                        }
                    } else {
                        if(from_rank != INVALID_LOCATION){
                            uint64 rank_bb = get_rank_bitboard(from_rank);
                            uint64 piece_bb = bb->piece_boards[side_to_move][piece] & rank_bb;
                            if(piece_bb){
                                int from = bit_scan_forward(piece_bb);
                                if(from != -1)
                                    move = MoveUtils::create_move(from, to, side_to_move, piece, additional_info);
                            } else {
                                bb->display();
                                throw std::runtime_error("[parse_pgn] QUIET, invalid piece, no piece on from_rank");
                            }
                        } else if(from_file != INVALID_LOCATION){
                            uint64 file_bb = get_file_bitboard(from_file);
                            uint64 piece_bb = bb->piece_boards[side_to_move][piece] & file_bb;
                            if(piece_bb){
                                int from = bit_scan_forward(piece_bb);
                                if(from != -1)
                                    move = MoveUtils::create_move(from, to, side_to_move, piece, additional_info);
                            } else {
                                cout<<str<<endl;
                                bb->display();
                                throw std::runtime_error("[parse_pgn] QUIET, invalid piece, no piece on from_file");
                            }
                        } else {
                            uint64 move_set =  MoveSet::get_all_attack_mask(piece, to, side_to_move);
                            uint64 piece_bb = bb->piece_boards[side_to_move][piece] & move_set;

                            if(move_set){
                                int from = bit_scan_forward(piece_bb);
                                if(from != -1)
                                    move = MoveUtils::create_move(from, to, side_to_move, piece, additional_info);
                                if(move == 0){
                                    cout<<"piece\n";
                                    cout<<MoveUtils::piece_as_string(piece)<<"\n";
                                    cout<<"to\n";
                                    cout<<MoveUtils::square_as_string(to)<<"\n";
                                    cout<<"move_set\n";
                                    bb->display_bitboard(move_set);
                                    cout<<"bb->piece_boards[side_to_move][piece\n";
                                    bb->display_bitboard(bb->piece_boards[side_to_move][piece]);
                                }
                            } else {
                                bb->display();
                                throw std::runtime_error("[parse_pgn] QUIET, invalid piece, no piece can get to to");
                            }
                        }
                    }
                }

                if(side_to_move == BLACK){
                    pgn_state = PGN_STATE::NUMBER;
                }
                MoveUtils::display(move);
                if(!apply_move_if_legal(move) || move == 0){
                    bb->display();
                    throw std::runtime_error("[parse_pgn] apply_move_if_legal, move not legal");
                };
                change_side_to_move();
            }
        }
    }
}
void Board::parse_uci_pgn(fs::path path){

    bi->set_board_info(initial_castle_rights, initial_ep_rights);
    bb->update();

    std::ifstream file(path);
    if(file.is_open()){
        std::string move_string;
        while(file >> move_string){
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
            if(side_to_move == WHITE)
                cout<<move_count<<"\n";
            MoveUtils::display(move);
            if(!apply_move_if_legal(move) || move == 0){
                bb->display();
                throw std::runtime_error("[parse_pgn] apply_move_if_legal, move not legal");
            };
            tt->increment_value_threefold();
            unsigned int value_threefold = tt->get_value_threefold();
            cout<<"get_value_threefold(): "<<value_threefold<<"\n";
            if(value_threefold == 3){  
                threefold_draw = true;
                return;
            } 
            change_side_to_move();
        }
    }
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
TranspositionTable* Board::get_transposition_table(){
    return tt;
}
int Board::get_initial_ep_rights(){
    return initial_ep_rights;
}
int Board::get_initial_castle_rights(){
    return initial_castle_rights;
}