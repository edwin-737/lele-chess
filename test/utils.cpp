
#ifdef ENABLE_PROFILER
#include <gperftools/profiler.h>
#endif

#include "utils.hpp"
#include "worker.hpp"
bool test_perft(std::string fen_path, unsigned int depth, unsigned int expected_result, std::vector<unsigned int> setup_moves)
{

    Bitboard _bb = Bitboard();
    BoardInfo _bi = BoardInfo();
    Bitboard* bb = &_bb;
    BoardInfo* bi = &_bi;
    Board _b = Board(fen_path, bb, bi);
    Board* b = &_b;
    for(unsigned int move: setup_moves){
        b->apply_move_if_legal(move);
        b->change_side_to_move();
    }
    PestoEvaluation _pesto = PestoEvaluation(b);
    PestoEvaluation* pesto = &_pesto;

    Search s = Search(b, pesto);
    cout<<"side to move: "<<MoveUtils::side_as_string(b->get_side_to_move())<<"\n";

    #ifdef ENABLE_PROFILER
        if (!ProfilerStart("test-perft-profile.prof")) {
            std::cerr << "ProfilerStart FAILED\n";
        }
    #endif

    unsigned int actual_result = s.perft(depth, depth, b->get_side_to_move());

    #ifdef ENABLE_PROFILER
        ProfilerStop();
    #endif
    cout<<"actual: "<<actual_result<<"\n";
    cout<<"expected: "<<expected_result<<"\n";
    duration<double> elapsed = s.stop - s.start;  // seconds as double (fractional)
    cout<<"time elapsed:"<< elapsed.count() << "\n";
    cout<<"nodes per second: "<<actual_result/elapsed.count()<<"\n";
    return actual_result == expected_result;
}

bool test_alpha_beta_new_movegen(std::string fen_path, unsigned int depth, std::vector<unsigned int> expected_moves)
{

    Bitboard _bb = Bitboard();
    BoardInfo _bi = BoardInfo();
    Bitboard* bb = &_bb;
    BoardInfo* bi = &_bi;
    Board _b = Board(fen_path, bb, bi);
    Board* b = &_b;

    PestoEvaluation _pesto = PestoEvaluation(b);
    PestoEvaluation* pesto = &_pesto;

    Search s = Search(b, pesto);
    pv_t* principal_var = (pv_t*) calloc(1, sizeof(pv_t));
    principal_var->len = 0;

    s.max_depth = 6;
    int alpha = -1e7;
    int beta = 1e7;
    std::atomic<bool> stop_flag = std::atomic<bool>(false);
    unsigned int side_to_move = b->get_side_to_move();
    s.alpha_beta_new_movegen(alpha, beta, 6, side_to_move, side_to_move, stop_flag, 0, principal_var);

    free(principal_var);
    bool found_match = false;

    cout<<"actual move: ";
    MoveUtils::display(s.selected_move);
    for(unsigned int move : expected_moves){
        cout<<"expected move: ";
        MoveUtils::display(move);
        if(move == s.selected_move)
            return true;
    }
    return false;
}

bool test_alpha_beta_new_movegen_short(std::string fen_path, unsigned int depth, std::vector<unsigned int> expected_moves)
{

    Bitboard _bb = Bitboard();
    BoardInfo _bi = BoardInfo();
    Bitboard* bb = &_bb;
    BoardInfo* bi = &_bi;
    Board _b = Board(fen_path, bb, bi);
    Board* b = &_b;

    PestoEvaluation _pesto = PestoEvaluation(b);
    PestoEvaluation* pesto = &_pesto;

    Search s = Search(b, pesto);
    pv_t* principal_var = (pv_t*) calloc(1, sizeof(pv_t));
    principal_var->len = 0;

    s.max_depth = 6;
    int alpha = -1e7;
    int beta = 1e7;
    std::atomic<bool> stop_flag = std::atomic<bool>(false);
    unsigned int side_to_move = b->get_side_to_move();
    s.alpha_beta_new_movegen_short(alpha, beta, 6, side_to_move, side_to_move, stop_flag, 0, principal_var);

    free(principal_var);
    bool found_match = false;

    cout<<"actual move: ";
    MoveUtils::display(s.selected_move);
    for(unsigned int move : expected_moves){
        cout<<"expected move: ";
        MoveUtils::display(move);
        if(move == s.selected_move)
            return true;
    }
    return false;

}
bool test_iterative_deepening(std::string fen_path, unsigned int depth, std::vector<unsigned int> expected_moves)
{

    Bitboard _bb = Bitboard();
    BoardInfo _bi = BoardInfo();
    Bitboard* bb = &_bb;
    BoardInfo* bi = &_bi;
    Board _b = Board(fen_path, bb, bi);
    Board* b = &_b;

    PestoEvaluation _pesto = PestoEvaluation(b);
    PestoEvaluation* pesto = &_pesto;

    Search s = Search(b, pesto);
    pv_t* principal_var = (pv_t*) calloc(1, sizeof(pv_t));
    principal_var->len = 0;

    s.max_depth = 6;
    int alpha = -1e7;
    int beta = 1e7;
    std::atomic<bool> stop_flag = std::atomic<bool>(false);
    unsigned int side_to_move = b->get_side_to_move();
    s.iterative_deepening(6, side_to_move, side_to_move, stop_flag, 0, principal_var);

    free(principal_var);
    bool found_match = false;

    cout<<"actual move: ";
    MoveUtils::display(s.selected_move);
    for(unsigned int move : expected_moves){
        cout<<"expected move: ";
        MoveUtils::display(move);
        if(move == s.selected_move)
            return true;
    }
    return false;
}

bool test_interactive_move_search(std::string fen_path, unsigned int depth, std::vector<std::string> input_moves, std::vector<unsigned int> expected_moves, unsigned int num_seconds)
{

    Bitboard _bb = Bitboard();
    BoardInfo _bi = BoardInfo();
    Bitboard* bb = &_bb;
    BoardInfo* bi = &_bi;
    Board _b = Board(fen_path, bb, bi);
    Board* b = &_b;

    PestoEvaluation _pesto = PestoEvaluation(b);
    PestoEvaluation* pesto = &_pesto;

    Search s = Search(b, pesto);
    pv_t* principal_var = (pv_t*) calloc(1, sizeof(pv_t));
    principal_var->len = 0;

    s.max_depth = depth;
    int alpha = -1e7;
    int beta = 1e7;
    unsigned int opp_side = b->get_side_to_move() ^ 1;
    auto w = std::make_shared<Worker>(s, opp_side, s.max_depth);
    return w->test_loop(num_seconds, input_moves, expected_moves);

}