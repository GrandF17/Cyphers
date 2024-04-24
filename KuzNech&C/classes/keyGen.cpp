#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

#include "../interface/constants.h"
#include "../kuznechik.cpp"

#ifndef KEY_GEN
#define KEY_GEN

using namespace std;

/**
 * @details KeyGen class
 *
 * @public only one function: generateRoundKeys(...)
 */
class KeyGen {
   private:
    /**
     * counting constant values for calculating round keys
     */
    vector<vector<uint8_t>> constant() {
        vector<vector<uint8_t>> constants(32, vector<uint8_t>(KUZ_CONST::BLOCK_SIZE));

        for (int i = 0; i < 32; i++) {
            for (int j = 0; j < KUZ_CONST::BLOCK_SIZE; j++)
                constants[i][j] = 0;
            constants[i][0] = (uint8_t)(i + 1);
        }

        for (int i = 0; i < 32; i++)
            constants[i] = (constants[i]);

        return constants;
    }

    /**
     * @details runs Feistel transformation for master key
     * to gen round keys (which amount eq to ROUNDS_AMOUNT constant)
     */
    vector<vector<uint8_t>> feistelTransform(vector<uint8_t> in_lKey, vector<uint8_t> in_rKey, vector<uint8_t> iterKonst) {
        vector<uint8_t> internal;
        vector<uint8_t> rKeyOut = in_lKey;

        internal = xFunc(in_lKey, iterKonst);
        internal = sFunc(internal);
        internal = lFunc(internal);

        vector<uint8_t> lKeyOut = xFunc(internal, in_rKey);
        vector<vector<uint8_t>> key(2);

        key[0] = lKeyOut;
        key[1] = rKeyOut;

        return key;
    }

    /**
     * @details generates round keys for their further usage in xFunc
     */
    vector<vector<uint8_t>> expandKey(vector<uint8_t> lKey, vector<uint8_t> rKey) {
        vector<vector<uint8_t>> key(KUZ_CONST::ROUNDS_AMOUNT, vector<uint8_t>(64));
        vector<vector<uint8_t>> iterK = constant();

        vector<vector<uint8_t>> iter12(2);
        vector<vector<uint8_t>> iter34(2);

        key[0] = lKey;
        key[1] = rKey;
        iter12[0] = lKey;
        iter12[1] = rKey;

        for (size_t i = 0; i < 4; i++) {
            iter34 = feistelTransform(iter12[0], iter12[1], iterK[0 + 8 * i]);
            iter12 = feistelTransform(iter34[0], iter34[1], iterK[1 + 8 * i]);
            iter34 = feistelTransform(iter12[0], iter12[1], iterK[2 + 8 * i]);
            iter12 = feistelTransform(iter34[0], iter34[1], iterK[3 + 8 * i]);
            iter34 = feistelTransform(iter12[0], iter12[1], iterK[4 + 8 * i]);
            iter12 = feistelTransform(iter34[0], iter34[1], iterK[5 + 8 * i]);
            iter34 = feistelTransform(iter12[0], iter12[1], iterK[6 + 8 * i]);
            iter12 = feistelTransform(iter34[0], iter34[1], iterK[7 + 8 * i]);

            key[2 * i + 2] = iter12[0];
            key[2 * i + 3] = iter12[1];
        }

        return key;
    }

    vector<uint8_t> readKey(const string& filename, size_t fileShift = 0) {
        ifstream file(filename, ios::binary);
        if (!file.is_open()) {
            cerr << "Error opening file: " << filename << endl;
            exit(1);
        }

        // move the reading pointer to position eq fileShift
        file.seekg(fileShift, ios::beg);

        vector<uint8_t> masteKey(KUZ_CONST::MASTER_KEY_BYTES);
        for (size_t i = 0; i < KUZ_CONST::MASTER_KEY_BYTES; i++) {
            uint8_t value = 0;
            for (int j = 0; j < 8; j++) {
                char bit;
                file.get(bit);

                if (file.eof()) {
                    cerr << "Error: End of file reached prematurely." << endl;
                    exit(1);
                }

                /**
                 * convert the character '0' or '1' to the numeric values 0 or 1.
                 */
                if (bit == '0' || bit == '1') {
                    value |= (bit - '0') << (7 - j);  // shift bits left
                } else {
                    cerr << "Error: Invalid bit encountered." << endl;
                    exit(1);
                }
            }

            masteKey[i] = value;
        }

        return masteKey;
    }

   public:
    vector<vector<uint8_t>> generateRoundKeys(const string& filename, size_t fileShift = 0) {
        vector<uint8_t> masterKey = readKey(filename, fileShift);

        vector<uint8_t> leftHalf(masterKey.begin(), masterKey.begin() + KUZ_CONST::MASTER_KEY_BYTES / 2);
        vector<uint8_t> rightHalf(masterKey.begin() + KUZ_CONST::MASTER_KEY_BYTES / 2, masterKey.end());

        return expandKey(leftHalf, rightHalf);
    }
};

#endif