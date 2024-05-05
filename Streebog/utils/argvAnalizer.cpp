#ifndef ARGV_ANALIZER
#define ARGV_ANALIZER

#include <regex>

#include "../interfaces/interfaces.h"

using namespace std;

size_t paramIndex(int argc, char** argv, regex pattern) {
    for (size_t i = 0; i < argc; i++) {
        string str = argv[i];
        if (regex_match(str, pattern)) return i;
    }

    throw invalid_argument("Prameter not found in argv!");
}

string getParam(int argc, char** argv, regex pattern) {
    string input = argv[paramIndex(argc, argv, pattern)];
    regex argPattern("=([^\\s]*)");
    smatch match;

    if (regex_search(input, match, argPattern)) {
        if (match.size() > 1) {
            string extractedString = match[1];
            return extractedString;
        }
    }

    throw invalid_argument("Incorrect value in argv string!");
}

ProgramParams extractProgramParams(int argc, char** argv) {
    ProgramParams params;
    regex pattern("--[a-zA-Z]+=\\\"([^\"]*)\\\"");

    params.logFile->param = getParam(argc, argv, params.logFile->regPattern);
    params.key->param = getParam(argc, argv, params.key->regPattern);
    params.offset->param = (size_t)stoull(getParam(argc, argv, params.offset->regPattern));
    params.keyLen->param = (size_t)stoull(getParam(argc, argv, params.keyLen->regPattern));
    params.mode->param = (size_t)stoull(getParam(argc, argv, params.mode->regPattern));
    params.countChecksum->param = (bool)stoull(getParam(argc, argv, params.countChecksum->regPattern));

    return params;
}

#endif