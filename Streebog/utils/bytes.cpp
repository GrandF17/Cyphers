#ifndef BYTES
#define BYTES

#include <vector>

using namespace std;

vector<uint8_t> XOR(vector<uint8_t> a, vector<uint8_t> b) {
    for (size_t i = 0; i < a.size(); i++) a[i] ^= b[i];
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

#endif