#include <vector>
#include <algorithm>
#include <stdlib.h>
#include <string>
#include <catch2/catch_test_macros.hpp>

#include "const.hpp"
#include "evaluation.hpp"
#include "move.hpp"
#include "board_squares.hpp"
#include "search.hpp"
#include "magics.hpp"
#include "bitboard.hpp"
#include "board_info.hpp"
#include "move_set.hpp"
#include "transposition_table.hpp"
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
        unsigned piece_location = 64;
        while((piece_location = bit_scan_forward(piece_board)) != INVALID_LOCATION){
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

bool are_vectors_equal(vector<unsigned int> actual, vector<unsigned int> expected){
    if(actual.size() != expected.size())
        return false;
    sort(actual.begin(), actual.end());
    sort(expected.begin(), expected.end());
    for(int i = 0 ; i < actual.size() ; i ++){
        if(actual[i] != expected[i])
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
    delete TranspositionTable::instanceptr;
    delete Bitboard::instanceptr;
    delete BoardInfo::instanceptr;
    delete Evaluation::instanceptr;
    b = nullptr;
    TranspositionTable::instanceptr = nullptr;
    Bitboard::instanceptr = nullptr;
    BoardInfo::instanceptr = nullptr;
    Evaluation::instanceptr = nullptr;
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
    delete TranspositionTable::instanceptr;
    delete Bitboard::instanceptr;
    delete BoardInfo::instanceptr;
    delete Evaluation::instanceptr;
    b = nullptr;
    TranspositionTable::instanceptr = nullptr;
    Bitboard::instanceptr = nullptr;
    BoardInfo::instanceptr = nullptr;
    Evaluation::instanceptr = nullptr;
}
TEST_CASE("Bishop and Rook Legal Move Set", "[MoveSet]"){
    BoardSquares::init_files();
    BoardSquares::init_ranks();
    BoardSquares::init_squares();
    MoveSet::set_attack_sets();
    MoveSet::init_attack_masks();
    Board* b = new Board();    
    const char* fen_path = "./positions/bishop_move_set_test.txt";
    b->parse_fen(fen_path);
    SECTION("bishop has correct legal moveset"){

        std::vector<unsigned int> expected_white_bishop_legal_moveset_squares = {d4, c5, f4, g5, h6};
        std::vector<unsigned int> expected_black_bishop_legal_moveset_squares = {e5, d4, c3, b2, g5, h4};

        unsigned int white_bishop_location = INVALID_LOCATION;
        unsigned int black_bishop_location = INVALID_LOCATION;
        for(unsigned int sq = 0; sq < NUM_SQUARES ; sq ++){
            if(get_square_bitboard(sq) & b->get_bitboard()->piece_boards[WHITE][pBISHOP]){
                white_bishop_location = sq;
            } else if(get_square_bitboard(sq) & b->get_bitboard()->piece_boards[BLACK][pBISHOP]){
                black_bishop_location = sq;
            }
        }

        uint64 white_bishop_legal_moveset = MoveSet::get_bishop_legal_move_set(b->get_bitboard(), white_bishop_location, WHITE);
        uint64 black_bishop_legal_moveset = MoveSet::get_bishop_legal_move_set(b->get_bitboard(), black_bishop_location, BLACK);

        uint64 expected_white_bishop_legal_moveset = 0;
        for(int i = 0 ; i < expected_white_bishop_legal_moveset_squares.size() ; i ++){
            expected_white_bishop_legal_moveset ^= get_square_bitboard(expected_white_bishop_legal_moveset_squares[i]);
        }
        uint64 expected_black_bishop_legal_moveset = 0;
        for(int i = 0 ; i < expected_black_bishop_legal_moveset_squares.size() ; i ++){
            expected_black_bishop_legal_moveset ^= get_square_bitboard(expected_black_bishop_legal_moveset_squares[i]);
        }
        REQUIRE(expected_white_bishop_legal_moveset == white_bishop_legal_moveset);
        REQUIRE(expected_black_bishop_legal_moveset == black_bishop_legal_moveset);
    }
    SECTION("bishop has correct move set"){
        std::vector<unsigned int> expected_white_bishop_moveset_squares = {d4, c5, f4, g5, h6, d2, f2};
        std::vector<unsigned int> expected_black_bishop_moveset_squares = {e5, d4, c3, b2, g5, h4, e7, g7};

        unsigned int white_bishop_location = INVALID_LOCATION;
        unsigned int black_bishop_location = INVALID_LOCATION;
        for(unsigned int sq = 0; sq < NUM_SQUARES ; sq ++){
            if(get_square_bitboard(sq) & b->get_bitboard()->piece_boards[WHITE][pBISHOP]){
                white_bishop_location = sq;
            } else if(get_square_bitboard(sq) & b->get_bitboard()->piece_boards[BLACK][pBISHOP]){
                black_bishop_location = sq;
            }
        }

        uint64 white_bishop_moveset = MoveSet::get_bishop_move_set(b->get_bitboard(), white_bishop_location);
        uint64 black_bishop_moveset = MoveSet::get_bishop_move_set(b->get_bitboard(), black_bishop_location);

        uint64 expected_white_bishop_moveset = 0;
        for(int i = 0 ; i < expected_white_bishop_moveset_squares.size() ; i ++){
            expected_white_bishop_moveset ^= get_square_bitboard(expected_white_bishop_moveset_squares[i]);
        }
        uint64 expected_black_bishop_moveset = 0;
        for(int i = 0 ; i < expected_black_bishop_moveset_squares.size() ; i ++){
            expected_black_bishop_moveset ^= get_square_bitboard(expected_black_bishop_moveset_squares[i]);
        }

        REQUIRE(expected_white_bishop_moveset == white_bishop_moveset);
        REQUIRE(expected_black_bishop_moveset == black_bishop_moveset);
    }


    delete b;
    delete TranspositionTable::instanceptr;
    delete Bitboard::instanceptr;
    delete BoardInfo::instanceptr;
    delete Evaluation::instanceptr;
    b = nullptr;
    TranspositionTable::instanceptr = nullptr;
    Bitboard::instanceptr = nullptr;
    BoardInfo::instanceptr = nullptr;
    Evaluation::instanceptr = nullptr;
}
TEST_CASE("Blocking Pieces Bitboard", "[Moveset]"){
    BoardSquares::init_files();
    BoardSquares::init_ranks();
    BoardSquares::init_squares();
    MoveSet::set_attack_sets();
    MoveSet::init_attack_masks();

    Board* b = new Board();    
    const char* fen_path = "./positions/bishop_move_set_test.txt";
    b->parse_fen(fen_path);
    Evaluation* e = Evaluation::get_instance();
    // e->init_blocking_pieces();
    SECTION("Bishop has correct blocking pieces Bitboard"){
        std::vector<unsigned int> expected_white_blocking_squares = {d2, f2};
        std::vector<unsigned int> expected_black_blocking_squares = {e7, g7};

        unsigned int white_bishop_location = INVALID_LOCATION;
        unsigned int black_bishop_location = INVALID_LOCATION;
        for(unsigned int sq = 0; sq < NUM_SQUARES ; sq ++){
            if(get_square_bitboard(sq) & b->get_bitboard()->piece_boards[WHITE][pBISHOP]){
                white_bishop_location = sq;
            } else if(get_square_bitboard(sq) & b->get_bitboard()->piece_boards[BLACK][pBISHOP]){
                black_bishop_location = sq;
            }
        }
        uint64 white_bishop_blocking_set = MoveSet::get_bishop_blocking_set(b->get_bitboard(), white_bishop_location, WHITE);
        uint64 black_bishop_blocking_set = MoveSet::get_bishop_blocking_set(b->get_bitboard(), black_bishop_location, BLACK);

        uint64 expected_white_bishop_blocking_set = 0;
        for(int i = 0 ; i < expected_white_blocking_squares.size() ; i ++){
            expected_white_bishop_blocking_set ^= get_square_bitboard(expected_white_blocking_squares[i]);
        }
        uint64 expected_black_bishop_blocking_set = 0;
        for(int i = 0 ; i < expected_black_blocking_squares.size() ; i ++){
            expected_black_bishop_blocking_set ^= get_square_bitboard(expected_black_blocking_squares[i]);
        }
        REQUIRE(expected_white_bishop_blocking_set == white_bishop_blocking_set);
        REQUIRE(expected_black_bishop_blocking_set == black_bishop_blocking_set);
        
    }


    delete b;
    delete TranspositionTable::instanceptr;
    delete Bitboard::instanceptr;
    delete BoardInfo::instanceptr;
    delete Evaluation::instanceptr;
    b = nullptr;
    TranspositionTable::instanceptr = nullptr;
    Bitboard::instanceptr = nullptr;
    BoardInfo::instanceptr = nullptr;
    Evaluation::instanceptr = nullptr;
}

TEST_CASE("Blocking Pieces", "[Bitboard]"){

    BoardSquares::init_files();
    BoardSquares::init_ranks();
    BoardSquares::init_squares();
    MoveSet::set_attack_sets();
    MoveSet::init_attack_masks();

    Board* b = new Board();
    string fen_path = "./positions/bishop_move_set_test.txt";
    b->parse_fen(fen_path);
    Evaluation* e = Evaluation::get_instance();
    // e->init_blocking_pieces();
    SECTION("Initial blocking pieces", "[Bitboard]"){
        std::vector<unsigned int> expected_white_blocking_squares = {d2, f2, c5};
        std::vector<unsigned int> expected_black_blocking_squares = {e7, g7, b2};

        unsigned int white_bishop_location = INVALID_LOCATION;
        unsigned int black_bishop_location = INVALID_LOCATION;
        for(unsigned int sq = 0; sq < NUM_SQUARES ; sq ++){
            if(get_square_bitboard(sq) & b->get_bitboard()->piece_boards[WHITE][pBISHOP]){
                white_bishop_location = sq;
            } else if(get_square_bitboard(sq) & b->get_bitboard()->piece_boards[BLACK][pBISHOP]){
                black_bishop_location = sq;
            }
        }
        unsigned int white_bishop_key = e->get_piece_key_on_square(white_bishop_location);
        unsigned int black_bishop_key =  e->get_piece_key_on_square(black_bishop_location);
        cout<<"white_bishop_location: "<<white_bishop_location<<endl;
        cout<<"black_bishop_location: "<<black_bishop_location<<endl;
        cout<<"white_bishop_key: "<<white_bishop_key<<endl;
        cout<<"black_bishop_key: "<<black_bishop_key<<endl;

        std::vector<unsigned int> blocking_pieces_white_bishop = e->get_blocking_piece_keys(white_bishop_key);
        for(int i = 0 ; i < blocking_pieces_white_bishop.size() ; i ++){
            cout<<"white: "<<blocking_pieces_white_bishop[i]<<endl;
        }

        std::vector<unsigned int> blocking_pieces_black_bishop = e->get_blocking_piece_keys(black_bishop_key);
        cout<<"num black blocking squares: "<< blocking_pieces_black_bishop.size()<<endl;
        for(int i = 0 ; i < blocking_pieces_black_bishop.size() ; i ++){
            cout<<"black: "<<blocking_pieces_black_bishop[i]<<endl;
        }
        bool white_correct = are_vectors_equal(expected_white_blocking_squares, blocking_pieces_white_bishop);
        bool black_correct = are_vectors_equal(expected_black_blocking_squares, blocking_pieces_black_bishop);
        REQUIRE(white_correct);
        REQUIRE(black_correct);
    }


    delete b;
    delete TranspositionTable::instanceptr;
    delete Bitboard::instanceptr;
    delete BoardInfo::instanceptr;
    delete Evaluation::instanceptr;
    b = nullptr;
    TranspositionTable::instanceptr = nullptr;
    Bitboard::instanceptr = nullptr;
    BoardInfo::instanceptr = nullptr;
    Evaluation::instanceptr = nullptr;
}

TEST_CASE("Attack table", "[Evaluation]"){

    BoardSquares::init_files();
    BoardSquares::init_ranks();
    BoardSquares::init_squares();
    MoveSet::set_attack_sets();
    MoveSet::init_attack_masks();

    Board* b = new Board();
    Evaluation* e = Evaluation::get_instance();
    string fen_path = "./positions/starting_position.txt";
    b->parse_fen(fen_path);
    // e->init_attack_table();
    SECTION("Initial attack tables", "[Evaluation]"){

        int expected_attack_table[] = {
            0, 5, 1, 1, 1, 1, 5, 0,
            5, 7, 1, 16, 16, 1, 7, 5,
            16, 18, 25, 18, 18, 25, 18, 16,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            -16, -18, -25, -18, -18, -25, -18, -16,
            -5, -7, -1, -16, -16, -1, -7, -5,
            0, -5, -1, -1, -1, -1, -5, 0,
        };
        for(int sq = 0 ; sq < NUM_SQUARES ; sq ++){
            REQUIRE(e->get_attack_table_val(sq) == expected_attack_table[sq]);
        }
    }

    delete b;
    delete TranspositionTable::instanceptr;
    delete Bitboard::instanceptr;
    delete BoardInfo::instanceptr;
    delete Evaluation::instanceptr;
    b = nullptr;
    TranspositionTable::instanceptr = nullptr;
    Bitboard::instanceptr = nullptr;
    BoardInfo::instanceptr = nullptr;
    Evaluation::instanceptr = nullptr;
}

TEST_CASE("En Passant Rights Updated", "[BoardInfo]"){

    BoardSquares::init_files();
    BoardSquares::init_ranks();
    BoardSquares::init_squares();
    MoveSet::set_attack_sets();
    MoveSet::init_attack_masks();

    Board* b = new Board();    
    const char* fen_path = "./positions/starting_position.txt";
    b->parse_fen(fen_path);
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


    delete b;
    delete TranspositionTable::instanceptr;
    delete Bitboard::instanceptr;
    delete BoardInfo::instanceptr;
    delete Evaluation::instanceptr;
    b = nullptr;
    TranspositionTable::instanceptr = nullptr;
    Bitboard::instanceptr = nullptr;
    BoardInfo::instanceptr = nullptr;
    Evaluation::instanceptr = nullptr;
}

TEST_CASE("En Passant Move Generated", "[MoveGen]"){

    BoardSquares::init_files();
    BoardSquares::init_ranks();
    BoardSquares::init_squares();
    MoveSet::set_attack_sets();
    MoveSet::init_attack_masks();

    Board* b = new Board();    
    const char* fen_path = "./positions/starting_position.txt";
    b->parse_fen(fen_path);
    vector<unsigned int> move_list = {
        MoveUtils::create_move(e2, e4, WHITE, pPAWN, DOUBLE_PAWN_PUSH),
        MoveUtils::create_move(f7, f5, BLACK, pPAWN, DOUBLE_PAWN_PUSH),
        MoveUtils::create_move(e4, e5, WHITE, pPAWN, QUIET_MOVE),
        MoveUtils::create_move(d7, d5, BLACK, pPAWN, DOUBLE_PAWN_PUSH),
    };
    MoveGen mg = MoveGen(WHITE, mEP_CAPTURE);
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
    MoveGen mg1 = MoveGen(WHITE, mEP_CAPTURE);
    // MoveGen mg1 = MoveGen(WHITE);
    // mg1.set_move_type(mEP_CAPTURE);

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

    MoveGen mg2 = MoveGen(WHITE, mEP_CAPTURE);

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
    MoveGen mg3 = MoveGen(WHITE,  mEP_CAPTURE);
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
    delete b;
    delete Bitboard::instanceptr;
    delete BoardInfo::instanceptr;
    delete TranspositionTable::instanceptr;
    b = nullptr;
    Bitboard::instanceptr = nullptr;
    BoardInfo::instanceptr = nullptr;
    TranspositionTable::instanceptr = nullptr;
}

TEST_CASE("Only Captures","[MoveGen]"){

    BoardSquares::init_files();
    BoardSquares::init_ranks();
    BoardSquares::init_squares();
    MoveSet::set_attack_sets();
    MoveSet::init_attack_masks();

    Board* b = new Board();
    const char* fen_path = "./positions/starting_position.txt";
    b->parse_fen(fen_path);
    // MoveGen mg = MoveGen(WHITE, mQUIET, true);
    MoveGen mg = MoveGen(WHITE);
    mg.set_gen_type(ONLY_CAPTURES);

    SECTION("No captures generated with starting position"){
        unsigned int move = 0;
        int move_count = 0;
        while((move = mg.get_move()) != NO_MOVES_LEFT){
            if(move == INCREMENTING_MOVE_TYPE)
                continue;
            move_count++;
        }
        REQUIRE(move_count == 0);
    }
    delete b;
    delete Bitboard::instanceptr;
    delete BoardInfo::instanceptr;
    b = nullptr;
    Bitboard::instanceptr = nullptr;
    BoardInfo::instanceptr = nullptr;

    b = new Board();
    const char* queen_trade_path = "./positions/queen_trade.txt";
    b->parse_fen(queen_trade_path);

    // MoveGen mg1 = MoveGen(BLACK, mQUIET, true);
    MoveGen mg1 = MoveGen(BLACK);
    mg1.set_gen_type(ONLY_CAPTURES);

    vector<unsigned int> expected_moves = {
        MoveUtils::create_move(f8, a3, BLACK, pBISHOP,  CAPTURE, pPAWN),
        MoveUtils::create_move(d8, d1, BLACK, pQUEEN, CAPTURE, pQUEEN),
    };
    vector<unsigned int> actual_moves;
    SECTION("One capture generated with queen trade position"){
        unsigned int move = 0;
        int move_count = 0;
        
        while((move = mg1.get_move()) != NO_MOVES_LEFT){
            if(move == INCREMENTING_MOVE_TYPE)
                continue;
            move_count++;
            actual_moves.push_back(move);
        }
        for(int i = 0 ; i < actual_moves.size() ; i ++){
            // cout<<actual_moves[i]<<" ";
            MoveUtils::display(actual_moves[i]);
            cout<<" ";
        }
        cout<<endl;
        REQUIRE(move_count == 2);
        REQUIRE(are_vectors_equal(actual_moves, expected_moves));
    }
    delete b;
    delete Bitboard::instanceptr;
    delete BoardInfo::instanceptr;
    delete Evaluation::instanceptr;
    delete TranspositionTable::instanceptr;
    b = nullptr;
    Bitboard::instanceptr = nullptr;
    BoardInfo::instanceptr = nullptr;
    TranspositionTable::instanceptr = nullptr;
    Evaluation::instanceptr = nullptr;

}
TEST_CASE("Number of nodes during search","[MoveGen]"){


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
        REQUIRE(s->perft(1, 1, WHITE) == 20);
        REQUIRE(s->num_captures == 0);
    }
    SECTION("Depth = 2"){
        REQUIRE(s->perft(2, 2, WHITE) == 400);
        REQUIRE(s->num_captures == 0);
    }
    SECTION("Depth = 3"){
        REQUIRE(s->perft(3, 3, WHITE) == 8902);
        REQUIRE(s->num_captures == 34);
    }
    delete b;
    delete Bitboard::instanceptr;
    delete BoardInfo::instanceptr;
    b = nullptr;
    Bitboard::instanceptr = nullptr;
    BoardInfo::instanceptr = nullptr;
    TranspositionTable::instanceptr = nullptr;
}
TEST_CASE("Number of nodes during search depth 4","[MoveGen]"){


    BoardSquares::init_files();
    BoardSquares::init_ranks();
    BoardSquares::init_squares();
    MoveSet::set_attack_sets();
    MoveSet::init_attack_masks();

    Board* b = new Board();
    const char* fen_path = "./positions/starting_position.txt";
    b->parse_fen(fen_path);

    Search* s = new Search(b);
    SECTION("Depth = 4"){
        REQUIRE(s->perft(4, 4, WHITE) == 197281);
        REQUIRE(s->num_captures == 1576);
    }
    delete b;
    delete Bitboard::instanceptr;
    delete BoardInfo::instanceptr;
    delete Evaluation::instanceptr;
    b = nullptr;
    Bitboard::instanceptr = nullptr;
    BoardInfo::instanceptr = nullptr;
    TranspositionTable::instanceptr = nullptr;
    Evaluation::instanceptr = nullptr;
}
TEST_CASE("Number of nodes during search depth 5","[MoveGen]"){

    BoardSquares::init_files();
    BoardSquares::init_ranks();
    BoardSquares::init_squares();
    MoveSet::set_attack_sets();
    MoveSet::init_attack_masks();

    Board* b = new Board();
    const char* fen_path = "./positions/starting_position.txt";
    b->parse_fen(fen_path);
    Search* s = new Search(b);
    SECTION("Depth = 5"){
        REQUIRE(s->perft(5, 5, WHITE) == 4865609);
        REQUIRE(s->num_captures == 82719);
    }
    delete b;
    delete Bitboard::instanceptr;
    delete BoardInfo::instanceptr;
    delete Evaluation::instanceptr;
    b = nullptr;
    Bitboard::instanceptr = nullptr;
    BoardInfo::instanceptr = nullptr;
    TranspositionTable::instanceptr = nullptr;
    Evaluation::instanceptr = nullptr;
}
TEST_CASE("promotions during search", "[MoveGen]"){
    BoardSquares::init_files();
    BoardSquares::init_ranks();
    BoardSquares::init_squares();
    MoveSet::set_attack_sets();
    MoveSet::init_attack_masks();

    Board* b = new Board();
    string fen_path = "./positions/ep_fen.txt";
    b->parse_fen(fen_path);
    Search* s = new Search(b);


    SECTION("Depth = 5"){
        // s->search(1, WHITE);
        REQUIRE(s->perft(5, 5, WHITE) == 16422290);
        REQUIRE(s->num_captures == 669892);
        REQUIRE(s->num_promotions == 628);
        REQUIRE(s->num_capture_promotions == 9808);

    }
    delete b;
    delete TranspositionTable::instanceptr;
    delete Bitboard::instanceptr;
    delete BoardInfo::instanceptr;
    delete Evaluation::instanceptr;
    b = nullptr;
    TranspositionTable::instanceptr = nullptr;
    Bitboard::instanceptr = nullptr;
    BoardInfo::instanceptr = nullptr;
    Evaluation::instanceptr = nullptr;
}
TEST_CASE("castles during search", "[MoveGen]"){

    BoardSquares::init_files();
    BoardSquares::init_ranks();
    BoardSquares::init_squares();
    MoveSet::set_attack_sets();
    MoveSet::init_attack_masks();

    Board* b = new Board();
    string fen_path = "./positions/castle_fen.txt";
    b->parse_fen(fen_path);
    Search* s = new Search(b);


    SECTION("Depth = 5"){
        REQUIRE(s->perft(5, 5, WHITE) == 22273312);
        REQUIRE(s->num_castles == 19682);

    }

    delete b;
    delete Bitboard::instanceptr;
    delete BoardInfo::instanceptr;
    delete TranspositionTable::instanceptr;
    delete Evaluation::instanceptr;
    b = nullptr;
    Bitboard::instanceptr = nullptr;
    BoardInfo::instanceptr = nullptr;
    TranspositionTable::instanceptr = nullptr;
    Evaluation::instanceptr = nullptr;

}
TEST_CASE("Unique Zobrist Hash vals", "[TranspositionTable]"){
    BoardSquares::init_files();
    BoardSquares::init_ranks();
    BoardSquares::init_squares();
    MoveSet::set_attack_sets();
    MoveSet::init_attack_masks();

    Board* b = new Board();
    string fen_path = "./positions/starting_position.txt";
    b->parse_fen(fen_path);
    vector<uint64> seen_zobrist_vals;
    bool found_duplicate = false;
    for(int i = 0 ; i < NUM_ZOBRIST_VALS ; i ++){
        for(int j = 0 ; j < seen_zobrist_vals.size() ; j ++){
            if(seen_zobrist_vals[j] == b->tt->zobrist_vals[i])
                found_duplicate = true;
        }
        seen_zobrist_vals.push_back(b->tt->zobrist_vals[i]);
    }
    REQUIRE(!found_duplicate);
    delete b;
    delete Bitboard::instanceptr;
    delete BoardInfo::instanceptr;
    delete TranspositionTable::instanceptr;
    delete Evaluation::instanceptr;
    b = nullptr;
    Bitboard::instanceptr = nullptr;
    BoardInfo::instanceptr = nullptr;
    TranspositionTable::instanceptr = nullptr;
    Evaluation::instanceptr = nullptr;
}
TEST_CASE("Updating hash val", "[TranspositionTable]"){
    BoardSquares::init_files();
    BoardSquares::init_ranks();
    BoardSquares::init_squares();
    MoveSet::set_attack_sets();
    MoveSet::init_attack_masks();

    Board* b = new Board();
    string fen_path = "./positions/starting_position.txt";
    b->parse_fen(fen_path);

    cout<<"initial hash_val: "<<b->tt->hash_val<<endl;
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
                    REQUIRE(b->tt->hash_val != seen_hash_vals[i-1]);
                }
                seen_hash_vals.push_back(b->tt->hash_val);
            }

            bool found_hash_val = true;
            for(int i = 3 ; i >= 0 ; i --){
                b->reverse_move(move_order1[i]);
                if(i > 0 && b->tt->hash_val != seen_hash_vals[i - 1]){
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
                    REQUIRE(b->tt->hash_val != seen_hash_vals[i-1]);
                }
                seen_hash_vals.push_back(b->tt->hash_val);
            }

            bool found_hash_val = true;
            for(int i = 6 ; i >= 0 ; i --){
                b->reverse_move(move_order1[i]);
                if(i > 0 && b->tt->hash_val != seen_hash_vals[i - 1]){
                    found_hash_val = false;
                }

            }

            REQUIRE(found_hash_val == true);
        }
        SECTION("hash_val from transposition should the same"){
            vector<uint64> seen_hash_vals;
            for(int i = 0 ; i < 7 ; i ++){
                b->apply_move(move_order1[i]);
                seen_hash_vals.push_back(b->tt->hash_val);
            }
            for(int i = 6 ; i >= 0 ; i --){
                b->reverse_move(move_order1[i]);
            }
            for(int i = 0 ; i < 7 ; i ++){
                b->apply_move(move_order2[i]);
            }
            // at index=6, the position of the move_order2 should be equal to the position after move_order1
            REQUIRE(b->tt->hash_val == seen_hash_vals[6]);
            for(int i = 6 ; i >= 0 ; i --){
                b->reverse_move(move_order2[i]);
            }

            for(int i = 0 ; i < 6 ; i ++){
                b->apply_move(move_order2[i]);
            }
            // at index=5, the position of the move_order2 should not be equal to the position after move_order1
            REQUIRE(b->tt->hash_val != seen_hash_vals[5]);
        }
    }
    delete b;
    delete TranspositionTable::instanceptr;
    delete Bitboard::instanceptr;
    delete BoardInfo::instanceptr;
    delete Evaluation::instanceptr;
    b = nullptr;
    TranspositionTable::instanceptr = nullptr;
    Bitboard::instanceptr = nullptr;
    BoardInfo::instanceptr = nullptr;
    Evaluation::instanceptr = nullptr;
}
TEST_CASE("Updating hash val en passant", "[TranspositionTable]"){
    BoardSquares::init_files();
    BoardSquares::init_ranks();
    BoardSquares::init_squares();
    MoveSet::set_attack_sets();
    MoveSet::init_attack_masks();

    Board* b = new Board();
    string fen_path = "./positions/starting_position.txt";
    b->parse_fen(fen_path);

    cout<<"initial hash_val: "<<b->tt->hash_val<<endl;
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
                    REQUIRE(b->tt->hash_val != seen_hash_vals[i-1]);
                }
                seen_hash_vals.push_back(b->tt->hash_val);
            }

            bool found_hash_val = true;
            for(int i = 3 ; i >= 0 ; i --){
                b->reverse_move(move_order1[i]);
                if(i > 0 && b->tt->hash_val != seen_hash_vals[i - 1]){
                    found_hash_val = false;
                }

            }

            REQUIRE(found_hash_val == true);
        }
        SECTION("hash_val from transposition should the same"){
            vector<uint64> seen_hash_vals;
            for(int i = 0 ; i < 4 ; i ++){
                b->apply_move(move_order1[i]);
                seen_hash_vals.push_back(b->tt->hash_val);
            }
            unsigned int move_order1_ep_right = BoardInfo::get_instance()->peek_ep_right();

            for(int i = 3 ; i >= 0 ; i --){
                b->reverse_move(move_order1[i]);
            }
            for(int i = 0 ; i < 4 ; i ++){
                b->apply_move(move_order2[i]);
            }            
            unsigned int move_order2_ep_right = BoardInfo::get_instance()->peek_ep_right();
            REQUIRE(move_order1_ep_right != move_order2_ep_right);
            REQUIRE(b->tt->hash_val != seen_hash_vals[3]);
        }
    }
    delete b;
    delete TranspositionTable::instanceptr;
    delete Bitboard::instanceptr;
    delete BoardInfo::instanceptr;
    delete Evaluation::instanceptr;
    b = nullptr;
    TranspositionTable::instanceptr = nullptr;
    Bitboard::instanceptr = nullptr;
    BoardInfo::instanceptr = nullptr;
    Evaluation::instanceptr = nullptr;
}
TEST_CASE("Transposition Table cach matches (simple)", "[TranspositionTable]"){
    BoardSquares::init_files();
    BoardSquares::init_ranks();
    BoardSquares::init_squares();
    MoveSet::set_attack_sets();
    MoveSet::init_attack_masks();

    Board* b = new Board();
    string fen_path = "./positions/transposition_test.txt";
    b->parse_fen(fen_path);
    Search* s = new Search(b);
    cout<<"before search tt_found_count[3]: "<<s->tt_found_count[3]<<endl;
    s->perft(5,5, WHITE, 0, true);
    SECTION("Depth = 4, tt_found_count == tt_match_count"){
        REQUIRE(s->tt_found_count[1] == 0);
        REQUIRE(s->tt_found_count[2] == 28);
        REQUIRE(s->tt_found_count[3] == 520);
    }

    delete b;
    delete s;
    delete TranspositionTable::instanceptr;
    delete Bitboard::instanceptr;
    delete BoardInfo::instanceptr;
    delete Evaluation::instanceptr;
    b = nullptr;
    s = nullptr;
    TranspositionTable::instanceptr = nullptr;
    Bitboard::instanceptr = nullptr;
    BoardInfo::instanceptr = nullptr;
    Evaluation::instanceptr = nullptr;
}
TEST_CASE("Transposition Table cache matches", "[TranspositionTable]"){
    BoardSquares::init_files();
    BoardSquares::init_ranks();
    BoardSquares::init_squares();
    MoveSet::set_attack_sets();
    MoveSet::init_attack_masks();

    Board* b = new Board();
    string fen_path = "./positions/starting_position.txt";
    b->parse_fen(fen_path);
    Search* s = new Search(b);
    cout<<"before search tt_found_count[3]: "<<s->tt_found_count[3]<<endl;
    unsigned int nodes = s->perft(6, 6, WHITE, 0, true);
    SECTION("Depth = 6, tt_found_count == tt_match_count"){
        REQUIRE(s->tt_found_count[2] == 1300);
        REQUIRE(s->tt_found_count[3] == 67152);
        REQUIRE(s->tt_found_count[4] == 1284092);
        REQUIRE(s->tt_found_count[5] == 16699235);
        REQUIRE(nodes == 119060324);
    }
    delete b;
    delete s;
    delete TranspositionTable::instanceptr;
    delete Bitboard::instanceptr;
    delete BoardInfo::instanceptr;
    delete Evaluation::instanceptr;
    b = nullptr;
    s = nullptr;
    TranspositionTable::instanceptr = nullptr;
    Bitboard::instanceptr = nullptr;
    BoardInfo::instanceptr = nullptr;
    Evaluation::instanceptr = nullptr;
}
TEST_CASE("Alpha beta pruning selected move", "[Search]"){
    BoardSquares::init_files();
    BoardSquares::init_ranks();
    BoardSquares::init_squares();
    MoveSet::set_attack_sets();
    MoveSet::init_attack_masks();

    Board* b = new Board();
    string fen_path = "./positions/bratko-kopec/bk_1.txt";
    b->parse_fen(fen_path);
    Search* s = new Search(b);

    SECTION("bk 1"){
        pv_t* principal_var = (pv_t*) calloc(1, sizeof(pv_t));
        principal_var->len = 0;

        s->max_depth = 6;
        int alpha = -1e5;
        int beta = 1e5;
        s->alpha_beta(alpha, beta, 6, BLACK, BLACK, 0, principal_var);

        free(principal_var);

        MoveUtils::display(s->selected_move);
        REQUIRE(s->selected_move == MoveUtils::create_move(d6, d1, BLACK, pQUEEN));
    }
    delete b;
    delete s;
    delete TranspositionTable::instanceptr;
    delete Bitboard::instanceptr;
    delete BoardInfo::instanceptr;
    delete Evaluation::instanceptr;
    b = nullptr;
    s = nullptr;
    TranspositionTable::instanceptr = nullptr;
    Bitboard::instanceptr = nullptr;
    BoardInfo::instanceptr = nullptr;
    Evaluation::instanceptr = nullptr;
}
TEST_CASE("Alpha beta pruning selected move with transpositions", "[Search]"){
    BoardSquares::init_files();
    BoardSquares::init_ranks();
    BoardSquares::init_squares();
    MoveSet::set_attack_sets();
    MoveSet::init_attack_masks();

    Board* b = new Board();
    string fen_path = "./positions/bratko-kopec/bk_1.txt";
    b->parse_fen(fen_path);
    Search* s = new Search(b);

    SECTION("bk 1"){
        pv_t* principal_var = (pv_t*) calloc(1, sizeof(pv_t));
        principal_var->len = 0;

        s->max_depth = 6;
        int alpha = -1e5;
        int beta = 1e5;
        s->alpha_beta(alpha, beta, 6, BLACK, BLACK, 0, principal_var, true);

        free(principal_var);

        MoveUtils::display(s->selected_move);
        REQUIRE(s->selected_move == MoveUtils::create_move(d6, d1, BLACK, pQUEEN));
    }
    delete b;
    delete s;
    delete TranspositionTable::instanceptr;
    delete Bitboard::instanceptr;
    delete BoardInfo::instanceptr;
    delete Evaluation::instanceptr;
    b = nullptr;
    TranspositionTable::instanceptr = nullptr;
    Bitboard::instanceptr = nullptr;
    BoardInfo::instanceptr = nullptr;
    Evaluation::instanceptr = nullptr;
}
TEST_CASE("Piece Key to Square Table", "[Bitboard]"){
    BoardSquares::init_files();
    BoardSquares::init_ranks();
    BoardSquares::init_squares();
    MoveSet::set_attack_sets();
    MoveSet::init_attack_masks();

    Board* b = new Board();
    Evaluation* e = Evaluation::get_instance();
    string fen_path = "./positions/starting_position.txt";
    b->parse_fen(fen_path);
    vector<unsigned int> piece_key_vals = {
        // white
        0, 1, 2, 3, 4, 5, 6, 7,  // pawns
        8, 9,                    // knights
        16, 17,                  // bishops
        24, 25,                  // rooks
        32,                      // queen
        40,                      // king
        // black
        48, 49, 50, 51, 52, 53, 54, 55, // pawns
        56, 57,                         // knights
        64, 65,                         // bishops
        72, 73,                         // rooks
        80,                             // queen
        88                              // king
    };

    SECTION("piece keys present in piece_to_square map"){
        set<unsigned int> found_squares;
        for(unsigned int i = 0 ; i < piece_key_vals.size() ; i ++){
            unsigned int expected_piece_key = piece_key_vals[i];
            unsigned int value_of_expected_key = e->get_square_of_piece_key(expected_piece_key);
            auto square = found_squares.find(expected_piece_key);
            bool square_not_found = square == found_squares.end();
            cout<<"expected_piece_key: "<<expected_piece_key<<"\n";
            REQUIRE(square_not_found);
            REQUIRE(value_of_expected_key < NUM_SQUARES);
            found_squares.insert(expected_piece_key);
        }
    }
    SECTION("square to piece key values"){
        for(unsigned int i = 0 ; i < piece_key_vals.size() ; i ++){
            int cnt_equal = 0;
            unsigned int expected_piece_key = piece_key_vals[i];
            for(int j = 0 ; j < NUM_SQUARES; j ++){
                unsigned int actual_piece_key = e->get_piece_key_on_square(j);
                if(actual_piece_key == expected_piece_key){
                    cnt_equal++;
                    cout<<"expected_piece_key: "<<expected_piece_key<<"\n";
                    cout<<"actual_piece_key: "<<actual_piece_key<<"\n";
                }
            }
            REQUIRE(cnt_equal == 1);
        }
    }

    delete b;
    delete TranspositionTable::instanceptr;
    delete Bitboard::instanceptr;
    delete BoardInfo::instanceptr;
    delete Evaluation::instanceptr;
    b = nullptr;
    TranspositionTable::instanceptr = nullptr;
    Bitboard::instanceptr = nullptr;
    BoardInfo::instanceptr = nullptr;
    Evaluation::instanceptr = nullptr;
}