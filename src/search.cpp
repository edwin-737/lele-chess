#include "search.hpp"
#include "move_gen.hpp"
#include "move_set.hpp"
unsigned int Search::perft(int original_depth, int depth_left, unsigned int side, unsigned int root_move){

    if(depth_left == 0){

        if(MoveUtils::is_ep_capture(root_move)){
            num_captures ++;
            num_ep_captures ++;
        } else if(MoveUtils::is_capture(root_move)){
            num_captures ++;
        } else if(MoveUtils::is_castle(root_move)){
            num_castles ++;
        }   
        if(b->get_bitboard()->attacked(side, b->get_king_location(side))){
            num_checks ++;
        } 
        return 1ULL;
    } else if(depth_left == original_depth - 1){
        cout<<MoveUtils::move_as_string(root_move)<<": ";
    }
    MoveGen mg = MoveGen(side);
    unsigned int move = 0;
    int num_legal_moves = 0;
    unsigned int ans = 0;
    while((move = mg.get_move()) != NO_MOVES_LEFT){
        if(move == INCREMENTING_MOVE_TYPE)
            continue;

        if(b->apply_move_if_legal(move)){
            ans += perft(original_depth, depth_left - 1, side ^ 1, move);
            // mg.add_move(move);
            b->reverse_move(move);
        } 
    }
    if(depth_left == original_depth - 1){
        cout<<ans<<endl;
    }
    return ans;
}

int Search::alpha_beta(int alpha, int beta, int depth_left, unsigned int side){

    // LINE line;
    if(depth_left == 0){
        // return eval->get_material();
        // return side == WHITE ? eval->get_material() : -eval->get_material();
        // cout<<"calling quiesce\n";
        return quiesce(alpha, beta, side);
        // return quiesce(alpha, beta, side);

    }
    // int best_value = -1e5;
    MoveGen mg = MoveGen(side);
    unsigned int move = 0;
    while((move = mg.get_move()) != NO_MOVES_LEFT){
        if(move == INCREMENTING_MOVE_TYPE)
            continue;
        if(b->apply_move_if_legal(move)){

            int score = -alpha_beta(-beta, -alpha, depth_left - 1, side ^ 1);
            b->reverse_move(move);


            // if(depth_left == max_depth){
            //     cout<<"move\n";
            //     MoveUtils::display(move);
            //     cout<<"score\n";
            //     cout<<score<<endl;
            // }
            if(score >= beta){
                return beta;
            } if(score > alpha){
                alpha = score;
                if(depth_left == max_depth){
                    selected_move = move;
                    cout<<"selected move\n";
                    MoveUtils::display(selected_move);
                }
            } 
            // else if(score < alpha && side == BLACK){
            //     alpha = score;
            //     if(depth_left == max_depth){
            //         selected_move = move;
            //         cout<<"selected move\n";
            //         MoveUtils::display(selected_move);
            //     }
            // }
        }
    }
    // return best_value;
    return alpha;
}
int Search::quiesce(int alpha, int beta, unsigned int side, int depth){
    int static_eval = eval->get_material();
    // Stand Pat
    // if(depth > 0)
    //     return static_eval;
    int best_value = static_eval;
    if( best_value >= beta )
        return best_value;
    if( best_value > alpha )
        alpha = best_value;

    // cout<<"quiesce depth: "<<depth<<endl;
    MoveGen mg = MoveGen(side, 0, true);
    // MoveUtils::display(mg.get_move());
    unsigned int move = 0;
    while((move = mg.get_move()) != NO_MOVES_LEFT){
        if(move == INCREMENTING_MOVE_TYPE || (!MoveUtils::is_capture(move) && !MoveUtils::is_ep_capture(move))){
            continue;
        }
        // cout<<"is capture"<<endl;
        // MoveUtils::display(move);

        if(b->apply_move_if_legal(move)){
            int score = -quiesce(-beta, -alpha, side ^ 1, depth + 1);
            b->reverse_move(move);
            if(score >= beta)
                return score;
            if(score > best_value)
                best_value = score;
            if(score > alpha)
                alpha = score;
        }

    }

    return best_value;
}
// float Search::alpha_beta(float alpha, float beta, int depth_left, int side, Line* line ){
//     Line line;
//     cout << "alpha beta\n";
//     cout << "depth_left: " << depth_left << "\n";
//     cout << "beta " << beta << "\n";
//     cout << "alpha " << alpha << "\n";
//     if (depth_left == 0){
//         pline->cmove = 0;
//         return quiesce(alpha, beta, side);
//     }
//     nodes++;
//     cout << "===> nodes: " << nodes << "\n";
//     Moves move_list[MAX_MOVES];
//     find_moves(side, move_list);

//     for (int i = 0; i < MAX_MOVES; i++)
//     {
//         if (move_list[i].side == -1)
//             break;

//         uint64 from_to = get_square_bitboard(move_list[i].from) ^ get_square_bitboard(move_list[i].to);
//         gs.apply_move(move_list[i]);
//         cout << "alpha beta move\n";
//         move_list[i].print();
//         float score = -alpha_beta(-beta, -alpha, depth_left - 1, side ^ 1, &line);
//         gs.reverse_move(move_list[i]);
//         if (score >= beta)
//         {
//             cout << "===> score >= beta\n";
//             cout << "===> depth_left: " << depth_left << "\n";
//             return beta;
//         }
//         if (score > alpha)
//         {
//             cout << "===> score > alpha\n";
//             cout << score << " " << alpha << "\n";
//             alpha = score;
//             pline->move_seq[0] = move_list[i];
//             memcpy(pline->move_seq + 1, line.move_seq, line.cmove * sizeof(Moves));
//             pline->cmove = line.cmove + 1;
//         }
//     }

//     cout << "===> alpha-beta final alpha: " << alpha << "\n";
//     cout << "===> depth_left: " << depth_left << "\n";
//     return alpha;
// }
// float Search::quiesce(float alpha, float beta, unsigned int side){

// }
float Search::evaluate(){
    return 0.f;
    // return eval->get_material();
}