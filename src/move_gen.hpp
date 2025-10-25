#ifndef movegen_h
#define movegen_h
#include <stack>
#include <map>
#include "const.hpp"
#include "move.hpp"
#include "bitboard.hpp"
#include "board_info.hpp"
#include "board_squares.hpp"
#include "magics.hpp"
using namespace std;
using namespace BoardSquares;
class MoveGen{
    /**
     * Move generation class which keeps a stack of moves being explored during search
     */
public:
    MoveGen():move_type(QUIET_MOVE),side(WHITE),piece(pPAWN),from(0),to(0),ep_from(EP_START){
    }
    MoveGen(
        int _side, int _move_type=mQUIET, bool _only_captures=false): side(_side), move_type(_move_type), gen_type(ALL_MOVES), only_captures(_only_captures), piece(pPAWN), from(0), to(0), ep_from(EP_START){
        }
    void set_gen_type(int _gen_type);
    void set_move_type(int _move_type);
    int get_special_move_type();
    unsigned int get_move();
    unsigned int get_special_move();
    unsigned int get_capture();
private:
    bool initialise_piece();
    bool update_piece();
    bool update_from();
    bool update_to();
    bool can_castle_kingside(int side);
    bool can_castle_queenside(int side);
    unsigned int get_ep_capture(int side);
    Bitboard* bb = Bitboard::get_instance();
    BoardInfo* bi = BoardInfo::get_instance();
    unsigned int move_type = mQUIET, promoted_piece = pKNIGHT, promotion_counter = 0, gen_type, piece, from, to;
    int max_special_moves[7] = {
        250, 1, 1, 2, 8, 8, 8
    };
    bool only_captures;
    int ep_from;
    const int side;
    int depth;
    uint64 piece_board, move_set;
    bool initialised = false;
};
#endif