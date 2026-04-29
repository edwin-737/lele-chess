#include <_ctype.h>
#include <cctype>
#include <iostream>
#include "move.hpp"
#include "board_info.hpp"
#include "const.hpp"
#include "board_squares.hpp"
using namespace std;
using namespace BoardSquares;
namespace MoveUtils{
    unsigned int create_move(unsigned int from, unsigned int to, unsigned int side, unsigned int piece, unsigned int additional_info, unsigned int captured_piece, unsigned int ep_capture_file){
        return (from << 23) | (to << 17) | (side << 16) | (piece << 12) | (captured_piece << 8) | (additional_info << 4) | ep_capture_file;
    }
    unsigned int get_from(unsigned int move){
        return (move >> 23) & 0x3f;
    }
    unsigned int get_to(unsigned int move){
        return (move >> 17) & 0x3f;
    }
    unsigned int get_side(unsigned int move){
        return (move >> 16) & 0x1;
    }
    unsigned int get_piece(unsigned int move){
        return ((move >> 12) & 0xf);
    }
    unsigned int get_captured_piece(unsigned int move){
        return ((move >> 8) & 0xf);
    }
    unsigned int get_additional_info(unsigned int move){
        return (move >> 4) & 0xf;
    }
    unsigned int get_ep_capture_file(unsigned int move){
        return move & 0xf;
    }
    bool is_quiet(unsigned int move){
        return get_additional_info(move) == QUIET_MOVE;
    }
    bool is_capture(unsigned int move){
        return get_additional_info(move) == CAPTURE;
    }
    bool is_double_pawn_push(unsigned int move){
        return get_additional_info(move) == DOUBLE_PAWN_PUSH;
    }
    bool is_ep_capture(unsigned int move){
        return get_additional_info(move) == EP_CAPTURE;
    }
    bool is_castle(unsigned int move){
        return is_king_castle(move) || is_queen_castle(move);
    }
    bool is_king_castle(unsigned int move){
        return get_additional_info(move) == KING_CASTLE;
    }
    bool is_queen_castle(unsigned int move){
        return get_additional_info(move) == QUEEN_CASTLE;
    }
    bool is_promotion(unsigned int move){
        return 
            get_additional_info(move) == QUEEN_PROMOTION || 
            get_additional_info(move) == BISHOP_PROMOTION ||
            get_additional_info(move) == KNIGHT_PROMOTION || 
            get_additional_info(move) == ROOK_PROMOTION;
    }
    bool is_capture_promotion(unsigned int move){
        return
            get_additional_info(move) == QUEEN_CAPTURE_PROMOTION ||
            get_additional_info(move) == BISHOP_CAPTURE_PROMOTION ||
            get_additional_info(move) == KNIGHT_CAPTURE_PROMOTION ||
            get_additional_info(move) == ROOK_CAPTURE_PROMOTION;
    }
    bool is_queen_promotion(unsigned int move){
        return get_additional_info(move) == QUEEN_PROMOTION;
    }
    bool is_knight_promotion(unsigned int move){
        return get_additional_info(move) == KNIGHT_PROMOTION;
    }
    bool is_bishop_promotion(unsigned int move){
        return get_additional_info(move) == BISHOP_PROMOTION;
    }
    bool is_rook_promotion(unsigned int move){
        return get_additional_info(move) == ROOK_PROMOTION;
    }
    bool is_queen_capture_promotion(unsigned int move){
        return get_additional_info(move) == QUEEN_CAPTURE_PROMOTION;
    }
    bool is_knight_capture_promotion(unsigned int move){
        return get_additional_info(move) == KNIGHT_CAPTURE_PROMOTION;
    }
    bool is_bishop_capture_promotion(unsigned int move){
        return get_additional_info(move) == BISHOP_CAPTURE_PROMOTION;
    }
    bool is_rook_capture_promotion(unsigned int move){
        return get_additional_info(move) == ROOK_CAPTURE_PROMOTION;
    }
    bool is_final_rank(unsigned int square){
        return (get_rank_bitboard(7) & get_square_bitboard(square)) || (get_rank_bitboard(0) & get_square_bitboard(square));
    }
    void display(unsigned int move){
        string side = side_as_string(get_side(move));
        string from = square_as_string(get_from(move));
        string to = square_as_string(get_to(move));
        string piece = piece_as_string(get_piece(move));
        // if(is_promotion(move) || is_capture_promotion(move)){}
        cout<<side<<" "<<piece<<" "<<from<<" "<<to<<" "<<endl;

    }

