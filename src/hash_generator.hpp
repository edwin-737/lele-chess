#ifndef HASH_GENERATOR_H
#define HASH_GENERATOR_H
#define NUM_ZOBRIST_KEYS 771
typedef enum ZOBRIST_KEYS {
    zBLACK_TO_MOVE = 768,
    zCASTLE_RIGHTS = 769,
    zEP_RIGHTS = 785
} ZOBRIST_KEYS;
class RngState {
public:
    RngState(): val(0xabcdabcd12341234){}
    unsigned long long next();
    unsigned long long val;
};
#endif