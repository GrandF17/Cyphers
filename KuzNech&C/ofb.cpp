#include <cstdint>
#include <iomanip>
#include <iostream>
#include <regex>
#include <vector>

#include "./classes/keyGen.cpp"
#include "./classes/logger.cpp"
#include "./interface/interfaces.h"
#include "kuznechik.cpp"

using namespace std;

static vector<uint8_t> block =
    {0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x00, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11};

size_t paramIndex(int argc, char** argv, regex pattern) {
    for (size_t i = 0; i < argc; i++) {
        string str = argv[i];
        if (regex_match(str, pattern)) return i;
    }

    throw invalid_argument("Not found parameter, make sure you are entering correct values!");
}

string param(int argc, char** argv, regex pattern) {
    string input = argv[paramIndex(argc, argv, pattern)];
    regex argPattern("=([^\\s]*)");
    smatch match;

    if (regex_search(input, match, argPattern)) {
        if (match.size() > 1) {
            string extractedString = match[1];
            return extractedString;
        }
    }

    throw invalid_argument("Incorrect value in argv string. Make sure you are entering correct values!");
}

/**
 * recursive function to search all
 * parameters passed in argv[] array
 */
ProgramParams extractProgramParams(int argc, char** argv) {
    ProgramParams params;
    regex pattern("--[a-zA-Z]+=\\\"([^\"]*)\\\"");

    params.file->param = param(argc, argv, params.file->regPattern);
    params.logFile->param = param(argc, argv, params.logFile->regPattern);
    params.key->param = param(argc, argv, params.key->regPattern);
    params.offset->param = (size_t)stoull(param(argc, argv, params.offset->regPattern));

    return params;
}

int main(int argc, char** argv) {
    /**
     * main class that copntains all arguments
     */
    ProgramParams params = extractProgramParams(argc, argv);
    KeyGen keyGen;
    Logger logger = Logger(params.logFile->param, false);

    /**
     * to store logs in main and then write them down to file
     */
    vector<string> logs;

    /**
     * writing logs to vector (1 step)
     */
    {
        logs.push_back("Plain text: ");
        logs.push_back(logger.bytesToString(block));
    }

    vector<vector<uint8_t>> keys = keyGen.generateRoundKeys(params.key->param, params.offset->param);

    /**
     * writing logs to vector (2 step)
     */
    {
        logs.push_back("Master key: ");
        for (auto key : keys) logs.push_back(logger.bytesToString(key));
    }

    vector<uint8_t> encriptedBlock = encript(block, keys);

    /**
     * writing logs to vector (3 step)
     */
    {
        logs.push_back("Ecripted text: ");
        logs.push_back(logger.bytesToString(encriptedBlock));
    }

    vector<uint8_t> decriptedBlock = decript(encriptedBlock, keys);

    /**
     * writing logs to vector (3 step)
     */
    {
        logs.push_back("Decripted text: ");
        logs.push_back(logger.bytesToString(decriptedBlock));
    }

    // output of logs
    logger.log(logs);

    return 0;
}