    unsigned int square_as_uint(string square){
        if(square.length() != 2){
            return 0;
        }
        unsigned int file = square[0] - 'a';
        unsigned int rank = square[1] - '0' - 1;
        return rank * 8 + file;
    }

    unsigned int promoted_piece_as_uint(string promoted_piece){
        if(promoted_piece == "k"){
            return pQUEEN;
        } else if(promoted_piece == "b"){
            return pBISHOP;
        } else if(promoted_piece == "r"){
            return pBISHOP;
        } else if(promoted_piece == "q"){
            return pKNIGHT;
        }
        return 0;
    }
    string side_as_string(unsigned int side){
        if(side == WHITE){
            return "white";
        } else {
            return "black";
        }
    }
    string piece_as_string(unsigned int piece){
        if(piece == pPAWN)
            return "pawn";
        else if(piece == pKNIGHT)
            return "knight";
        else if(piece == pBISHOP)
            return "bishop";
        else if(piece == pROOK)
            return "rook";
        else if(piece == pQUEEN)
            return "queen";
        else if(piece == pKING)
            return "king";
        return "INVALID_PIECE";
    }

    string move_as_string(unsigned int move){
        string from = square_as_string(get_from(move));
        string to = square_as_string(get_to(move));
        string move_as_string = from + to;
        if(is_promotion(move)){
            if(is_knight_promotion(move))
                move_as_string += "n";
            else if(is_bishop_promotion(move))
                move_as_string += "b";
            else if(is_rook_promotion(move))
                move_as_string += "r";
            else if(is_queen_promotion(move))
                move_as_string += "q";
        } else if(is_capture_promotion(move)){
            if(is_knight_capture_promotion(move))
                move_as_string += "n";
            else if(is_bishop_capture_promotion(move))
                move_as_string += "b";
            else if(is_rook_capture_promotion(move))
                move_as_string += "r";
            else if(is_queen_capture_promotion(move))
                move_as_string += "q";

        }
        return move_as_string;
    }
    string square_as_string(unsigned int square){
        char rank = '1' + (square / 8);
        char file = 'a' + (square % 8);
        string s = {file, rank};
        return s;

    }
    unsigned int get_piece(string move){
        if(move.length() == 2){
            return pPAWN;
        } else if(move[0] == 'N'){
            return pKNIGHT;
        } else if(move[0] == 'B'){
            return pBISHOP;
        } else if(move[0] == 'R'){
            return pROOK;
        } else if(move[0] == 'Q'){
            return pQUEEN;
        } else if(move[0] == 'K'){
            return pKING;
        } else if(islower(move[0])){
            return pPAWN;
        } else if(move[0] == '0'){
            return pKING;
        }
        return NO_PIECE;
    }
    unsigned int get_to(string move){
        unsigned long len = move.length();
        if(len == 2){
            return get_square(move);
        }
        string first_two = move.substr(0, 2);
        string last_two = move.substr(len - 2);
        string last_char = move.substr(len - 1);
        string third_fourth = move.substr(2, 2);
        if(is_capture(move) && is_check(move)){
            if(has_from_file(move) || has_from_rank(move)){
                string third_fourth = move.substr(2, 2);
                return get_square(third_fourth);
            } else {
                string second_third = move.substr(1, 2);
                return get_square(second_third);
            }
        }
        if(is_capture(move)){
            if(has_from_file(move) || has_from_rank(move)){
                string fourth_fifth = move.substr(3, 2);
                return get_square(fourth_fifth);
            } else {
                return get_square(third_fourth);
            }
        } else if(is_check(move)){
            if(has_from_file(move) || has_from_rank(move)){
                string third_fourth = move.substr(2, 2);
                return get_square(third_fourth);
            } else {
                string second_third = move.substr(1, 2);
                return get_square(second_third);
            }
        }
        else if(last_char == "O"){
            return INVALID_LOCATION;
        } else if(is_capture_promotion(move)){
            return get_square(third_fourth);
        } else if(is_promotion(move)) {
            return get_square(first_two);
        }
        return get_square(last_two);
    }
    bool is_pawn_move(string move){
        if(move.length() == 2){
            return true;
        } 
        return false;
    }
    bool has_from_file(string move){
        unsigned long len = move.length();
        if(len >= 4 && 
            (isalpha(move[1]) && move[1] >= 'a' && move[1] <= 'h') && 
            (isalpha(move[2]) && move[2] >= 'a' && move[2] <= 'h')
        )
            return true;
        return false;
    }
    bool has_from_rank(string move){
        unsigned long len = move.length();
        if(len >= 4 && 
            (isalpha(move[1]) && move[1] >= '1' && move[1] <= '8') && 
            (isalpha(move[2]) && move[2] >= 'a' && move[2] <= 'h')
        )
            return true;
        return false;
    }
    bool is_capture(string move){
        if(move.find('x') != string::npos){
            return true;
        }
        return false;
    }
    bool is_check(string move){
        if(move.find('+') != string::npos){
            return true;
        }
        return false;
    }
    bool is_castle(string move){
        return move[0] == 'O';
    }
    bool is_promotion(string move){
        if(move.find('=') != string::npos){
            return true;
        }
        return false;
    }
    bool is_capture_promotion(string move){
        if(is_capture(move) && is_promotion(move))
            return true;
        return false;
    }
    bool is_potentially_double_pawn_push(unsigned int side, string move){
        unsigned long len = move.length();
        if(len == 2){
            if(move[1] == '4' && side == WHITE)
                return true;
            else if(move[1] == '5' && side == BLACK)
                return true;
        }
        return false;
    }
    unsigned int get_pawn_from_file(string move){
        return move[0] - 'a';
    }
    unsigned int get_from_file(string move){
        const char from_file = move[1];
        return from_file - 'a';
    }
    unsigned int get_from_rank(string move){
        const char from_rank = move[1];
        return from_rank - 'a';
    }
    unsigned int get_castle_move(unsigned int side, string move){
        unsigned long len = move.length();
        if(side == WHITE){
            if(len == 3)
                return MoveUtils::create_move(e1, g1, side, pKING, KING_CASTLE);
            else
                return MoveUtils::create_move(e1, c1, side, pKING, QUEEN_CASTLE);
        } else {
            if(len == 3)
                return MoveUtils::create_move(e8, g8, side, pKING, KING_CASTLE);
            else
                return MoveUtils::create_move(e8, c8, side, pKING, QUEEN_CASTLE);
        }
    }
    unsigned int get_promoted_piece(string move){
        unsigned long len = move.length();
        char promoted_piece_char = move[len - 1];
        if(move[len - 1] == '+'){ // check so use the second to last character
            promoted_piece_char = move[len - 2];
        }
        if(promoted_piece_char == 'N')
            return pKNIGHT;
        else if(promoted_piece_char == 'B')
            return pBISHOP;
        else if(promoted_piece_char == 'R')
            return pROOK;
        else if(promoted_piece_char == 'Q')
            return pQUEEN;
        return NO_PIECE;
    }
    unsigned int get_square(string square){
        const char file = square[0];
        const char rank = square[1];
        int file_num = file - 'a';
        int rank_num = rank - '1';
        return rank_num * 8 + file_num;
    }
};