#include <cstdint>
#include <iostream>
#include <vector>

#include "constants.h"
#include "interfaces.h"

using namespace std;

/**
 * ====== ==========
 * crypto functions:
 */
vector<uint8_t> xFunc(vector<uint8_t> a, vector<uint8_t> b) {
    vector<uint8_t> c;
    for (size_t i = 0; i < a.size(); i++)
        c.push_back(a[i] ^ b[i]);

    return c;
}

vector<uint8_t> sFunc(vector<uint8_t> a) {
    vector<uint8_t> b;
    for (auto byte : a)
        b.push_back(KUZ_CONST::SBOX[byte]);

    return b;
}

vector<uint8_t> rFunc(vector<uint8_t> a) {
    uint8_t a_15 = 0x00;
    vector<uint8_t> internal(KUZ_CONST::BLOCK_SIZE);
    for (int i = 15; i >= 0; i--) {
        if (i == 0)
            internal[15] = a[i];
        else
            internal[i - 1] = a[i];
        a_15 ^= gfMul(a[i], KUZ_CONST::LIN_VEC[i]);
    }
    internal[15] = a_15;
    return internal;
}

vector<uint8_t> lFunc(vector<uint8_t> a) {
    vector<uint8_t> out_data(a.size());
    vector<uint8_t> internal = a;

    for (size_t i = 0; i < KUZ_CONST::BLOCK_SIZE; i++)
        internal = rFunc(internal);

    out_data = internal;
    return out_data;
}

uint8_t gfMul(uint8_t a, uint8_t b) {
    uint8_t c = 0;
    uint8_t hi_bit;
    for (int i = 0; i < 8; i++) {
        if (b & 1) {
            c ^= a;
        }
        hi_bit = a & 0x80;
        a <<= 1;
        if (hi_bit) {
            a ^= 0xc3;  // polynomial x^8 + x^7 + x^6 + x + 1
        }
        b >>= 1;
    }
    return c;
}