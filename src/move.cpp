#include <iostream>
#include "move.hpp"
#include "const.hpp"
using namespace std;
namespace MoveUtils{
    unsigned int create_move(unsigned int from, unsigned int to, unsigned int side, unsigned int piece, unsigned int captured_piece, unsigned int additional_info, unsigned int ep_capture_file){
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
    bool is_capture(unsigned int move){
        return get_additional_info(move) == CAPTURE;
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
    void display(unsigned int move){
        string side = side_as_string(get_side(move));
        string from = square_as_string(get_from(move));
        string to = square_as_string(get_to(move));
        string piece = piece_as_string(get_piece(move));
        cout<<side<<" "<<piece<<" "<<from<<" "<<to<<" "<<endl;

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
        return from + to;
    }
    string square_as_string(unsigned int square){
        char rank = '1' + (square / 8);
        char file = 'a' + (square % 8);
        string s = {file, rank};
        return s;

    }
};