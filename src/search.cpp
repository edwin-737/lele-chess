#include "search.hpp"
#include "move_gen.hpp"
#include "const.hpp"
unsigned int Search::perft(int original_depth, int depth_left, unsigned int side, unsigned int root_move, bool transposition){
    if(depth_left == 0){
        if(MoveUtils::is_ep_capture(root_move)){
            num_captures ++;
            num_ep_captures ++;
        } else if(MoveUtils::is_capture(root_move)){
            num_captures ++;
        } else if(MoveUtils::is_castle(root_move)){
            num_castles ++;
        } else if(MoveUtils::is_promotion(root_move)){
            num_promotions ++;
        } else if(MoveUtils::is_capture_promotion(root_move)){
            num_capture_promotions ++;
        }
        if(b->get_bitboard()->attacked(side, b->get_king_location(side))){
            num_checks ++;
        } 
        return 1ULL;
    } 
    else if(depth_left == original_depth - 1){
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
            if(original_depth == 1){
                cout<<MoveUtils::move_as_string(move)<<": 1\n";
            }
            int depth_searched = original_depth - depth_left;
            if(!transposition){
                unsigned int perft_val = perft(original_depth, depth_left - 1, side ^ 1, move);
                ans += perft_val;
            } else {
                unsigned int tt_val = tt->get_value_perft(depth_searched);
                if(!tt_val){
                    unsigned int perft_val = perft(original_depth, depth_left - 1, side ^ 1, move, true);
                    ans += perft_val;
                    tt->add_value_perft(depth_searched, perft_val);
                    tt_not_found_count[depth_searched] ++;
                }
                else{
                    ans += tt_val;
                    tt_found_count[depth_searched] ++;
                }
            }
            b->reverse_move(move);
        } 
    }
    if(depth_left == original_depth - 1){
        cout<<ans<<endl;
    }
    return ans;
}

