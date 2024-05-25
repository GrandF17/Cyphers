#ifndef LOG
#define LOG

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace std;

void printVectorAsHex(const vector<uint8_t>& vec) {
    for (const auto& byte : vec) {
        cout << hex << setw(2) << setfill('0') << static_cast<int>(byte) << " ";
    }
    cout << dec << endl;
}

#endif