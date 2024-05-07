#ifndef TESTS
#define TESTS

#include <cstdint>
#include <cstdio>
#include <iomanip>
#include <vector>

#include "../classes/key.cpp"
#include "../libs/kuznechik.h"

using namespace std;

bool compareVectors(const vector<uint8_t>& vec1, const vector<uint8_t>& vec2) {
    if (vec1.size() != vec2.size()) {
        return false;
    }

    return equal(vec1.begin(), vec1.end(), vec2.begin());
}

bool encriptBlock(vector<vector<uint8_t>> keys) {
    Kuznechik kuz;
    static vector<uint8_t> plain = {
        0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x00,
        0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa, 0x99, 0x88};

    static vector<uint8_t> cypher = {
        0xb4, 0x53, 0x49, 0xf3, 0xb0, 0x17, 0xf4, 0x0b,
        0x60, 0xcb, 0xdf, 0xd2, 0x55, 0xff, 0x13, 0xcc};

    vector<uint8_t> enc = kuz.encrypt(plain, keys);
    return compareVectors(enc, cypher);
};

bool decriptBlock(vector<vector<uint8_t>> keys) {
    Kuznechik kuz;
    static vector<uint8_t> plain = {
        0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x00,
        0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa, 0x99, 0x88};

    static vector<uint8_t> cypher = {
        0xb4, 0x53, 0x49, 0xf3, 0xb0, 0x17, 0xf4, 0x0b,
        0x60, 0xcb, 0xdf, 0xd2, 0x55, 0xff, 0x13, 0xcc};

    vector<uint8_t> dec = kuz.decrypt(cypher, keys);
    return compareVectors(dec, plain);
};

// bool ping_pong(vector<vector<uint8_t>> keys) {
//     Kuznechik kuz;
//     static vector<uint8_t> plain = {
//         0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x00,
//         0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa, 0x99, 0x88};

//     static vector<uint8_t> cypher = {
//         0xb4, 0x53, 0x49, 0xf3, 0xb0, 0x17, 0xf4, 0x0b,
//         0x60, 0xcb, 0xdf, 0xd2, 0x55, 0xff, 0x13, 0xcc};

//     vector<uint8_t> enc = kuz.encrypt(plain, keys);
//     return compareVectors(enc, cypher);
// };

bool test() {
    Key keygen;
    size_t total = 2;
    size_t passed = 0;
    vector<vector<uint8_t>> masterKey = {
        {0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
         0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77},
        {0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
         0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef}};

    vector<vector<uint8_t>> keys = keygen.createTestKey(masterKey[0], masterKey[1]);

    passed += encriptBlock(keys);
    passed += decriptBlock(keys);

    return total == passed;
}

#endif