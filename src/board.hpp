#ifndef board_h
#define board_h
#include <filesystem>
#include "const.hpp"
#include "board_squares.hpp"
#include "bitboard.hpp"
#include "board_info.hpp"
#include "evaluation.hpp"
#include "transposition_table.hpp"
using namespace BoardSquares;


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
        king_location[WHITE] = e1;
        king_location[BLACK] = e8;
    }
    void apply_move(unsigned int move);
    void reverse_move(unsigned int move);
    bool is_move_legal(unsigned int move);
    bool apply_move_if_legal(unsigned int move);
    void update_castle_rights(unsigned int move);
    void update_en_passant_rights(unsigned int move);
    bool can_castle_kingside(unsigned int side);
    bool can_castle_queenside(unsigned int side);
    void parse_fen(std::filesystem::path path);
    void parse_pgn(std::filesystem::path path);
    unsigned int get_side_to_move();
    void change_side_to_move();
    int get_piece_location(unsigned int side, unsigned int piece);
    int get_king_location(unsigned int side);
    int get_initial_ep_rights();
    int get_initial_castle_rights();
    unsigned int create_move_using_pgn(unsigned int from, unsigned int to, unsigned int promoted_piece = NO_PIECE);
    BoardInfo* get_board_info();
    Bitboard* get_bitboard();
    int king_location[NUM_SIDES];
    TranspositionTable* tt = TranspositionTable::get_instance();
private:
    void update_king_location(int side, int square);
    BoardInfo* bi = BoardInfo::get_instance();
    Bitboard* bb = Bitboard::get_instance();
    Evaluation* eval = Evaluation::get_instance();
    unsigned int side_to_move = WHITE;
    int initial_ep_rights = NO_EP_RIGHTS, initial_castle_rights = 0xf;
};
#endif