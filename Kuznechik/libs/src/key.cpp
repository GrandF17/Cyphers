
#ifndef KUZNECHIK_LIBS_KEY_GENERATION_SRC_KEY_CPP_INCLUDED
#define KUZNECHIK_LIBS_KEY_GENERATION_SRC_KEY_CPP_INCLUDED

#include "../include/key.h"

#include <cstdint>
#include <cstring>
#include <fstream>
#include <iomanip>  // unique_ptr, make_unique
#include <iostream>
#include <vector>

#include "../../constants.h"
#include "../include/kuznechik.h"

using namespace std;

/**
 * counting constant values for calculating round keys
 */
vector<vector<uint8_t>> Key::constants() {
    vector<vector<uint8_t>> constants(32, vector<uint8_t>(constants::BLOCK_SIZE));

    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < constants::BLOCK_SIZE; j++)
            constants[i][j] = 0;
        constants[i][0] = (uint8_t)(i + 1);
    }

    for (int i = 0; i < 32; i++)
        constants[i] = (constants[i]);

    return constants;
}

/**
 * reading file, that contains bit sequence, to create key
 */
vector<uint8_t> Key::readKey(const string& filename, size_t fileShift) {
    ifstream file(filename, ios::binary);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        exit(1);
    }

    // move the reading pointer to position eq fileShift
    file.seekg(fileShift, ios::beg);

    vector<uint8_t> masteKey(constants::MASTER_KEY_BYTES);
    for (size_t i = 0; i < constants::MASTER_KEY_BYTES; i++) {
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

/**
 * @details generates round keys for their further usage in xFunc
 * based on Feistel transformation
 */
vector<vector<uint8_t>> Key::expandKey(vector<uint8_t> lKey, vector<uint8_t> rKey) {
    Kuznechik kuz;
    vector<vector<uint8_t>> key(constants::ROUNDS_AMOUNT, vector<uint8_t>(64));
    vector<vector<uint8_t>> iterK = constants();

    vector<vector<uint8_t>> iter12(2);
    vector<vector<uint8_t>> iter34(2);

    key[0] = lKey;
    key[1] = rKey;
    iter12[0] = lKey;
    iter12[1] = rKey;

    for (size_t i = 0; i < 4; i++) {
        iter34 = kuz.feistelTransform(iter12[0], iter12[1], iterK[0 + 8 * i]);
        iter12 = kuz.feistelTransform(iter34[0], iter34[1], iterK[1 + 8 * i]);
        iter34 = kuz.feistelTransform(iter12[0], iter12[1], iterK[2 + 8 * i]);
        iter12 = kuz.feistelTransform(iter34[0], iter34[1], iterK[3 + 8 * i]);
        iter34 = kuz.feistelTransform(iter12[0], iter12[1], iterK[4 + 8 * i]);
        iter12 = kuz.feistelTransform(iter34[0], iter34[1], iterK[5 + 8 * i]);
        iter34 = kuz.feistelTransform(iter12[0], iter12[1], iterK[6 + 8 * i]);
        iter12 = kuz.feistelTransform(iter34[0], iter34[1], iterK[7 + 8 * i]);

        key[2 * i + 2] = iter12[0];
        key[2 * i + 3] = iter12[1];
    }

    return key;
}

vector<vector<uint8_t>> Key::leftAndRight(const string& filename, size_t fileShift) {
    vector<uint8_t> masterKey = readKey(filename, fileShift);

    vector<uint8_t> leftHalf(masterKey.begin(), masterKey.begin() + constants::MASTER_KEY_BYTES / 2);
    vector<uint8_t> rightHalf(masterKey.begin() + constants::MASTER_KEY_BYTES / 2, masterKey.end());

    return {leftHalf, rightHalf};
}

/**
 * @param fileName file where to get from data for key creation,
 * default value = "./1048576.key"
 * @param offset offset in the key file [...key(size)]
 */
Key::Key(const string fileName, const size_t offset)
    : masterKey(make_unique<vector<vector<uint8_t>>>(leftAndRight(fileName, offset))),
      keys(make_unique<vector<vector<uint8_t>>>(expandKey((*masterKey)[0], (*masterKey)[1]))) {}

Key::~Key() = default;

//////////////////////////////////////////
/// additional functionality for tests ///
vector<vector<uint8_t>> Key::createTestKey(vector<uint8_t> lKey, vector<uint8_t> rKey) {
    return expandKey(lKey, rKey);
}

#endif