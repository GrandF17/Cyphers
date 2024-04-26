#include <fstream>
#include <string>
#include <vector>

#ifndef FILES_OP
#define FILES_OP

using namespace std;

vector<uint8_t> read(string filename) {
    ifstream myfile(filename, ios::binary);
    vector<uint8_t> data((istreambuf_iterator<char>(myfile)), istreambuf_iterator<char>());
    myfile.close();
    return data;
}

void save(vector<uint8_t> data, string outpuFilename, int size) {
    ofstream outfile(outpuFilename, ios::out | ios::binary);
    if (size != 0) {
        outfile.write((char *)&data[0], size);
    } else {
        outfile.write((char *)&data[0], data.size());
    }
}

#endif