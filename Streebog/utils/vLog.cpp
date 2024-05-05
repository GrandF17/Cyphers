#ifndef VECTOR_LOG
#define VECTOR_LOG

#include <iomanip>
#include <iostream>
#include <vector>

using namespace std;

void printVector(vector<uint8_t> a) {
    cout << hex << setfill('0');
    cout << "0x";
    for (const auto& byte : a) {
        cout << setw(2) << static_cast<int>(byte);
    }
    cout << endl;
}

#endif