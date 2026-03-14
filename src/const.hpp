#ifndef const_h
#define const_h

#define INCREMENTING_MOVE_TYPE 1ULL
#define NO_MOVES_LEFT 0ULL
#define WHITE_KING_CASTLE_SQUARES 0x60ULL
#define WHITE_QUEEN_CASTLE_SQUARES 0xeULL
#define BLACK_KING_CASTLE_SQUARES 0x6000000000000000ULL
#define BLACK_QUEEN_CASTLE_SQUARES 0xe00000000000000ULL
#define EP_START -1
#define EP_FINISHED 8
#define CHECKMATE_EVAL(max_depth,depth_left) -100000+(max_depth-depth_left)
#define STALEMATE_EVAL 0

#define INITIAL_CASTLE_RIGHTS 0xf
#define NO_EP_RIGHTS 0x8
#define NO_PIECE 0x8
#define NO_PIECE_KEY 97
#define NO_DIRECTION 9
#define MAX_CAPTURE_VALUE 9
#define INVALID_LOCATION 64
enum GenType {
    ALL_MOVES,
    ONLY_CAPTURES,
    ONLY_QUIET
};
enum Side
{
    WHITE,
    BLACK
};
enum DirectionType
{
    mPAWN_WHITE,
    mPAWN_BLACK,
    mKNIGHT,
    mBISHOP,
    mROOK,
    mQUEEN,
    mKING
};
enum Piece
{
    pPAWN,
    pKNIGHT,
    pBISHOP,
    pROOK,
    pQUEEN,
    pKING,
};
enum Directions {
    SOUTH,
    SOUTHWEST,
    WEST,
    NORTHWEST,
    NORTH,
    NORTHEAST,
    EAST,
    SOUTHEAST
};
inline const int piece_values[] = {
    1, 3, 3, 5, 9, 10000
};
#define NUM_SIDES 2
#define NUM_PIECE_TYPES 6
#define NUM_MOVE_TYPES 7
#define MAX_NUM_PIECES 8
#define NUM_PIECE_KEYS 100
#define NUM_SQUARES 64
#define NUM_DIRECTIONS 8
inline int PAWN_WHITE[]={
    0,  0,  0,  0,  0,  0,  0, 0,
    5, 10, 10,-20,-20, 10, 10,  5,
    5, -5,-10,  0,  0,-10, -5,  5,
    0,  0,  0, 20, 20,  0,  0,  0,
    5,  5, 10, 25, 25, 10,  5,  5,
    10, 10, 20, 30, 30, 20, 10, 10,
    50, 50, 50, 50, 50, 50, 50, 50,
    0,  0,  0,  0,  0,  0,  0,  0
};
inline int PAWN_BLACK[]={
    0,  0,  0,  0,  0,  0,  0,  0,
    50, 50, 50, 50, 50, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
    5,  5, 10, 25, 25, 10,  5,  5,
    0,  0,  0, 20, 20,  0,  0,  0,
    5, -5,-10,  0,  0,-10, -5,  5,
    5, 10, 10,-20,-20, 10, 10,  5,
    0,  0,  0,  0,  0,  0,  0, 0
};
inline int KNIGHT_WHITE[]={
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50,
};
inline int KNIGHT_BLACK[]={
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50,
};
inline int BISHOP_WHITE[]={
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -20,-10,-10,-10,-10,-10,-10,-20,
};
inline int BISHOP_BLACK[]={
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -20,-10,-10,-10,-10,-10,-10,-20,
};
inline int ROOK_WHITE[] = {
    0,  0,  0,  5,  5,  0,  0,  0,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    5, 10, 10, 10, 10, 10, 10,  5,
    0,  0,  0,  0,  0,  0,  0,  0,
};
inline int ROOK_BLACK[]={
    0,  0,  0,  0,  0,  0,  0,  0,
    5, 10, 10, 10, 10, 10, 10,  5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    0,  0,  0,  5,  5,  0,  0,  0
};
inline int QUEEN_WHITE[]={
    -10, -5, -5, -3, -3, -5, -5, -10,
    -5,  0,  0,  0,  0,  0,  0,-5,
    -5,  0,  3,  3,  3,  3,  0, -5,
    0,  0,  3,  3,  3,  3,  0, 0,
    0,  0,   3,  3,  3,  3,  0, 0,
    -5,  0,   3,  3,  3,  3,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -10, -5, -5, -3, -3, -5, -5, -10,
};
inline int QUEEN_BLACK[]={
    -10, -5, -5, -3, -3, -5, -5, -10,
    -5,  0,  0,  0,  0,  0,  0,-5,
    -5,  0,  3,  3,  3,  3,  0, -5,
    0,  0,  3,  3,  3,  3,  0, 0,
    0,  0,   3,  3,  3,  3,  0, 0,
    -5,  0,   3,  3,  3,  3,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -10, -5, -5, -3, -3, -5, -5, -10,
};
inline int KING_MIDDLE_GAME_WHITE[]={
    20, 30, 10,  0,  0, 10, 30, 20,
    20, 20,  0,  0,  0,  0, 20, 20,
    -10,-20,-20,-20,-20,-20,-20,-10,
    -20,-30,-30,-40,-40,-30,-30,-20,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
};
inline int KING_MIDDLE_GAME_BLACK[]={
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -20,-30,-30,-40,-40,-30,-30,-20,
    -10,-20,-20,-20,-20,-20,-20,-10,
    20, 20,  0,  0,  0,  0, 20, 20,
    20, 30, 10,  0,  0, 10, 30, 20
};
inline int KING_END_GAME_WHITE[] = {
    -50,-30,-30,-30,-30,-30,-30,-50,
    -30,-30,  0,  0,  0,  0,-30,-30,
    -30,-10, 20, 30, 30, 20,-10,-30,
    -30,-10, 30, 40, 40, 30,-10,-30,
    -30,-10, 30, 40, 40, 30,-10,-30,
    -30,-10, 20, 30, 30, 20,-10,-30,
    -30,-20,-10,  0,  0,-10,-20,-30,
    -50,-40,-30,-20,-20,-30,-40,-50,
};
inline int KING_END_GAME_BLACK[] = {
    -50,-40,-30,-20,-20,-30,-40,-50,
    -30,-20,-10,  0,  0,-10,-20,-30,
    -30,-10, 20, 30, 30, 20,-10,-30,
    -30,-10, 30, 40, 40, 30,-10,-30,
    -30,-10, 30, 40, 40, 30,-10,-30,
    -30,-10, 20, 30, 30, 20,-10,-30,
    -30,-30,  0,  0,  0,  0,-30,-30,
    -50,-30,-30,-30,-30,-30,-30,-50
};
inline int* PIECE_SQUARE_TABLE[2][6]={
    {PAWN_WHITE, KNIGHT_WHITE, BISHOP_WHITE, ROOK_WHITE, QUEEN_WHITE, KING_MIDDLE_GAME_WHITE},
    {PAWN_BLACK, KNIGHT_BLACK, BISHOP_BLACK, ROOK_BLACK, QUEEN_BLACK, KING_MIDDLE_GAME_BLACK}
};
inline int MATERIAL_VALUE[] = {1, 3, 3, 5, 9, 5000};

#endif