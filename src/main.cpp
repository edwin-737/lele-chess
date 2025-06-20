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
#define NONE 0
#define FEN 1
#define DEPTH 2
#define SIDE 3
Bitboard* Bitboard::instanceptr=nullptr;
BoardInfo* BoardInfo::instanceptr=nullptr;

int main(int argc, char** argv)
{
    int arg_state = NONE;
    char* fen_path = (char*) calloc(256, sizeof(char));
    int depth = 1;
    bool include_fen = false;
    int side = WHITE;
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
        }  else if(arg_state == SIDE){
            side = BLACK ? argv[i][0] == 'b' : WHITE;
            cout<<"side: "<< side << endl;
            arg_state = NONE;
        } else{
            if(strcmp(argv[i], "-f") == 0){
                arg_state = FEN;
                include_fen = true;
            } 
            else if(strcmp(argv[i], "-d") == 0){
                arg_state = DEPTH;
            } else if(strcmp(argv[i], "-s") == 0){
                arg_state = SIDE;
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
    // cout<<"initial ep rights\n";
    // cout<<b->get_board_info()->peek_ep_right()<<endl;
    // cout<<"initial castle rights\n";
    // cout<<b->get_board_info()->peek_castle_right()<<endl;

    Search* s = new Search(b);
    auto start = high_resolution_clock::now();
    // s->search(depth, side);
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
    // s->move_breakdown();
    // MoveSet::set_attack_sets();
    // MoveSet::init_attack_masks();
    // GameState gs = GameState();
    // Evaluation eval = Evaluation();
    // Searcher searcher = Searcher(gs, eval);

    // // for(int i = 0 ; i < 4 ; i ++)
    // //     searcher.gs.apply_move_if_legal(castle_test_move_list[i]);
    // // for(int i = 0 ; i < 9 ; i ++)
    // //     searcher.gs.apply_move_if_legal(scotch_move_list[i]);
    // // searcher.gs.bb.display();
    // if(include_fen){
    //     fs::path input_fen_path(fen_path);
    //     searcher.gs.parse_fen(input_fen_path);
    // }
    // cout<<"finished applying moves\n";
    // auto start = high_resolution_clock::now();
    // Line line;
    // float alpha = -1e5;
    // float beta = 1e5;
    // // searcher.alpha_beta(alpha, beta, 3, WHITE, &line);
    // searcher.alpha_beta(alpha, beta, depth, BLACK, &line);

    // for(int i = 0 ; i < depth ; i ++)
    //     line.move_seq[i].print();

    // auto stop = high_resolution_clock::now();    
    // auto duration = duration_cast<microseconds>(stop - start);
    // cout << "Nodes searched: " << searcher.nodes << endl;
    // cout << "Time taken by function: "
    //      << duration.count() << " microseconds" << endl;

    // free(fen_path);
}