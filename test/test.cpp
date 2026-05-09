
#include <vector>
#include <algorithm>
#include <stdlib.h>
#include <string>
#include <catch2/catch_test_macros.hpp>

#include "const.hpp"
#include "move.hpp"
#include "board_squares.hpp"
#include "pesto.hpp"
#include "search.hpp"
#include "magics.hpp"
#include "bitboard.hpp"
#include "board_info.hpp"
#include "move_set.hpp"
#include "transposition_table.hpp"
#include "utils.hpp"
#include "move_gen.hpp"
using namespace std::chrono;
using namespace std;
#define NONE 0
#define FEN 1
#define DEPTH 2

bool are_pieces_in_correct_locations(Board* b, vector<vector<int>> expected_piece_locations, int piece){

    bool correct_locations = true;
    for(int side = 0 ; side < NUM_SIDES ; side ++){
        uint64 piece_board = b->get_bitboard()->piece_boards[side][piece];
        int piece_location = 64;
        while((piece_location = bit_scan_forward(piece_board)) != -1){
            // remove "piece_location" square piece_location the piece_board
            piece_board ^= get_square_bitboard(piece_location);
            bool found_piece_location = false;
            for(int idx = 0 ; idx < expected_piece_locations[side].size() ; idx ++){
                if(expected_piece_locations[side][idx] == piece_location){
                    found_piece_location = true;
                    break;
                }
            }
            if(!found_piece_location)
                correct_locations = false;
        }
    }
    return correct_locations;
}

bool are_move_vectors_equal(vector<unsigned int> actual_moves, vector<unsigned int> expected_moves){
    if(actual_moves.size() != expected_moves.size())
        return false;
    sort(actual_moves.begin(), actual_moves.end());
    sort(expected_moves.begin(), expected_moves.end());
    for(int i = 0 ; i < actual_moves.size() ; i ++){
        if(actual_moves[i] != expected_moves[i])
            return false;
    }
    return true;
}
TEST_CASE("Piece locations for starting position", "[Parsing FEN]"){

    BoardSquares::init_files();
    BoardSquares::init_ranks();
    BoardSquares::init_squares();
    MoveSet::set_attack_sets();
    MoveSet::init_attack_masks();


    const char* fen_path = "./positions/starting_position.txt";
    Bitboard _bb = Bitboard();
    BoardInfo _bi = BoardInfo();
    Bitboard* bb = &_bb;
    BoardInfo* bi = &_bi;
    Board _b = Board(fen_path, bb, bi);
    Board* b = &_b;
    b->get_bitboard()->display();

    SECTION("Pawn locations"){
        vector<vector<int>> expected_pawn_locations = {
            {a2, b2, c2, d2, e2, f2, g2, h2},
            {a7, b7, c7, d7, e7, f7, g7, h7}
        };
        bool correct_piece_locations = are_pieces_in_correct_locations(b, expected_pawn_locations, pPAWN);
        REQUIRE(correct_piece_locations);
    }
    SECTION("Knight locations"){
        vector<vector<int>> expected_knight_locations = {
            {b1, g1},
            {b8, g8}
        };
        bool correct_piece_locations = are_pieces_in_correct_locations(b, expected_knight_locations, pKNIGHT);
        REQUIRE(correct_piece_locations);
    }
    SECTION("Bishop locations"){
        vector<vector<int>> expected_bishop_locations = {
            {c1, f1},
            {c8, f8}
        };
        bool correct_piece_locations = are_pieces_in_correct_locations(b, expected_bishop_locations, pBISHOP);
        REQUIRE(correct_piece_locations);
    }
    SECTION("Rook locations"){
        vector<vector<int>> expected_rook_locations = {
            {a1, h1},
            {a8, h8}
        };
        bool correct_piece_locations = are_pieces_in_correct_locations(b, expected_rook_locations, pROOK);
        REQUIRE(correct_piece_locations);
    }
    SECTION("Queen locations"){
        vector<vector<int>> expected_queen_locations = {
            {d1},
            {d8}
        };
        bool correct_piece_locations = are_pieces_in_correct_locations(b, expected_queen_locations, pQUEEN);
        REQUIRE(correct_piece_locations);
    }
    SECTION("King locations"){
        vector<vector<int>> expected_king_locations = {
            {e1},
            {e8}
        };
        bool correct_piece_locations = are_pieces_in_correct_locations(b, expected_king_locations, pKING);
        REQUIRE(correct_piece_locations);
    }
}

TEST_CASE("Piece locations for fen test position", "[Parsing FEN]"){


    const char* fen_path = "./positions/fen_test_position.txt";
    Bitboard _bb = Bitboard();
    BoardInfo _bi = BoardInfo();
    Bitboard* bb = &_bb;
    BoardInfo* bi = &_bi;
    Board _b = Board(fen_path, bb, bi);
    Board* b = &_b;
    SECTION("Pawn locations"){
        vector<vector<int>> expected_pawn_locations = {
            {a2, c2, d3, e5, g4, h5},
            {a7, b5, d7, f7, h7}
        };

        bool correct_piece_locations = are_pieces_in_correct_locations(b, expected_pawn_locations, pPAWN);
        REQUIRE(correct_piece_locations);
    }
    SECTION("Knight locations"){
        vector<vector<int>> expected_knight_locations = {
            {d5, g7},
            {a6, f6}
        };

        bool correct_piece_locations = are_pieces_in_correct_locations(b, expected_knight_locations, pKNIGHT);
        REQUIRE(correct_piece_locations);
    }
    SECTION("Bishop locations"){
        vector<vector<int>> expected_bishop_locations = {
            {e7},
            {c8, g1}
        };
        bool correct_piece_locations = are_pieces_in_correct_locations(b, expected_bishop_locations, pBISHOP);
        REQUIRE(correct_piece_locations);
    }
    SECTION("Rook locations"){
        vector<vector<int>> expected_rook_locations = {
            {},
            {a8, h8}
        };
        bool correct_piece_locations = are_pieces_in_correct_locations(b, expected_rook_locations, pROOK);
        REQUIRE(correct_piece_locations);
    }
    SECTION("Queen locations"){
        vector<vector<int>> expected_queen_locations = {
            {},
            {a1}
        };
        bool correct_piece_locations = are_pieces_in_correct_locations(b, expected_queen_locations, pQUEEN);
        REQUIRE(correct_piece_locations);
    }
    SECTION("King locations"){
        vector<vector<int>> expected_king_locations = {
            {e2},
            {d8}
        };
        bool correct_piece_locations = are_pieces_in_correct_locations(b, expected_king_locations, pKING);
        REQUIRE(correct_piece_locations);
    }
}

