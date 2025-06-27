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
int Search::negamax(int depth_left, unsigned int side, unsigned int starting_side){
    if(depth_left == 0){
        if(starting_side == BLACK)
            return eval->get_material();
        else 
            return -eval->get_material();
    }
    int best_val = -100000;

    MoveGen mg = MoveGen(side);
    unsigned int move = 0;
    unsigned int selected_move = 0;
    bool no_moves_left = true;
    while((move = mg.get_move()) != NO_MOVES_LEFT){
        if(move == INCREMENTING_MOVE_TYPE)
            continue;

        if(b->apply_move_if_legal(move)){
            int new_val;
            if(depth_left == max_depth)
                new_val = -negamax(depth_left - 1, side ^ 1, starting_side);
            else 
                new_val = negamax(depth_left - 1, side ^ 1, starting_side);

            b->reverse_move(move);
            
            if(new_val > best_val){
                selected_move = move;
                if(depth_left == max_depth){
                    cout<<"selected move: ";
                    MoveUtils::display(selected_move);
                    cout<<"new val: "<<new_val<<endl;
                    cout<<"best val: "<<best_val<< endl;
                }
            } 
            best_val = max(best_val, new_val);
            no_moves_left = false;
        }
    }
    if(no_moves_left){

        if(b->get_bitboard()->attacked(side, b->get_king_location(side))){
            return -100000 + (max_depth - depth_left); 
        } else {
            return 0;
        }
    }
    // cout<<"depth_left: "<<depth_left<<endl;
    // MoveUtils::display(selected_move);
    // cout<<"best_val: "<<best_val<<endl<<endl;
    return best_val;
}
int Search::alpha_beta(int alpha, int beta, int depth_left, unsigned int side, unsigned int starting_side, unsigned int root_move){


    if(depth_left == 0){

        if(starting_side == BLACK)
            return eval->get_material();
        else 
            return -eval->get_material();

    }

    MoveGen mg = MoveGen(side);
    unsigned int move = 0;
    bool no_moves_left = true;

    while((move = mg.get_move()) != NO_MOVES_LEFT){
        if(move == INCREMENTING_MOVE_TYPE)
            continue;

        if(b->apply_move_if_legal(move)){
            int score;
            if(depth_left > max_depth - 1){
                score = -alpha_beta(-beta, -alpha, depth_left - 1, side ^ 1, starting_side, move);
            } else {
                score = alpha_beta(alpha, beta,  depth_left - 1, side ^ 1, starting_side, move);
            }

            b->reverse_move(move);
            if(score >= beta){
                return beta;
            } if(score > alpha){
                if(depth_left == max_depth){
                    selected_move = move;
                    cout<<"selected_move\n";
                    MoveUtils::display(selected_move);
                    cout<<"score: "<<score<<endl;
                    cout<<"alpha: "<<alpha<<endl;
                } 
                alpha = score;
            } 
            no_moves_left = false;
        }

    }

    if(no_moves_left){

        if(side == starting_side){
            return -1e5;
        } else {
            return 1e5;
        }
        // if(starting_side == BLACK)
        //     return eval->get_material();
        // else 
        //     return -eval->get_material();
    }
    return alpha;
}
int Search::quiesce(int alpha, int beta, int depth, unsigned int side,  unsigned int starting_side){
    int static_eval = eval->get_material();
    // if(starting_side == BLACK){
    //     static_eval = -static_eval;
    // }
    // int static_eval = eval->get_material();
    // Stand Pat
    int best_value = static_eval;
    if( best_value >= beta )
        return best_value;
    if( best_value > alpha )
        alpha = best_value;

    MoveGen mg = MoveGen(side, 0, true);
    unsigned int move = 0;
    while((move = mg.get_move()) != NO_MOVES_LEFT){
        if(move == INCREMENTING_MOVE_TYPE || (!MoveUtils::is_capture(move) && !MoveUtils::is_ep_capture(move))){
            continue;
        }
        // cout<<"is capture"<<endl;
        // MoveUtils::display(move);

        if(b->apply_move_if_legal(move)){
            int score = -quiesce(-beta, -alpha, 0, side ^ 1, starting_side);
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
int Search::alpha_beta_1(int alpha, int beta, int depth_left, unsigned int side, unsigned int starting_side){
    if(depth_left == 0){
        int val = eval->get_material();
        return (starting_side == WHITE) ? val : -val;
    }
    if((starting_side ^ 1) == side){
        int value = -1e5;
        MoveGen mg = MoveGen(side);
        unsigned int move = 0;

        while((move = mg.get_move()) != NO_MOVES_LEFT){
            if(move == INCREMENTING_MOVE_TYPE)
                continue;

            if(b->apply_move_if_legal(move)){
                value = max(value, alpha_beta_1(alpha, beta, depth_left - 1, side ^ 1, starting_side));
                b->reverse_move(move);
                if(value >= beta)
                    break;
                if(value > alpha){

                    if(depth_left == max_depth){
                        selected_move = move;
                        cout<<"selected move\n";
                        MoveUtils::display(selected_move);
                        cout<<"alpha after: "<<value<<endl;
                    }
                    alpha = value;
                }
            }
        }
        return value;
    } else {
        int value = 1e5;
        MoveGen mg = MoveGen(side);
        unsigned int move = 0;

        while((move = mg.get_move()) != NO_MOVES_LEFT){
            if(move == INCREMENTING_MOVE_TYPE)
                continue;

            if(b->apply_move_if_legal(move)){
                value = min(value, alpha_beta_1(alpha, beta, depth_left - 1, side ^ 1, starting_side));
                b->reverse_move(move);
                if(value <= alpha)
                    break;
                if(value < beta){
                    if(depth_left == max_depth){
                        selected_move = move;
                        cout<<"selected move\n";
                        MoveUtils::display(selected_move);
                        cout<<"beta after: "<<value<<endl;
                    }
                    beta = value;
                }
            }
        }

        return value;
    }
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
float Search::evaluate(){
    return 0.f;
    // return eval->get_material();
}