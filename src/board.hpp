#ifndef board_h
#define board_h
#include <stack>
#include <set>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <cctype>
#include "const.hpp"
#include "board_squares.hpp"
#include "move.hpp"
#include "bitboard.hpp"
#include "board_info.hpp"
#include "evaluation.hpp"
using namespace std;
using namespace BoardSquares;
namespace fs = filesystem;


typedef enum fen_state{
    POSITION,
    SIDE_TO_MOVE,
    CASTLE_RIGHTS,
    EP_TARGET_SQUARE,
    FINISHED
} FEN_STATE;

class Board{
public:
    Board(): side_to_move(WHITE){
        // piece_locations[WHITE][pPAWN].insert({a2,b2,c2,d2,e2,f2,g2,h2});
        // piece_locations[WHITE][pKNIGHT].insert({b1, g1});
        // piece_locations[WHITE][pBISHOP].insert({c1, f1});
        // piece_locations[WHITE][pROOK].insert({a1, h1});
        // piece_locations[WHITE][pQUEEN].insert({d1});
        // piece_locations[WHITE][pKING].insert({e1});
        king_location[WHITE] = e1;
        // piece_locations[BLACK][pPAWN].insert({a7,b7,c7,d7,e7,f7,g7,h7});
        // piece_locations[BLACK][pKNIGHT].insert({b8, g8});
        // piece_locations[BLACK][pBISHOP].insert({c8, f8});
        // piece_locations[BLACK][pROOK].insert({a8, h8});
        // piece_locations[BLACK][pQUEEN].insert({d8});
        // piece_locations[BLACK][pKING].insert({e8});
        king_location[BLACK] = e8;
    }
    void apply_move(unsigned int move, int update_num_moves=0);
    void reverse_move(unsigned int move, int update_num_moves=0);
    bool is_move_legal(unsigned int move);
    bool apply_move_if_legal(unsigned int move, int update_num_moves=0);
    void update_castle_rights(unsigned int move);
    void update_en_passant_rights(unsigned int move);
    bool can_castle_kingside(unsigned int side);
    bool can_castle_queenside(unsigned int side);
    void parse_fen(fs::path path);
    void parse_pgn(fs::path path);
    int get_side_to_move();
    void change_side_to_move();
    int get_piece_location(int side, int piece);
    int get_king_location(int side);
    int get_initial_ep_rights();
    int get_initial_castle_rights();
    BoardInfo* get_board_info();
    Bitboard* get_bitboard();
    set<int> piece_locations[NUM_SIDES][NUM_PIECE_TYPES];
    int king_location[NUM_SIDES];
private:
    void update_piece_locations(int side, int piece, int from, int to);
    void update_king_location(int side, int square);
    void init_piece_locations();
    BoardInfo* bi = BoardInfo::get_instance();
    Bitboard* bb = Bitboard::get_instance();
    Evaluation* eval = Evaluation::get_instance();
    int side_to_move;
    int initial_ep_rights = 0, initial_castle_rights = 0;
};
#endif