#include <stdarg.h>
#include <string.h>

#include <bitset>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "../classes/logger.cpp"
#include "../interfaces/constants.h"
#include "../utils/time.cpp"

using namespace std;

#ifndef STRIBOG
#define STRIBOG

class Stribog {
   private:
    vector<uint8_t> numToBytes(uint64_t number) {
        vector<uint8_t> bytes(64, 0x00);
        while (number > 0) {
            bytes.insert(bytes.begin(), static_cast<uint8_t>(number & 0xFF));
            number >>= 8;  // Сдвигаем число на 8 бит (1 байт)
        }
        return bytes;
    }

    vector<uint8_t> stringToBytes(const string& str) {
        vector<uint8_t> bytes;
        for (char c : str) {
            bytes.push_back(static_cast<uint8_t>(c));
        }
        return bytes;
    }

    vector<uint8_t> XOR(vector<uint8_t> a, vector<uint8_t> b) {
        for (size_t i = 0; i < a.size(); i++)
            a[i] ^= b[i];

        return a;
    }

    vector<uint8_t> ADD(vector<uint8_t> a, vector<uint8_t> b) {
        bool overflow = 0;
        for (int i = a.size() - 1; i >= 0; i--) {
            uint16_t sum = a[i] + b[i] + overflow;
            a[i] = (uint8_t)sum;
            overflow = sum > 255 ? 1 : 0;
        }

        return a;
    }

    vector<uint8_t> S(vector<uint8_t> block) {
        vector<uint8_t> result(STRIBOG_CONST::BLOCK_SIZE);
        for (size_t i = 0; i < result.size(); i++)
            result[i] = STRIBOG_CONST::SBOX[block[i]];

        return result;
    }

    vector<uint8_t> P(vector<uint8_t> block) {
        vector<uint8_t> result(STRIBOG_CONST::BLOCK_SIZE);
        for (size_t i = 0; i < result.size(); i++)
            result[i] = block[STRIBOG_CONST::PBOX[i]];

        return result;
    }

    vector<uint8_t> L(vector<uint8_t> block) {
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

    vector<uint8_t> keySchedule(vector<uint8_t> K, size_t i) {
        K = XOR(K, STRIBOG_CONST::C[i]);
        K = S(K);
        K = P(K);
        K = L(K);
        return K;
    }

    vector<uint8_t> E(vector<uint8_t> K, vector<uint8_t> m) {
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

    vector<uint8_t> G_n(vector<uint8_t> N, vector<uint8_t> h, vector<uint8_t> m) {
        vector<uint8_t> K = XOR(h, N);
        K = S(K);
        K = P(K);
        K = L(K);
        vector<uint8_t> t = E(K, m);
        t = XOR(t, h);
        vector<uint8_t> newh = XOR(t, m);
        return newh;
    }

   public:
    vector<uint8_t> stribog256(vector<uint8_t> message) {
        /**
         * first step (define vars)
         */

        vector<uint8_t> h(64, 0x01);  // if 256 --> set 0x01 ELSE if 512 --> set 0x00
        vector<uint8_t> N(64, 0), S(64, 0), m(64, 0);

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
            copy(message.begin() + textPointer, message.begin() + textPointer + 64, m.begin());

            h = G_n(N, h, m);
            N = ADD(N, numToBytes(512));
            S = ADD(S, m);
        }

        // by now pointer is negative and is in range: [-64; -1].
        // m   =   0......0 | 1 | [ leftMessage ]
        size_t leftBytes = textPointer + 64;
        m.resize(64, 0x00);  // fill vector with zeroes
        copy(message.begin(), message.end(), back_inserter(m));
        m[64 - leftBytes - 1] = 0x01;

        G_n(N, h, m);
        ADD(N, numToBytes((64 + textPointer) * 8));
        ADD(S, m);

        vector<uint8_t> zero(64, 0x00);
        h = G_n(zero, h, N);
        h = G_n(zero, h, S);

        return h;
    }

    /**
     * @overload of stribog256(vector<uint8_t> message)
     */
    vector<uint8_t> stribog256(string message) {
        return stribog256(stringToBytes(message));
    }
};

#endif