TEST_CASE("parse_uci_pgn == parse_fen at the end", "[Board]"){
    const char* starting_fen_path = "./positions/starting_position.txt";
    Bitboard _bb = Bitboard();
    BoardInfo _bi = BoardInfo();
    Bitboard* bb = &_bb;
    BoardInfo* bi = &_bi;

    SECTION("parse_uci_pgn == parse_fen at the end 1"){
        const char* pgn_path = "pgn/tests/test_1.uci";
        Board _b_pgn = Board(starting_fen_path, bb, bi);
        Board* b_pgn = &_b_pgn;
        b_pgn->parse_uci_pgn(pgn_path);
        const char* final_fen_path = "positions/tests/test_1_end.txt";
        Board _b_fen = Board(final_fen_path, bb, bi);
        Board* b_fen = &_b_fen;
        for(unsigned int side = 0 ; side < NUM_SIDES ; side ++){
            for(unsigned int piece = 0 ; piece < NUM_PIECE_TYPES ; piece ++){
                REQUIRE(b_pgn->get_bitboard()->piece_boards[side][piece] == b_fen->get_bitboard()->piece_boards[side][piece]);
            }
        }
    }
    SECTION("parse_uci_pgn == parse_fen at the end 2"){
        const char* pgn_path = "pgn/tests/test_2.uci";
        Board _b_pgn = Board(starting_fen_path, bb, bi);
        Board* b_pgn = &_b_pgn;
        b_pgn->parse_uci_pgn(pgn_path);
        const char* final_fen_path = "positions/tests/test_2_end.txt";
        Board _b_fen = Board(final_fen_path, bb, bi);
        Board* b_fen = &_b_fen;
        for(unsigned int side = 0 ; side < NUM_SIDES ; side ++){
            for(unsigned int piece = 0 ; piece < NUM_PIECE_TYPES ; piece ++){
                REQUIRE(b_pgn->get_bitboard()->piece_boards[side][piece] == b_fen->get_bitboard()->piece_boards[side][piece]);
            }
        }
    }
}
TEST_CASE("En Passant Rights Updated", "[En Passant]"){


    const char* fen_path = "./positions/starting_position.txt";
    Bitboard _bb = Bitboard();
    BoardInfo _bi = BoardInfo();
    Bitboard* bb = &_bb;
    BoardInfo* bi = &_bi;
    Board _b = Board(fen_path, bb, bi);
    Board* b = &_b;
    vector<unsigned int> move_list = {
        MoveUtils::create_move(e2, e4, WHITE, pPAWN, DOUBLE_PAWN_PUSH),
        MoveUtils::create_move(f7, f5, BLACK, pPAWN, DOUBLE_PAWN_PUSH),
        MoveUtils::create_move(e4, e5, WHITE, pPAWN, QUIET_MOVE),
        MoveUtils::create_move(d7, d5, BLACK, pPAWN, DOUBLE_PAWN_PUSH),
    };
    SECTION("en passant rights updated"){
        b->apply_move(move_list[0]);
        REQUIRE(b->get_board_info()->peek_ep_right() == (e4 - a4));

        b->apply_move(move_list[1]);
        REQUIRE(b->get_board_info()->peek_ep_right() == (f5 - a5));

        b->apply_move(move_list[2]);
        REQUIRE(b->get_board_info()->peek_ep_right() == NO_EP_RIGHTS);

        b->apply_move(move_list[3]);
        REQUIRE(b->get_board_info()->peek_ep_right() == (d5 - a5));
        
        b->reverse_move(move_list[3]);
        REQUIRE(b->get_board_info()->peek_ep_right() == NO_EP_RIGHTS);

        b->reverse_move(move_list[2]);
        REQUIRE(b->get_board_info()->peek_ep_right() == (f5 - a5));
    }
}

