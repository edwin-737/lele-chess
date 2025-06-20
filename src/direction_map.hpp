
#ifndef directionmap_h
#define directionmap_h
#include <map>
#include "direction.hpp"
typedef unsigned long long uint64;
using namespace std;
namespace DirectionMap{
    inline map<int,uint64> general = {
        {Direction::General::north_east, 9ULL},
        {Direction::General::north, 8ULL},
        {Direction::General::north_west, 7ULL},
        {Direction::General::east, 1ULL},
        {Direction::General::west, 1ULL},
        {Direction::General::south_west, 9ULL},
        {Direction::General::south, 8ULL},
        {Direction::General::south_east, 7ULL}
    };
    inline map<int, uint64> knight = {
        {Direction::Knight::north_left, 15ULL},
        {Direction::Knight::north_right, 17ULL},
        {Direction::Knight::west_up, 6ULL},
        {Direction::Knight::east_up, 10ULL},
        {Direction::Knight::west_down, 10ULL},
        {Direction::Knight::east_down, 6ULL},
        {Direction::Knight::south_left, 17ULL},
        {Direction::Knight::south_right, 15ULL}
    };

};
#endif