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
#define INCREMENTING_MOVE_TYPE 1ULL
#define NO_MOVES_LEFT 0ULL
#define WHITE_KING_CASTLE_SQUARES 0x60ULL
#define WHITE_QUEEN_CASTLE_SQUARES 0xeULL
#define BLACK_KING_CASTLE_SQUARES 0x6000000000000000ULL
#define BLACK_QUEEN_CASTLE_SQUARES 0xe00000000000000ULL
#define EP_START -1
#define EP_FINISHED 8
class MoveGen{
    /**
     * Move generation class which keeps a stack of moves being explored during search
     */
public:
    MoveGen():move_type(QUIET_MOVE),side(WHITE),piece(pPAWN),from(0),to(0),ep_from(EP_START){
    }
    MoveGen(
        int _side, int _move_type=mQUIET, bool _only_captures=false, int _piece=0, int _from=0, int _to=0): side(_side), move_type(_move_type), only_captures(_only_captures), piece(_piece), from(_from), to(_to), ep_from(EP_START){
            // if(_from > 0 && _to > 0)
            //     initialised = true;
        }
    int get_special_move_type();
    unsigned int get_move();
    unsigned int get_special_move();
    unsigned int get_capture();
    // void add_move(unsigned int move);
    // void add_capture(unsigned int capture);
private:
    // vector<unsigned int> moves, captures;
    bool initialise_piece();
    bool update_piece();
    int get_next_piece();
    bool update_from();
    int get_next_from();
    bool update_to();
    int get_next_to();
    bool can_castle_kingside(int side);
    bool can_castle_queenside(int side);
    unsigned int get_ep_capture(int side);
    Bitboard* bb = Bitboard::get_instance();
    BoardInfo* bi = BoardInfo::get_instance();
    unsigned int move_type = mQUIET, piece, from, to;
    int max_special_moves[7] = {
        250, 1, 1, 2, 8, 8, 8
    };
    int num_special_moves[7] = {
        0, 0, 0, 0, 0, 0, 0
    };
    bool finished_special_move[13],only_captures;
    int ep_from;
    const int side;
    int depth;
    uint64 piece_board, move_set;
    bool initialised = false;
};
#endif