TEST_CASE("En Passant Move Generated", "[En Passant]"){


    const char* fen_path = "./positions/starting_position.txt";
    Bitboard _bb = Bitboard();
    BoardInfo _bi = BoardInfo();
    Bitboard* bb = &_bb;
    BoardInfo* bi = &_bi;
    Board _b = Board(fen_path, bb, bi);
    Board* b = &_b;
    vector<unsigned int> move_list = {
        MoveUtils::create_move(e2, e4, WHITE, pPAWN, DOUBLE_PAWN_PUSH),
        MoveUtils::create_move(f7, f5, BLACK, pPAWN, DOUBLE_PAWN_PUSH),
        MoveUtils::create_move(e4, e5, WHITE, pPAWN, QUIET_MOVE),
        MoveUtils::create_move(d7, d5, BLACK, pPAWN, DOUBLE_PAWN_PUSH),
    };
    MoveGen mg = MoveGen(b, WHITE, mEP_CAPTURE);
    // MoveGen mg = MoveGen(WHITE);
    // mg.set_move_type(mEP_CAPTURE);

    SECTION("move type is mEP_CAPTURE"){
        REQUIRE(mg.get_special_move_type() == mEP_CAPTURE);
    }
    SECTION("single en passant move generated"){

        for(auto move: move_list){
            b->apply_move(move);
        }        

        unsigned int expected_move = MoveUtils::create_move(e5, d6, WHITE, pPAWN, EP_CAPTURE, pPAWN, File::d);
        unsigned int move = mg.get_special_move();

        REQUIRE(expected_move == move);

        vector<unsigned int> reversed_move_list(move_list.rbegin(), move_list.rend());
        for(auto move: reversed_move_list){
            b->reverse_move(move);
        }
    }
    move_list = {
        MoveUtils::create_move(a2, a4, WHITE, pPAWN, DOUBLE_PAWN_PUSH),
        MoveUtils::create_move(c7, c5, BLACK, pPAWN, DOUBLE_PAWN_PUSH),
        MoveUtils::create_move(a4, a5, WHITE, pPAWN, QUIET_MOVE),
        MoveUtils::create_move(b7, b5, BLACK, pPAWN, DOUBLE_PAWN_PUSH)
    };    
    MoveGen mg1 = MoveGen(b, WHITE, mEP_CAPTURE);

    SECTION("queen side (a-b capture) en passant move generated"){

        for(auto move: move_list){
            b->apply_move(move);
        }        

        unsigned int expected_move = MoveUtils::create_move(a5, b6, WHITE, pPAWN, EP_CAPTURE, pPAWN,  File::b);
        unsigned int move = mg1.get_special_move();

        REQUIRE(expected_move == move);

        vector<unsigned int> reversed_move_list(move_list.rbegin(), move_list.rend());
        for(auto move: reversed_move_list){
            b->reverse_move(move);
        }
    }        
    move_list = {
        MoveUtils::create_move(b2, b4, WHITE, pPAWN, DOUBLE_PAWN_PUSH),
        MoveUtils::create_move(c7, c5, BLACK, pPAWN, DOUBLE_PAWN_PUSH),
        MoveUtils::create_move(b4, b5, WHITE, pPAWN, QUIET_MOVE),
        MoveUtils::create_move(a7, a5, BLACK, pPAWN, DOUBLE_PAWN_PUSH)
    };

    MoveGen mg2 = MoveGen(b, WHITE, mEP_CAPTURE);

    // MoveGen mg2 = MoveGen(WHITE);
    // mg2.set_move_type(mEP_CAPTURE);

    SECTION("queen side (b-a capture) en passant move generated"){

        for(auto move: move_list){
            b->apply_move(move);
        }

        unsigned int expected_move = MoveUtils::create_move(b5, a6, WHITE, pPAWN, EP_CAPTURE, pPAWN, File::a);
        unsigned int move = mg2.get_special_move();

        REQUIRE(expected_move == move);

        vector<unsigned int> reversed_move_list(move_list.rbegin(), move_list.rend());
        for(auto move: reversed_move_list){
            b->reverse_move(move);
        }
    }    
    move_list = {
        MoveUtils::create_move(h2, h4, WHITE, pPAWN, DOUBLE_PAWN_PUSH),
        MoveUtils::create_move(c7, c5, BLACK, pPAWN, DOUBLE_PAWN_PUSH),
        MoveUtils::create_move(h4, h5, WHITE, pPAWN, QUIET_MOVE),
        MoveUtils::create_move(g7, g5, BLACK, pPAWN, DOUBLE_PAWN_PUSH)
    };
    MoveGen mg3 = MoveGen(b, WHITE,  mEP_CAPTURE);
    // MoveGen mg3 = MoveGen(WHITE);
    // mg3.set_move_type(mEP_CAPTURE);

    SECTION("king side (h-g capture) en passant move generated"){

        for(auto move: move_list){
            b->apply_move(move);
        }        

        unsigned int expected_move = MoveUtils::create_move(h5, g6, WHITE, pPAWN, EP_CAPTURE, pPAWN, File::g);
        unsigned int move = mg3.get_special_move();

        REQUIRE(expected_move == move);

        vector<unsigned int> reversed_move_list(move_list.rbegin(), move_list.rend());
        for(auto move: reversed_move_list){
            b->reverse_move(move);
        }
    }
}

TEST_CASE("Initial value for pesto evaluation", "[PestoEvaluation]"){

    string fen_path = "./positions/starting_position.txt";
    Bitboard _bb = Bitboard();
    BoardInfo _bi = BoardInfo();
    Bitboard* bb = &_bb;
    BoardInfo* bi = &_bi;
    Board _b = Board(fen_path, bb, bi);
    Board* b = &_b;
    b->parse_fen(fen_path);

    SECTION("init_evaluation"){
        PestoEvaluation _pesto = PestoEvaluation(b);
        PestoEvaluation* pesto = &_pesto;
        pesto->init_evaluate();
        REQUIRE(pesto->get_evaluation(WHITE, WHITE) == 0);
        REQUIRE(pesto->gamePhase == 24);
    }
    SECTION("update_evaluation (basic)"){
        PestoEvaluation _pesto = PestoEvaluation(b);
        PestoEvaluation* pesto = &_pesto;
        pesto->init_evaluate();
        REQUIRE(pesto->get_evaluation(WHITE, WHITE) == 0);
        REQUIRE(pesto->gamePhase == 24);

        unsigned int move1 = MoveUtils::create_move(e2, e4, WHITE, pPAWN, DOUBLE_PAWN_PUSH);
        b->apply_move(move1);
        pesto->update_evaluation(move1);
        REQUIRE(pesto->get_evaluation(WHITE, WHITE) != 0);
        REQUIRE(pesto->gamePhase == 24);
        
        b->reverse_move(move1);
        pesto->update_evaluation(move1, 1);
        REQUIRE(pesto->get_evaluation(WHITE, WHITE) == 0);
        REQUIRE(pesto->gamePhase == 24);

    }
}

