#ifndef KUZNECHIK
#define KUZNECHIK

#include "./kuznechik.h"

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <vector>

#include "../interfaces/constants.h"
#include "../interfaces/interfaces.h"

using namespace std;

inline uint8_t Kuznechik::gfMul(uint8_t a, uint8_t b) {
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

inline vector<uint8_t> Kuznechik::xFunc(const vector<uint8_t>& a, const vector<uint8_t>& b) {
    vector<uint8_t> c;
    for (size_t i = 0; i < a.size(); i++)
        c.push_back(a[i] ^ b[i]);

    return c;
}

inline vector<uint8_t> Kuznechik::sFunc(const vector<uint8_t>& a) {
    vector<uint8_t> b;
    for (auto byte : a)
        b.push_back(KUZ_CONST::SBOX[byte]);

    return b;
}

inline vector<uint8_t> Kuznechik::sFuncInv(const vector<uint8_t>& a) {
    vector<uint8_t> b;
    for (auto byte : a)
        b.push_back(KUZ_CONST::INV_SBOX[byte]);

    return b;
}

inline vector<uint8_t> Kuznechik::rFunc(const vector<uint8_t>& a) {
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

inline vector<uint8_t> Kuznechik::rFuncInv(const vector<uint8_t>& a) {
    vector<uint8_t> internal(KUZ_CONST::BLOCK_SIZE);
    uint8_t a_0 = a[15];

    for (int i = 1; i < KUZ_CONST::BLOCK_SIZE; i++) {
        internal[i] = a[i - 1];
        a_0 ^= gfMul(internal[i], KUZ_CONST::LIN_VEC[i]);
    }

    internal[0] = a_0;
    return internal;
}

inline vector<uint8_t> Kuznechik::lFunc(const vector<uint8_t>& a) {
    vector<uint8_t> b = a;
    for (size_t i = 0; i < KUZ_CONST::BLOCK_SIZE; i++)
        b = rFunc(b);

    return b;
}

inline vector<uint8_t> Kuznechik::lFuncInv(const vector<uint8_t>& a) {
    vector<uint8_t> b = a;
    for (size_t i = 0; i < KUZ_CONST::BLOCK_SIZE; i++)
        b = rFuncInv(b);

    return b;
}

//////////////////////////////

vector<uint8_t> Kuznechik::encrypt(const vector<uint8_t>& block, const vector<vector<uint8_t>>& keys) {
    if (block.size() != KUZ_CONST::BLOCK_SIZE) throw "Block size incorrect (encript)";
    vector<uint8_t> cypherText = block;

    for (size_t i = 0; i < 9; i++) {
        cypherText = xFunc(keys[i], cypherText);
        cypherText = sFunc(cypherText);
        cypherText = lFunc(cypherText);
    }
    cypherText = xFunc(cypherText, keys[9]);

    return cypherText;
}

vector<uint8_t> Kuznechik::decrypt(const vector<uint8_t>& block, const vector<vector<uint8_t>>& keys) {
    if (block.size() != KUZ_CONST::BLOCK_SIZE) throw "Block size incorrect (decript)";
    vector<uint8_t> plaintext = block;

    plaintext = xFunc(plaintext, keys[9]);
    for (int i = 8; i >= 0; i--) {
        plaintext = lFuncInv(plaintext);
        plaintext = sFuncInv(plaintext);
        plaintext = xFunc(keys[i], plaintext);
    }

    return plaintext;
}

/**
 * @details runs Feistel transformation for master key
 * to gen round keys (which amount eq to ROUNDS_AMOUNT constant)
 */
vector<vector<uint8_t>> Kuznechik::feistelTransform(
    const vector<uint8_t>& lKey,
    const vector<uint8_t>& rKey,
    const vector<uint8_t>& iterator) {
    vector<uint8_t> internal;
    vector<uint8_t> rKeyOut = lKey;

    internal = xFunc(lKey, iterator);
    internal = sFunc(internal);
    internal = lFunc(internal);

    vector<uint8_t> lKeyOut = xFunc(internal, rKey);
    vector<vector<uint8_t>> key(2);

    key[0] = lKeyOut;
    key[1] = rKeyOut;

    return key;
}

Kuznechik::Kuznechik() {}

//////////////////////////////

vector<uint8_t> encode(uint64_t number) {
    vector<uint8_t> bytes(8, 0x00);
    vector<uint8_t> tmp;  // stores only bytes on bumber (probably less than 64)

    while (number > 0) {
        tmp.push_back(static_cast<uint8_t>(number & 0xFF));
        number >>= 8;  // shift right number by 1 byte
    }

    reverse(tmp.begin(), tmp.end());
    copy(tmp.begin(), tmp.end(), bytes.end() - tmp.size());

    return bytes;
}

uint64_t decode(vector<uint8_t> bytes) {
    if (bytes.size() > sizeof(uint64_t)) {
        cerr << "Error: Vector size exceeds uint64_t size!" << endl;
        return 0;
    }

    uint64_t number = 0;
    for (size_t i = 0; i < bytes.size(); ++i) {
        number = (number << 8) | bytes[i];  // shift left number by 1 byte
    }

    return number;
}

vector<uint8_t> encryptOFB(const vector<uint8_t>& data,
                           const vector<vector<uint8_t>>& keys,
                           const vector<uint8_t>& IV) {
    Kuznechik kuz;

    vector<uint8_t> size = encode(data.size());  // 64 bits / 8 bytes
    cout << data.size() << endl;
    vector<uint8_t> feedback = IV;
    vector<uint8_t> encryptedData;

    for (int i = 0; i < data.size(); i += KUZ_CONST::BLOCK_SIZE) {
        vector<uint8_t> blockResult = kuz.encrypt(feedback, keys);
        for (int j = 0; j < KUZ_CONST::BLOCK_SIZE; j++)
            encryptedData.push_back(data[i + j] ^ blockResult[j]);

        feedback = blockResult;  // update output feedback
    }
    // got such data space managment:
    // [[...size_of_file], [...IV], [...encryptedData]]
    vector<uint8_t> result(size.size() + feedback.size() + encryptedData.size());
    copy(size.begin(), size.end(), result.begin());
    copy(IV.begin(), IV.end(), result.begin() + size.size());
    copy(encryptedData.begin(), encryptedData.end(), result.begin() + size.size() + feedback.size());

    return result;
}

vector<uint8_t> decryptOFB(const vector<uint8_t>& data, const vector<vector<uint8_t>>& keys) {
    Kuznechik kuz;
    vector<uint8_t> result;

    // extract a row vector - the final encrypted value of the transferred IV
    vector<uint8_t> size(data.begin(), data.begin() + 8);
    vector<uint8_t> feedback(data.begin() + 8, data.begin() + 8 + KUZ_CONST::BLOCK_SIZE);

    for (int i = 8 + KUZ_CONST::BLOCK_SIZE; i < data.size(); i += KUZ_CONST::BLOCK_SIZE) {
        vector<uint8_t> blockResult = kuz.encrypt(feedback, keys);
        for (int j = 0; j < KUZ_CONST::BLOCK_SIZE; ++j)
            result.push_back(data[i + j] ^ blockResult[j]);

        feedback = blockResult;  // update output feedback
    }

    result.resize(decode(size));
    return result;
}

#endif