# rm -rf build
cmake -S . -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo -DENABLE_GPERFTOOLS=ON -DENABLE_PERFT=1
cmake --build build