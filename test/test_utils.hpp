#include <string>
#include <vector>
#include "board.hpp"
#include "bitboard.hpp"
#include "search.hpp"
bool test_perft(std::string fen_path, unsigned int depth, unsigned int expected_result, std::vector<unsigned int> setup_moves={});
bool test_alpha_beta_new_movegen(std::string fen_path, unsigned int depth, std::vector<unsigned int> expected_moves);
bool test_alpha_beta_new_movegen_short(std::string fen_path, unsigned int depth, std::vector<unsigned int> expected_moves);
bool test_iterative_deepening(std::string fen_path, unsigned int depth, std::vector<unsigned int> expected_moves);
bool test_interactive_move_search(std::string fen_path, unsigned int depth, std::vector<std::string> input_moves, std::vector<unsigned int> expected_moves, unsigned int num_seconds=20);
