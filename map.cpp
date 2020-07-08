#include <cstdlib>
#include <cassert>

#include "map.h"

// Game playfield
static const char map[] = "1111111311111111"\
                        "1              1"\
                        "1      11111   1"\
                        "1     0        1"\
                        "1     0  1110001"\
                        "1     3        1"\
                        "1   10000      1"\
                        "3   1   11100  1"\
                        "1   1   0      1"\
                        "1   1   1  00001"\
                        "1       1      1"\
                        "1       1      1"\
                        "1       0      3"\
                        "1 5555555      1"\
                        "1              1"\
                        "1111111311111111";
                        
Map::Map() : w(16), h(16){
    // (map_h + 1) for the null terminated string
	assert(sizeof(map) == w * h + 1);
}

int Map::get(const size_t i, const size_t j){
    assert(i < w && j < h && sizeof(map) == w * h + 1);
    return map[i + j * w] - '0';
}

bool Map::is_empty(const size_t i, const size_t j){
    assert(i < w && sizeof(map) == w * h + 1);
    return map[i + j * w] == ' ';
}