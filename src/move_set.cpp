#include "move_set.hpp"
#include "move.hpp"
#include "direction.hpp"
#include "direction_map.hpp"
#include "board_squares.hpp"
#include "utils.hpp"
#include "magics.hpp"
#include "const.hpp"
using namespace MoveSet;
using namespace BoardSquares;

void MoveSet::init_attack_masks()
{
    for(int sq = 0 ; sq < 64 ; sq ++)
    {
        uint64 king_square = get_square_bitboard(sq);
        uint64 king_possible_moves;
        uint64 king_clip_file_a = king_square & get_clear_file_bitboard(File::a);
        uint64 king_clip_file_h = king_square & get_clear_file_bitboard(File::h);

        uint64 king_ne = king_clip_file_h <<  DirectionMap::general[Direction::General::north_east];
        uint64 king_n = king_square <<  DirectionMap::general[Direction::General::north];
        uint64 king_nw = king_clip_file_h <<  DirectionMap::general[Direction::General::north_west];
        uint64 king_e = king_clip_file_h <<  DirectionMap::general[Direction::General::east];
        uint64 king_w = king_clip_file_a >>  DirectionMap::general[Direction::General::west];
        uint64 king_sw = king_clip_file_a >>  DirectionMap::general[Direction::General::south_west];
        uint64 king_s = king_square >>  DirectionMap::general[Direction::General::south];
        uint64 king_se = king_clip_file_a >>  DirectionMap::general[Direction::General::south_east];
        uint64 king_possible_moves_vec[] = {king_ne, king_n, king_nw, king_e, king_w, king_sw, king_s, king_se};
        king_attack_mask[sq] = OR_mult(king_possible_moves_vec,8);


        uint64 knight_square = get_square_bitboard(sq);
        uint64 knight_attack_set;
        int ab[] = {File::a, File::b};
        int gh[] = {File::g, File::h};
        uint64 clear_files_ab = get_clear_file_bitboard(File::a) & get_clear_file_bitboard(File::b);
        uint64 clear_files_gh = get_clear_file_bitboard(File::g) & get_clear_file_bitboard(File::h);

        uint64 knight_clip_file_a = knight_square & get_clear_file_bitboard(File::a);
        uint64 knight_clip_file_ab = knight_square & clear_files_ab;
        uint64 knight_clip_file_h = knight_square & get_clear_file_bitboard(File::h);
        uint64 knight_clip_file_gh = knight_square & clear_files_gh;
        uint64 eu = knight_clip_file_gh << DirectionMap::knight[Direction::Knight::east_up];
        uint64 nl = knight_clip_file_a << DirectionMap::knight[Direction::Knight::north_left];
        uint64 nr = knight_clip_file_h << DirectionMap::knight[Direction::Knight::north_right];
        uint64 wu = knight_clip_file_ab << DirectionMap::knight[Direction::Knight::west_up];
        uint64 wd = knight_clip_file_ab >> DirectionMap::knight[Direction::Knight::west_down];
        uint64 sl = knight_clip_file_a >> DirectionMap::knight[Direction::Knight::south_left];
        uint64 sr = knight_clip_file_h >> DirectionMap::knight[Direction::Knight::south_right];
        uint64 ed = knight_clip_file_gh >> DirectionMap::knight[Direction::Knight::east_down];
        uint64 knight_possible_moves_vec[] = {nr, nl, wu, wd, sl, sr, ed, eu};
        knight_attack_mask[sq] = OR_mult(knight_possible_moves_vec, 8);


        uint64 white_pawn_square = get_square_bitboard(sq);
        uint64 white_pawn_clip_file_a = white_pawn_square & get_clear_file_bitboard(File::a);
        uint64 white_pawn_clip_file_h = white_pawn_square & get_clear_file_bitboard(File::h);
        uint64 ne = white_pawn_clip_file_h << 9;
        uint64 nw = white_pawn_clip_file_a << 7;
        white_pawn_attack_mask[sq] = nw | ne;


        uint64 black_pawn_square = get_square_bitboard(sq);
        uint64 black_pawn_clip_file_a = black_pawn_square & get_clear_file_bitboard(File::a);
        uint64 black_pawn_clip_file_h = black_pawn_square & get_clear_file_bitboard(File::h);
        uint64 se = black_pawn_clip_file_h >> 7;
        uint64 sw = black_pawn_clip_file_a >> 9;
        black_pawn_attack_mask[sq] = se | sw;

    }
}

