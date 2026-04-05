#include "pesto.hpp"
#include "const.hpp"
#include "move.hpp"
#include "board_squares.hpp"
#include "utils.hpp"
using namespace BoardSquares;
PestoEvaluation* PestoEvaluation::instanceptr=nullptr;

/*PeSTO adopted from https://www.chessprogramming.org/PeSTO%27s_Evaluation_Function*/
PestoEvaluation* PestoEvaluation::get_instance(unsigned int side_to_move){
    if(instanceptr == nullptr)
        instanceptr = new PestoEvaluation(side_to_move);
    return instanceptr;
}
void PestoEvaluation::init_tables(){
    int pc, p, sq;
    for (p = PAWN; p <= KING; p++) {
        for (sq = 0; sq < 64; sq++) {
            // mg_table[pc]  [sq] = mg_value[p] + mg_pesto_table[p][sq];
            // eg_table[pc]  [sq] = eg_value[p] + eg_pesto_table[p][sq];
            // mg_table[pc+1][sq] = mg_value[p] + mg_pesto_table[p][FLIP(sq)];
            // eg_table[pc+1][sq] = eg_value[p] + eg_pesto_table[p][FLIP(sq)];
            mg_table[PIECE_WITH_SIDE(p, WHITE)][FLIP(sq)] = mg_value[p] + mg_pesto_table[p][sq];
            eg_table[PIECE_WITH_SIDE(p, WHITE)][FLIP(sq)] = eg_value[p] + eg_pesto_table[p][sq];
            mg_table[PIECE_WITH_SIDE(p, BLACK)][sq] = mg_value[p] + mg_pesto_table[p][sq];
            eg_table[PIECE_WITH_SIDE(p, BLACK)][sq] = eg_value[p] + eg_pesto_table[p][sq];
            // cout<<"pc: "<<pc<<"sq: "<<sq<<", mg_table[pc][sq]: "<<mg_table[pc][sq]<<", eg_table[pc]: "<<eg_table[pc][sq]<<"\n";
            // cout<<"pc + 1: "<<pc + 1<<", sq: "<<sq<<", mg_table[pc + 1][sq]: "<<mg_table[pc + 1][sq]<<", eg_table[pc + 1]: "<<eg_table[pc + 1][sq]<<"\n";
        }
    }
}
int PestoEvaluation::calculate_evaluation(){
    int _mg[2], _eg[2], _gamePhase=0;
    _mg[WHITE] = 0;
    _mg[BLACK] = 0;
    _eg[WHITE] = 0;
    _eg[BLACK] = 0;

    for(int side = 0 ; side < NUM_SIDES ; side ++){
        for(int piece = 0 ; piece < NUM_PIECE_TYPES ; piece ++){
            uint64 b = bb->piece_boards[side][piece];
            for(int sq = 0 ; sq < NUM_SQUARES ; sq ++){
                uint64 sq_bb = get_square_bitboard(sq);
                if(sq_bb & b){
                    unsigned int piece_with_side = PIECE_WITH_SIDE(piece, side);
                    _mg[side] += mg_table[piece_with_side][sq];
                    _eg[side] += eg_table[piece_with_side][sq];
                    _gamePhase += gamephaseInc[piece_with_side];
                }
            }

        }
    }
    
    // cout<<"[init_evaluation] init_evaluate mg: "<<mg[WHITE]<<" "<<mg[BLACK]<<"\n";
    // cout<<"[init_evaluation] init_evaluate eg: "<<eg[WHITE]<<" "<<eg[BLACK]<<"\n";
    /* tapered eval */
    int mgScore = _mg[WHITE] - _mg[BLACK];
    int egScore = _eg[WHITE] - _eg[BLACK];
    int mgPhase = _gamePhase;
    if (mgPhase > 24) mgPhase = 24; /* in case of early promotion */
    int egPhase = 24 - mgPhase;
    // cout<<"[init_evaluation] "<<"mgPhase: "<<mgPhase<<" egPhase: "<<egPhase<<"\n";
    int initial_eval = ((mgScore * mgPhase) + (egScore * egPhase));
    return initial_eval;
}
int PestoEvaluation::init_evaluate(){

    mg[WHITE] = 0;
    mg[BLACK] = 0;
    eg[WHITE] = 0;
    eg[BLACK] = 0;

    for(int side = 0 ; side < NUM_SIDES ; side ++){
        for(int piece = 0 ; piece < NUM_PIECE_TYPES ; piece ++){
            uint64 b = bb->piece_boards[side][piece];
            for(int sq = 0 ; sq < NUM_SQUARES ; sq ++){
                uint64 sq_bb = get_square_bitboard(sq);
                if(sq_bb & b){
                    unsigned int piece_with_side = PIECE_WITH_SIDE(piece, side);
                    mg[side] += mg_table[piece_with_side][sq];
                    eg[side] += eg_table[piece_with_side][sq];
                    gamePhase += gamephaseInc[piece_with_side];
                }
            }

        }
    }
    
    // cout<<"[init_evaluation] init_evaluate mg: "<<mg[WHITE]<<" "<<mg[BLACK]<<"\n";
    // cout<<"[init_evaluation] init_evaluate eg: "<<eg[WHITE]<<" "<<eg[BLACK]<<"\n";
    /* tapered eval */

    int mgScore = mg[WHITE] - mg[BLACK];
    int egScore = eg[WHITE] - eg[BLACK];
    // if(side_to_move == BLACK){
    //     mgScore = mg[BLACK] - mg[WHITE];
    //     egScore = eg[BLACK] - eg[WHITE];
    // } 
    int mgPhase = gamePhase;
    if (mgPhase > 24) mgPhase = 24; /* in case of early promotion */
    int egPhase = 24 - mgPhase;
    // cout<<"[init_evaluation] "<<"mgPhase: "<<mgPhase<<" egPhase: "<<egPhase<<"\n";
    int initial_eval = ((mgScore * mgPhase) + (egScore * egPhase));
    // cout<<"eval: "<<initial_eval<<"\n";
    this->evaluation[WHITE] = initial_eval;
    this->evaluation[BLACK] = -initial_eval;
    return initial_eval;
}
int PestoEvaluation::update_evaluation(unsigned int move, int reverse)
{   
    int side = MoveUtils::get_side(move);
    int side_to_move = OTHER(MoveUtils::get_side(move));
    int prev_side_to_move = OTHER(side_to_move);
    int from = MoveUtils::get_from(move);
    int to = MoveUtils::get_to(move);

    int pc = MoveUtils::get_piece(move);
    int other_side = OTHER(side);
    int piece_with_side = PIECE_WITH_SIDE(pc, side);
    // reverse == 1, reverse_mult = -1
    // reverse == 0, reverse_mult = 1
    int reverse_mult = 2 * (reverse ^ 1) - 1;
    // cout<<"[update_evaluation]: reverse_mult "<<reverse_mult<<"\n";
    // cout<<"[update_evaluation]: mg[WHITE] "<<mg[WHITE]<<"\n";
    // cout<<"[update_evaluation]: mg[BLACK] "<<mg[BLACK]<<"\n";
    // cout<<"[update_evaluation]: eg[WHITE] "<<eg[WHITE]<<"\n";
    // cout<<"[update_evaluation]: eg[BLACK] "<<eg[BLACK]<<"\n";
    if(MoveUtils::is_quiet(move) || MoveUtils::is_double_pawn_push(move)){
        // cout<<"[update_evaluation]: is_quiet"<<"\n";
        // cout<<"[update_evaluation]: pc from mg change: "<<MoveUtils::piece_as_string(pc)<<" "<<(mg_table[piece_with_side][from] * reverse_mult)<<"\n";
        // cout<<"[update_evaluation]: pc to mg change: "<<MoveUtils::piece_as_string(pc)<<" "<<-(mg_table[piece_with_side][to] * reverse_mult)<<"\n";
        // cout<<"[update_evaluation]: pc from eg change: "<<MoveUtils::piece_as_string(pc)<<" "<<(eg_table[piece_with_side][from] * reverse_mult)<<"\n";
        // cout<<"[update_evaluation]: pc to eg change: "<<MoveUtils::piece_as_string(pc)<<" "<<-(eg_table[piece_with_side][to] * reverse_mult)<<"\n";
        // cout<<"PestoEvaluation::update_evaluation: double_pawn_push\n";
        // cout<<"Before\nmg[WHITE]: "<<mg[WHITE]<<"\nmg[BLACK]: "<<mg[BLACK]<<"\n";
        mg[side] -= (mg_table[piece_with_side][from] * reverse_mult);
        eg[side] -= (eg_table[piece_with_side][from] * reverse_mult);

        mg[side] += (mg_table[piece_with_side][to] * reverse_mult);
        eg[side] += (eg_table[piece_with_side][to] * reverse_mult);
        // cout<<"After\nmg[WHITE]: "<<mg[WHITE]<<"\nmg[BLACK]: "<<mg[BLACK]<<"\n";
    }
    else if(MoveUtils::is_capture(move)){
        int captured_pc = MoveUtils::get_captured_piece(move);
        int other_side = side ^ 1;
        int captured_piece_with_side = PIECE_WITH_SIDE(captured_pc, other_side);
        mg[other_side] -= (mg_table[captured_piece_with_side][to] * reverse_mult);
        eg[other_side] -= (eg_table[captured_piece_with_side][to] * reverse_mult);

        gamePhase -= (gamephaseInc[captured_piece_with_side] * reverse_mult);

        mg[side] -= (mg_table[piece_with_side][from] * reverse_mult);
        eg[side] -= (eg_table[piece_with_side][from] * reverse_mult);

        mg[side] += (mg_table[piece_with_side][to] * reverse_mult);
        eg[side] += (eg_table[piece_with_side][to] * reverse_mult);

        // cout<<"[update_evaluation]: gamePhase: "<<gamePhase<<"\n";
        // cout<<"[update_evaluation]: pc from mg: "<<MoveUtils::piece_as_string(pc)<<" "<<(mg_table[piece_with_side][from] * reverse_mult)<<"\n";
        // cout<<"[update_evaluation]: pc to mg: "<<MoveUtils::piece_as_string(pc)<<" "<<(mg_table[piece_with_side][to] * reverse_mult)<<"\n";
        // cout<<"[update_evaluation]: pc from eg: "<<MoveUtils::piece_as_string(pc)<<" "<<(eg_table[piece_with_side][from] * reverse_mult)<<"\n";
        // cout<<"[update_evaluation]: pc to eg: "<<MoveUtils::piece_as_string(pc)<<" "<<(eg_table[piece_with_side][to] * reverse_mult)<<"\n";
        // cout<<"[update_evaluation]: captured_pc to mg change: "<<MoveUtils::piece_as_string(captured_pc)<<" "<<-(mg_table[captured_piece_with_side][to] * reverse_mult)<<"\n";
        // cout<<"[update_evaluation]: captured_pc to eg change: "<<MoveUtils::piece_as_string(captured_pc)<<" "<<-(eg_table[captured_piece_with_side][to] * reverse_mult)<<"\n";

    } else if(MoveUtils::is_castle(move)){

        if(MoveUtils::is_king_castle(move) && side == WHITE){
            mg[side] -= (mg_table[WHITE_KING][e1] * reverse_mult);
            mg[side] += (mg_table[WHITE_KING][g1] * reverse_mult);
            mg[side] -= (mg_table[WHITE_ROOK][h1] * reverse_mult);
            mg[side] += (mg_table[WHITE_ROOK][f1] * reverse_mult);

            eg[side] -= (eg_table[WHITE_KING][e1] * reverse_mult);
            eg[side] += (eg_table[WHITE_KING][g1] * reverse_mult);
            eg[side] -= (eg_table[WHITE_ROOK][h1] * reverse_mult);
            eg[side] += (eg_table[WHITE_ROOK][f1] * reverse_mult);
        } else if(MoveUtils::is_queen_castle(move) && side == WHITE){
            mg[side] -= (mg_table[WHITE_KING][e1] * reverse_mult);
            mg[side] += (mg_table[WHITE_KING][c1] * reverse_mult);
            mg[side] -= (mg_table[WHITE_ROOK][a1] * reverse_mult);
            mg[side] += (mg_table[WHITE_ROOK][d1] * reverse_mult);

            eg[side] -= (eg_table[WHITE_KING][e1] * reverse_mult);
            eg[side] += (eg_table[WHITE_KING][c1] * reverse_mult);
            eg[side] -= (eg_table[WHITE_ROOK][a1] * reverse_mult);
            eg[side] += (eg_table[WHITE_ROOK][d1] * reverse_mult);
        } else if(MoveUtils::is_king_castle(move) && side == BLACK){
            mg[side] -= (mg_table[BLACK_KING][e8] * reverse_mult);
            mg[side] += (mg_table[BLACK_KING][g8] * reverse_mult);
            mg[side] -= (mg_table[BLACK_ROOK][h8] * reverse_mult);
            mg[side] += (mg_table[BLACK_ROOK][f8] * reverse_mult);

            eg[side] -= (eg_table[BLACK_KING][e8] * reverse_mult);
            eg[side] += (eg_table[BLACK_KING][g8] * reverse_mult);
            eg[side] -= (eg_table[BLACK_ROOK][h8] * reverse_mult);
            eg[side] += (eg_table[BLACK_ROOK][f8] * reverse_mult);
        } else if(MoveUtils::is_queen_castle(move) && side == BLACK){
            mg[side] -= (mg_table[BLACK_KING][e8] * reverse_mult);
            mg[side] += (mg_table[BLACK_KING][c8] * reverse_mult);
            mg[side] -= (mg_table[BLACK_ROOK][a8] * reverse_mult);
            mg[side] += (mg_table[BLACK_ROOK][d8] * reverse_mult);

            eg[side] -= (eg_table[BLACK_KING][e8] * reverse_mult);
            eg[side] += (eg_table[BLACK_KING][c8] * reverse_mult);
            eg[side] -= (eg_table[BLACK_ROOK][a8] * reverse_mult);
            eg[side] += (eg_table[BLACK_ROOK][d8] * reverse_mult);
        } 
    } else if(MoveUtils::is_ep_capture(move)){

        int captured_file = MoveUtils::get_ep_capture_file(move);
        int captured_sq =  side == WHITE ? a5 + captured_file : a4 + captured_file;
        if(side == WHITE)
            captured_sq = FLIP(captured_sq);
        int other_side_pawn = PIECE_WITH_SIDE(pPAWN, other_side);
        int side_pawn = PIECE_WITH_SIDE(pPAWN, side);
        mg[side] -= (mg_table[side_pawn][from] * reverse_mult);
        mg[side] += (mg_table[side_pawn][to] * reverse_mult);
        mg[other_side] -= (mg_table[other_side_pawn][captured_sq] * reverse_mult);

        eg[side] -= (eg_table[side_pawn][from] * reverse_mult);
        eg[side] += (eg_table[side_pawn][to] * reverse_mult);
        eg[other_side] -= (eg_table[other_side_pawn][captured_sq] * reverse_mult);
    } else if(MoveUtils::is_promotion(move)){
        mg[side] -= (mg_table[pPAWN][from] * reverse_mult);
        eg[side] -= (eg_table[pPAWN][from] * reverse_mult);
        gamePhase -= (gamephaseInc[piece_with_side] * reverse_mult);
        if(MoveUtils::is_knight_promotion(move)){            
            mg[side] += (mg_table[PIECE_WITH_SIDE(pKNIGHT, side)][to] * reverse_mult);
            eg[side] += (eg_table[PIECE_WITH_SIDE(pKNIGHT, side)][to] * reverse_mult);
            gamePhase += (gamephaseInc[PIECE_WITH_SIDE(pKNIGHT, side)] * reverse_mult);
        } else if(MoveUtils::is_bishop_promotion(move)){
            mg[side] += (mg_table[PIECE_WITH_SIDE(pBISHOP, side)][to] * reverse_mult);
            eg[side] += (eg_table[PIECE_WITH_SIDE(pBISHOP, side)][to] * reverse_mult);
            gamePhase += (gamephaseInc[PIECE_WITH_SIDE(pBISHOP, side)] * reverse_mult);
        } else if(MoveUtils::is_rook_promotion(move)){
            mg[side] += (mg_table[PIECE_WITH_SIDE(pROOK, side)][to] * reverse_mult);
            eg[side] += (eg_table[PIECE_WITH_SIDE(pROOK, side)][to] * reverse_mult);
            gamePhase += (gamephaseInc[PIECE_WITH_SIDE(pROOK, side)] * reverse_mult);
        } else if(MoveUtils::is_queen_promotion(move)){
            mg[side] += (mg_table[PIECE_WITH_SIDE(pQUEEN, side)][to] * reverse_mult);
            eg[side] += (eg_table[PIECE_WITH_SIDE(pQUEEN, side)][to] * reverse_mult);
            gamePhase += (gamephaseInc[PIECE_WITH_SIDE(pQUEEN, side)] * reverse_mult);
        }


    } else if(MoveUtils::is_capture_promotion(move)){

        int captured_pc = MoveUtils::get_captured_piece(move);
        int other_side = side ^ 1;
        int captured_piece_with_side = PIECE_WITH_SIDE(captured_pc, other_side);
        mg[side] -= (mg_table[pPAWN][from] * reverse_mult);
        eg[side] -= (eg_table[pPAWN][from] * reverse_mult);
        mg[other_side] -= (mg_table[MoveUtils::get_captured_piece(move)][to] * reverse_mult);
        eg[other_side] -= (eg_table[MoveUtils::get_captured_piece(move)][to] * reverse_mult);
        gamePhase -= (gamephaseInc[piece_with_side] * reverse_mult);
        gamePhase -= (gamephaseInc[captured_piece_with_side] * reverse_mult);
        if(MoveUtils::is_knight_promotion(move)){            
            mg[side] += (mg_table[PIECE_WITH_SIDE(pKNIGHT, side)][to] * reverse_mult);
            eg[side] += (eg_table[PIECE_WITH_SIDE(pKNIGHT, side)][to] * reverse_mult);
            gamePhase += (gamephaseInc[PIECE_WITH_SIDE(pKNIGHT, side)] * reverse_mult);
        } else if(MoveUtils::is_bishop_promotion(move)){
            mg[side] += (mg_table[PIECE_WITH_SIDE(pBISHOP, side)][to] * reverse_mult);
            eg[side] += (eg_table[PIECE_WITH_SIDE(pBISHOP, side)][to] * reverse_mult);
            gamePhase += (gamephaseInc[PIECE_WITH_SIDE(pBISHOP, side)] * reverse_mult);
        } else if(MoveUtils::is_rook_promotion(move)){
            mg[side] += (mg_table[PIECE_WITH_SIDE(pROOK, side)][to] * reverse_mult);
            eg[side] += (eg_table[PIECE_WITH_SIDE(pROOK, side)][to] * reverse_mult);
            gamePhase += (gamephaseInc[PIECE_WITH_SIDE(pROOK, side)] * reverse_mult);
        } else if(MoveUtils::is_queen_promotion(move)){
            mg[side] += (mg_table[PIECE_WITH_SIDE(pQUEEN, side)][to] * reverse_mult);
            eg[side] += (eg_table[PIECE_WITH_SIDE(pQUEEN, side)][to] * reverse_mult);
            gamePhase += (gamephaseInc[PIECE_WITH_SIDE(pQUEEN, side)] * reverse_mult);
        }

    }

    /* tapered eval */
    int mgScore = mg[WHITE] - mg[BLACK];
    int egScore = eg[WHITE] - eg[BLACK];
    if(side_to_move == BLACK){
        mgScore = mg[BLACK] - mg[WHITE];
        egScore = eg[BLACK] - eg[WHITE];
    } 
    int mgPhase = gamePhase;
    if (mgPhase > 24) mgPhase = 24; /* in case of early promotion */
    int egPhase = 24 - mgPhase;
    // cout<<"[update_evaluation] "<<"mgPhase: "<<mgPhase<<"\n";
    // cout<<"[update_evaluation] "<<"egPhase: "<<egPhase<<"\n";
    this->evaluation[side_to_move] = (mgScore * mgPhase + egScore * egPhase);
    this->evaluation[side_to_move ^ 1] = -this->evaluation[side_to_move];
    return this->evaluation[side_to_move];
}
int PestoEvaluation::get_evaluation(unsigned int starting_side, unsigned int side){

    // cout<<"[get_evaluation]: mg[WHITE] "<<mg[WHITE]<<"\n";
    // cout<<"[get_evaluation]: mg[BLACK] "<<mg[BLACK]<<"\n";
    // cout<<"[get_evaluation]: eg[WHITE] "<<eg[WHITE]<<"\n";
    // cout<<"[get_evaluation]: eg[BLACK] "<<eg[BLACK]<<"\n";
    // return this->evaluation[WHITE];
    // return this->evaluation[starting_side];
    return side == WHITE ? evaluation[WHITE] : -evaluation[WHITE];
    // return side == starting_side ? evaluation[side] : -evaluation[side];
    // return evaluation[side];
    // return evaluation;
    // return evaluation;
}
void PestoEvaluation::set_evaluation(int val){
    evaluation[WHITE] = val;
}