#include <chrono>
#include <stdlib.h>
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
    NONE,
    FEN,
    DEPTH,
    SIDE,
    TASK
} ArgState;
Bitboard* Bitboard::instanceptr=nullptr;
BoardInfo* BoardInfo::instanceptr=nullptr;

int main(int argc, char** argv)
{
    int arg_state = NONE;
    char* fen_path = (char*) calloc(256, sizeof(char));
    int depth = 1;
    bool include_fen = false;
    int side = WHITE;
    int task = PERFT;
    for(int i = 0 ; i < argc ; i ++){
        char* arg = argv[i];
        if(arg_state == FEN){
            strcpy(fen_path, arg);
            cout<<"fen path: "<<fen_path<<endl;
            arg_state = NONE;
        } else if(arg_state == DEPTH){
            depth = argv[i][0] - '0';
            cout<<"depth: "<<depth<<endl;
            arg_state = NONE;
        } else if(arg_state == SIDE){
            side = BLACK ? argv[i][0] == 'b' : WHITE;
            cout<<"side: "<< side << endl;
            arg_state = NONE;
        } else if(arg_state == TASK){
            task = ALPHA_BETA ? argv[i][0] == 'a' : PERFT;
            cout<<"task: "<<task<<endl;
            arg_state = NONE;
        } else{
            if(strcmp(argv[i], "-f") == 0){
                arg_state = FEN;
                include_fen = true;
            } else if(strcmp(argv[i], "-d") == 0){
                arg_state = DEPTH;
            } else if(strcmp(argv[i], "-s") == 0){
                arg_state = SIDE;
            }
             else if(strcmp(argv[i], "-j") == 0){
                arg_state = TASK;
            }
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

    BoardInfo::set_castle_rights(b->get_initial_castle_rights());
    BoardInfo::set_ep_rights(b->get_initial_ep_rights());
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
        cout<<"castles: "<<s->num_castles<<endl<<endl;
        cout<<"move_counts: \n";
    } else {
        cout<<"search max depth: "<<s->max_depth<<endl;
        cout<<"search depth: "<<depth<<endl;
        cout<<"side: "<<side<<endl;

        int score = s->alpha_beta(-1e5, 1e5, depth, side, side);

    }
}