#ifndef board_info_h
#define board_info_h
#include <stack>
#include <iostream>
#include "const.hpp"
using namespace std;
class BoardInfo{
private:
    // castle_rights: 4 bits
    // castle_rights[0]: white kingside castle
    // castle_rights[1]: white queenside castle
    // castle_rights[2]: black kingside castle
    // castle_rights[3]: black queenside castle

    // ep_rights: 4 bits
    // letter: file of pawn that was just double pawn pushed and that can be captured
    // 0: a | 1: b ... 7: h
public:        
    stack<unsigned int> castle_rights_stack, ep_rights_stack, move_stack, bi_stack;
    unsigned int num_nodes = 0, num_captures = 0, num_ep_captures = 0, num_checks = 0, num_checkmates = 0, num_castles = 0;
    static BoardInfo* instanceptr;
    unsigned int bi_arr[60];
    int depth = -1;    
    BoardInfo(){
        bi_arr[depth] = ((INITIAL_CASTLE_RIGHTS << 4) | NO_EP_RIGHTS); 
        bi_stack.push((INITIAL_CASTLE_RIGHTS << 4) | NO_EP_RIGHTS);
    }
    BoardInfo(unsigned int initial_castle_rights, unsigned int initial_ep_rights){
        bi_stack.push((initial_castle_rights << 4) | initial_ep_rights);
    }
    BoardInfo(BoardInfo& b) = delete;
    static BoardInfo* get_instance(){
        if(instanceptr == nullptr){
            instanceptr = new BoardInfo();
            return instanceptr;
        }
        return instanceptr;
    }
    void set_board_info(unsigned int castle_rights, unsigned int ep_rights){
        if(instanceptr == nullptr){
            return;
        }
        instanceptr->add_board_info(castle_rights, ep_rights);
        cout<<"set peek castle rights: "<<instanceptr->peek_castle_right()<<endl;
        cout<<"set peek ep rights: "<<instanceptr->peek_ep_right()<<endl;
    }
    void add_board_info(int castle_right, int ep_right){
        depth ++;
        bi_arr[depth] = (castle_right << 4) | ep_right;
    }
    void remove_board_info(){
        // if(!bi_stack.empty())
        //     bi_stack.pop();
        depth --;
    }
    unsigned int peek_board_info(){
        // if(!bi_stack.empty())
        //     return bi_stack.top();
        // else 
        //     return 0;
        if(depth >= 0)
            return bi_arr[depth];
        return 0;
    }
    unsigned int peek_castle_right(){
        return peek_board_info() >> 4;
    }
    unsigned int peek_ep_right(){
        return peek_board_info() & 0xf;
    }
};
#endif