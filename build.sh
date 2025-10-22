# rm -rf build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DENABLE_GPERFTOOLS=ON
cmake --build build