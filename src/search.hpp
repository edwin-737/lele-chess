#ifndef search_h
#define search_h
#include <chrono>

#include "board.hpp"
#include "const.hpp"
#include "transposition_table.hpp"
#include "pesto.hpp"

using namespace std::chrono;
using namespace std;
#define DEPTH_LIMIT 20
typedef struct pv {
    int len;              // Number of moves in the variation.
    unsigned int moves[DEPTH_LIMIT];  // The principal variation
} pv_t;

typedef struct pv_node {
    unsigned int move;
    struct pv_node* next;
    int len;
} pv_node_t;

class Search{
public:
    Search(Board* _b, PestoEvaluation* _pesto, int _max_depth=5): b(_b), pesto(_pesto),max_depth(_max_depth){
        pesto->init_evaluate();
    }

    unsigned int perft(int original_depth,int depth_left, unsigned int side, unsigned int root_move = 0ULL, bool transposition = false);
    unsigned int perft_ordered(int original_depth, int depth_left, unsigned int side, unsigned int root_move = 0ULL, bool transposition = false);
    int alpha_beta(int alpha, int beta, int depth_left, unsigned int side, unsigned int starting_side, unsigned int root_move=0, pv_t* pv=nullptr, bool transposition = false, bool use_pesto=true, pv_t* prev_variation=nullptr);
    int quiesce(int alpha, int beta, int depth, unsigned int side, unsigned int starting_side, pv_t* pv=nullptr, bool transposition=false, bool use_pesto=true);
    int evaluate(bool use_pesto=false);
    int static_exchange_evaluation(unsigned int side, int square);
    int static_exchange_evaluation(int move);
    int iterative_deepening(int depth, unsigned int side, unsigned int starting_side, bool transposition=false, bool use_pesto=false);
    unsigned int num_nodes = 0, num_captures = 0, num_ep_captures = 0, num_checks = 0, num_checkmates = 0, num_castles = 0, num_promotions = 0, num_capture_promotions = 0;
    int max_depth;
    unsigned int selected_move; 
    int side_to_move;
    int tt_not_found_count[10] = {0};
    int tt_found_count[10] = {0};
    int tt_match_count[10] = {0};
    std::chrono::time_point<std::chrono::steady_clock> start = high_resolution_clock::now();
    std::chrono::time_point<std::chrono::steady_clock> stop = high_resolution_clock::now();
    unsigned int searched_move_eval=0;
    bool searched_move_found=false;
    int searched_move_depth=false;
    unsigned int searched_move=MoveUtils::create_move(e4, d5, WHITE, pPAWN, CAPTURE);
private:
    Board* b;
    PestoEvaluation* pesto;
    vector<unsigned int> selected_moves[DEPTH_LIMIT];
    int material = 0;

};

#endif