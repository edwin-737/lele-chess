#include <unordered_set>
#include <atomic>
#include <cstring>
#include <assert.h>

#include "search.hpp"
#include "move_gen.hpp"
#include "const.hpp"
unsigned int Search::perft_loop(MoveGen mg, int original_depth, int depth_left, unsigned int side, unsigned int root_move, bool transposition){

    move_gen_state_t mg_state = mg.initialise(pPAWN);
    unsigned int move = 0;
    int num_legal_moves = 0;
    unsigned int ans = 0;
    while((move = mg.get_move(mg_state)) != NO_MOVES_LEFT){
        if(move == INCREMENTING_MOVE_TYPE){
            continue;
        }
        mg_state = mg.update(mg_state);
        if(b->apply_move_if_legal(move)){
            // if(MoveUtils::get_from(root_move) == d6 && MoveUtils::get_to(root_move) == e5 && MoveUtils::get_piece(root_move) == pQUEEN)
            //     cout<<"\n"<<MoveUtils::move_as_string(move)<<": LEGAL, root move is queen d6 e5\n";
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
        } else {
            // if(MoveUtils::get_from(root_move) == d6 && MoveUtils::get_to(root_move) == e5 && MoveUtils::get_piece(root_move) == pQUEEN)
            //     cout<<"\n"<<MoveUtils::move_as_string(move)<<": ILLEGAL, root move is queen d6 e5\n";
        }

    }
    return ans;
}
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
        return 1;
    } 
    else if(depth_left == original_depth - 1){
        if(MoveUtils::get_from(root_move) == a1 && MoveUtils::get_to(root_move) == a1){
            cout<<"invalid root_move: ";
            MoveUtils::display(root_move);
            cout<<"\n";
        }
        cout<<MoveUtils::move_as_string(root_move)<<": ";
    }
    MoveGen mg_captures = MoveGen(b, side);
    mg_captures.set_gen_type(ONLY_CAPTURES);
    unsigned int perft_captures = perft_loop(mg_captures, original_depth, depth_left, side, root_move, transposition);

    MoveGen mg_quiet = MoveGen(b, side);
    mg_quiet.set_gen_type(ONLY_QUIET);
    unsigned int perft_quiet = perft_loop(mg_quiet, original_depth, depth_left, side, root_move, transposition);

    unsigned int ans = perft_captures + perft_quiet;
    if(depth_left == original_depth - 1){
        cout<<ans<<endl;
    }
    stop = std::chrono::steady_clock::now();
    return perft_captures + perft_quiet;

}

