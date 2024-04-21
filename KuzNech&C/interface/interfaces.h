#include <constants.h>

#include <cstdint>
#include <iostream>
#include <vector>

using namespace std;

/**
 * @param b has len = BLOCK_SIZE        (in bits)
 * @param q has len = BLOCK_SIZE / 8    (in bytes)
 */
typedef union {
    uint8_t b[KUZ_CONST::BLOCK_SIZE];
    uint64_t q[KUZ_CONST::BLOCK_SIZE / 8];
} vect_t;

/**
 * @param keys - round keys. Len = ROUNDS_AMOUNT
 */
typedef struct {
    vect_t keys[KUZ_CONST::ROUNDS_AMOUNT];
} round_keys_t;