uint64 MoveSet::get_king_attack_mask(int sq)
{
    return king_attack_mask[sq];
}
uint64 MoveSet::get_knight_attack_mask(int sq)
{
    return knight_attack_mask[sq];
}
uint64 MoveSet::get_white_pawn_attack_mask(int sq)
{
    return white_pawn_attack_mask[sq];
}
uint64 MoveSet::get_black_pawn_attack_mask(int sq)
{
    return black_pawn_attack_mask[sq];
}
uint64 MoveSet::get_white_pawn_forward_mask(Bitboard* bb, int sq)
{
    uint64 pawn_square = get_square_bitboard(sq);
    uint64 pawn_front_1 = (pawn_square << 8) & ~bb->all;
    // uint64 pawn_front_1 = (pawn_square << 8) & ~all;

    if(sq >= 16) return pawn_front_1;
    uint64 pawn_front_2 = ((pawn_square << 16) &  ~bb->all) & pawn_front_1 << 8;

    uint64 pawn_front = pawn_front_1 | pawn_front_2;
    return pawn_front;
}
uint64 MoveSet::get_black_pawn_forward_mask(Bitboard* bb, int sq){

    uint64 pawn_square = get_square_bitboard(sq);
    uint64 pawn_front_1 = (pawn_square >> 8) & ~bb->all;

    if(sq <= 47) return pawn_front_1;
    uint64 pawn_front_2 = ((pawn_square >> 16) &  ~bb->all) & pawn_front_1 >> 8;

    uint64 pawn_front = pawn_front_1 | pawn_front_2;
    return pawn_front;
}
uint64 MoveSet::get_pawn_forward_mask(Bitboard* bb, int sq, int side)
{
    return side == Side::WHITE ? get_white_pawn_forward_mask(bb, sq) : get_black_pawn_forward_mask(bb, sq);
}
void MoveSet::set_rook_attack_set(int sq)
{
    uint64 block;
    uint64 mask = rmask(sq);
    int n = count_1s(mask);
    for(int i = 0 ; i < 1 << n ; i ++){
        block = index_to_uint64(i, n, mask);
        int index = transform(block, mg.rook_magics[sq], RBits[sq]);
        rook_attack_set[sq][index] = ratt(sq, block);
    }
}
void MoveSet::set_bishop_attack_set(int sq){
    uint64 block;
    uint64 mask = bmask(sq);
    int n = count_1s(mask);
    for(int i = 0 ; i < 1 << n ; i ++){
        block = index_to_uint64(i, n, mask);
        int index = transform(block, mg.bishop_magics[sq], BBits[sq]);
        bishop_attack_set[sq][index] = batt(sq, block);
    }
}
void MoveSet::set_attack_sets()
{
    for (int square = 0; square < NUM_SQUARES; square++)
    {
        set_bishop_attack_set(square);
        set_rook_attack_set(square);
    }

}
uint64 MoveSet::get_white_pawn_attack_set(Bitboard* bb, int sq)
{
    // return get_white_pawn_attack_mask(sq) & bb->black;
    return get_white_pawn_attack_mask(sq) & bb->collective_piece_boards[BLACK];
}
uint64 MoveSet::get_black_pawn_attack_set(Bitboard* bb,int sq)
{
    // return get_black_pawn_attack_mask(sq) & bb->white;
    return get_black_pawn_attack_mask(sq) & bb->collective_piece_boards[WHITE];
}
uint64 MoveSet::get_pawn_attack_set(Bitboard* bb, int sq, int side)
{
    return side == Side::WHITE ? get_white_pawn_attack_set(bb, sq) : get_black_pawn_attack_set(bb, sq);

}
uint64 MoveSet::get_knight_attack_set(Bitboard* bb, int sq, int side)
{
    // return side == Side::WHITE ? get_knight_attack_mask(sq) & ~bb->white : get_knight_attack_mask(sq) & ~bb->black;
    return get_knight_attack_mask(sq) & ~bb->collective_piece_boards[side];
}
uint64 MoveSet::get_king_attack_set(Bitboard* bb, int sq, int side)
{
    // return side == Side::WHITE ? get_king_attack_mask(sq) & ~bb->white : get_king_attack_mask(sq) & ~bb->black;
    return get_king_attack_mask(sq) & ~bb->collective_piece_boards[side];
}
uint64 MoveSet::get_bishop_attack_set(Bitboard* bb, int sq, int side)
{
    // return side == Side::WHITE ? get_bishop_attack_set(bb, sq) & ~bb->white : get_bishop_attack_set(bb, sq) & ~bb->black; 
    return get_bishop_attack_set(bb, sq) & ~bb->collective_piece_boards[side];

}
uint64 MoveSet::get_rook_attack_set(Bitboard* bb, int sq, int side)
{
    // return side == Side::WHITE ? get_rook_attack_set(bb, sq) & ~bb->white : get_rook_attack_set(bb, sq) & ~bb->black; 
    return get_rook_attack_set(bb, sq) & ~bb->collective_piece_boards[side];


}
uint64 MoveSet::get_bishop_attack_set(Bitboard* bb, int sq)
{
    uint64 cur_occ = bb->all & bmask(sq);
    int index = transform(cur_occ, mg.bishop_magics[sq], BBits[sq]);
    uint64 attack_set = bishop_attack_set[sq][index];
    return attack_set;
}
uint64 MoveSet::get_rook_attack_set(Bitboard* bb, int sq)
{
    uint64 cur_occ = bb->all & rmask(sq);
    int index = transform(cur_occ, mg.rook_magics[sq], RBits[sq]);
    uint64 attack_set = rook_attack_set[sq][index];
    return attack_set;
}
uint64 MoveSet::get_all_move_set(Bitboard* bb, int piece, int sq, int side)
{
    switch(piece){
        case Piece::pPAWN:
            return get_pawn_attack_set(bb, sq, side) | get_pawn_forward_mask(bb, sq, side);
        case Piece::pBISHOP:
            return get_bishop_attack_set(bb, sq, side);
        case Piece::pROOK:
            return get_rook_attack_set(bb, sq, side);
        case Piece::pKNIGHT:
            return get_knight_attack_set(bb, sq, side);
        case Piece::pKING:
            return get_king_attack_set(bb, sq, side);
        case Piece::pQUEEN:
            return get_bishop_attack_set(bb, sq, side) | get_rook_attack_set(bb, sq, side);
        default:
            return 0;
    }
}
uint64 MoveSet::get_capture_move_set(Bitboard* bb, int piece, int sq, int side)
{
    switch(piece){
        case Piece::pPAWN:
            return get_pawn_attack_set(bb, sq, side) & bb->collective_piece_boards[side ^ 1];
        case Piece::pBISHOP:
            return get_bishop_attack_set(bb, sq, side) & bb->collective_piece_boards[side ^ 1];
        case Piece::pROOK:
            return get_rook_attack_set(bb, sq, side) & bb->collective_piece_boards[side ^ 1];
        case Piece::pKNIGHT:
            return get_knight_attack_set(bb, sq, side) & bb->collective_piece_boards[side ^ 1];
        case Piece::pKING:
            return get_king_attack_set(bb, sq, side) & bb->collective_piece_boards[side ^ 1];
        case Piece::pQUEEN:
            return (get_bishop_attack_set(bb, sq, side) | get_rook_attack_set(bb, sq, side)) & bb->collective_piece_boards[side ^ 1];
        default:
            return 0;
    }
}
uint64 MoveSet::get_quiet_move_set(Bitboard* bb, int piece, int sq, int side)
{
    switch(piece){
        case Piece::pPAWN:
            return get_pawn_forward_mask(bb, sq, side) & (~bb->collective_piece_boards[side ^ 1]);
        case Piece::pBISHOP:
            return get_bishop_attack_set(bb, sq, side) & (~bb->collective_piece_boards[side ^ 1]);
        case Piece::pROOK:
            return get_rook_attack_set(bb, sq, side) & (~bb->collective_piece_boards[side ^ 1]);
        case Piece::pKNIGHT:
            return get_knight_attack_set(bb, sq, side) & (~bb->collective_piece_boards[side ^ 1]);
        case Piece::pKING:
            return get_king_attack_set(bb, sq, side) & (~bb->collective_piece_boards[side ^ 1]);
        case Piece::pQUEEN:
            return (get_bishop_attack_set(bb, sq, side) | get_rook_attack_set(bb, sq, side)) & (~bb->collective_piece_boards[side ^ 1]);
        default:
            return 0;
    }
}
bool MoveSet::king_attacked_by_move(Bitboard* bb, int piece, int sq, int side){
    return get_capture_move_set(bb, piece, sq, side) & bb->piece_boards[side ^ 1][pKING];
}
