#ifndef INTERFACES
#define INTERFACES

#include <string.h>

#include <cstdint>
#include <iostream>
#include <regex>
#include <vector>

#include "./constants.h"

using namespace std;

/**
 * @param str is regex value for which we search in argv
 * @param param is current parameter extracted out of braces (casually path to a file)
 */
template <typename T>
class ArgParam {
   public:
    regex regPattern;
    T param;

    ArgParam(const regex& _pattern)
        : regPattern(_pattern) {}
};

/**
 * @param logFile   - where logs will be put
 * @param file      - data to ecnrypt/decript
 * @param key       - file with bit sequence length >= 256 bits
 * @param offset    - offset in file that contains key (only if there are more than 256) default = 0
 */
class ProgramParams {
   public:
    ArgParam<size_t>* mode;
    ArgParam<string>* file;
    ArgParam<string>* key;
    ArgParam<size_t>* offset;
    ArgParam<string>* logFile;
    ArgParam<bool>* countChecksum;

    ProgramParams()
        : mode(new ArgParam<size_t>(regex("--mode=([^\"]*)"))),
          file(new ArgParam<string>(regex("--f=([^\"]*)"))),
          key(new ArgParam<string>(regex("--k=([^\"]*\\.key)"))),
          offset(new ArgParam<size_t>(regex("--o=([^\"]*)"))),
          logFile(new ArgParam<string>(regex("--l=([^\"]*)"))),
          countChecksum(new ArgParam<bool>(regex("--checksum=([^\"]*)"))) {}

    ~ProgramParams() {
        delete logFile;
        delete file;
        delete key;
        delete offset;
    }
};

#endif