int Search::alpha_beta_new_movegen(int alpha, int beta, int depth_left, unsigned int side, unsigned int starting_side, std::atomic<bool>& stop_flag, unsigned int root_move, pv_t* principal_variation, bool transposition, bool use_pesto, pv_t* prev_variation){

    int prev_eval = pesto->get_evaluation(WHITE, side);    
    pv_t line;
    line.len = 0;
    if(depth_left == 0)
        return quiesce(alpha, beta, 2, side, starting_side, stop_flag, &line, transposition, use_pesto);

    MoveGen mg_captures = MoveGen(b, side);
    MoveGen mg_quiet = MoveGen(b, side);
    mg_captures.set_gen_type(ONLY_CAPTURES);
    mg_quiet.set_gen_type(ONLY_QUIET);

    move_gen_state_t mg_captures_state = mg_captures.initialise(pPAWN);
    move_gen_state_t mg_quiet_state = mg_quiet.initialise(pPAWN);

    unsigned int move = 0;
    bool no_moves_left = true;
    std::unordered_set<unsigned int> seen_moves;
    if(prev_variation != nullptr && max_depth - depth_left < prev_variation->len){
        int score = 0;
        if(stop_flag.load()){
            pesto->set_evaluation(prev_eval);
            return score;
        }
        bool threefold = false;
        move = prev_variation->moves[max_depth-depth_left];
        if(b->apply_move_if_legal(move)){

            b->tt.increment_value_threefold();
            if(use_pesto)
                pesto->update_evaluation(move, 0);
            int depth_searched = 10 - depth_left;

            if(b->tt.get_value_threefold() == 3){
                score = STALEMATE_EVAL;
                threefold = true;
            } else {
                score = -alpha_beta_new_movegen(-beta, -alpha, depth_left - 1, side ^ 1, starting_side, stop_flag, move, &line, transposition, use_pesto);
            }

            b->tt.decrement_value_threefold();
            b->reverse_move(move);
            if(use_pesto)
                pesto->update_evaluation(move, 1);
            if(score >= beta){
                num_nodes++;
                return beta;
            } 
            if(score > alpha){
                if(!threefold){
                    principal_variation->moves[0] = move;
                    memcpy(principal_variation->moves + 1, line.moves, line.len * sizeof(unsigned int));
                    principal_variation->len = line.len + 1;
                } else {
                    principal_variation->moves[0] = move;
                    principal_variation->len = 1;
                }
                if(depth_left == max_depth){
                    selected_move = move;
                    cout<<"-----------------\n";
                    if(threefold) cout<<"stalemate\n";
                    display_selected_move(alpha, score);

                    if(verbose){
                        display_variation(principal_variation, CURRENT_VAR);
                    }
                } 
                alpha = score;
            } 
            no_moves_left = false;
        }
    }

    while((move = mg_captures.get_move(mg_captures_state)) != NO_MOVES_LEFT){
        bool threefold = false;
        if(move == INCREMENTING_MOVE_TYPE)
            continue;
        if(!(MoveUtils::is_capture(move) || MoveUtils::is_ep_capture(move)|| MoveUtils::is_capture_promotion(move))){
            get_board_instance()->get_bitboard()->display();
            MoveUtils::display(move);
        }
        assert((MoveUtils::is_capture(move) || MoveUtils::is_ep_capture(move) || MoveUtils::is_capture_promotion(move)));
        mg_captures_state = mg_captures.update(mg_captures_state);
        if(b->apply_move_if_legal(move)){
            b->tt.increment_value_threefold();
            int prev_eval = pesto->get_evaluation(starting_side, side);
            if(use_pesto)
                pesto->update_evaluation(move, 0);
            int score = 0;
            int depth_searched = 10 - depth_left;
            if(b->tt.get_value_threefold() == 3){
                score = STALEMATE_EVAL;
                threefold = true;
            } else {
                score = -alpha_beta_new_movegen(-beta, -alpha, depth_left - 1, side ^ 1, starting_side, stop_flag, move, &line, transposition, use_pesto);
            }

            b->tt.decrement_value_threefold();
            b->reverse_move(move);
            if(use_pesto)
                pesto->update_evaluation(move, 1);
            // ensures only the previous best move is returned
            if(stop_flag.load()){
                pesto->set_evaluation(prev_eval);
                return CHECKMATE_EVAL(max_depth, depth_left);
            }
            if(score >= beta){
                num_nodes++;
                return beta;
            } 
            if(score > alpha){

                principal_variation->moves[0] = move;
                memcpy(principal_variation->moves + 1, line.moves, line.len * sizeof(unsigned int));
                principal_variation->len = line.len + 1;

                if(!threefold){
                    principal_variation->moves[0] = move;
                    memcpy(principal_variation->moves + 1, line.moves, line.len * sizeof(unsigned int));
                    principal_variation->len = line.len + 1;
                } else {
                    principal_variation->moves[0] = move;
                    principal_variation->len = 1;
                }
                if(threefold){
                    selected_move = move;
                    cout<<"-----------------\n";
                    cout<<"stalemate\n";
                    display_selected_move(score, alpha);
                }
                else if(depth_left == max_depth){
                    selected_move = move;

                    cout<<"-----------------\n";
                    if(threefold) cout<<"stalemate\n";
                    display_selected_move(alpha, score);

                    if(verbose){
                        display_variation(principal_variation, CURRENT_VAR);
                    }
                } 
                alpha = score;
            } 
            no_moves_left = false;
        }
    }

    while((move = mg_quiet.get_move(mg_quiet_state)) != NO_MOVES_LEFT){
        bool threefold = false;
        if(move == INCREMENTING_MOVE_TYPE)
            continue;
        mg_quiet_state = mg_quiet.update(mg_quiet_state);
        if(seen_moves.find(move) != seen_moves.end()){
            cout<<"move seen: ";
            MoveUtils::display(move);
            continue;
        }
        if(b->apply_move_if_legal(move)){
            b->tt.increment_value_threefold();
            if(use_pesto)
                pesto->update_evaluation(move, 0);
            int score = 0;
            int depth_searched = 10 - depth_left;


            if(b->tt.get_value_threefold() == 3){
                score = STALEMATE_EVAL;
                threefold = true;
            } else {
                score = -alpha_beta_new_movegen(-beta, -alpha, depth_left - 1, side ^ 1, starting_side, stop_flag, move, &line, transposition, use_pesto);
            }
            b->tt.decrement_value_threefold();
            b->reverse_move(move);
            if(use_pesto)
                pesto->update_evaluation(move, 1);

            // ensures only the previous best move is returned
            if(stop_flag.load()){
                pesto->set_evaluation(prev_eval);
                return CHECKMATE_EVAL(max_depth, depth_left);
            }
            if(score >= beta){
                num_nodes++;
                return beta;
            } 
            if(score > alpha){

                principal_variation->moves[0] = move;
                memcpy(principal_variation->moves + 1, line.moves, line.len * sizeof(unsigned int));
                principal_variation->len = line.len + 1;

                if(!threefold){
                    principal_variation->moves[0] = move;
                    memcpy(principal_variation->moves + 1, line.moves, line.len * sizeof(unsigned int));
                    principal_variation->len = line.len + 1;
                } else {
                    principal_variation->moves[0] = move;
                    principal_variation->len = 1;
                }
                if(depth_left == max_depth){
                    selected_move = move;
                    selected_moves[max_depth + 2].push_back(selected_move);

                    cout<<"-----------------\n";
                    if(threefold) cout<<"stalemate\n";
                    display_selected_move(alpha, score);

                    if(verbose){
                        display_variation(principal_variation, CURRENT_VAR);
                    }
                } 
                alpha = score;
            } 
            no_moves_left = false;
        }
    }

    if(no_moves_left){
        if(b->get_bitboard()->attacked(side, b->get_king_location(side))){
            return CHECKMATE_EVAL(max_depth, depth_left);
        } else {
            return STALEMATE_EVAL;
        }
    }
    if(depth_left == max_depth){
        display_variation(principal_variation, SELECTED_VAR);
    }
    return alpha;
}

