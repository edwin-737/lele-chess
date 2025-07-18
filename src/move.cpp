#include <iostream>
#include <bitset>
#include "move.hpp"
#include "const.hpp"
#include "board_squares.hpp"
using namespace std;
using namespace BoardSquares;
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
    int get_ep_capture_sq(unsigned int side, unsigned int ep_capture_file){
        return side == WHITE ? a5 + ep_capture_file : a4 + ep_capture_file;
    }
    bool is_quiet(unsigned int move){
        return get_additional_info(move) == QUIET_MOVE;
    }
    bool is_double_pawn_push(unsigned int move){
        return get_additional_info(move) == DOUBLE_PAWN_PUSH;
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

    bool is_capture_promotion(unsigned int move){
        return is_knight_capture_promotion(move) || is_bishop_capture_promotion(move) || is_rook_capture_promotion(move) || is_queen_capture_promotion(move);
    }
    bool is_knight_capture_promotion(unsigned int move){
        return MoveUtils::get_additional_info(move) == KNIGHT_CAPTURE_PROMOTION;
    }
    bool is_bishop_capture_promotion(unsigned int move){
        return MoveUtils::get_additional_info(move) == BISHOP_CAPTURE_PROMOTION;
    }
    bool is_rook_capture_promotion(unsigned int move){
        return MoveUtils::get_additional_info(move) == ROOK_CAPTURE_PROMOTION;
    }
    bool is_queen_capture_promotion(unsigned int move){
        return MoveUtils::get_additional_info(move) == QUEEN_CAPTURE_PROMOTION;
    }
    bool is_promotion(unsigned int move){
        return is_knight_promotion(move) || is_bishop_promotion(move) || is_rook_promotion(move) || is_queen_promotion(move);
    }
    bool is_knight_promotion(unsigned int move){
        return MoveUtils::get_additional_info(move) == KNIGHT_PROMOTION;
    }
    bool is_bishop_promotion(unsigned int move){
        return MoveUtils::get_additional_info(move) == BISHOP_PROMOTION;
    }
    bool is_rook_promotion(unsigned int move){
        return MoveUtils::get_additional_info(move) == ROOK_PROMOTION;
    }
    bool is_queen_promotion(unsigned int move){
        return MoveUtils::get_additional_info(move) == QUEEN_PROMOTION;
    }
    void display(unsigned int move){
        string side = side_as_string(get_side(move));
        string from = square_as_string(get_from(move));
        string to = square_as_string(get_to(move));
        string piece = piece_as_string(get_piece(move));
        string captured_piece = piece_as_string(get_captured_piece(move));
        string additional_info = additional_info_as_string(get_additional_info(move));
        cout<<from<<" "<<to<<" "<<side<<" "<<piece<<" "<<captured_piece<<" "<<additional_info<<endl;
    }
    string additional_info_as_string(unsigned int additional_info){
        if(additional_info == QUIET_MOVE)
            return "quiet";
        else if(additional_info == CAPTURE)
            return "capture";
        else if(additional_info == KING_CASTLE)
            return "kingside castle";
        else if(additional_info == QUEEN_CASTLE)
            return "queenside castle";
        else if(additional_info == EP_CAPTURE)
            return "en passant";
        else if(additional_info == DOUBLE_PAWN_PUSH)
            return "double pawn push";
        else{
            std::string binary = std::bitset<32>(additional_info).to_string();
            binary.erase(0, binary.find_first_not_of('0'));
            return binary;
        }

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
        return "NO_CAPTURED_PIECE";
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