TEST_CASE("Update evaluation for pesto evaluation", "[PestoEvaluation]"){

    string fen_path = "./positions/bratko-kopec/bk_2.txt";
    Bitboard _bb = Bitboard();
    BoardInfo _bi = BoardInfo();
    Bitboard* bb = &_bb;
    BoardInfo* bi = &_bi;
    Board _b = Board(fen_path, bb, bi);
    Board* b = &_b;
    b->get_bitboard()->display();


    int expected_mg[2], expected_eg[2];
    int expected_mg_phase = 10;
    expected_mg[BLACK] = ((-27 - 4 - 4 + 24 + 38 - 12) + (82 * 6) + \
                         (-1) + (337) +\
                         (-17 + 17) + (477 * 2) +
                         (-28));
    expected_mg[WHITE] = ((-27 + 12 + 17 + 6 + 10 - 23) + (82 * 6) +\
                         (-9) + (337) + \
                         (1 - 16) + (477 * 2)+\
                         (-16));
    int expected_eg_phase = 14;
    expected_eg[BLACK] = ((13 + 7 - 6 + 0 + 2 - 8) + (94 * 6) +\
                            (-2) + (281) +\
                            (-1 - 1) + (512 * 2) + \
                            (-14));
    expected_eg[WHITE] = ((13 - 7 - 7 - 8 + 3 + 17) + (94 * 6) +\
                            (-3) + (281)+\
                            (-5 + 3) + (512 * 2)+\
                            (4));
    SECTION("init_evaluation"){

        PestoEvaluation _pesto = PestoEvaluation(b);
        PestoEvaluation* pesto = &_pesto;

        int actual_eval = pesto->init_evaluate();
        int expected_mg_score = -expected_mg[BLACK] + expected_mg[WHITE];
        int expected_eg_score = -expected_eg[BLACK] + expected_eg[WHITE];
        int expected_score = (expected_mg_score * expected_mg_phase) + (expected_eg_score * expected_eg_phase);
        REQUIRE(pesto->gamePhase == expected_mg_phase);
        REQUIRE(actual_eval == expected_score);
        REQUIRE(actual_eval == pesto->get_evaluation(WHITE, WHITE));


        unsigned int rook_capture = MoveUtils::create_move(c3, c6, WHITE, pROOK, ADDITIONAL_INFO::CAPTURE, pPAWN);
        b->apply_move(rook_capture);
        pesto->update_evaluation(rook_capture);
        int new_eval = pesto->get_evaluation(WHITE, WHITE);

        int expected_white_rook_mg_change = (26 - (-16));
        int expected_white_rook_eg_change = (7 - (-5));
        int expected_black_pawn_mg_change = (-(-4 + 82));
        int expected_black_pawn_eg_change = (-(-6 + 94));
        cout<<"expected_mg[WHITE]: "<<expected_mg[WHITE]<<"\n";
        cout<<"expected_mg[BLACK]: "<<expected_mg[BLACK]<<"\n";
        cout<<"expected_eg[WHITE]: "<<expected_eg[WHITE]<<"\n";
        cout<<"expected_eg[BLACK]: "<<expected_eg[BLACK]<<"\n";
        int expected_mg_after[2], expected_eg_after[2];
        expected_mg_after[WHITE] = expected_mg[WHITE] + expected_white_rook_mg_change;
        expected_mg_after[BLACK] = expected_mg[BLACK] + expected_black_pawn_mg_change;
        expected_eg_after[WHITE] = expected_eg[WHITE] + expected_white_rook_eg_change;
        expected_eg_after[BLACK] = expected_eg[BLACK] + expected_black_pawn_eg_change;

        cout<<"expected_mg_after[WHITE]: "<<expected_mg_after[WHITE]<<"\n";
        cout<<"expected_mg_after[BLACK]: "<<expected_mg_after[BLACK]<<"\n";
        cout<<"expected_eg_after[WHITE]: "<<expected_eg_after[WHITE]<<"\n";
        cout<<"expected_eg_after[BLACK]: "<<expected_eg_after[BLACK]<<"\n";
        int expected_mg_score_after = -expected_mg_after[BLACK] + expected_mg_after[WHITE];
        int expected_eg_score_after = -expected_eg_after[BLACK] + expected_eg_after[WHITE];
        int expected_score_after = (expected_mg_score_after * expected_mg_phase) + (expected_eg_score_after * expected_eg_phase);
        REQUIRE(new_eval == expected_score_after);

        pesto->update_evaluation(rook_capture, 1);
    }
}

TEST_CASE("Only Captures","[MoveGen]"){

    string fen_path = "./positions/starting_position.txt";
    Bitboard _bb = Bitboard();
    BoardInfo _bi = BoardInfo();
    Bitboard* bb = &_bb;
    BoardInfo* bi = &_bi;
    Board _b = Board(fen_path, bb, bi);
    Board* b = &_b;
    b->get_bitboard()->display();

    SECTION("No captures generated with starting position"){
        const char* fen_path = "./positions/starting_position.txt";
        b->parse_fen(fen_path);
        MoveGen mg = MoveGen(b, WHITE);
        mg.set_gen_type(ONLY_CAPTURES);
        unsigned int move = 0;
        int move_count = 0;
        while((move = mg.get_move()) != NO_MOVES_LEFT){
            if(move == INCREMENTING_MOVE_TYPE)
                continue;
            move_count++;
        }
        REQUIRE(move_count == 0);
    }

    SECTION("One capture generated with queen trade position"){

        const char* queen_trade_path = "./positions/queen_trade.txt";
        b->parse_fen(queen_trade_path);

        // MoveGen mg1 = MoveGen(BLACK, mQUIET, true);
        MoveGen mg1 = MoveGen(b, BLACK);
        mg1.set_gen_type(ONLY_CAPTURES);

        vector<unsigned int> expected_moves = {
            MoveUtils::create_move(f8, a3, BLACK, pBISHOP,  CAPTURE, pPAWN),
            MoveUtils::create_move(d8, d1, BLACK, pQUEEN, CAPTURE, pQUEEN),
        };
        vector<unsigned int> actual_moves;
        unsigned int move = 0;
        int move_count = 0;
        
        while((move = mg1.get_move()) != NO_MOVES_LEFT){
            if(move == INCREMENTING_MOVE_TYPE)
                continue;
            move_count++;
            actual_moves.push_back(move);
        }
        for(int i = 0 ; i < actual_moves.size() ; i ++){
            MoveUtils::display(actual_moves[i]);
            cout<<" ";
        }
        cout<<endl;
        REQUIRE(move_count == 2);
        REQUIRE(are_move_vectors_equal(actual_moves, expected_moves));
    }

    SECTION("pawn explored first"){

        const char* perft_ordered = "./positions/bugs/test_perft_ordered.txt";
        b->parse_fen(perft_ordered);
        b->get_bitboard()->display();
        MoveGen mg1 = MoveGen(b, WHITE);
        mg1.set_gen_type(ONLY_CAPTURES);
        unsigned int move;  
        bool move_exists=false;
        while((move = mg1.get_move()) != NO_MOVES_LEFT){
            if(move == INCREMENTING_MOVE_TYPE){
                cout<<"continuing\n";
                continue;
            }
            REQUIRE(MoveUtils::get_additional_info(move) == ADDITIONAL_INFO::CAPTURE);
            REQUIRE(MoveUtils::get_piece(move) == pPAWN);
            move_exists=true;
            break;
        }
        REQUIRE(move_exists==true);

    }

}

