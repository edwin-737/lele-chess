#ifndef evaluation_h
#define evaluation_h
#include "const.hpp"
typedef unsigned long long uint64;

class Evaluation{
private:
    Evaluation(){}

    int material = 0;
    int piece_square_value = 0;
public:
    static Evaluation* instanceptr;
    Evaluation(const Evaluation& obj) = delete;
    static Evaluation* get_instance();
    int get_material();
    int get_piece_square_values();
    int get_evaluation();
    void update_material(unsigned int move, bool reverse=false);
    void update_piece_square_value(unsigned int move, bool reverse=false);
};
#endif