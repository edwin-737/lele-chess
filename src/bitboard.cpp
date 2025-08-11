#include "bitboard.hpp"
#include "move.hpp"
#include "board_squares.hpp"
#include "utils.hpp"
#include "const.hpp"
#include "move_set.hpp"
using namespace BoardSquares;
Bitboard* Bitboard::get_instance(){
    if(instanceptr == nullptr)
        instanceptr = new Bitboard();
    return instanceptr;
}
bool Bitboard::is_initialised(){
    return initialised;
}
void Bitboard::update()
{
    // white = OR_mult(piece_boards[WHITE], 6);
    // black = OR_mult(piece_boards[BLACK], 6);
    // all = white | black;
    collective_piece_boards[WHITE] = OR_mult(piece_boards[WHITE], 6);
    collective_piece_boards[BLACK] = OR_mult(piece_boards[BLACK], 6);
    all = collective_piece_boards[WHITE] | collective_piece_boards[BLACK];
    // cout<<"updating collective piece boards\n";
    // cout<<"white collective piece board\n";
    // display_bitboard(collective_piece_boards[WHITE]);
    // cout<<"black collective piece board\n";
    // display_bitboard(collective_piece_boards[BLACK]);
    // cout<<"all collective piece board\n";
    // display_bitboard(all);
}
void Bitboard::init_piece_boards()
{

    for (int side = 0; side < NUM_SIDES; side++)
        for (int piece = 0; piece < NUM_PIECE_TYPES; piece++)
            piece_boards[side][piece] = 0ULL;

}


unsigned int Bitboard::get_piece_on_square(unsigned int side, unsigned int square)
{
    for (int piece = 0; piece < NUM_PIECE_TYPES; piece++)
        if (piece_boards[side][piece] & BoardSquares::get_square_bitboard(square))
            return piece;
    return NO_PIECE;
}

bool Bitboard::any_piece_on_square(int square){
    // return any_piece_on_square(WHITE, square) | any_piece_on_square(BLACK, square);
    return all & get_square_bitboard(square);
}

bool Bitboard::any_piece_on_square(int side, int square){
    uint64 square_board = get_square_bitboard(square);
    for(int piece = 0 ; piece < NUM_PIECE_TYPES ; piece ++){
        if(piece_boards[side][piece] & square_board)
            return true;
    }
    return false;
}


bool Bitboard::attacked(unsigned int defending_side, unsigned int sq)
{
    // this->update();
    int attacking_side = defending_side ^ 1;

    uint64 pawns = piece_boards[attacking_side][pPAWN];
    if(MoveSet::get_pawn_attack_set(this, sq, defending_side) & pawns) return true;

    uint64 knights = piece_boards[attacking_side][pKNIGHT];
    if(MoveSet::get_knight_attack_set(this, sq, defending_side) & knights) return true;

    uint64 bishop_attack_set = MoveSet::get_bishop_attack_set(this, sq, defending_side);
    uint64 queens = piece_boards[attacking_side][pQUEEN];
    if(bishop_attack_set & queens) return true;   

    uint64 bishops = piece_boards[attacking_side][pBISHOP];
    if(bishop_attack_set & bishops) return true;

    uint64 rook_attack_set = MoveSet::get_rook_attack_set(this, sq, defending_side);
    if(rook_attack_set & queens) return true;

    uint64 rooks = piece_boards[attacking_side][pROOK];
    if(rook_attack_set & rooks) return true;

    uint64 kings = piece_boards[attacking_side][pKING];
    if(MoveSet::get_king_attack_set(this, sq, defending_side) & kings) return true;

    return false;
}

unsigned int Bitboard::get_lowest_value_attacker_to(unsigned int defending_side, unsigned int sq)
{
    for(int piece = 0 ; piece < NUM_PIECE_TYPES ; piece ++)
        if(attacked(defending_side, sq))
            return piece;
    return NO_PIECE;
}

unsigned int Bitboard::get_captured_piece(unsigned int move)
{
    int side = MoveUtils::get_side(move);
    int to = MoveUtils::get_to(move);
    for(int piece = 0 ; piece < NUM_PIECE_TYPES ; piece ++)
        if(this->piece_boards[side ^ 1][piece] & get_square_bitboard(to))
            return piece;
    return NO_PIECE;
}


void Bitboard::display(){
    char board[64];
    for(int sq = 0 ; sq < 64; sq ++){
        board[sq] = '.';
    }
    for(int side = 0 ; side < NUM_SIDES ; side ++){
        for(int piece = 0 ; piece < NUM_PIECE_TYPES ; piece ++){
            int set_bits[64];
            get_set_bit_indices(piece_boards[side][piece], set_bits);
            for(int i = 0 ; i < 64 ; i ++){
                if(set_bits[i] == 0 || set_bits[i] > 64)
                    break;
                if(set_bits[i] > 0){
                    // board[set_bits[i] - 1] = '0' + piece;
                    char piece_char = '/';
                    if(piece == pPAWN){
                        piece_char = 'P';
                    } else if(piece == pKNIGHT){
                        piece_char = 'N';
                    } else if(piece == pBISHOP){
                        piece_char = 'B';
                    } else if(piece == pROOK){
                        piece_char = 'R';
                    } else if(piece == pQUEEN){
                        piece_char = 'Q';
                    } else if(piece == pKING){
                        piece_char= 'K';
                    }
                    if(side == BLACK) piece_char = tolower(piece_char);
                    board[set_bits[i] - 1] = piece_char;
                }
            }
        }
    }
    print_board(board);
}
void Bitboard::display_bitboard(uint64 b){
    char board[64];
    for(int sq = 0 ; sq < 64; sq ++){
        board[sq] = '.';
    }
    int set_bits[64];
    get_set_bit_indices(b, set_bits);
    for(int i = 0 ; i < 64 ; i ++){
        if(set_bits[i] == 0 || set_bits[i] > 64)
            break;
        if(set_bits[i] > 0){
            // board[set_bits[i] - 1] = '0' + piece;
            char piece_char = '1';
            board[set_bits[i] - 1] = piece_char;
        }
    }
    print_board(board);
}