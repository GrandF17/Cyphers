#include <cstdint>
#include <iostream>
#include <vector>

#include "./constants.h"

#ifndef KUZ_INTERFACES
#define KUZ_INTERFACES

using namespace std;

/**
 * @param keys - round keys. Len = ROUNDS_AMOUNT
 */
typedef struct {
    vector<vector<uint8_t>> keys[KUZ_CONST::ROUNDS_AMOUNT];
} roundKeys;

#endif