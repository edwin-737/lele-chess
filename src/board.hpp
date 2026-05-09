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
#include "transposition_table.hpp"
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
    Board(){
    }
    Board(fs::path fen_path, Bitboard* _bb, BoardInfo* _bi): bb(_bb), bi(_bi), side_to_move(WHITE){
        // Bitboard _bb = Bitboard();
        // BoardInfo _bi = BoardInfo();
        // bb = &_bb;
        // bi = &_bi;
        // bb = new Bitboard();
        // bi = new BoardInfo();
        king_location[WHITE] = e1;
        king_location[BLACK] = e8;
        parse_fen(fen_path);
    }
    void apply_move(unsigned int move);
    void reverse_move(unsigned int move);
    bool is_move_legal(unsigned int move);
    bool apply_move_if_legal(unsigned int move);
    void update_castle_rights(unsigned int move);
    void update_en_passant_rights(unsigned int move);
    bool can_castle_kingside(unsigned int side);
    bool can_castle_queenside(unsigned int side);
    void parse_fen(fs::path path);
    void parse_uci_pgn(fs::path path, int last_move=200, bool verbose=false);
    unsigned int get_side_to_move();
    void change_side_to_move();
    int get_piece_location(unsigned int side, unsigned int piece);
    int get_king_location(unsigned int side);
    int get_initial_ep_rights();
    int get_initial_castle_rights();
    unsigned int create_move_using_pgn(unsigned int from, unsigned int to, unsigned int promoted_piece = NO_PIECE);
    BoardInfo* get_board_info();
    Bitboard* get_bitboard();
    set<int> piece_locations[NUM_SIDES][NUM_PIECE_TYPES];
    int king_location[NUM_SIDES];
    TranspositionTable tt = TranspositionTable();

    bool threefold_draw = false;
    int move_count = 0;
    bool shown_capture_promotion = false;
    bool shown_promotion = false;
private:
    void update_piece_locations(int side, int piece, int from, int to);
    void update_king_location(int side, int square);
    void init_piece_locations();
    BoardInfo* bi;
    Bitboard* bb;
    unsigned int side_to_move = WHITE;
    int initial_ep_rights = NO_EP_RIGHTS, initial_castle_rights = 0x0;
};
#endif