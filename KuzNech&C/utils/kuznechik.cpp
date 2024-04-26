#include <cstdint>
#include <iostream>
#include <vector>

#include "../interfaces/constants.h"
#include "../interfaces/interfaces.h"

#ifndef KUZ_CRYPTO
#define KUZ_CRYPTO

using namespace std;

/**
 * ====== ==========
 * crypto functions:
 */
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
            /**
             * polynomial x^7 + x^6 + x + 1
             *
             * 0xC3 ~ [12 * 16 + 3 = 195 (C (hex) = 12 (dec), 3 = 3)] ~ 11000011
             *
             * 1*x^7 + 1*x^6 + 0*x^5 + 0*x^4 + 0*x^3 + 0*x^2 + 1*x^1 + 1*x^0
             */
            a ^= 0xc3;
        }
        b >>= 1;
    }
    return c;
}

// ====================
// ==== X Function ====
vector<uint8_t> xFunc(vector<uint8_t> a, vector<uint8_t> b) {
    vector<uint8_t> c;
    for (size_t i = 0; i < a.size(); i++)
        c.push_back(a[i] ^ b[i]);

    return c;
}
// ==== X Function ====
// ====================

// ====================
// ==== S Function ====
vector<uint8_t> sFunc(vector<uint8_t> a) {
    vector<uint8_t> b;
    for (auto byte : a)
        b.push_back(KUZ_CONST::SBOX[byte]);

    return b;
}

vector<uint8_t> sFuncInv(vector<uint8_t> a) {
    vector<uint8_t> b;
    for (auto byte : a)
        b.push_back(KUZ_CONST::INV_SBOX[byte]);

    return b;
}
// ==== S Function ====
// ====================

// ====================
// ==== R Function ====
vector<uint8_t> rFunc(vector<uint8_t> a) {
    uint8_t a_15 = 0x00;
    vector<uint8_t> internal(KUZ_CONST::BLOCK_SIZE);
    for (int i = 15; i >= 0; i--) {
        if (i == 0)
            internal[15] = a[0];
        else
            internal[i - 1] = a[i];
        a_15 ^= gfMul(a[i], KUZ_CONST::LIN_VEC[i]);
    }
    internal[15] = a_15;
    return internal;
}

vector<uint8_t> rFuncInv(vector<uint8_t> block) {
    vector<uint8_t> internal(KUZ_CONST::BLOCK_SIZE);
    uint8_t a_0 = block[15];

    for (int i = 1; i < KUZ_CONST::BLOCK_SIZE; i++) {
        internal[i] = block[i - 1];
        a_0 ^= gfMul(internal[i], KUZ_CONST::LIN_VEC[i]);
    }

    internal[0] = a_0;
    return internal;
}
// ==== R Function ====
// ====================

// ====================
// ==== L Function ====
vector<uint8_t> lFunc(vector<uint8_t> a) {
    vector<uint8_t> b = a;
    for (size_t i = 0; i < KUZ_CONST::BLOCK_SIZE; i++)
        b = rFunc(b);

    return b;
}

static vector<uint8_t> lFuncInv(vector<uint8_t> a) {
    vector<uint8_t> b = a;
    for (size_t i = 0; i < KUZ_CONST::BLOCK_SIZE; i++)
        b = rFuncInv(b);

    return b;
}
// ==== L Function ====
// ====================

// ======= = ======= ==========
// encript / decript functions:
vector<uint8_t> encript(vector<uint8_t> block, vector<vector<uint8_t>> keys) {
    vector<uint8_t> cypherText = block;

    for (size_t i = 0; i < 9; i++) {
        cypherText = xFunc(keys[i], cypherText);
        cypherText = sFunc(cypherText);
        cypherText = lFunc(cypherText);
    }
    cypherText = xFunc(cypherText, keys[9]);
    return cypherText;
}

vector<uint8_t> decript(vector<uint8_t> block, vector<vector<uint8_t>> keys) {
    vector<uint8_t> plaintext = block;

    plaintext = xFunc(plaintext, keys[9]);
    for (int i = 8; i >= 0; i--) {
        plaintext = lFuncInv(plaintext);
        plaintext = sFuncInv(plaintext);
        plaintext = xFunc(keys[i], plaintext);
    }

    return plaintext;
}

#endif