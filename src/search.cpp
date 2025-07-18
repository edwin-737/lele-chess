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
        num_nodes++;
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
        // MoveUtils::display(move);
        if(b->apply_move_if_legal(move)){
            uint64 tt_val = b->get_transposition_table_value(b->get_current_hash_val(), depth_left);
            if(tt_val > 0){
                ans += tt_val;
            } else {
                unsigned int perft_val = perft(original_depth, depth_left - 1, side ^ 1, move);
                ans += perft_val;
                b->update_transposition_table(b->get_current_hash_val(), perft_val, depth_left);
            };

            b->reverse_move(move);
            // unsigned int perft_val = perft(original_depth, depth_left - 1, side ^ 1, move);
            // ans += perft_val;
            // b->reverse_move(move);
        } 
    }
    if(depth_left == original_depth - 1){
        cout<<ans<<endl;
    }
    return ans;
}

int Search::alpha_beta(int alpha, int beta, int depth_left, unsigned int side, unsigned int starting_side, unsigned int root_move){
    if(depth_left == 0) 
        return quiesce(alpha, beta, depth_left, side, starting_side);

    MoveGen mg = MoveGen(side);
    unsigned int move = 0;
    bool no_moves_left = true;

    while((move = mg.get_move()) != NO_MOVES_LEFT){
        if(move == INCREMENTING_MOVE_TYPE)
            continue;

        if(b->apply_move_if_legal(move)){
            int score = -alpha_beta(-beta, -alpha, depth_left - 1, side ^ 1, starting_side);

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
        if(b->get_bitboard()->attacked(side, b->get_king_location(side))){
            return -100000 + (max_depth - depth_left); 
        } else {
            return 0;
        }
    }
    return alpha;
}
int Search::quiesce(int alpha, int beta, int depth, unsigned int side,  unsigned int starting_side){
    int static_eval = side == BLACK ? -evaluate() : evaluate();

    // Stand Pat
    int best_value = static_eval;
    if( best_value >= beta )
        return best_value;
    if( best_value > alpha )
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
float Search::evaluate(){
    return eval->get_material();
}