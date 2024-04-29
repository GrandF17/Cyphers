#include <fstream>
#include <string>
#include <vector>

#include "../classes/logger.cpp"

#ifndef FILES_OP
#define FILES_OP

using namespace std;

vector<uint8_t> read(string filename, class Logger* logger) {
    vector<uint8_t> data;
    ifstream myfile(filename, ios::binary);

    if (!myfile.is_open()) {
        logger->log({"Error: Failed to open file " + filename});
        return data;
    }

    data.assign((istreambuf_iterator<char>(myfile)), istreambuf_iterator<char>());

    if (myfile.bad()) {
        logger->log({"Error: Failed to read file " + filename});
        myfile.close();
        return data;
    }

    myfile.close();

    if (myfile.is_open()) {
        logger->log({"Error : Failed to close file " + filename});
        return data;
    }

    return data;
}

void save(vector<uint8_t> data, string outputFilename, int size, class Logger* logger) {
    ofstream outfile(outputFilename, ios::out | ios::binary);
    if (!outfile.is_open()) {
        logger->log({"Error: Failed to open file " +
                     outputFilename +
                     " for writing"});
        return;
    }

    if (size != 0 && size <= data.size()) {
        outfile.write(reinterpret_cast<char*>(&data[0]), size);
    } else {
        outfile.write(reinterpret_cast<char*>(&data[0]), data.size());
    }

    if (outfile.bad()) {
        logger->log({"Error: Failed to write file " + outputFilename});
        outfile.close();
        return;
    }

    outfile.close();

    if (outfile.is_open()) {
        logger->log({"Error: Failed to close file " + outputFilename});
        return;
    }
}

#endif