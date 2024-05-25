#ifndef RANDOM_H
#define RANDOM_H

#include <fstream>
#include <iostream>

#include "../classes/key.cpp"
#include "magma.h"

// increasing this might increase performance a tiny bit
// at cost of cryptostrength
#define CHANGE_KEY_BLOCK_SIZE 32

// if abs(balance-0.5) > BALANCE_THRESHOLD,
// generated sequence is considered to be bad
#define BALANCE_THRESHOLD 0.01

class Random {
   public:
    Random();
    Random(const size_t& offset);
    Random(const string& keyFile, const size_t& offset);
    ~Random();

    vector<uint8_t> genSequence(const size_t len);
    static double balance(const vector<uint8_t>& seq);

   private:
    Key seed;

    uint64_t genBlock(const vector<uint8_t>& key);
    vector<uint8_t> genGamma();
    uint64_t genPRN();

    vector<uint8_t> XOR(vector<uint8_t> a, vector<uint8_t> b);
    vector<uint8_t> u64ToVec(uint64_t num);
    uint64_t vecToU64(vector<uint8_t> vec);
};

#endif