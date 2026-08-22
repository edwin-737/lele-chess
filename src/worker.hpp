#include "search.hpp"
#define MAX_NUM_MOVES 200
class Worker {
public:
    Worker(Search _s, unsigned int _side, int _depth):s(_s), side(_side), depth(_depth){
        stop_flag.store(false);
    }
    void run_task(std::atomic<bool>& stop_flag);
    void loop(long long num_seconds_per_move, int num_iterations=MAX_NUM_MOVES);
    bool test_loop(long long num_seconds_per_move, std::vector<std::string> input_moves, std::vector<unsigned int> exptected_moves);
private:
    Search s;
    unsigned int side;
    int depth;
    std::atomic<bool> stop_flag;
};