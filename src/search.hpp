#ifndef search_h
#define search_h
#include <map>
#include <string>
#include <iostream>
#include <deque>
#include "board.hpp"
#include "move_gen.hpp"
#include "evaluation.hpp"
using namespace std;
// typedef struct LINE {
//     int cmove;              // Number of moves in the line.
//     unsigned int argmove[MAX_DEPTH];  // The line.
// }   LINE;
class Search{
public:
    Search(Board* _b, int _max_depth=5): b(_b), max_depth(_max_depth){

    }

    unsigned int perft(int original_depth,int depth_left, unsigned int side, unsigned int root_move = 0ULL);
    // int alpha_beta(int alpha, int beta, int depth_left, unsigned int side);
    int alpha_beta(int alpha, int beta, int depth_left, unsigned int side);
    int quiesce(int alpha, int beta, unsigned int side, int depth=0);
    float evaluate();
    int static_exchange_evaluation(unsigned int side, int square);
    int static_exchange_evaluation(int move);
    unsigned int num_nodes = 0, num_captures = 0, num_ep_captures = 0, num_checks = 0, num_checkmates = 0, num_castles = 0;
    const int max_depth;
private:
    Board* b;

    Evaluation* eval = Evaluation::get_instance();
    int side_to_move;
    // LINE* pv;
    // unsigned int moves[MAX_DEPTH];
    // int variation_idx = 0, depth_idx = MAX_DEPTH - 1;
    unsigned int selected_move; 
    int material = 0;
};

#endif