TEST_CASE("Move generation total","[MoveGen]"){


    const char* queen_trade_path = "./positions/queen_trade.txt";
    Bitboard _bb = Bitboard();
    BoardInfo _bi = BoardInfo();
    Bitboard* bb = &_bb;
    BoardInfo* bi = &_bi;
    Board _b = Board(queen_trade_path, bb, bi);
    Board* b = &_b;

    MoveGen mg_captures = MoveGen(b, BLACK);
    MoveGen mg_quiet = MoveGen(b, BLACK);
    MoveGen mg = MoveGen(b, BLACK);
    mg_captures.set_gen_type(ONLY_CAPTURES);
    mg_quiet.set_gen_type(ONLY_QUIET);
    SECTION("num_captures + num_quiet == num_moves"){
        unsigned int move = 0;
        int num_captures = 0;
        int num_quiet = 0;
        int num_total = 0;
        while((move = mg_captures.get_move()) != NO_MOVES_LEFT){
            if(move == INCREMENTING_MOVE_TYPE)
                continue;
            num_captures++;
        }
        while((move = mg_quiet.get_move()) != NO_MOVES_LEFT){
            if(move == INCREMENTING_MOVE_TYPE)
                continue;
            num_quiet ++;
        }
        while((move = mg.get_move()) != NO_MOVES_LEFT){
            if(move == INCREMENTING_MOVE_TYPE)
                continue;
            num_total ++;
        }
        cout<<endl;
        REQUIRE(num_captures == 2);
        REQUIRE(num_captures + num_quiet == num_total);
    }

}
#if defined(ENABLE_PERFT) && (ENABLE_PERFT != 0)
TEST_CASE("Number of nodes during search","[Perft]"){



    const char* fen_path = "./positions/starting_position.txt";
    Bitboard _bb = Bitboard();
    BoardInfo _bi = BoardInfo();
    Bitboard* bb = &_bb;
    BoardInfo* bi = &_bi;
    Board _b = Board(fen_path, bb, bi);
    Board* b = &_b;
    
    PestoEvaluation _pesto = PestoEvaluation(b);
    PestoEvaluation* pesto = &_pesto;
    Search s = Search(b, pesto);
    SECTION("Depth = 1"){
        REQUIRE(s.perft(1, 1, WHITE) == 20);
        REQUIRE(s.num_captures == 0);
    }
    SECTION("Depth = 2"){
        REQUIRE(s.perft(2, 2, WHITE) == 400);
        REQUIRE(s.num_captures == 0);
    }
    SECTION("Depth = 3"){
        REQUIRE(s.perft(3, 3, WHITE) == 8902);
        REQUIRE(s.num_captures == 34);
    }

}

TEST_CASE("Perft king move miss", "[Search]"){
    string fen_path = "./positions/bugs/backrank_miss_e5f3_g1h1_c6c5.txt";
    Bitboard _bb = Bitboard();
    BoardInfo _bi = BoardInfo();
    Bitboard* bb = &_bb;
    BoardInfo* bi = &_bi;
    Board _b = Board(fen_path, bb, bi);
    Board* b = &_b;

    SECTION("king move generated"){
        MoveGen mg = MoveGen(b, WHITE);
        unsigned int move = 0;
        bool king_move_found = false;
        bool h1g2_move_found = false;
        while((move = mg.get_move()) != NO_MOVES_LEFT){
            if(MoveUtils::get_piece(move) == pKING){
                king_move_found = true;
                cout<<"king move found: ";
                MoveUtils::display(move);
            } if(move == MoveUtils::create_move(h1, g2, WHITE, pKING)){
                h1g2_move_found = true;
            }
        }
        REQUIRE(king_move_found);
        REQUIRE(h1g2_move_found);
    }
    SECTION("correct number of nodes searched"){
        PestoEvaluation _pesto = PestoEvaluation(b);
        PestoEvaluation* pesto = &_pesto;

        Search s = Search(b, pesto);
        REQUIRE(s.perft(2, 2, WHITE) == 1473);
    }
}

TEST_CASE("Number of nodes during search depth 4","[Perft]"){

    const char* fen_path = "./positions/starting_position.txt";
    Bitboard _bb = Bitboard();
    BoardInfo _bi = BoardInfo();
    Bitboard* bb = &_bb;
    BoardInfo* bi = &_bi;
    Board _b = Board(fen_path, bb, bi);
    Board* b = &_b;

    PestoEvaluation _pesto = PestoEvaluation(b);
    PestoEvaluation* pesto = &_pesto;
    Search s = Search(b, pesto);

    SECTION("Depth = 4"){
        REQUIRE(s.perft(4, 4, WHITE) == 197281);
        REQUIRE(s.num_captures == 1576);
    }

}
TEST_CASE("Number of nodes during search depth 5","[Perft]"){

    const char* fen_path = "./positions/starting_position.txt";
    Bitboard _bb = Bitboard();
    BoardInfo _bi = BoardInfo();
    Bitboard* bb = &_bb;
    BoardInfo* bi = &_bi;
    Board _b = Board(fen_path, bb, bi);
    Board* b = &_b;

    PestoEvaluation _pesto = PestoEvaluation(b);
    PestoEvaluation* pesto = &_pesto;
    Search s = Search(b, pesto);

    SECTION("Depth = 5"){
        REQUIRE(s.perft(5, 5, WHITE) == 4865609);
        REQUIRE(s.num_captures == 82719);
    }

}

