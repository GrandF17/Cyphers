#ifndef STREEBOG
#define STREEBOG

#include "./streebog.h"

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include "../interfaces/constants.h"

using namespace std;

vector<uint8_t> Streebog::encode(uint64_t number) {
    vector<uint8_t> bytes(64, 0x00);
    vector<uint8_t> tmp;  // stores only bytes on bumber (probably less than 64)

    while (number > 0) {
        tmp.push_back(static_cast<uint8_t>(number & 0xFF));
        number >>= 8;  // shift right number by 1 byte
    }

    reverse(tmp.begin(), tmp.end());
    copy(tmp.begin(), tmp.end(), bytes.end() - tmp.size());

    return bytes;
}

vector<uint8_t> Streebog::encode(const string& str) {
    vector<uint8_t> bytes;
    for (auto c : str)
        bytes.push_back(static_cast<uint8_t>(c));

    return bytes;
}

string Streebog::decode(const vector<uint8_t>& bytes) {
    stringstream ss;
    ss << hex << setfill('0');
    for (const auto& byte : bytes) {
        ss << setw(2) << static_cast<int>(byte);
    }
    return "0x" + ss.str();
}

inline vector<uint8_t> Streebog::XOR(vector<uint8_t> a, vector<uint8_t> b) {
    for (size_t i = 0; i < a.size(); i++) a[i] ^= b[i];
    return a;
}

inline vector<uint8_t> Streebog::ADD(vector<uint8_t> a, vector<uint8_t> b) {
    bool overflow = 0;
    for (int i = a.size() - 1; i >= 0; i--) {
        uint16_t sum = a[i] + b[i] + overflow;
        a[i] = (uint8_t)sum;
        overflow = sum > 255 ? 1 : 0;
    }

    return a;
}

inline vector<uint8_t> Streebog::S(const vector<uint8_t>& block) {
    vector<uint8_t> result(STREEBOG_CONST::BLOCK_SIZE);
    for (size_t i = 0; i < result.size(); i++)
        result[i] = STREEBOG_CONST::SBOX[block[i]];

    return result;
}

inline vector<uint8_t> Streebog::P(const vector<uint8_t>& block) {
    vector<uint8_t> result(STREEBOG_CONST::BLOCK_SIZE);
    for (size_t i = 0; i < result.size(); i++)
        result[i] = block[STREEBOG_CONST::PBOX[i]];

    return result;
}

inline vector<uint8_t> Streebog::L(vector<uint8_t> block) {
    /**
     * all operations below are based on feature of data type "union"
     * it allows to share data space between all variables
     */
    union {
        uint8_t bytes[64];
        uint64_t longs[8];
    } in, out;

    for (int i = 0; i < 64; i++)
        in.bytes[i] = block[63 - i];

    memset(&out, 0, 64);
    for (size_t i = 0; i < 8; i++)
        for (size_t shift = 0; shift < 64; shift++)
            if ((in.longs[i] >> (63 - shift)) & 0x01)
                out.longs[i] ^= A[shift];

    for (size_t i = 0; i < 64; i++)
        block[i] = out.bytes[63 - i];

    return block;
};

inline vector<uint8_t> Streebog::keySchedule(vector<uint8_t> K, size_t i) {
    K = XOR(K, STREEBOG_CONST::C[i]);
    K = S(K);
    K = P(K);
    K = L(K);
    return K;
}

inline vector<uint8_t> Streebog::E(vector<uint8_t> K, const vector<uint8_t>& m) {
    vector<uint8_t> state = XOR(K, m);

    for (int i = 0; i < 12; i++) {
        state = S(state);
        state = P(state);
        state = L(state);
        K = keySchedule(K, i);
        state = XOR(state, K);
    }

    return state;
}

inline vector<uint8_t> Streebog::G(
    const vector<uint8_t>& N,
    const vector<uint8_t>& h,
    const vector<uint8_t>& m) {
    vector<uint8_t> K = XOR(h, N);
    K = S(K);
    K = P(K);
    K = L(K);

    vector<uint8_t> t = E(K, m);
    t = XOR(t, h);

    vector<uint8_t> newH = XOR(t, m);
    return newH;
}

inline vector<uint8_t> Streebog::countHash(
    const vector<uint8_t>& message,
    vector<uint8_t> h,
    vector<uint8_t> N,
    vector<uint8_t> S,
    vector<uint8_t> m) {
    /**
     * second step (check if maessage size < 512)
     * if NOT do:
     * - copy last 512 bits to m
     * - h = g(N, m, h)
     * - N = (N + 512) mod 2^512
     * - S = (S + m) mod 2^512
     * - go to second step -->
     */

    int textPointer = message.size() - 64;
    for (; textPointer >= 0; textPointer -= 64) {
        copy(message.begin() + textPointer, message.begin() + textPointer + 64, back_inserter(m));

        h = G(N, h, m);
        N = ADD(N, encode(512));
        S = ADD(S, m);

        m.clear();
    }

    // by now pointer is negative and is in range: [-64; -1].
    // m   =   0......0 | 1 | [ leftMessage ]
    size_t leftBytes = textPointer + 64;
    m.resize(64 - leftBytes - 1, 0x00);
    m.push_back(0x01);

    copy(message.end() - leftBytes, message.end(), back_inserter(m));

    h = G(N, h, m);
    N = ADD(N, encode((64 + textPointer) * 8));
    S = ADD(S, m);

    vector<uint8_t> zero(64, 0x00);
    h = G(zero, h, N);
    h = G(zero, h, S);

    return h;
}

vector<uint8_t> Streebog::streebog256(const vector<uint8_t>& message) {
    /**
     * first step (define vars)
     */

    vector<uint8_t> h(64, 0x01);  // if 256 --> set 0x01
    vector<uint8_t> N(64, 0x00), S(64, 0x00), m;

    h = countHash(message, h, N, S, m);

    // return first 32 bytes of hashed str
    return vector<uint8_t>(h.begin(), h.begin() + 32);
}

/**
 * @overload of streebog256(vector<uint8_t> message)
 */
vector<uint8_t> Streebog::streebog256(const string& message) {
    return streebog256(encode(message));
}

vector<uint8_t> Streebog::streebog512(const vector<uint8_t>& message) {
    /**
     * first step (define vars)
     */

    vector<uint8_t> h(64, 0x00);  // if 512 --> set 0x00
    vector<uint8_t> N(64, 0x00), S(64, 0x00), m;

    h = countHash(message, h, N, S, m);

    // return all bytes of hashed str
    return h;
}

/**
 * @overload of streebog256(vector<uint8_t> message)
 */
vector<uint8_t> Streebog::streebog512(const string& message) {
    return streebog512(encode(message));
}

Streebog::Streebog() {}

//////////////////////////////

string streebog(const string& str, KEY_LEN len) {
    Streebog hash = Streebog();
    switch (len) {
        case k64:
            return hash.decode(hash.streebog512(str));
            break;

        default:
            return hash.decode(hash.streebog256(str));
            break;
    }
}

vector<uint8_t> streebog(const vector<uint8_t>& bytes, KEY_LEN len) {
    Streebog hash = Streebog();
    switch (len) {
        case k64:
            return hash.streebog512(bytes);
            break;

        default:
            return hash.streebog256(bytes);
            break;
    }
}

#endif