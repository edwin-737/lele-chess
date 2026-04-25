# ./build/engine > alpha_beta_output/output_bk1_transposition.txt -f ./positions/bratko-kopec/bk_1.txt -j a -d 6 -s b -t 1
# ./build/engine > alpha_beta_output/output_bk1.txt -f ./positions/bratko-kopec/bk_1.txt -j a -d 6 -s b -t 0
# ./build/engine > alpha_beta_output/output_bk2_transposition.txt -f ./positions/bratko-kopec/bk_2.txt -j a -d 8 -s w -t 1
# ./build/engine > alpha_beta_output/output_bk2_iterative.txt -f ./positions/bratko-kopec/bk_2.txt -j a -d 8 -s w -t 0 -i 1
# ./build/engine > alpha_beta_output/output_bk2.txt -f ./positions/bratko-kopec/bk_2.txt -j a -d 10 -s w -t 0 -i 1
# ./build/engine > alpha_beta_output/output_bk2.txt -f ./positions/bratko-kopec/bk_2.txt -j a -d 10 -s w -t 0
# ./build/engine > alpha_beta_output/output_bk3_transposition.txt -f ./positions/bratko-kopec/bk_3.txt -j a -d 6 -s b -t 1
# ./build/engine > alpha_beta_output/output_bk3.txt -f ./positions/bratko-kopec/bk_3.txt -j a -d 6 -s b -t 0
# ./build/engine > alpha_beta_output/output_bk4_transposition.txt -f ./positions/bratko-kopec/bk_4.txt -j a -d 8 -s w -t 1
# ./build/engine > alpha_beta_output/output_bk4.txt -f ./positions/bratko-kopec/bk_4.txt -j a -d 8 -s w -t 0
# ./build/engine > output.txt -f ./positions/bratko-kopec/bk_5.txt -j a -d 6 -s w -t 1
# ./build/engine > output.txt -f ./positions/test_position_real.txt -j a -d 6 -s w -t 0
# ./build/engine > alpha_beta_output/output_test_position_real_iterative.txt -f ./positions/test_position_real.txt -j a -d 8 -s w -t 0 -i 1



./build/engine > alpha_beta_output/output_test_position_now.txt -f ./positions/bugs/backrank_miss.txt -j a -d 10 -s b -t 0 -i 1