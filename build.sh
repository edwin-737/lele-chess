# rm -rf build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DENABLE_GPERFTOOLS=ON
cmake --build build