int Search::alpha_beta_new_movegen_short(int alpha, int beta, int depth_left, unsigned int side, unsigned int starting_side, std::atomic<bool>& stop_flag, unsigned int root_move, pv_t* pv, bool transposition, bool use_pesto, pv_t* prev_variation){

    MoveGen mg_captures = MoveGen(b, side);
    mg_captures.set_gen_type(ONLY_CAPTURES);
    alpha_beta_state_t state = {
        .alpha = alpha,
        .beta = beta
    };

    alpha_beta_state_t new_state = alpha_beta_new_movegen_loop(mg_captures, state, depth_left, side, starting_side, stop_flag, root_move, pv, transposition, use_pesto, prev_variation);
    if(new_state.is_beta_returned && new_state.score >= state.beta)
        return state.beta;
    if(new_state.is_score_returned)
        return state.score;

    MoveGen mg_quiet = MoveGen(b, side);
    mg_quiet.set_gen_type(ONLY_QUIET);
    new_state = alpha_beta_new_movegen_loop(mg_quiet, state, depth_left, side, starting_side, stop_flag, root_move, pv, transposition, use_pesto, prev_variation);
    if(new_state.is_beta_returned && new_state.score >= state.beta)
        return state.beta;
    if(new_state.is_score_returned)
        return new_state.score;

    return new_state.alpha;
}

alpha_beta_state_t Search::alpha_beta_new_movegen_prev_var_check(alpha_beta_state_t prev_state, int depth_left, unsigned int side, unsigned int starting_side, std::atomic<bool>& stop_flag, unsigned int root_move, pv_t* principal_variation, bool transposition, bool use_pesto, pv_t* prev_variation){
    pv_t line;
    line.len = 0;
    int prev_eval = pesto->get_evaluation(WHITE, side);
    unsigned int move = 0;
    bool no_moves_left = true;
    std::unordered_set<unsigned int> seen_moves;
    alpha_beta_state_t state = prev_state;
    if(prev_variation != nullptr && max_depth - depth_left < prev_variation->len){
        if(stop_flag.load()){
            pesto->set_evaluation(prev_eval);
            return state;
        }
        bool threefold = false;
        move = prev_variation->moves[max_depth-depth_left];
        if(b->apply_move_if_legal(move)){

            b->tt.increment_value_threefold();
            if(use_pesto)
                pesto->update_evaluation(move, 0);
            int depth_searched = 10 - depth_left;

            if(b->tt.get_value_threefold() == 3){
                state.score = STALEMATE_EVAL;
                threefold = true;
            } else {
                state.score = -alpha_beta_new_movegen(-state.beta, -state.alpha, depth_left - 1, side ^ 1, starting_side, stop_flag, move, &line, transposition, use_pesto);
            }

            b->tt.decrement_value_threefold();
            b->reverse_move(move);
            if(use_pesto)
                pesto->update_evaluation(move, 1);
            if(state.score >= state.beta){
                num_nodes++;
                state.is_beta_returned = true;
                return state;
            } 
            if(state.score > state.alpha){
                if(!threefold){
                    principal_variation->moves[0] = move;
                    memcpy(principal_variation->moves + 1, line.moves, line.len * sizeof(unsigned int));
                    principal_variation->len = line.len + 1;
                } else {
                    principal_variation->moves[0] = move;
                    principal_variation->len = 1;
                }
                if(depth_left == max_depth){
                    selected_move = move;
                    cout<<"-----------------\n";
                    cout<<"selected_move:";
                    if(threefold) cout<<"stalemate\n";
                    MoveUtils::display(selected_move);
                    cout<<"score: "<<state.score<<endl;
                    cout<<"alpha: "<<state.alpha<<endl;
                    cout<<"-----------------\n";

                    if(verbose){
                        cout<<"-----------------\n";
                        cout<<"current variation\n";
                        cout<<"principal_variation length: "<<principal_variation->len<<endl;
                        for(int i = 0 ; i < principal_variation->len ; i ++){
                            cout<<i<<": ";
                            MoveUtils::display(principal_variation->moves[i]);
                        }
                        stop = std::chrono::steady_clock::now();
                        duration<double> elapsed = stop - start;  // seconds as double (fractional)
                        cout<<"time elapsed: "<<elapsed.count()<<endl;
                        cout<<"node count: "<<num_nodes<<endl;
                        cout<<"-----------------\n";
                    }
                } 
                state.alpha = state.score;
            } 
            no_moves_left = false;
        }
    }
    return state;
}

