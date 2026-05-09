#include <unordered_set>
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
    MoveGen mg = MoveGen(b, side);
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
                unsigned int tt_val = b->tt.get_value_perft(depth_searched);
                if(!tt_val){
                    unsigned int perft_val = perft(original_depth, depth_left - 1, side ^ 1, move, true);
                    ans += perft_val;
                    b->tt.add_value_perft(depth_searched, perft_val);
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
unsigned int Search::perft_ordered(int original_depth, int depth_left, unsigned int side, unsigned int root_move, bool transposition){
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
    MoveGen mg_captures = MoveGen(b, side);
    MoveGen mg_quiet = MoveGen(b, side);
    mg_captures.set_gen_type(ONLY_CAPTURES);
    mg_quiet.set_gen_type(ONLY_QUIET);
    unsigned int move = 0;
    int num_legal_moves = 0;
    unsigned int ans = 0;
    while((move = mg_captures.get_move()) != NO_MOVES_LEFT){
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
                unsigned int tt_val = b->tt.get_value_perft(depth_searched);
                if(!tt_val){
                    unsigned int perft_val = perft(original_depth, depth_left - 1, side ^ 1, move, true);
                    ans += perft_val;
                    b->tt.add_value_perft(depth_searched, perft_val);
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
    while((move = mg_quiet.get_move()) != NO_MOVES_LEFT){
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
                unsigned int tt_val = b->tt.get_value_perft(depth_searched);
                if(!tt_val){
                    unsigned int perft_val = perft(original_depth, depth_left - 1, side ^ 1, move, true);
                    ans += perft_val;
                    b->tt.add_value_perft(depth_searched, perft_val);
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

int Search::alpha_beta(int alpha, int beta, int depth_left, unsigned int side, unsigned int starting_side, unsigned int root_move, pv_t* principal_variation, bool transposition, bool use_pesto, pv_t* prev_variation){
    // if(depth_left >= max_depth - 2 && MoveUtils::get_from(root_move) == e4 && MoveUtils::get_to(root_move) == d5){
    //     cout<<"depth_left: "<<depth_left<<"\n";
    // }
    pv_t line;
    line.len = 0;
    if(depth_left == 0)
        return quiesce(alpha, beta, 2, side, starting_side, &line, transposition, use_pesto);

    MoveGen mg_captures = MoveGen(b, side);
    MoveGen mg_quiet = MoveGen(b, side);
    mg_captures.set_gen_type(ONLY_CAPTURES);
    mg_quiet.set_gen_type(ONLY_QUIET);

    unsigned int move = 0;
    bool no_moves_left = true;

    std::unordered_set<unsigned int> seen_moves;
    if(prev_variation != nullptr && max_depth - depth_left < prev_variation->len){
        move = prev_variation->moves[max_depth-depth_left];
        if(b->apply_move_if_legal(move)){

            b->tt.increment_value_threefold();
            if(use_pesto)
                pesto->update_evaluation(move, 0);
            int score = 0;
            int depth_searched = 10 - depth_left;
            // if(transposition){
            //     int tt_val = b->tt.get_value_eval(depth_searched);
            //     if(tt_val == DEFAULT_EVAL){
            //         score = -alpha_beta(-beta, -alpha, depth_left - 1, side ^ 1, starting_side, move, &line, transposition, use_pesto);
            //         b->tt.add_value_eval(depth_searched, score);
            //     } else {
            //         score = tt_val;
            //     }
            // } else {

            if(b->tt.get_value_threefold() == 3){
                score = STALEMATE_EVAL;
            } else {
                score = -alpha_beta(-beta, -alpha, depth_left - 1, side ^ 1, starting_side, move, &line, transposition, use_pesto);
            }                
            // }

            // if(mg_captures.searched_move_found){
            //     searched_move_found = true;
            // }
            b->tt.decrement_value_threefold();
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
                    stop = high_resolution_clock::now();
                    duration<double> elapsed = stop - start;  // seconds as double (fractional)
                    cout<<"time elapsed: "<<elapsed.count()<<endl;
                    cout<<"node count: "<<num_nodes<<endl;
                    cout<<"-----------------\n";
                } 
                alpha = score;
            } 
            no_moves_left = false;
            // }
        }
    }

    while((move = mg_captures.get_move()) != NO_MOVES_LEFT){
        if(move == INCREMENTING_MOVE_TYPE)
            continue;
        if(b->apply_move_if_legal(move)){
            b->tt.increment_value_threefold();
            int prev_eval = pesto->get_evaluation(starting_side, side);
            if(use_pesto)
                pesto->update_evaluation(move, 0);
            int score = 0;
            int depth_searched = 10 - depth_left;
            if(b->tt.get_value_threefold() == 3){
                score = STALEMATE_EVAL;
            } else {
                score = -alpha_beta(-beta, -alpha, depth_left - 1, side ^ 1, starting_side, move, &line, transposition, use_pesto);
            }
            // if(!searched_move_found && depth_left == max_depth && MoveUtils::get_from(move) == e4){
            //     searched_move_depth = depth_left;
            //     searched_move_found = true;
            //     searched_move_eval = score;
            //     if(searched_move_eval >= INT_MAX){
            //         cout<<"searched_move_eval: "<<searched_move_eval<<"\n";
            //         cout<<"depth_left: "<<depth_left<<"\n";
            //     }

            // }
            b->tt.decrement_value_threefold();
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
                    stop = high_resolution_clock::now();
                    duration<double> elapsed = stop - start;  // seconds as double (fractional)
                    cout<<"time elapsed: "<<elapsed.count()<<endl;
                    cout<<"node count: "<<num_nodes<<endl;
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
        if(seen_moves.find(move) != seen_moves.end()){
            cout<<"move seen: ";
            MoveUtils::display(move);
            continue;
        }
        if(MoveUtils::get_from(move) == e4 && MoveUtils::get_to(move) == d5){
            searched_move_found = true;
        }
        if(b->apply_move_if_legal(move)){
            b->tt.increment_value_threefold();
            if(use_pesto)
                pesto->update_evaluation(move, 0);
            int score = 0;
            int depth_searched = 10 - depth_left;
            // if(transposition){
            //     int tt_val = b->tt.get_value_eval(depth_searched);
            //     if(tt_val == DEFAULT_EVAL){
            //         score = -alpha_beta(-beta, -alpha, depth_left - 1, side ^ 1, starting_side, move, &line, transposition, use_pesto);
            //         b->tt.add_value_eval(depth_searched, score);
            //     } else {
            //         score = tt_val;
            //     }
            // } else {

            if(b->tt.get_value_threefold() == 3){
                score = STALEMATE_EVAL;
            } else {
                score = -alpha_beta(-beta, -alpha, depth_left - 1, side ^ 1, starting_side, move, &line, transposition, use_pesto);
            }
            // }
            b->tt.decrement_value_threefold();
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
                    selected_moves[max_depth + 2].push_back(selected_move);

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
                    stop = high_resolution_clock::now();
                    duration<double> elapsed = stop - start;  // seconds as double (fractional)
                    cout<<"time elapsed: "<<elapsed.count()<<endl;
                    cout<<"node count: "<<num_nodes<<endl;
                    cout<<"-----------------\n";
                } 
                alpha = score;
            } 
            no_moves_left = false;
        }
    }

    if(no_moves_left){
        if(b->get_bitboard()->attacked(side, b->get_king_location(side))){
            return CHECKMATE_EVAL(max_depth, depth_left);
            // if(starting_side == side)
            //     return -CHECKMATE_EVAL(max_depth,depth_left);
            // else 
            //     return CHECKMATE_EVAL(max_depth, depth_left);
        } else {
            return STALEMATE_EVAL;
        }
    }
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
    if(use_pesto){
        static_eval =  pesto->get_evaluation(WHITE, side);
        if(depth == 0){
            return static_eval;
        }
    }
    else{
        return 0;
    }
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

    MoveGen mg = MoveGen(b, side);
    mg.set_gen_type(ONLY_CAPTURES);

    unsigned int move = 0;
    while((move = mg.get_move()) != NO_MOVES_LEFT){
        if(move == INCREMENTING_MOVE_TYPE || (!MoveUtils::is_capture(move) && !MoveUtils::is_ep_capture(move))){
            continue;
        }
        if(b->apply_move_if_legal(move)){
            b->tt.increment_value_threefold();
            if(use_pesto)
                pesto->update_evaluation(move, 0);
            int score = 0;

            if(b->tt.get_value_threefold() == 3){
                score = STALEMATE_EVAL;
            } else {
                score = -quiesce(-beta, -alpha, depth-1, side ^ 1, starting_side, &line, transposition, use_pesto);                
            }
            // int score = -quiesce(-beta, -alpha, depth-1, side ^ 1, starting_side, &line, transposition, use_pesto);
            if(move == searched_move){
                searched_move_eval = score;
                searched_move_found = true;
            }
            b->tt.decrement_value_threefold();
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

                // cout<<"-----------------\n";
                // cout<<"current variation\n";
                // cout<<"principal_variation length: "<<principal_variation->len<<endl;
                // for(int i = 0 ; i < principal_variation->len ; i ++){
                //     cout<<i<<": ";
                //     MoveUtils::display(principal_variation->moves[i]);
                // }
                // stop = high_resolution_clock::now();
                // duration<double> elapsed = stop - start;  // seconds as double (fractional)
                // cout<<"time elapsed: "<<elapsed.count()<<endl;
                // cout<<"node count: "<<num_nodes<<endl;
                // cout<<"-----------------\n";
                alpha = score;
            // }
            }
        }
    }

    num_nodes++;
    return best_value;
}

int Search::iterative_deepening(int depth, unsigned int side, unsigned int starting_side, bool transposition, bool use_pesto){

    cout<<"----------------------------\n";
    cout<<"iterative deepening\n";
    cout<<"----------------------------\n";

    int score = DEFAULT_EVAL;
    pv_t pv1 = {};
    pv_t pv2 = {};
    pv_t* principal_variation = &pv1;
    pv_t* prev_variation = &pv2;
    principal_variation->len = 0;
    prev_variation->len = 0;
    int alpha = -1e7;
    int beta = 1e7;

    for(int d = 2 ; d <= depth ; d++){
        int prev_eval = pesto->get_evaluation(starting_side, side);        
        num_nodes=0;
        max_depth = d;
        cout<<"----------------------------\n";
        cout<<"depth = "<<d<<"\n";
        cout<<"----------------------------\n";
        cout<<"----------------------------\n";
        cout<<"starting_eval = "<<prev_eval<<"\n";
        cout<<"selected_moves:\n";
        for(int i = 0 ; i < selected_moves[max_depth].size() ; i ++){
            cout<<"["<<i<<"]: ";
            MoveUtils::display(selected_moves[max_depth][i]);
        }
        cout<<"----------------------------\n";

        cout<<"-----------------\n";
        cout<<"prev variation\n";
        cout<<"prev_variation length: "<<prev_variation->len<<endl;
        for(int i = 0 ; i < prev_variation->len ; i ++){
            cout<<i<<": ";
            MoveUtils::display(prev_variation->moves[i]);
        }
        cout<<"-----------------\n";
        score = alpha_beta(alpha, beta, d, side, side, 0, principal_variation, false, true, prev_variation);
        if(searched_move_found){
            cout<<"-----------------\n";
            cout<<"searched_move: ";
            MoveUtils::display(searched_move);
            cout<<"searched_move_eval: "<<searched_move_eval<<"\n";
            cout<<"searched_move_depth: "<<searched_move_depth<<"\n";
            cout<<"-----------------\n";
        }
        searched_move_found = false;
        memcpy(prev_variation, principal_variation, sizeof(pv_t));
        pv1 = {};

        stop = high_resolution_clock::now();
        duration<double> elapsed = stop - start;  // seconds as double (fractional)
        cout<<"time elapsed: "<<elapsed.count()<<endl;
        cout<<"node count: "<<num_nodes<<endl;
        pesto->set_evaluation(prev_eval);
    }
    return score;
}
