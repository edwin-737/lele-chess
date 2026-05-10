#include <thread>
#include <iostream>
#include <string>
#include "worker.hpp"
void Worker::run_task(std::atomic<bool>& stop_flag){
    // TODO: debug why i cant pass side as an argument and needed to hardcode BLACK
    int score = s.iterative_deepening(depth, BLACK, BLACK, stop_flag, false, true);
    cout<<"worker interrupted: "<<score<<"\n";
}
void Worker::loop(long long num_seconds_per_move){
    s.init_evaluate();
    while(true){
        stop_flag.store(false);
        std::string move_string;
        std::cout<<"Enter your next move: ";
        std::cin >> move_string;
        std::cout<<"You entered: "<<move_string<<"\n";

        unsigned int move = s.get_board_instance()->parse_single_move(move_string, true);
        s.pesto->update_evaluation(move);
        std::cout<<"move parsed: ";
        MoveUtils::display(move);
        s.get_board_instance()->get_bitboard()->display();
        cout<<"side to move: "<<side<<"\n";
        cout<<"castle rights: "<< s.get_board_instance()->get_board_info()->peek_castle_right()<<"\n";
        cout<<"ep rights: "<<s.get_board_instance()->get_board_info()->peek_ep_right()<<"\n";

        s.start = high_resolution_clock::now();
        std::thread t(&Worker::run_task, this, std::ref(stop_flag));

        std::this_thread::sleep_for(std::chrono::seconds(num_seconds_per_move));

        stop_flag.store(true);
        if (t.joinable()) {
            t.join();
            std::cout << "Thread joined successfully\n";
        }
        s.get_board_instance()->apply_move_if_legal(s.selected_move);
        std::cout<<"Move selected: ";
        MoveUtils::display(s.selected_move);
        s.pesto->update_evaluation(s.selected_move);

        s.get_board_instance()->get_bitboard()->display();
        s.get_board_instance()->change_side_to_move();
    }
}