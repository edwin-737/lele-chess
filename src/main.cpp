#include "pesto.hpp"
#include <chrono>
#include <stdlib.h>
#include <vector>
#include <string>
#include <atomic>
#include <thread>
#ifdef ENABLE_PROFILER
#include <gperftools/profiler.h>
#endif
#include "move.hpp"
#include "board_squares.hpp"
#include "search.hpp"
#include "bitboard.hpp"
#include "move_set.hpp"
#include "worker.hpp"
using namespace std::chrono;
typedef enum Task{
    PERFT,
    ALPHA_BETA
} Task;

typedef enum ArgState{
    ARG_TYPE,
    FEN,
    PGN,
    DEPTH,
    SIDE,
    TASK,
    TRANSPOSITION,
    ITERATIVE_DEEPEN,
    USER_INPUT
} ArgState;
void perft(Search s, int depth, unsigned int side){
    auto start = high_resolution_clock::now();
    #ifdef ENABLE_PROFILER
        ProfilerStart("perf-profile.prof");
    #endif
    unsigned int num_nodes = s.perft(depth, depth, side);
    
    #ifdef ENABLE_PROFILER
        ProfilerStop();
    #endif
    auto stop = high_resolution_clock::now();
    duration<double> elapsed = stop - start;  // seconds as double (fractional)
    cout<<"depth = "<<depth<<endl;
    cout<<"time = "<<elapsed.count()<<endl;
    cout << "nodes per second = " << num_nodes  /  elapsed.count()<< endl;
    cout<<"nodes = "<<num_nodes<<endl;
    cout<<"captures = "<<s.num_captures<<endl;
    cout<<"ep_captures = "<<s.num_ep_captures<<endl;
    cout<<"checks = "<<s.num_checks<<endl;
    cout<<"checkmates = "<<s.num_checkmates<<endl;
    cout<<"castles = "<<s.num_castles<<endl;
    cout<<"promotions = "<<s.num_promotions<<endl;
    cout<<"capture promotions = "<<s.num_capture_promotions<<endl;
}
void move_search(Search s, bool iterative_deepen, bool transposition, int depth, unsigned int side){

    cout<<"side = "<<MoveUtils::side_as_string(side)<<endl;
    s.max_depth = depth;

    #ifdef ENABLE_PROFILER
        ProfilerStart("alphabeta-profile.prof");
    #endif

    pv_t* principal_variation = (pv_t*) calloc(1, sizeof(pv_t));
    principal_variation->len = 0;
    pv_node_t* principal_variation_node = (pv_node_t*) calloc(1, sizeof(pv_node_t));
    principal_variation_node->move = 0;
    int alpha = -1e7;
    int beta = 1e7;

    if(iterative_deepen){
        std::atomic<bool> stop_flag(false);
        int score = s.iterative_deepening(depth, side, side, stop_flag);
    } else {
        std::atomic<bool> stop_flag = std::atomic<bool>(false);
        int score = s.alpha_beta(alpha, beta, depth, side, side, stop_flag, 0, principal_variation, transposition, true);
    }
    
    free(principal_variation);
    free(principal_variation_node);
    #ifdef ENABLE_PROFILER
        ProfilerStop();
    #endif
    // auto stop = high_resolution_clock::now();
    duration<double> elapsed = s.stop - s.start;  // seconds as double (fractional)
    cout<<"depth = "<<depth<<endl;
    cout<<"time = "<<elapsed.count()<<endl;
    cout<<"nodes = "<<s.num_nodes<<endl;
}
void move_search_worker(atomic<bool>& stop_flag, Search s, unsigned int side_to_move){
    int score = s.iterative_deepening(s.max_depth, side_to_move, side_to_move, stop_flag);
}
void interactive_move_search(Search s, long long num_seconds_per_move, unsigned int opp_side){
    cout<<"interactive move search\n";
    s.get_board_instance()->get_bitboard()->display();
    s.max_depth = 10;
    // Worker w = Worker(s, side_to_move, s.max_depth);
    // w.loop(num_seconds_per_move);
    auto w = std::make_shared<Worker>(s, opp_side, s.max_depth);
    w->loop(num_seconds_per_move);
}
int main(int argc, char** argv)
{
    int arg_state = ARG_TYPE;
    vector<string> arg_list(argv, argv + argc);
    string fen_path = "", pgn_path = "";
    int depth = 1;
    bool include_fen = false;
    bool include_pgn = false;
    unsigned int side = WHITE;
    int task = PERFT;
    bool transposition = false;
    bool iterative_deepen = false;
    bool user_input = false;
    for(int i = 0 ; i < argc ; i ++){
        if(arg_state == ARG_TYPE){
            if(arg_list[i] == "-f"){
                arg_state = FEN;
                include_fen = true;
            } else if(arg_list[i] == "-p"){
                arg_state = PGN;
                include_pgn = true;
            } else if(arg_list[i] == "-d"){
                arg_state = DEPTH;
            } else if(arg_list[i] == "-s"){
                arg_state = SIDE;
            } else if(arg_list[i] =="-j"){
                arg_state = TASK;
            } else if(arg_list[i] == "-t"){
                arg_state = TRANSPOSITION;
            } else if(arg_list[i] == "-i"){
                arg_state = ITERATIVE_DEEPEN;
            } else if(arg_list[i] == "-u"){
                arg_state = USER_INPUT;
            }
        } else if(arg_state == FEN){
            fen_path = arg_list[i];
            cout<<"fen path: "<<fen_path<<endl;
            arg_state = ARG_TYPE;
        } else if(arg_state == PGN){
            pgn_path = arg_list[i];
            cout<<"pgn path: "<<pgn_path<<endl;
            arg_state = ARG_TYPE;
        } else if(arg_state == DEPTH){
            depth = stoi(arg_list[i]);
            cout<<"depth: "<<depth<<endl;
            arg_state = ARG_TYPE;
        } else if(arg_state == SIDE){
            side = BLACK ? arg_list[i] == "b" : WHITE;
            cout<<"side: "<<side<<endl;
            arg_state = ARG_TYPE;
        } else if(arg_state == TASK){
            task = ALPHA_BETA ? arg_list[i] == "a" : PERFT;
            cout<<"task: "<<task<<endl;
            arg_state = ARG_TYPE;
        } else if(arg_state == TRANSPOSITION){
            transposition = arg_list[i] == "1";
            cout<<"transposition: "<<transposition<<endl;
            arg_state = ARG_TYPE;
        } else if(arg_state == ITERATIVE_DEEPEN){
            iterative_deepen = arg_list[i] == "1";
            cout<<"iterative_deepen: "<<iterative_deepen<<endl;
            arg_state = ARG_TYPE;
        } else if(arg_state == USER_INPUT){
            user_input = arg_list[i] == "1";
            cout<<"user_input: "<<user_input<<endl;
            arg_state = ARG_TYPE;
        }
    }
    BoardSquares::init_files();
    BoardSquares::init_ranks();
    BoardSquares::init_squares();
    MoveSet::set_attack_sets();
    MoveSet::init_attack_masks();


    cout << "Making objects \n";
    Bitboard _bb = Bitboard();
    BoardInfo _bi = BoardInfo();
    Board b;
    if(include_fen){
        b = Board(fen_path, &_bb, &_bi);
    } else if(include_pgn){
        b = Board("./positions/starting_position.txt", &_bb, &_bi);
        b.parse_uci_pgn(pgn_path);
    } else {
        b = Board("./positions/starting_position.txt", &_bb, &_bi);
    }
    b.get_bitboard()->display();
    PestoEvaluation pesto = PestoEvaluation(&b);

    Search s = Search(&b, &pesto, depth);

    if (task == PERFT) {
        perft(s, depth, side);
    } else if(user_input){
        cout<<"using user input\n";
        interactive_move_search(s, 10, side);
    } else {
        move_search(s, iterative_deepen, transposition, depth, side);

    }
}