TEST_CASE("perft == perft_ordered","[Perft]"){

    const char* fen_path = "./positions/starting_position.txt";
    Bitboard _bb = Bitboard();
    BoardInfo _bi = BoardInfo();
    Bitboard* bb = &_bb;
    BoardInfo* bi = &_bi;
    Board _b = Board(fen_path, bb, bi);
    Board* b = &_b;

    PestoEvaluation _pesto = PestoEvaluation(b);
    PestoEvaluation* pesto = &_pesto;
    Search s = Search(b, pesto);

    SECTION("Depth = 5"){
        REQUIRE(s.perft(5, 5, WHITE) == s.perft_ordered(5, 5, WHITE));
    }

}
TEST_CASE("promotions during search", "[Perft]"){

    string fen_path = "./positions/ep_fen.txt";
    Bitboard _bb = Bitboard();
    BoardInfo _bi = BoardInfo();
    Bitboard* bb = &_bb;
    BoardInfo* bi = &_bi;
    Board _b = Board(fen_path, bb, bi);
    Board* b = &_b;

    PestoEvaluation _pesto = PestoEvaluation(b);
    PestoEvaluation* pesto = &_pesto;
    Search s = Search(b, pesto);


    SECTION("Depth = 5"){
        REQUIRE(s.perft(5, 5, WHITE) == 16422290);
        REQUIRE(s.num_captures == 669892);
        REQUIRE(s.num_promotions == 628);
        REQUIRE(s.num_capture_promotions == 9808);

    }

}
TEST_CASE("castles during search", "[Perft]"){


    string fen_path = "./positions/castle_fen.txt";
    Bitboard _bb = Bitboard();
    BoardInfo _bi = BoardInfo();
    Bitboard* bb = &_bb;
    BoardInfo* bi = &_bi;
    Board _b = Board(fen_path, bb, bi);
    Board* b = &_b;

    PestoEvaluation _pesto = PestoEvaluation(b);
    PestoEvaluation* pesto = &_pesto;
    Search s = Search(b, pesto);


    SECTION("Depth = 5"){
        REQUIRE(s.perft(5, 5, WHITE) == 22273312);
        REQUIRE(s.num_castles == 19682);

    }

}
TEST_CASE("Unique Zobrist Hash vals", "[TranspositionTable]"){

    string fen_path = "./positions/starting_position.txt";
    Bitboard _bb = Bitboard();
    BoardInfo _bi = BoardInfo();
    Bitboard* bb = &_bb;
    BoardInfo* bi = &_bi;
    Board _b = Board(fen_path, bb, bi);
    Board* b = &_b;

    PestoEvaluation _pesto = PestoEvaluation(b);
    PestoEvaluation* pesto = &_pesto;
    Search s = Search(b, pesto);

    vector<uint64> seen_zobrist_vals;
    bool found_duplicate = false;
    for(int i = 0 ; i < NUM_ZOBRIST_VALS ; i ++){
        for(int j = 0 ; j < seen_zobrist_vals.size() ; j ++){
            if(seen_zobrist_vals[j] == b->tt.zobrist_vals[i])
                found_duplicate = true;
        }
        seen_zobrist_vals.push_back(b->tt.zobrist_vals[i]);
    }
    REQUIRE(!found_duplicate);
}
TEST_CASE("Updating hash val", "[TranspositionTable]"){

    string fen_path = "./positions/starting_position.txt";
    Bitboard _bb = Bitboard();
    BoardInfo _bi = BoardInfo();
    Bitboard* bb = &_bb;
    BoardInfo* bi = &_bi;
    Board _b = Board(fen_path, bb, bi);
    Board* b = &_b;

    cout<<"initial hash_val: "<<b->tt.hash_val<<endl;
    SECTION("quiet moves"){

        unsigned int move_order1[4] = {
            MoveUtils::create_move(b2, b4, WHITE, pPAWN, DOUBLE_PAWN_PUSH),
            MoveUtils::create_move(c7, c5, BLACK, pPAWN, DOUBLE_PAWN_PUSH),
            MoveUtils::create_move(a2, a4, WHITE, pPAWN, DOUBLE_PAWN_PUSH),
            MoveUtils::create_move(a7, a5, BLACK, pPAWN, DOUBLE_PAWN_PUSH)
        };
        unsigned int move_order2[4] = {
            MoveUtils::create_move(a2, a4, WHITE, pPAWN, DOUBLE_PAWN_PUSH),
            MoveUtils::create_move(c7, c5, BLACK, pPAWN, DOUBLE_PAWN_PUSH),
            MoveUtils::create_move(b2, b4, WHITE, pPAWN, DOUBLE_PAWN_PUSH),
            MoveUtils::create_move(a7, a5, BLACK, pPAWN, DOUBLE_PAWN_PUSH)
        };
        SECTION("applying and reversing move"){
            vector<uint64> seen_hash_vals;
            for(int i = 0 ; i < 4 ; i ++){
                b->apply_move(move_order1[i]);
                if(i > 0){
                    REQUIRE(b->tt.hash_val != seen_hash_vals[i-1]);
                }
                seen_hash_vals.push_back(b->tt.hash_val);
            }

            bool found_hash_val = true;
            for(int i = 3 ; i >= 0 ; i --){
                b->reverse_move(move_order1[i]);
                if(i > 0 && b->tt.hash_val != seen_hash_vals[i - 1]){
                    found_hash_val = false;
                }

            }

            REQUIRE(found_hash_val == true);
        }
    }
    SECTION("captures"){
        unsigned int move_order1[7] = {
            MoveUtils::create_move(b2, b4, WHITE, pPAWN, DOUBLE_PAWN_PUSH),
            MoveUtils::create_move(a7, a5, BLACK, pPAWN, DOUBLE_PAWN_PUSH),
            MoveUtils::create_move(a2, a4, WHITE, pPAWN, DOUBLE_PAWN_PUSH),
            MoveUtils::create_move(b7, b5, BLACK, pPAWN, DOUBLE_PAWN_PUSH),
            MoveUtils::create_move(a4, b5, WHITE, pPAWN, CAPTURE),
            MoveUtils::create_move(c7, c5, BLACK, pPAWN, DOUBLE_PAWN_PUSH),
            MoveUtils::create_move(b4, a5, WHITE, pPAWN, CAPTURE)
        };
        unsigned int move_order2[7] = {
            MoveUtils::create_move(b2, b4, WHITE, pPAWN, DOUBLE_PAWN_PUSH),
            MoveUtils::create_move(a7, a5, BLACK, pPAWN, DOUBLE_PAWN_PUSH),
            MoveUtils::create_move(a2, a4, WHITE, pPAWN, DOUBLE_PAWN_PUSH),
            MoveUtils::create_move(b7, b5, BLACK, pPAWN, DOUBLE_PAWN_PUSH),
            MoveUtils::create_move(b4, a5, WHITE, pPAWN, CAPTURE),
            MoveUtils::create_move(c7, c5, BLACK, pPAWN, DOUBLE_PAWN_PUSH),
            MoveUtils::create_move(a4, b5, WHITE, pPAWN, CAPTURE)
        };
        SECTION("applying and reversing move"){
            vector<uint64> seen_hash_vals;
            for(int i = 0 ; i < 7 ; i ++){
                b->apply_move(move_order1[i]);
                if(i > 0){
                    REQUIRE(b->tt.hash_val != seen_hash_vals[i-1]);
                }
                seen_hash_vals.push_back(b->tt.hash_val);
            }

            bool found_hash_val = true;
            for(int i = 6 ; i >= 0 ; i --){
                b->reverse_move(move_order1[i]);
                if(i > 0 && b->tt.hash_val != seen_hash_vals[i - 1]){
                    found_hash_val = false;
                }

            }

            REQUIRE(found_hash_val == true);
        }
        SECTION("hash_val from transposition should the same"){
            vector<uint64> seen_hash_vals;
            for(int i = 0 ; i < 7 ; i ++){
                b->apply_move(move_order1[i]);
                seen_hash_vals.push_back(b->tt.hash_val);
            }
            for(int i = 6 ; i >= 0 ; i --){
                b->reverse_move(move_order1[i]);
            }
            for(int i = 0 ; i < 7 ; i ++){
                b->apply_move(move_order2[i]);
            }
            // at index=6, the position of the move_order2 should be equal to the position after move_order1
            REQUIRE(b->tt.hash_val == seen_hash_vals[6]);
            for(int i = 6 ; i >= 0 ; i --){
                b->reverse_move(move_order2[i]);
            }

            for(int i = 0 ; i < 6 ; i ++){
                b->apply_move(move_order2[i]);
            }
            // at index=5, the position of the move_order2 should not be equal to the position after move_order1
            REQUIRE(b->tt.hash_val != seen_hash_vals[5]);
        }
    }

}
TEST_CASE("Updating hash val en passant", "[TranspositionTable]"){

    string fen_path = "./positions/starting_position.txt";
    Bitboard _bb = Bitboard();
    BoardInfo _bi = BoardInfo();
    Bitboard* bb = &_bb;
    BoardInfo* bi = &_bi;
    Board _b = Board(fen_path, bb, bi);
    Board* b = &_b;

    cout<<"initial hash_val: "<<b->tt.hash_val<<endl;
    SECTION("quiet moves"){

        unsigned int move_order1[4] = {
            MoveUtils::create_move(a2, a4, WHITE, pPAWN, DOUBLE_PAWN_PUSH),
            MoveUtils::create_move(a7, a6, BLACK, pPAWN, QUIET_MOVE),
            MoveUtils::create_move(a4, a5, WHITE, pPAWN, QUIET_MOVE),
            MoveUtils::create_move(b7, b5, BLACK, pPAWN, DOUBLE_PAWN_PUSH)
        };
        unsigned int move_order2[4] = {
            MoveUtils::create_move(a2, a4, WHITE, pPAWN, DOUBLE_PAWN_PUSH),
            MoveUtils::create_move(b7, b5, BLACK, pPAWN, DOUBLE_PAWN_PUSH),
            MoveUtils::create_move(a4, a5, WHITE, pPAWN, QUIET_MOVE),
            MoveUtils::create_move(a7, a6, BLACK, pPAWN, QUIET_MOVE)
        };
        SECTION("applying and reversing move"){
            vector<uint64> seen_hash_vals;
            for(int i = 0 ; i < 4 ; i ++){
                b->apply_move(move_order1[i]);
                if(i > 0){
                    REQUIRE(b->tt.hash_val != seen_hash_vals[i-1]);
                }
                seen_hash_vals.push_back(b->tt.hash_val);
            }

            bool found_hash_val = true;
            for(int i = 3 ; i >= 0 ; i --){
                b->reverse_move(move_order1[i]);
                if(i > 0 && b->tt.hash_val != seen_hash_vals[i - 1]){
                    found_hash_val = false;
                }

            }

            REQUIRE(found_hash_val == true);
        }
        SECTION("hash_val from transposition should the same"){
            vector<uint64> seen_hash_vals;
            for(int i = 0 ; i < 4 ; i ++){
                b->apply_move(move_order1[i]);
                seen_hash_vals.push_back(b->tt.hash_val);
            }
            unsigned int move_order1_ep_right = b->get_board_info()->peek_ep_right();

            for(int i = 3 ; i >= 0 ; i --){
                b->reverse_move(move_order1[i]);
            }
            for(int i = 0 ; i < 4 ; i ++){
                b->apply_move(move_order2[i]);
            }            
            unsigned int move_order2_ep_right = b->get_board_info()->peek_ep_right();
            REQUIRE(move_order1_ep_right != move_order2_ep_right);
            REQUIRE(b->tt.hash_val != seen_hash_vals[3]);
        }
    }

}
TEST_CASE("Transposition Table cach matches (simple)", "[TranspositionTable]"){

    string fen_path = "./positions/transposition_test.txt";
    Bitboard _bb = Bitboard();
    BoardInfo _bi = BoardInfo();
    Bitboard* bb = &_bb;
    BoardInfo* bi = &_bi;
    Board _b = Board(fen_path, bb, bi);
    Board* b = &_b;

    PestoEvaluation _pesto = PestoEvaluation(b);
    PestoEvaluation* pesto = &_pesto;
    Search s = Search(b, pesto);

    cout<<"before search tt_found_count[3]: "<<s.tt_found_count[3]<<endl;
    unsigned int nodes = s.perft(4,4, WHITE, 0, true);
    SECTION("Depth = 4, tt_found_count == tt_match_count"){

        cout<<"after search tt_found_count[2]: "<<s.tt_found_count[2]<<endl;
        cout<<"after search tt_found_count[3]: "<<s.tt_found_count[3]<<endl;
        cout<<"after search tt_found_count[4]: "<<s.tt_found_count[4]<<endl;
        cout<<"nodes searched: "<<nodes<<endl;
        REQUIRE(s.tt_found_count[1] == 0);
        REQUIRE(s.tt_found_count[2] == 4);
        REQUIRE(s.tt_found_count[3] == 44);
    }

}
TEST_CASE("Transposition Table cache matches", "[TranspositionTable]"){

    string fen_path = "./positions/starting_position.txt";
    Bitboard _bb = Bitboard();
    BoardInfo _bi = BoardInfo();
    Bitboard* bb = &_bb;
    BoardInfo* bi = &_bi;
    Board _b = Board(fen_path, bb, bi);
    Board* b = &_b;

    PestoEvaluation _pesto = PestoEvaluation(b);
    PestoEvaluation* pesto = &_pesto;
    Search s = Search(b, pesto);

    cout<<"before search tt_found_count[3]: "<<s.tt_found_count[3]<<endl;
    unsigned int nodes = s.perft(6, 6, WHITE, 0, true);
    SECTION("Depth = 6, tt_found_count == tt_match_count"){
        REQUIRE(s.tt_found_count[2] == 1300);
        REQUIRE(s.tt_found_count[3] == 67152);
        REQUIRE(s.tt_found_count[4] == 1284092);
        REQUIRE(s.tt_found_count[5] == 16699235);
        REQUIRE(nodes == 119060324);
    }

}
#else
#endif
TEST_CASE("Alpha beta pruning selected move", "[Search]"){

    string fen_path = "./positions/bratko-kopec/bk_1.txt";
    Bitboard _bb = Bitboard();
    BoardInfo _bi = BoardInfo();
    Bitboard* bb = &_bb;
    BoardInfo* bi = &_bi;
    Board _b = Board(fen_path, bb, bi);
    Board* b = &_b;

    PestoEvaluation _pesto = PestoEvaluation(b);
    PestoEvaluation* pesto = &_pesto;

    Search s = Search(b, pesto);

    SECTION("bk 1"){
        pv_t* principal_var = (pv_t*) calloc(1, sizeof(pv_t));
        principal_var->len = 0;

        s.max_depth = 6;
        int alpha = -1e7;
        int beta = 1e7;
        s.alpha_beta(alpha, beta, 6, BLACK, BLACK, 0, principal_var);

        free(principal_var);
        cout<<"selected move: ";
        MoveUtils::display(s.selected_move);
        REQUIRE(s.selected_move == MoveUtils::create_move(d6, d1, BLACK, pQUEEN));
    }
    
}