alpha_beta_state_t Search::alpha_beta_new_movegen_loop(MoveGen mg, alpha_beta_state_t prev_state, int depth_left, unsigned int side, unsigned int starting_side, std::atomic<bool>& stop_flag, unsigned int root_move, pv_t* principal_variation, bool transposition, bool use_pesto, pv_t* prev_variation){

    pv_t line;
    line.len = 0;
    bool no_moves_left = true;

    alpha_beta_state_t state = prev_state;
    if(depth_left == 0){
        state.score = quiesce(state.alpha, state.beta, 2, side, starting_side, stop_flag, &line, transposition, use_pesto);
        state.is_score_returned = true;
        return state;
    }

    move_gen_state_t mg_state = mg.initialise(pPAWN);
    unsigned int move = 0;

    while((move = mg.get_move(mg_state)) != NO_MOVES_LEFT)
    {   
        bool threefold = false;
        if(move == INCREMENTING_MOVE_TYPE)
            continue;
        mg_state = mg.update(mg_state);
        if(b->apply_move_if_legal(move)){
            b->tt.increment_value_threefold();
            int prev_eval = pesto->get_evaluation(starting_side, side);
            if(use_pesto)
                pesto->update_evaluation(move, 0);
            int depth_searched = 10 - depth_left;
            if(b->tt.get_value_threefold() == 3){
                state.score = STALEMATE_EVAL;
                threefold = true;
            } else {
                state.score = -alpha_beta_new_movegen_short(-state.beta, -state.alpha, depth_left - 1, side ^ 1, starting_side, stop_flag, move, &line, transposition, use_pesto);
            }

            b->tt.decrement_value_threefold();
            b->reverse_move(move);
            if(use_pesto)
                pesto->update_evaluation(move, 1);
            // ensures only the previous best move is returned
            if(stop_flag.load()){
                pesto->set_evaluation(prev_eval);
                state.score = CHECKMATE_EVAL(max_depth, depth_left);
                return state;
            }
            if(state.score >= state.beta){
                num_nodes++;
                state.is_beta_returned = true;
                return state;
            } 
            if(state.score > state.alpha){

                principal_variation->moves[0] = move;
                memcpy(principal_variation->moves + 1, line.moves, line.len * sizeof(unsigned int));
                principal_variation->len = line.len + 1;

                if(!threefold){
                    principal_variation->moves[0] = move;
                    memcpy(principal_variation->moves + 1, line.moves, line.len * sizeof(unsigned int));
                    principal_variation->len = line.len + 1;
                } else {
                    principal_variation->moves[0] = move;
                    principal_variation->len = 1;
                }
                if(threefold){
                    selected_move = move;
                    cout<<"-----------------\n";
                    cout<<"stalemate\n";
                    cout<<"selected_move:";
                    MoveUtils::display(selected_move);
                    cout<<"score: "<<state.score<<endl;
                    cout<<"alpha: "<<state.alpha<<endl;
                    cout<<"-----------------\n";
                }
                else if(depth_left == max_depth){
                    selected_move = move;
                    cout<<"-----------------\n";
                    if(threefold) cout<<"stalemate\n";
                    display_selected_move(state.alpha, state.score);

                    if(verbose) display_variation(principal_variation, CURRENT_VAR);
                } 
                state.alpha = state.score;
            } 
            no_moves_left = false;
        }
    }
    return state;
}
int Search::quiesce(int alpha, int beta, int depth, unsigned int side,  unsigned int starting_side, std::atomic<bool>& stop_flag, pv_t* principal_variation, bool transposition, bool use_pesto){

    int static_eval =  pesto->get_evaluation(WHITE, side);
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

    move_gen_state_t mg_state = mg.initialise(pPAWN);
    unsigned int move = 0;

    while((move = mg.get_move(mg_state)) != NO_MOVES_LEFT){
        if(move == INCREMENTING_MOVE_TYPE){
            continue;
        }
        assert((MoveUtils::is_capture(move) || MoveUtils::is_ep_capture(move) || MoveUtils::is_capture_promotion(move)));
        mg_state = mg.update(mg_state);
        if(b->apply_move_if_legal(move)){
            b->tt.increment_value_threefold();
            if(use_pesto)
                pesto->update_evaluation(move, 0);
            int score = 0;

            if(b->tt.get_value_threefold() == 3){
                score = STALEMATE_EVAL;
            } else {
                score = -quiesce(-beta, -alpha, depth-1, side ^ 1, starting_side, stop_flag, &line, transposition, use_pesto);                
            }

            b->tt.decrement_value_threefold();
            b->reverse_move(move);
            if(use_pesto)
                pesto->update_evaluation(move, 1);

            // ensures only the previous best move is returned
            if(stop_flag.load()){
                pesto->set_evaluation(static_eval);
                return CHECKMATE_EVAL(max_depth, 1);
            }
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
                // stop =  std::chrono::steady_clock::now();
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

int Search::iterative_deepening(int depth, unsigned int side, unsigned int starting_side, std::atomic<bool>& stop_flag, bool transposition, bool use_pesto){

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
    unsigned int prev_move = 0;
    int prev_eval = pesto->get_evaluation(starting_side, side);    
    for(int d = 2 ; d <= depth ; d++){    
        num_nodes = 0;
        max_depth = d;
        cout<<"----------------------------\n";
        cout<<"depth = "<<d<<"\n";
        cout<<"----------------------------\n";
        cout<<"----------------------------\n";
        cout<<"starting_eval = "<<prev_eval<<"\n";
        display_selected_move(score, alpha);

        if(verbose){
            display_variation(prev_variation, PREVIOUS_VAR);
        }
        beta = prev_eval + 50;
        score = alpha_beta_new_movegen(alpha, beta, d, side, side, stop_flag, 0, principal_variation, false, true, prev_variation);
        display_selected_move(score, alpha);
        if(stop_flag.load()){
            pesto->set_evaluation(prev_eval);
            return score;
        }
        memcpy(prev_variation, principal_variation, sizeof(pv_t));
        pv1 = {};

        stop = std::chrono::steady_clock::now();
        duration<double> elapsed = stop - start;  // seconds as double (fractional)
        cout<<"time elapsed: "<<elapsed.count()<<endl;
        cout<<"node count: "<<num_nodes<<endl;
        pesto->set_evaluation(prev_eval);
    }
    return score;
}

int Search::get_evaluation(unsigned int side){
    return pesto->get_evaluation(WHITE, side);
}
int Search::init_evaluate(){
    return pesto->init_evaluate();
}
void Search::display_variation(pv_t* principal_variation, variation_type_e var_type){

    cout<<"-----------------\n";
    if(var_type == PREVIOUS_VAR)
        cout<<"previous variation\n";
    else if(var_type == CURRENT_VAR)
        cout<<"current variation\n";
    else
        cout<<"final variation\n";
    cout<<"principal_variation length: "<<principal_variation->len<<endl;
    for(int i = 0 ; i < principal_variation->len ; i ++){
        cout<<i<<": ";
        MoveUtils::display(principal_variation->moves[i]);
    }
    stop = std::chrono::steady_clock::now();
    duration<double> elapsed = stop - start;  // seconds as double (fractional)
    cout<<"time elapsed: "<<elapsed.count()<<endl;
    cout<<"node count: "<<num_nodes<<endl;
    cout<<"-----------------\n";
                    
}
void Search::display_selected_move(int alpha, int score){

    cout<<"-----------------\n";
    cout<<"selected_move:";
    MoveUtils::display(selected_move);
    cout<<"score: "<<score<<endl;
    cout<<"alpha: "<<alpha<<endl;
    cout<<"-----------------\n";
}
Board* Search::get_board_instance(){
    return b;
}