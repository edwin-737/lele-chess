#include <chrono>
#include <stdlib.h>
#include <vector>
#include <string>
#include "move.hpp"
// #include "evaluation.hpp"
#include "board_squares.hpp"
#include "search.hpp"
#include "magics.hpp"
#include "bitboard.hpp"
#include "board_info.hpp"
#include "move_set.hpp"
using namespace std::chrono;
typedef enum Task{
    PERFT,
    ALPHA_BETA
} Task;

typedef enum ArgState{
    ARG_TYPE,
    FEN,
    DEPTH,
    SIDE,
    TASK
} ArgState;
Bitboard* Bitboard::instanceptr=nullptr;
BoardInfo* BoardInfo::instanceptr=nullptr;

int main(int argc, char** argv)
{
    int arg_state = ARG_TYPE;
    vector<string> arg_list(argv, argv + argc);
    string fen_path = "";
    int depth = 1;
    bool include_fen = false;
    int side = WHITE;
    int task = PERFT;
    for(int i = 0 ; i < argc ; i ++){
        if(arg_state == ARG_TYPE){
            if(arg_list[i] == "-f"){
                arg_state = FEN;
                include_fen = true;
            } else if(arg_list[i] == "-d"){
                arg_state = DEPTH;
            } else if(arg_list[i] == "-s"){
                arg_state = SIDE;
            } else if(arg_list[i] =="-j"){
                arg_state = TASK;
            }
        } else if(arg_state == FEN){
            fen_path = arg_list[i];
            cout<<"fen path: "<<fen_path<<endl;
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
        }
    }
    BoardSquares::init_files();
    BoardSquares::init_ranks();
    BoardSquares::init_squares();
    MoveSet::set_attack_sets();
    MoveSet::init_attack_masks();

    Bitboard::get_instance();

    cout << "Making objects \n";
    Board* b = new Board();
    b->parse_fen(fen_path);
    b->get_bitboard()->display();

    // BoardInfo::set_castle_rights(b->get_initial_castle_rights());
    // BoardInfo::set_ep_rights(b->get_initial_ep_rights());
    BoardInfo::set_board_info(b->get_initial_castle_rights(), b->get_initial_ep_rights());

    Search* s = new Search(b, depth);
    auto start = high_resolution_clock::now();
    if(task == PERFT){
        unsigned int num_nodes = s->perft(depth, depth, side);
        auto stop = high_resolution_clock::now();    
        auto duration = duration_cast<microseconds>(stop - start);
        cout << "Time taken by perft at depth=" << depth<<":"
            << duration.count() << " microseconds" << endl;
        cout<<"nodes: "<<num_nodes<<endl;
        cout<<"captures: "<<s->num_captures<<endl;
        cout<<"ep_captures: "<<s->num_ep_captures<<endl;
        cout<<"checks: "<<s->num_checks<<endl;
        cout<<"checkmates: "<<s->num_checkmates<<endl;
        cout<<"castles: "<<s->num_castles<<endl;
        cout<<"promotions: "<<s->num_promotions<<endl;
        cout<<"capture promotions: "<<s->num_capture_promotions<<endl;
    } else {
        cout<<"search max depth: "<<s->max_depth<<endl;
        cout<<"search depth: "<<depth<<endl;
        cout<<"side: "<<side<<endl;

        int score = s->alpha_beta(-1e5, 1e5, depth, side, side);

    }
}