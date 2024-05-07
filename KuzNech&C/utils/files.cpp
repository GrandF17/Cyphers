#ifndef FILES
#define FILES

#include <fstream>
#include <string>
#include <vector>

#include "../classes/logger.cpp"

using namespace std;

vector<uint8_t> read(string filename, class Logger* logger) {
    vector<uint8_t> data;
    ifstream myfile(filename, ios::binary);

    if (!myfile.is_open()) {
        logger->log({"Error: Failed to open file " + filename});
        throw "Function read logged error (myfile.is_open()).";
    }

    data.assign((istreambuf_iterator<char>(myfile)), istreambuf_iterator<char>());

    if (myfile.bad()) {
        logger->log({"Error: Failed to read file " + filename});
        myfile.close();
        throw "Function read logged error (myfile.bad()).";
    }

    myfile.close();

    if (myfile.is_open()) {
        logger->log({"Error : Failed to close file " + filename});
        throw "Function read logged error (myfile.is_open()).";
    }

    return data;
}

void save(vector<uint8_t> data, string outputFilename, class Logger* logger) {
    ofstream outfile(outputFilename, ios::out | ios::binary);
    if (!outfile.is_open()) {
        logger->log({"Error: Failed to open file " +
                     outputFilename +
                     " for writing"});
        throw "Function save logged error (outfile.is_open()).";
    }

    outfile.write(reinterpret_cast<char*>(&data[0]), data.size());
    
    if (outfile.bad()) {
        logger->log({"Error: Failed to write file " + outputFilename});
        outfile.close();
        throw "Function save logged error (outfile.bad()).";
    }

    outfile.close();

    if (outfile.is_open()) {
        logger->log({"Error: Failed to close file " + outputFilename});
        throw "Function save logged error (outfile.is_open()).";
    }
}

#endif