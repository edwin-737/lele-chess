#include <chrono>
#include <vector>
#include <algorithm>
#include <stdlib.h>
#include <assert.h>
#include <catch2/catch_test_macros.hpp>

#include "move.hpp"
#include "board_squares.hpp"
#include "search.hpp"
#include "magics.hpp"
#include "bitboard.hpp"
#include "board_info.hpp"
#include "move_set.hpp"
#include "utils.hpp"
using namespace std::chrono;
using namespace std;
#define NONE 0
#define FEN 1
#define DEPTH 2
Bitboard* Bitboard::instanceptr=nullptr;
BoardInfo* BoardInfo::instanceptr=nullptr;

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

TEST_CASE("Piece locations for starting position", "[Parsing FEN]"){

    BoardSquares::init_files();
    BoardSquares::init_ranks();
    BoardSquares::init_squares();
    MoveSet::set_attack_sets();
    MoveSet::init_attack_masks();


    Board* b = new Board();
    const char* fen_path = "./positions/starting_position.txt";
    b->parse_fen(fen_path);
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
    delete b;
    delete Bitboard::instanceptr;
    delete BoardInfo::instanceptr;
    b = nullptr;
    Bitboard::instanceptr = nullptr;
    BoardInfo::instanceptr = nullptr;
}

TEST_CASE("Piece locations for fen test position", "[Parsing FEN]"){

    BoardSquares::init_files();
    BoardSquares::init_ranks();
    BoardSquares::init_squares();
    MoveSet::set_attack_sets();
    MoveSet::init_attack_masks();

    Board* b = new Board();
    const char* fen_path = "./positions/fen_test_position.txt";
    b->parse_fen(fen_path);
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
    delete b;
    delete Bitboard::instanceptr;
    delete BoardInfo::instanceptr;
    b = nullptr;
    Bitboard::instanceptr = nullptr;
    BoardInfo::instanceptr = nullptr;
}

