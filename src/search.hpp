#ifndef search_h
#define search_h
#include <chrono>
#include <atomic>
#include <vector>

#include "board.hpp"
#include "const.hpp"
#include "pesto.hpp"
#include "move_gen.hpp"

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

typedef struct alpha_beta_state {
    int alpha;
    int beta;
    int score;
    bool is_beta_returned=false;
    bool is_score_returned=false;
} alpha_beta_state_t;

typedef enum variation_type {
    PREVIOUS_VAR,
    CURRENT_VAR,
    SELECTED_VAR
} variation_type_e;


class Search{
public:
    Search(Board* _b, PestoEvaluation* _pesto, int _max_depth=5, bool _verbose=false): b(_b), pesto(_pesto),max_depth(_max_depth), verbose(_verbose){
        pesto->init_evaluate();
    }

    unsigned int perft_loop(MoveGen mg, int original_depth, int depth_left, unsigned int side, unsigned int root_move=0ULL, bool transposition = false);
    unsigned int perft(int original_depth, int depth_left, unsigned int side, unsigned int root_move=0ULL, bool transposition = false);
    int alpha_beta_new_movegen(int alpha, int beta, int depth_left, unsigned int side, unsigned int starting_side, std::atomic<bool>& stop_flag, unsigned int root_move=0, pv_t* pv=nullptr, bool transposition = false, bool use_pesto=true, pv_t* prev_variation=nullptr);
    int alpha_beta_new_movegen_short(int alpha, int beta, int depth_left, unsigned int side, unsigned int starting_side, std::atomic<bool>& stop_flag, unsigned int root_move=0, pv_t* pv=nullptr, bool transposition = false, bool use_pesto=true, pv_t* prev_variation=nullptr);
    alpha_beta_state_t alpha_beta_new_movegen_prev_var_check(alpha_beta_state_t prev_state, int depth_left, unsigned int side, unsigned int starting_side, std::atomic<bool>& stop_flag, unsigned int root_move=0, pv_t* pv=nullptr, bool transposition = false, bool use_pesto=true, pv_t* prev_variation=nullptr);
    alpha_beta_state_t alpha_beta_new_movegen_loop(MoveGen mg, alpha_beta_state_t prev_state, int depth_left, unsigned int side, unsigned int starting_side, std::atomic<bool>& stop_flag, unsigned int root_move=0, pv_t* pv=nullptr, bool transposition = false, bool use_pesto=true, pv_t* prev_variation=nullptr);
    int alpha_beta_new_movegen_ordered(int alpha, int beta, int depth_left, unsigned int side, unsigned int starting_side, std::atomic<bool>& stop_flag, unsigned int root_move=0, pv_t* pv=nullptr, bool transposition = false, bool use_pesto=true, pv_t* prev_variation=nullptr);
    int quiesce(int alpha, int beta, int depth, unsigned int side, unsigned int starting_side, std::atomic<bool>& stop_flag, pv_t* pv=nullptr, bool transposition=false, bool use_pesto=true);
    int iterative_deepening(int depth, unsigned int side, unsigned int starting_side, std::atomic<bool>& stop_flag, bool transposition=false, bool use_pesto=false);
    int get_evaluation(unsigned int side);
    int init_evaluate();
    void display_variation(pv_t* principal_variation, variation_type_e var_type=CURRENT_VAR);
    void display_selected_move(int alpha, int score);
    Board* get_board_instance();
    unsigned int num_nodes = 0, num_captures = 0, num_ep_captures = 0, num_checks = 0, num_checkmates = 0, num_castles = 0, num_promotions = 0, num_capture_promotions = 0;
    int max_depth;
    bool verbose;
    unsigned int selected_move; 
    int side_to_move;
    int tt_not_found_count[10] = {0};
    int tt_found_count[10] = {0};
    int tt_match_count[10] = {0};
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

    std::chrono::steady_clock::time_point stop = std::chrono::steady_clock::now();
    PestoEvaluation* pesto;
private:
    Board* b;
    vector<unsigned int> selected_moves[DEPTH_LIMIT];
    int material = 0;

};

#endif