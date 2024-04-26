#include <regex>

#include "../interfaces/interfaces.h"

#ifndef KUZ_ARGV_ANALIZER
#define KUZ_ARGV_ANALIZER

using namespace std;

size_t paramIndex(int argc, char** argv, regex pattern) {
    for (size_t i = 0; i < argc; i++) {
        string str = argv[i];
        if (regex_match(str, pattern)) return i;
    }

    throw invalid_argument("Not found parameter, make sure you are entering correct values!");
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

    throw invalid_argument("Incorrect value in argv string. Make sure you are entering correct values!");
}

// ========== ========= ==== ==== ======
// extracting arguments from argv string
ProgramParams extractProgramParams(int argc, char** argv) {
    ProgramParams params;
    regex pattern("--[a-zA-Z]+=\\\"([^\"]*)\\\"");

    params.file->param = getParam(argc, argv, params.file->regPattern);
    params.logFile->param = getParam(argc, argv, params.logFile->regPattern);
    params.key->param = getParam(argc, argv, params.key->regPattern);
    params.offset->param = (size_t)stoull(getParam(argc, argv, params.offset->regPattern));
    // params.mode->param = (KUZ_CONST::KUZ_MODE)stoull(getParam(argc, argv, params.mode->regPattern));

    // cout << params.mode->param << " - kuznechick mode running..." << endl;

    return params;
}

#endif