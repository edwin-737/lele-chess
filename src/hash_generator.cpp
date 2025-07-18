#include "hash_generator.hpp"

unsigned long long RngState::next(){
    unsigned long long x = val;
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    val = x;
    return x;
}