int Search::alpha_beta(int alpha, int beta, int depth_left, unsigned int side, unsigned int starting_side, unsigned int root_move, pv_t* principal_variation, bool transposition, bool use_pesto){
    pv_t line;
    line.len = 0;
    if(depth_left == 0)
        return quiesce(alpha, beta, depth_left, side, starting_side, &line, transposition, use_pesto);

    MoveGen mg = MoveGen(side);
    MoveGen mg_captures = MoveGen(side);
    MoveGen mg_quiet = MoveGen(side);
    mg_captures.set_gen_type(ONLY_CAPTURES);
    mg_quiet.set_gen_type(ONLY_QUIET);

    unsigned int move = 0;
    bool no_moves_left = true;

    while((move = mg_captures.get_move()) != NO_MOVES_LEFT){
        if(move == INCREMENTING_MOVE_TYPE)
            continue;

        if(b->apply_move_if_legal(move)){
            if(use_pesto)
                pesto->update_evaluation(move, 0);
            int score = 0;
            int depth_searched = 10 - depth_left;
            if(transposition){
                int tt_val = tt->get_value_eval(depth_searched);
                if(tt_val == DEFAULT_EVAL){
                    score = -alpha_beta(-beta, -alpha, depth_left - 1, side ^ 1, starting_side, move, &line, transposition, use_pesto);
                    tt->add_value_eval(depth_searched, score);
                } else {
                    score = tt_val;
                }
            } else {
                score = -alpha_beta(-beta, -alpha, depth_left - 1, side ^ 1, starting_side, move, &line, transposition, use_pesto);

            }
            b->reverse_move(move);
            if(use_pesto)
                pesto->update_evaluation(move, 1);
            if(score >= beta){
                num_nodes++;
                return beta;
            } 
            if(score > alpha){

                principal_variation->moves[0] = move;
                memcpy(principal_variation->moves + 1, line.moves, line.len * sizeof(unsigned int));
                principal_variation->len = line.len + 1;

                if(depth_left == max_depth){
                    selected_move = move;

                    cout<<"-----------------\n";
                    cout<<"selected_move:";
                    MoveUtils::display(selected_move);
                    cout<<"score: "<<score<<endl;
                    cout<<"alpha: "<<alpha<<endl;
                    cout<<"-----------------\n";


                    cout<<"-----------------\n";
                    cout<<"current variation\n";
                    cout<<"principal_variation length: "<<principal_variation->len<<endl;
                    for(int i = 0 ; i < principal_variation->len ; i ++){
                        cout<<i<<": ";
                        MoveUtils::display(principal_variation->moves[i]);
                    }
                    cout<<"-----------------\n";
                } 
                alpha = score;
            } 
            no_moves_left = false;
        }

    }

    while((move = mg_quiet.get_move()) != NO_MOVES_LEFT){
        if(move == INCREMENTING_MOVE_TYPE)
            continue;

        if(b->apply_move_if_legal(move)){
            if(use_pesto)
                pesto->update_evaluation(move, 0);
            int score = 0;
            int depth_searched = 10 - depth_left;
            if(transposition){
                int tt_val = tt->get_value_eval(depth_searched);
                if(tt_val == DEFAULT_EVAL){
                    score = -alpha_beta(-beta, -alpha, depth_left - 1, side ^ 1, starting_side, move, &line, transposition, use_pesto);
                    tt->add_value_eval(depth_searched, score);
                } else {
                    score = tt_val;
                }
            } else {
                score = -alpha_beta(-beta, -alpha, depth_left - 1, side ^ 1, starting_side, move, &line, transposition, use_pesto);

            }
            b->reverse_move(move);
            if(use_pesto)
                pesto->update_evaluation(move, 1);
            if(score >= beta){
                num_nodes++;
                return beta;
            } 
            if(score > alpha){

                principal_variation->moves[0] = move;
                memcpy(principal_variation->moves + 1, line.moves, line.len * sizeof(unsigned int));
                principal_variation->len = line.len + 1;

                if(depth_left == max_depth){
                    selected_move = move;

                    cout<<"-----------------\n";
                    cout<<"selected_move:";
                    MoveUtils::display(selected_move);
                    cout<<"score: "<<score<<endl;
                    cout<<"alpha: "<<alpha<<endl;
                    cout<<"-----------------\n";


                    cout<<"-----------------\n";
                    cout<<"current variation\n";
                    cout<<"principal_variation length: "<<principal_variation->len<<endl;
                    for(int i = 0 ; i < principal_variation->len ; i ++){
                        cout<<i<<": ";
                        MoveUtils::display(principal_variation->moves[i]);
                    }
                    cout<<"-----------------\n";
                } 
                alpha = score;
            } 
            no_moves_left = false;
        }

    }

    if(no_moves_left){
        if(b->get_bitboard()->attacked(side, b->get_king_location(side))){
            // return -CHECKMATE_EVAL(max_depth, depth_left);
            if(starting_side == side)
                return -CHECKMATE_EVAL(max_depth,depth_left);
            else 
                return CHECKMATE_EVAL(max_depth, depth_left);
        } else {
            return STALEMATE_EVAL;
        }
    }
    // while((move = mg.get_move()) != NO_MOVES_LEFT){
    //     if(move == INCREMENTING_MOVE_TYPE)
    //         continue;

    //     if(b->apply_move_if_legal(move)){
    //         if(use_pesto)
    //             pesto->update_evaluation(move, 0);
    //         int score = 0;
    //         int depth_searched = 10 - depth_left;
    //         if(transposition){
    //             int tt_val = tt->get_value_eval(depth_searched);
    //             if(tt_val == DEFAULT_EVAL){
    //                 score = -alpha_beta(-beta, -alpha, depth_left - 1, side ^ 1, starting_side, move, &line, transposition, use_pesto);
    //                 tt->add_value_eval(depth_searched, score);
    //             } else {
    //                 score = tt_val;
    //             }
    //         } else {
    //             score = -alpha_beta(-beta, -alpha, depth_left - 1, side ^ 1, starting_side, move, &line, transposition, use_pesto);

    //         }
    //         b->reverse_move(move);
    //         if(use_pesto)
    //             pesto->update_evaluation(move, 1);
    //         if(score >= beta){
    //             num_nodes++;
    //             return beta;
    //         } 
    //         if(score > alpha){

    //             principal_variation->moves[0] = move;
    //             memcpy(principal_variation->moves + 1, line.moves, line.len * sizeof(unsigned int));
    //             principal_variation->len = line.len + 1;

    //             if(depth_left == max_depth){
    //                 selected_move = move;

    //                 cout<<"-----------------\n";
    //                 cout<<"selected_move:";
    //                 MoveUtils::display(selected_move);
    //                 cout<<"score: "<<score<<endl;
    //                 cout<<"alpha: "<<alpha<<endl;
    //                 cout<<"-----------------\n";


    //                 cout<<"-----------------\n";
    //                 cout<<"current variation\n";
    //                 cout<<"principal_variation length: "<<principal_variation->len<<endl;
    //                 for(int i = 0 ; i < principal_variation->len ; i ++){
    //                     cout<<i<<": ";
    //                     MoveUtils::display(principal_variation->moves[i]);
    //                 }
    //                 cout<<"-----------------\n";
    //             } 
    //             alpha = score;
    //         } 
    //         no_moves_left = false;
    //     }

    // }

    // if(no_moves_left){
    //     if(b->get_bitboard()->attacked(side, b->get_king_location(side))){
    //         // return -CHECKMATE_EVAL(max_depth, depth_left);
    //         if(starting_side == side)
    //             return -CHECKMATE_EVAL(max_depth,depth_left);
    //         else 
    //             return CHECKMATE_EVAL(max_depth, depth_left);
    //     } else {
    //         return STALEMATE_EVAL;
    //     }
    // }
    if(depth_left == max_depth){
        cout<<"----------------------------\n";
        cout<<"final variation\n";
        cout<<"principal_variation length: "<<principal_variation->len<<endl;
        for(int i = 0 ; i < principal_variation->len ; i ++){
            cout<<i<<": ";
            MoveUtils::display(principal_variation->moves[i]);
        }
        cout<<"----------------------------\n";
    }
    return alpha;
}
int Search::quiesce(int alpha, int beta, int depth, unsigned int side,  unsigned int starting_side, pv_t* principal_variation, bool transposition, bool use_pesto){
    int static_eval;
    if(use_pesto)
        // static_eval =  pesto->get_evaluation(starting_side, side ^ 1);
        static_eval =  pesto->get_evaluation(WHITE, side);
    else
        static_eval = side == BLACK ? -eval->get_evaluation() : eval->get_evaluation();
    pv_t line;
    line.len = 0;
    // Stand Pat
    int best_value = static_eval;
    if( best_value >= beta ){
        num_nodes++;
        return best_value;
    }
    // int delta = 100;
    int delta = 0;
    if( best_value  > alpha )
        alpha = best_value;

    MoveGen mg = MoveGen(side);
    mg.set_gen_type(ONLY_CAPTURES);

    unsigned int move = 0;
    while((move = mg.get_move()) != NO_MOVES_LEFT){
        if(move == INCREMENTING_MOVE_TYPE || (!MoveUtils::is_capture(move) && !MoveUtils::is_ep_capture(move))){
            continue;
        }
        if(b->apply_move_if_legal(move)){
            if(use_pesto)
                pesto->update_evaluation(move, 0);
            int score = -quiesce(-beta, -alpha, 0, side ^ 1, starting_side, &line, transposition, use_pesto);
            b->reverse_move(move);
            if(use_pesto)
                pesto->update_evaluation(move, 1);
            if(score >= beta){
                num_nodes++;
                return score;
            }
            if(score > best_value){
                best_value = score;
            }
            if(score > alpha){
                principal_variation->moves[0] = move;
                memcpy(principal_variation->moves + 1, line.moves, line.len * sizeof(unsigned int));
                principal_variation->len = line.len + 1;

                alpha = score;
                // cout<<"selected move quiesce\n";
            }
        }

    }

    num_nodes++;
    return best_value;
}
int Search::evaluate(bool use_pesto){
    // return eval->get_material();
    if(!use_pesto)
        return eval->get_evaluation();
    else
        return pesto->get_evaluation(WHITE, WHITE);
}