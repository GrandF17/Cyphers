#ifndef KUZNECHIK_LIBS_INCLUDE_PARAMS_H_INCLUDED
#define KUZNECHIK_LIBS_INCLUDE_PARAMS_H_INCLUDED

#include <string.h>

#include <cstdint>
#include <iostream>
#include <regex>
#include <vector>

#include "../include/argument.h"
#include "constants.h"

using namespace std;

/**
 * @param mode              - what to do: encrypt/decrypt
 * @param file              - data to ecnrypt/decript
 * @param key               - file with bit sequence length >= 256 bits
 * @param offset            - offset in file that contains key (only if there are more than 256) default = 0
 * @param logFile           - where logs will be put
 * @param recountCheksum    - if executable file was changed --> recount its checksum
 */
class ProgramParams {
   public:
    Argument<size_t>* mode;
    Argument<string>* file;
    Argument<string>* key;
    Argument<size_t>* offset;
    Argument<string>* logFile;
    Argument<bool>* recountCheksum;

    ProgramParams(int argc, char** argv);
    ~ProgramParams();

   private:
    size_t paramIndex(int argc, char** argv, regex pattern);
    string getParam(int argc, char** argv, regex pattern);
};

#endif