TEST_CASE("En Passant Rights Updated", "[En Passant]"){

    BoardSquares::init_files();
    BoardSquares::init_ranks();
    BoardSquares::init_squares();
    MoveSet::set_attack_sets();
    MoveSet::init_attack_masks();

    Board* b = new Board();    
    const char* fen_path = "./positions/starting_position.txt";
    b->parse_fen(fen_path);
    vector<unsigned int> move_list = {
        MoveUtils::create_move(e2, e4, WHITE, pPAWN, NO_PIECE, DOUBLE_PAWN_PUSH),
        MoveUtils::create_move(f7, f5, BLACK, pPAWN, NO_PIECE, DOUBLE_PAWN_PUSH),
        MoveUtils::create_move(e4, e5, WHITE, pPAWN, NO_PIECE, QUIET_MOVE),
        MoveUtils::create_move(d7, d5, BLACK, pPAWN, NO_PIECE, DOUBLE_PAWN_PUSH),
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
    delete b;
    delete Bitboard::instanceptr;
    delete BoardInfo::instanceptr;
    b = nullptr;
    Bitboard::instanceptr = nullptr;
    BoardInfo::instanceptr = nullptr;
}

TEST_CASE("En Passant Move Generated", "[En Passant]"){

    BoardSquares::init_files();
    BoardSquares::init_ranks();
    BoardSquares::init_squares();
    MoveSet::set_attack_sets();
    MoveSet::init_attack_masks();

    Board* b = new Board();    
    const char* fen_path = "./positions/starting_position.txt";
    b->parse_fen(fen_path);
    vector<unsigned int> move_list = {
        MoveUtils::create_move(e2, e4, WHITE, pPAWN, NO_PIECE, DOUBLE_PAWN_PUSH),
        MoveUtils::create_move(f7, f5, BLACK, pPAWN, NO_PIECE, DOUBLE_PAWN_PUSH),
        MoveUtils::create_move(e4, e5, WHITE, pPAWN, NO_PIECE, QUIET_MOVE),
        MoveUtils::create_move(d7, d5, BLACK, pPAWN, NO_PIECE, DOUBLE_PAWN_PUSH),
    };
    MoveGen mg = MoveGen(WHITE, mEP_CAPTURE);

    SECTION("move type is mEP_CAPTURE"){
        REQUIRE(mg.get_special_move_type() == mEP_CAPTURE);
    }
    SECTION("single en passant move generated"){

        for(auto move: move_list){
            b->apply_move(move);
        }        

        unsigned int expected_move = MoveUtils::create_move(e5, d6, WHITE, pPAWN, pPAWN, EP_CAPTURE, File::d);
        unsigned int move = mg.get_special_move();

        REQUIRE(expected_move == move);

        vector<unsigned int> reversed_move_list(move_list.rbegin(), move_list.rend());
        for(auto move: reversed_move_list){
            b->reverse_move(move);
        }
    }
    move_list = {
        MoveUtils::create_move(a2, a4, WHITE, pPAWN, NO_PIECE, DOUBLE_PAWN_PUSH),
        MoveUtils::create_move(c7, c5, BLACK, pPAWN, NO_PIECE, DOUBLE_PAWN_PUSH),
        MoveUtils::create_move(a4, a5, WHITE, pPAWN, NO_PIECE, QUIET_MOVE),
        MoveUtils::create_move(b7, b5, BLACK, pPAWN, NO_PIECE, DOUBLE_PAWN_PUSH)
    };    
    MoveGen mg1 = MoveGen(WHITE, mEP_CAPTURE);

    SECTION("queen side (a-b capture) en passant move generated"){

        for(auto move: move_list){
            b->apply_move(move);
        }        

        unsigned int expected_move = MoveUtils::create_move(a5, b6, WHITE, pPAWN, pPAWN, EP_CAPTURE, File::b);
        unsigned int move = mg1.get_special_move();

        REQUIRE(expected_move == move);

        vector<unsigned int> reversed_move_list(move_list.rbegin(), move_list.rend());
        for(auto move: reversed_move_list){
            b->reverse_move(move);
        }
    }        
    move_list = {
        MoveUtils::create_move(b2, b4, WHITE, pPAWN, NO_PIECE, DOUBLE_PAWN_PUSH),
        MoveUtils::create_move(c7, c5, BLACK, pPAWN, NO_PIECE, DOUBLE_PAWN_PUSH),
        MoveUtils::create_move(b4, b5, WHITE, pPAWN, NO_PIECE, QUIET_MOVE),
        MoveUtils::create_move(a7, a5, BLACK, pPAWN, NO_PIECE, DOUBLE_PAWN_PUSH)
    };

    MoveGen mg2 = MoveGen(WHITE, mEP_CAPTURE);

    SECTION("queen side (b-a capture) en passant move generated"){

        for(auto move: move_list){
            b->apply_move(move);
        }        

        unsigned int expected_move = MoveUtils::create_move(b5, a6, WHITE, pPAWN, pPAWN, EP_CAPTURE, File::a);
        unsigned int move = mg2.get_special_move();

        REQUIRE(expected_move == move);

        vector<unsigned int> reversed_move_list(move_list.rbegin(), move_list.rend());
        for(auto move: reversed_move_list){
            b->reverse_move(move);
        }
    }    
    move_list = {
        MoveUtils::create_move(h2, h4, WHITE, pPAWN, NO_PIECE, DOUBLE_PAWN_PUSH),
        MoveUtils::create_move(c7, c5, BLACK, pPAWN, NO_PIECE, DOUBLE_PAWN_PUSH),
        MoveUtils::create_move(h4, h5, WHITE, pPAWN, NO_PIECE, QUIET_MOVE),
        MoveUtils::create_move(g7, g5, BLACK, pPAWN, NO_PIECE, DOUBLE_PAWN_PUSH)
    };
    MoveGen mg3 = MoveGen(WHITE,  mEP_CAPTURE);
    SECTION("king side (h-g capture) en passant move generated"){

        for(auto move: move_list){
            b->apply_move(move);
        }        

        unsigned int expected_move = MoveUtils::create_move(h5, g6, WHITE, pPAWN, pPAWN, EP_CAPTURE, File::g);
        unsigned int move = mg3.get_special_move();

        REQUIRE(expected_move == move);

        vector<unsigned int> reversed_move_list(move_list.rbegin(), move_list.rend());
        for(auto move: reversed_move_list){
            b->reverse_move(move);
        }
    }
    delete b;
    delete Bitboard::instanceptr;
    delete BoardInfo::instanceptr;
    b = nullptr;
    Bitboard::instanceptr = nullptr;
    BoardInfo::instanceptr = nullptr;
}


TEST_CASE("Number of nodes during search","[Search]"){


    BoardSquares::init_files();
    BoardSquares::init_ranks();
    BoardSquares::init_squares();
    MoveSet::set_attack_sets();
    MoveSet::init_attack_masks();

    Board* b = new Board();
    const char* fen_path = "./positions/starting_position.txt";
    b->parse_fen(fen_path);
    Search* s = new Search(b);

    SECTION("Depth = 1"){
        // s->search(1, WHITE);
        REQUIRE(s->perft(1, 1, WHITE) == 20);
        REQUIRE(s->num_captures == 0);
    }
    SECTION("Depth = 2"){
        // s->search(2, WHITE);
        REQUIRE(s->perft(2, 2, WHITE) == 400);
        REQUIRE(s->num_captures == 0);
    }
    SECTION("Depth = 3"){
        // s->search(3, WHITE);
        REQUIRE(s->perft(3, 3, WHITE) == 8902);
        REQUIRE(s->num_captures == 34);
    }
    SECTION("Depth = 4"){
        // s->search(4, WHITE);
        REQUIRE(s->perft(4, 4, WHITE) == 197281);
        REQUIRE(s->num_captures == 1576);
    }
    SECTION("Depth = 5"){
        // s->search(5, WHITE);
        REQUIRE(s->perft(5, 5, WHITE) == 4865609);
        REQUIRE(s->num_captures == 82719);
    }
    SECTION("Depth = 6"){
        // s->search(6, WHITE);
        // REQUIRE(s->num_nodes == 119060324);
        REQUIRE(s->perft(6, 6, WHITE) == 119060324);
        REQUIRE(s->num_captures == 2812008);
    }
    delete b;
    delete Bitboard::instanceptr;
    delete BoardInfo::instanceptr;
    b = nullptr;
    Bitboard::instanceptr = nullptr;
    BoardInfo::instanceptr = nullptr;
}
