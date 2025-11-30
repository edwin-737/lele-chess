#include "search.hpp"
#include "move_gen.hpp"
#include "move_set.hpp"
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
                unsigned int tt_val = tt->get_value(depth_searched);
                if(!tt_val){
                    unsigned int perft_val = perft(original_depth, depth_left - 1, side ^ 1, move, true);
                    ans += perft_val;
                    tt->add_value(depth_searched, perft_val);
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

int Search::alpha_beta(int alpha, int beta, int depth_left, unsigned int side, unsigned int starting_side, unsigned int root_move, pv_t* principal_variation){
    pv_t line;
    line.len = 0;
    if(depth_left == 0) 
        return quiesce(alpha, beta, depth_left, side, starting_side);

    MoveGen mg = MoveGen(side);
    unsigned int move = 0;
    bool no_moves_left = true;

    while((move = mg.get_move()) != NO_MOVES_LEFT){
        if(move == INCREMENTING_MOVE_TYPE)
            continue;

        if(b->apply_move_if_legal(move)){
            int score = -alpha_beta(-beta, -alpha, depth_left - 1, side ^ 1, starting_side, move, &line);

            b->reverse_move(move);
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
            return CHECKMATE_EVAL(max_depth,depth_left);
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
int Search::quiesce(int alpha, int beta, int depth, unsigned int side,  unsigned int starting_side){
    int static_eval = side == BLACK ? -evaluate() : evaluate();

    // Stand Pat
    int best_value = static_eval;
    if( best_value >= beta ){
        num_nodes++;
        return best_value;
    }
    // int delta = 100;
    int delta = 0;
    if( best_value  + delta > alpha )
        alpha = best_value;

    // MoveGen mg = MoveGen(side, 0, true);
    MoveGen mg = MoveGen(side);
    mg.set_gen_type(ONLY_CAPTURES);

    unsigned int move = 0;
    while((move = mg.get_move()) != NO_MOVES_LEFT){
        if(move == INCREMENTING_MOVE_TYPE || (!MoveUtils::is_capture(move) && !MoveUtils::is_ep_capture(move))){
            continue;
        }
        if(b->apply_move_if_legal(move)){
            int score = -quiesce(-beta, -alpha, 0, side ^ 1, starting_side);
            b->reverse_move(move);
            if(score >= beta){
                num_nodes++;
                return score;
            }
            if(score > best_value)
                best_value = score;
            if(score + delta > alpha)
                alpha = score;
        }

    }

    num_nodes++;
    return best_value;
}
int Search::evaluate(){
    // return eval->get_material();
    return eval->get_evaluation();
}