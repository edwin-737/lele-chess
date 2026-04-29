#ifndef move_h
#define move_h
#include <string>
#include "board_info.hpp"
using namespace std;
typedef enum add_info{
    // generated as normal moves
    QUIET_MOVE,
    DOUBLE_PAWN_PUSH,
    CAPTURE,
    // generated as special moves
    EP_CAPTURE,
    KING_CASTLE,
    QUEEN_CASTLE,
    KNIGHT_PROMOTION,
    BISHOP_PROMOTION,
    ROOK_PROMOTION,
    QUEEN_PROMOTION,
    KNIGHT_CAPTURE_PROMOTION,
    BISHOP_CAPTURE_PROMOTION,
    ROOK_CAPTURE_PROMOTION,
    QUEEN_CAPTURE_PROMOTION
} ADDITIONAL_INFO;
typedef enum move_types {
    mQUIET,
    mKING_CASTLE,
    mQUEEN_CASTLE,
    mEP_CAPTURE,
    mKNIGHT_PROMOTION,
    mBISHOP_PROMOTION,
    mROOK_PROMOTION,
    mQUEEN_PROMOTION
} MOVE_TYPES;
// 6 bits: from
// 6 bits: to
// 1 bit: side
// 4 bits: piece
// 4 bits: captured piece
// 4 bits: additional info
// 3 bits: ep capture file
namespace MoveUtils{
    unsigned int create_move(unsigned int from, unsigned int to, unsigned int side, unsigned int piece, unsigned int additional_info = 0, unsigned int captured_piece = NO_PIECE, unsigned int ep_capture_file = NO_EP_RIGHTS);
    unsigned int get_from(unsigned int move);
    unsigned int get_to(unsigned int move);
    unsigned int get_side(unsigned int move);
    unsigned int get_piece(unsigned int move);
    unsigned int get_captured_piece(unsigned int move);
    unsigned int get_additional_info(unsigned int move);
    unsigned int get_ep_capture_file(unsigned int move);
    bool is_quiet(unsigned int move);
    bool is_capture(unsigned int move);
    bool is_double_pawn_push(unsigned int move);
    bool is_ep_capture(unsigned int move);
    bool is_castle(unsigned int move);
    bool is_king_castle(unsigned int move);
    bool is_queen_castle(unsigned int move);
    bool is_promotion(unsigned int move);
    bool is_capture_promotion(unsigned int move);
    bool is_queen_promotion(unsigned int move);
    bool is_knight_promotion(unsigned int move);
    bool is_bishop_promotion(unsigned int move);
    bool is_rook_promotion(unsigned int move);
    bool is_queen_capture_promotion(unsigned int move);
    bool is_knight_capture_promotion(unsigned int move);
    bool is_bishop_capture_promotion(unsigned int move);
    bool is_rook_capture_promotion(unsigned int move);
    bool is_final_rank(unsigned int square);
    void display(unsigned int move);
    unsigned int square_as_uint(string square);
    unsigned int promoted_piece_as_uint(string piece);
    string piece_as_string(unsigned int piece);
    string side_as_string(unsigned int side);
    string move_as_string(unsigned int move);
    string square_as_string(unsigned int square);
    bool is_quiet_pawn_move(string move);
    bool has_from_file(string move);
    bool has_from_rank(string move);
    bool is_capture(string move);
    bool is_check(string move);
    bool is_castle(string move);
    bool is_promotion(string move);
    bool is_capture_promotion(string move);
    bool is_potentially_double_pawn_push(unsigned int side, string move);
    unsigned int get_pawn_from_file(string move);
    unsigned int get_piece(string move);
    unsigned int get_to(string move);
    unsigned int get_from_file(string move);
    unsigned int get_from_rank(string move);
    unsigned int get_castle_move(unsigned int side, string move);
    unsigned int get_promoted_piece(string move);
    unsigned int get_square(string square);
};
#endif