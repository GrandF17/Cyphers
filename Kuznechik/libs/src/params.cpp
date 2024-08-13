#ifndef KUZNECHIK_LIBS_SRC_PARAMS_CPP_INCLUDED
#define KUZNECHIK_LIBS_SRC_PARAMS_CPP_INCLUDED

#include "../include/params.h"

#include <regex>

using namespace std;

size_t ProgramParams::paramIndex(int argc, char** argv, regex pattern) {
    for (size_t i = 0; i < argc; i++) {
        string str = argv[i];
        if (regex_match(str, pattern)) return i;
    }

    throw invalid_argument("Prameter not found in argv!");
}

string ProgramParams::getParam(int argc, char** argv, regex pattern) {
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

///////////////////////

ProgramParams::ProgramParams(int argc, char** argv)
    : mode(new Argument<size_t>(regex("-mode=([^\"]*)"))),
      file(new Argument<string>(regex("-f=([^\"]*)"))),
      key(new Argument<string>(regex("-k=([^\"]*\\.key)"))),
      offset(new Argument<size_t>(regex("-o=([^\"]*)"))),
      logFile(new Argument<string>(regex("-l=([^\"]*)"))),
      recountCheksum(new Argument<bool>(regex("-rc=([^\"]*)"))) {
    file->param = getParam(argc, argv, file->regexPattern);
    logFile->param = getParam(argc, argv, logFile->regexPattern);
    key->param = getParam(argc, argv, key->regexPattern);
    offset->param = (size_t)stoull(getParam(argc, argv, offset->regexPattern));
    mode->param = (size_t)stoull(getParam(argc, argv, mode->regexPattern));
    recountCheksum->param = (bool)stoull(getParam(argc, argv, recountCheksum->regexPattern));
}

ProgramParams::~ProgramParams() {
    delete mode;
    delete file;
    delete key;
    delete file;
    delete offset;
    delete logFile;
    delete recountCheksum;
}

#endif