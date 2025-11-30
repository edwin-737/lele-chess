#ifndef search_h
#define search_h
#include <map>
#include <string>
#include <iostream>
#include <deque>
#include "board.hpp"
#include "move_gen.hpp"
#include "evaluation.hpp"
#include "transposition_table.hpp"
using namespace std;
typedef struct pv {
    int len;              // Number of moves in the variation.
    unsigned int moves[20];  // The principal variation
} pv_t;

class Search{
public:
    Search(Board* _b, int _max_depth=5): b(_b), max_depth(_max_depth){
    }

    unsigned int perft(int original_depth,int depth_left, unsigned int side, unsigned int root_move = 0ULL, bool transposition = false);
    int alpha_beta(int alpha, int beta, int depth_left, unsigned int side, unsigned int starting_side, unsigned int root_move=0, pv_t* = nullptr);
    int quiesce(int alpha, int beta, int depth, unsigned int side, unsigned int starting_side);
    int evaluate();
    int static_exchange_evaluation(unsigned int side, int square);
    int static_exchange_evaluation(int move);
    unsigned int num_nodes = 0, num_captures = 0, num_ep_captures = 0, num_checks = 0, num_checkmates = 0, num_castles = 0, num_promotions = 0, num_capture_promotions = 0;
    int max_depth;
    unsigned int selected_move; 
    int side_to_move;
    int tt_not_found_count[10] = {0};
    int tt_found_count[10] = {0};
    int tt_match_count[10] = {0};
private:
    Board* b;
    Evaluation* eval = Evaluation::get_instance();
    TranspositionTable* tt = TranspositionTable::get_instance();
    int material = 0;
};

#endif