TEST_CASE("Alpha beta pruning selected move backrank bug", "[Search]"){

    string fen_path = "./positions/bugs/backrank_miss.txt";
    Bitboard _bb = Bitboard();
    BoardInfo _bi = BoardInfo();
    Bitboard* bb = &_bb;
    BoardInfo* bi = &_bi;
    Board _b = Board(fen_path, bb, bi);
    Board* b = &_b;

    PestoEvaluation _pesto = PestoEvaluation(b);
    PestoEvaluation* pesto = &_pesto;

    Search s = Search(b, pesto);

    SECTION("perft backrank mate miss"){
        REQUIRE(s.perft(5, 5, BLACK) == 39666064);
    }
    SECTION("alpha_beta backrank mate miss"){
        pv_t* principal_var = (pv_t*) calloc(1, sizeof(pv_t));
        principal_var->len = 0;

        s.max_depth = 6;
        int alpha = -1e7;
        int beta = 1e7;
        s.alpha_beta(alpha, beta, 6, BLACK, BLACK, 0, principal_var);

        free(principal_var);

        cout<<"selected move: ";
        MoveUtils::display(s.selected_move);
        REQUIRE(MoveUtils::get_piece(s.selected_move) == pKNIGHT);
        REQUIRE(s.selected_move != MoveUtils::create_move(e5, c4, BLACK, pKNIGHT));
        REQUIRE(s.selected_move == MoveUtils::create_move(e5, g6, BLACK, pKNIGHT));
    }

}

