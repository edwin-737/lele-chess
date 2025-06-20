#ifndef direction_h
#define direction_h
namespace Direction{
    enum General
    {
        north_west,
        north,
        north_east,
        east,
        west,
        south_east,
        south,
        south_west,
    };
    enum Knight
    {
        north_left,
        north_right,
        east_up,
        east_down,
        south_left,
        south_right,
        west_up,
        west_down
    };
    enum Castling
    {
        KING_SIDE,
        QUEEN_SIDE
    };
};
#endif