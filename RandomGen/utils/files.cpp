#ifndef FILES
#define FILES

#include <fstream>
#include <string>
#include <vector>

using namespace std;

void save(vector<uint8_t> data, const string& outputFilename) {
    ofstream outfile(outputFilename, ios::out | ios::binary);
    if (!outfile.is_open()) {
        throw "Function save logged error (outfile.is_open()).";
    }

    outfile.write(reinterpret_cast<char*>(&data[0]), data.size());
    // // to write to file as ASCII 0/1:
    // for (uint8_t byte : data) {
    //     for (int i = 7; i >= 0; --i) {
    //         bool bit = (byte >> i) & 1;
    //         outfile << bit;
    //     }
    // }

    if (outfile.bad()) {
        outfile.close();
        throw "Function save logged error (outfile.bad()).";
    }

    outfile.close();

    if (outfile.is_open()) {
        throw "Function save logged error (outfile.is_open()).";
    }
}

#endif