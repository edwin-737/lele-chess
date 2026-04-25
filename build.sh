# rm -rf build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DENABLE_GPERFTOOLS=ON -DENABLE_PERFT=1
cmake --build build