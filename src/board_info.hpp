#ifndef board_info_h
#define board_info_h
#include <stack>
using namespace std;
#define INITIAL_CASTLE_RIGHTS 0xf
#define NO_EP_RIGHTS 0x8
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
    stack<int> castle_rights_stack, ep_rights_stack, move_stack, bi_stack;
    unsigned int num_nodes = 0, num_captures = 0, num_ep_captures = 0, num_checks = 0, num_checkmates = 0, num_castles = 0;
    static BoardInfo* instanceptr;
    BoardInfo(){
        // cout<<"initial board info\n";
        bi_stack.push((INITIAL_CASTLE_RIGHTS << 4) | NO_EP_RIGHTS);
        // cout<<bi_stack.top()<<endl;
        // cout<<"initial castle right: "<<peek_castle_right()<<endl;
        // cout<<"initial ep right: "<<peek_ep_right()<<endl;
        // castle_rights_stack.push(INITIAL_CASTLE_RIGHTS);
        // ep_rights_stack.push(NO_EP_RIGHTS);
    }
    BoardInfo(int initial_castle_rights, int initial_ep_rights){
        bi_stack.push((initial_castle_rights << 4) | initial_ep_rights);

        // castle_rights_stack.push(initial_castle_rights);
        // ep_rights_stack.push(initial_ep_rights);
    }
    BoardInfo(BoardInfo& b) = delete;
    static BoardInfo* get_instance(){
        if(instanceptr == nullptr){
            instanceptr = new BoardInfo();
            return instanceptr;
        }
        return instanceptr;
    }
    static void set_board_info(int castle_rights, int ep_rights){
        if(instanceptr == nullptr){
            return;
        }
        // cout<<"set castle_rights: "<<castle_rights<<endl;
        // cout<<"set ep_rights: "<<ep_rights<<endl;
        // instanceptr->remove_board_info();
        instanceptr->add_board_info(castle_rights, ep_rights);
        cout<<"set peek castle rights: "<<instanceptr->peek_castle_right()<<endl;
        cout<<"set peek ep rights: "<<instanceptr->peek_ep_right()<<endl;
    }
    static void set_ep_rights(int ep_rights){
        if(instanceptr == nullptr){
            return;
        }
        instanceptr->remove_ep_right();
        instanceptr->add_ep_right(ep_rights);
    }
    static void set_castle_rights(int castle_rights){
        if(instanceptr == nullptr){
            return;
        }
        instanceptr->remove_castle_right();
        instanceptr->add_castle_right(castle_rights);
    }
    void add_board_info(int castle_right, int ep_right){
        // cout<<"add board info\n";
        // cout<<"add castle right: "<<castle_right<<endl;
        // cout<<"add ep right: "<<ep_right<<endl;
        bi_stack.push((castle_right << 4) | ep_right);
        // cout<<bi_stack.top()<<endl;
    }
    void remove_board_info(){
        if(!bi_stack.empty())
            bi_stack.pop();
    }
    int peek_board_info(){
        if(!bi_stack.empty())
            return bi_stack.top();
        else 
            return 0;
    }
    void add_castle_right(int castle_right){
        castle_rights_stack.push(castle_right);
    }
    void remove_castle_right(){
        if(!castle_rights_stack.empty())
            castle_rights_stack.pop();
    }
    int peek_castle_right(){
        return peek_board_info() >> 4;
        // if(!castle_rights_stack.empty())
        //     return castle_rights_stack.top();
        // else 
        //     return 0;
    }
    void add_ep_right(int ep_right){
        ep_rights_stack.push(ep_right);
    }
    void remove_ep_right(){
        if(!ep_rights_stack.empty())
            ep_rights_stack.pop();
    }
    int peek_ep_right(){

        // cout<<"peek ep_rights: "<<(peek_board_info() & 0xf)<<endl;
        return peek_board_info() & 0xf;
        // if(!ep_rights_stack.empty())
        //     return ep_rights_stack.top();
        // else
        //     return 0;
    }
    void add_move(int move){
        move_stack.push(move);
    }
    void remove_move(){
        if(!move_stack.empty())
            move_stack.pop();
    }
    int peek_move(){
        if(!move_stack.empty())
            return move_stack.top();
        else
            return 0;
    }
};
#endif