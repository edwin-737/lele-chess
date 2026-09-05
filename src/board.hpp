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
        for(unsigned int sq = 0 ; sq < NUM_SQUARES ; sq ++){
            castle_rights_rook_mask[WHITE][sq] = 0b1111;
            castle_rights_rook_mask[BLACK][sq] = 0b1111;
        }

        castle_rights_rook_mask[WHITE][a1] = 0b1011;
        castle_rights_rook_mask[WHITE][h1] = 0b0111;
        castle_rights_rook_mask[BLACK][a8] = 0b1110;
        castle_rights_rook_mask[BLACK][h8] = 0b1101;

        castle_rights_king_mask[WHITE] = 0x3;
        castle_rights_king_mask[BLACK] = 0xc;
        
        castle_king_from[WHITE][KING_CASTLE] = e1;
        castle_king_from[BLACK][KING_CASTLE] = e8;
        castle_king_from[WHITE][QUEEN_CASTLE] = e1;
        castle_king_from[BLACK][QUEEN_CASTLE] = e8;
        castle_king_to[WHITE][KING_CASTLE] = g1;
        castle_king_to[BLACK][KING_CASTLE] = g8;
        castle_king_to[WHITE][QUEEN_CASTLE] = c1;
        castle_king_to[BLACK][QUEEN_CASTLE] = c8;
        
        castle_rook_from[WHITE][KING_CASTLE] = h1;
        castle_rook_from[BLACK][KING_CASTLE] = h8;
        castle_rook_from[WHITE][QUEEN_CASTLE] = a1;
        castle_rook_from[BLACK][QUEEN_CASTLE] = a8;
        castle_rook_to[WHITE][KING_CASTLE] = f1;
        castle_rook_to[BLACK][KING_CASTLE] = f8;
        castle_rook_to[WHITE][QUEEN_CASTLE] = d1;
        castle_rook_to[BLACK][QUEEN_CASTLE] = d8;

        king_from_to_bb[WHITE][KING_CASTLE] = get_from_to(e1, g1);
        king_from_to_bb[BLACK][KING_CASTLE] = get_from_to(e8, g8);
        king_from_to_bb[WHITE][QUEEN_CASTLE] = get_from_to(e1, c1);
        king_from_to_bb[BLACK][QUEEN_CASTLE] = get_from_to(e8, c8);

        rook_from_to_bb[WHITE][KING_CASTLE] = get_from_to(h1, f1);
        rook_from_to_bb[BLACK][KING_CASTLE] = get_from_to(h8, f8);
        rook_from_to_bb[WHITE][QUEEN_CASTLE] = get_from_to(a1, d1);
        rook_from_to_bb[BLACK][QUEEN_CASTLE] = get_from_to(a8, d8);

        parse_fen(fen_path);
    }
    bool reverse_move_if_king_attacked(unsigned int move);
    bool apply_promotion_move(
        unsigned int move,
        unsigned int from, 
        unsigned int to,
        unsigned int side,
        unsigned int promoted_piece, 
        unsigned int king_location, 
        uint64 piece_square_hash_val
    );
    bool apply_capture_promotion_move(
        unsigned int move, 
        unsigned int from, 
        unsigned int to,
        unsigned int side,
        unsigned int promoted_piece, 
        unsigned int captured_piece,
        unsigned int king_location, 
        uint64 piece_square_hash_val
    );
    bool apply_move(unsigned int move);
    void reverse_move(unsigned int move);
    bool is_move_legal(unsigned int move);
    bool apply_move_if_legal(unsigned int move);
    void update_castle_rights(unsigned int move);
    void update_en_passant_rights(unsigned int move);
    bool can_castle_kingside(unsigned int side);
    bool can_castle_queenside(unsigned int side);
    void parse_fen(fs::path path);
    unsigned int parse_single_move(string move_string, bool verbose=false);
    void parse_uci_pgn(fs::path path, int last_move=200, bool verbose=false);
    unsigned int get_side_to_move();
    void change_side_to_move();
    int get_piece_location(unsigned int side, unsigned int piece);
    unsigned int get_king_location(unsigned int side);
    int get_initial_ep_rights();
    int get_initial_castle_rights();
    unsigned int create_move_using_pgn(unsigned int from, unsigned int to, unsigned int promoted_piece = NO_PIECE);
    BoardInfo* get_board_info();
    Bitboard* get_bitboard();
    set<int> piece_locations[NUM_SIDES][NUM_PIECE_TYPES];
    unsigned int king_location[NUM_SIDES];

    unsigned int castle_rights_rook_mask[NUM_SIDES][NUM_SQUARES];
    unsigned int castle_rights_king_mask[NUM_SIDES];
    unsigned int castle_king_from[NUM_SIDES][NUM_MOVE_TYPES];
    unsigned int castle_king_to[NUM_SIDES][NUM_MOVE_TYPES];
    unsigned int castle_rook_from[NUM_SIDES][NUM_MOVE_TYPES];
    unsigned int castle_rook_to[NUM_SIDES][NUM_MOVE_TYPES];
    uint64 king_from_to_bb[NUM_SIDES][NUM_MOVE_TYPES];
    uint64 rook_from_to_bb[NUM_SIDES][NUM_MOVE_TYPES];
    TranspositionTable tt = TranspositionTable();

    bool threefold_draw = false;
    int move_count = 0;
    bool shown_capture_promotion = false;
    bool shown_promotion = false;
private:
    void update_piece_locations(int side, int piece, int from, int to);
    void update_king_location(unsigned int side, unsigned int square);
    void init_piece_locations();
    BoardInfo* bi;
    Bitboard* bb;
    unsigned int side_to_move = WHITE;
    int initial_ep_rights = NO_EP_RIGHTS, initial_castle_rights = 0x0;
};
#endif