TEST_CASE("Transposition Table threefold repitition values", "[TranspositionTable]"){
    const char* fen_path = "./positions/starting_position.txt";
    const char* pgn_path = "./pgn/tests/transposition_captures.uci";

    Bitboard _bb = Bitboard();
    BoardInfo _bi = BoardInfo();
    Bitboard* bb = &_bb;
    BoardInfo* bi = &_bi;
    Board _b = Board(fen_path, bb, bi);
    Board* b = &_b;
    SECTION("pos 1 first time"){
        b->parse_uci_pgn(pgn_path, 11);
        REQUIRE(b->tt.get_value_threefold() == 1);
    }
    SECTION("pos 1 repitition"){
        b->parse_uci_pgn(pgn_path, 15);
        REQUIRE(b->tt.get_value_threefold() == 2);
    }
    SECTION("pos 2 first time"){
        b->parse_uci_pgn(pgn_path, 23);
        REQUIRE(b->tt.get_value_threefold() == 1);
    }
    SECTION("pos 2 repitition"){
        b->parse_uci_pgn(pgn_path, 27);
        REQUIRE(b->tt.get_value_threefold() == 2);
    }
}

TEST_CASE("Iterative deepening avoid threefold repitition in winning position", "[Search]"){
    string fen_path = "./positions/starting_position.txt";
    Bitboard _bb = Bitboard();
    BoardInfo _bi = BoardInfo();
    Bitboard* bb = &_bb;
    BoardInfo* bi = &_bi;

    Board _b = Board(fen_path, bb, bi);
    Board* b = &_b;

    PestoEvaluation _pesto = PestoEvaluation(b);
    PestoEvaluation* pesto = &_pesto;

    SECTION("Avoid a5a1 to prevent threefold"){
        string pgn_path = "./pgn/tests/avoid_draw.uci";
        b->parse_uci_pgn(pgn_path);
        Search s = Search(b, pesto, 8, true);
        int score = s.iterative_deepening(6, BLACK, BLACK);
        unsigned int drawing_move = MoveUtils::create_move(a5, a1, BLACK, pQUEEN);
        cout<<"selected move: \n";
        MoveUtils::display(s.selected_move);
        REQUIRE(s.selected_move != drawing_move);
    }
    SECTION("at 57 moves, there should be two repititions for move 53 and 57"){
        string pgn_path = "./pgn/tests/avoid_draw_1.uci";
        b->parse_uci_pgn(pgn_path, 57);
        Search s = Search(b, pesto, 8, true);
        uint64 tt_val = b->tt.get_value_threefold();
        REQUIRE(tt_val == 2);
    }
    SECTION("Avoid e7e8 to prevent threefold"){
        string pgn_path = "./pgn/tests/avoid_draw_1.uci";
        b->get_bitboard()->display();
        b->parse_uci_pgn(pgn_path);
        b->get_bitboard()->display();

        Search s = Search(b, pesto, 8, true);

        int score = s.iterative_deepening(8, BLACK, BLACK);
        unsigned int drawing_move = MoveUtils::create_move(e7, e8, BLACK, pKING);
        unsigned int winning_move = MoveUtils::create_move(e7, e6, BLACK, pKING);
        cout<<"selected move: \n";
        MoveUtils::display(s.selected_move);
        REQUIRE(s.selected_move != drawing_move);
        REQUIRE(s.selected_move == winning_move);
    }
}

TEST_CASE("Alpha beta pruning take threefold repitition in losing position", "[Search]"){
    string fen_path = "./positions/starting_position.txt";
    string pgn_path = "./pgn/tests/take_draw.uci";
    Bitboard _bb = Bitboard();
    BoardInfo _bi = BoardInfo();
    Bitboard* bb = &_bb;
    BoardInfo* bi = &_bi;

    Board _b = Board(fen_path, bb, bi);
    Board* b = &_b;
    b->parse_uci_pgn(pgn_path);

    PestoEvaluation _pesto = PestoEvaluation(b);
    PestoEvaluation* pesto = &_pesto;

    Search s = Search(b, pesto);
    SECTION("make f4c1 to take threefold"){
        int score = s.iterative_deepening(6, BLACK, BLACK);
        unsigned int drawing_move = MoveUtils::create_move(f4, c1, BLACK, pQUEEN);
        cout<<"selected move: \n";
        MoveUtils::display(s.selected_move);
        REQUIRE(s.selected_move == drawing_move);
        REQUIRE(score == 0);
    }
}