#ifndef search_h
#define search_h
#include <map>
#include <string>
#include <iostream>
#include "board.hpp"
#include "move_gen.hpp"
#include "evaluation.hpp"
using namespace std;
struct Line{
    int cmove;
    int move_seq[5];
};
class Search{
public:
    Search(Board* _b){
        b = _b;
    }

    unsigned int perft(int original_depth,int depth_left, unsigned int side, unsigned int root_move = 0ULL);
    int alpha_beta(int alpha, int beta, int depth_left, unsigned int side);
    int quiesce(int alpha, int beta, unsigned int side);
    float evaluate();
    int static_exchange_evaluation(unsigned int side, int square);
    int static_exchange_evaluation(int move);
    unsigned int num_nodes = 0, num_captures = 0, num_ep_captures = 0, num_checks = 0, num_checkmates = 0, num_castles = 0;
private:
    Board* b;
    Evaluation* eval = Evaluation::get_instance();
    int side_to_move;
    Line* pline;
    int material = 0;
};

#endif