#ifndef BIDIRECTIONAL_MAP_H
#define BIDIRECTIONAL_MAP_H
template <typename K, typename V, int SIZE_K, int SIZE_V>
class BidirectionalMap {
public:
    BidirectionalMap(){}
    K kv[SIZE_K];
    V vk[SIZE_V];

};
#endif