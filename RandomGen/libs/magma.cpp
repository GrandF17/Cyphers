#ifndef MAGMA_LC
#define MAGMA_LC

#include "magma.h"
#include <cstring>
#include <cstdint>
#include <stdexcept>
#include <algorithm>
#include <vector>

#include "../interfaces/constants.h"

using namespace std;

uint32_t MagmaLC::getPart(const vector<uint8_t>& key, const size_t& from) {
    uint32_t part = 0;
    for (size_t i = from; i < from + 4; i++) {
        part += key[i];
        part = part << 8;
    }

    return part;
}

vector<uint8_t> MagmaLC::changeKey(vector<uint8_t> newKey) {
    if (newKey.size() != 32) throw runtime_error("Key len should be eq 32 bytes!");

    /*
     * reverse key to match endianness
     * 0x12345678 --> 0x78563412
     * 0x[12][34][56][78] ~ [0001 0010] [0011 0100] [0101 0110] [0111 1000]
     * 0x[12][34][56][78] --> 0x[78][56][34][12]
     */
    for (size_t i = 0; i < newKey.size(); i += 4) {
        swap(newKey[i], newKey[i + 3]);
        swap(newKey[i + 1], newKey[i + 2]);
    }

    return newKey;
}

MagmaLC::MagmaLC() {}

////////////////////////////////

uint64_t MagmaLC::encrypt(uint64_t in, const vector<uint8_t>& masterKey) {
    uint32_t left = (in >> 32) & 0xFFFFFFFF;
    uint32_t right = in & 0xFFFFFFFF;

    for (int i = 0; i < 24; i++) {
        uint32_t key = getPart(masterKey, (i % 8) * 4);
        G(left, right, key);
    }

    for (int i = 24; i < 31; i++) {
        uint32_t key = getPart(masterKey, (7 - (i % 8)) * 4);
        G(left, right, key);
    }

    return G_final(left, right, getPart(masterKey, 0));
}

void MagmaLC::G(uint32_t& left, uint32_t& right, const uint32_t& key) {
    uint32_t swap_tmp = right;
    right += key;
    uint32_t sboxOut = 0;

    for (int i = 0; i < 8; i++)
        sboxOut = (sboxOut << 4) | PI[7 - i][(right >> (28 - i * 4)) & 0xF];

    right = (sboxOut << 11) | ((sboxOut >> 21) & 0x07FF);
    right ^= left;
    left = swap_tmp;
}

uint64_t MagmaLC::G_final(uint32_t& left, uint32_t& right, const uint32_t& key) {
    uint32_t swap_tmp = right;
    right += key;
    uint32_t sboxOut = 0;

    for (int i = 0; i < 8; i++)
        sboxOut = (sboxOut << 4) | PI[7 - i][(right >> (28 - i * 4)) & 0xF];

    right = (sboxOut << 11) | ((sboxOut >> 21) & 0x07FF);
    right ^= left;

    return (uint64_t(right) << 32) | swap_tmp;
}

#endif
