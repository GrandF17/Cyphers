#include <string.h>

#include <cstdint>
#include <iostream>
#include <vector>

#include "./constants.h"

#ifndef KUZ_INTERFACES
#define KUZ_INTERFACES

using namespace std;

/**
 * @param keys - round keys. Len = ROUNDS_AMOUNT
 */
typedef struct {
    vector<vector<uint8_t>> keys[KUZ_CONST::ROUNDS_AMOUNT];
} roundKeys;

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
    ArgParam<string>* logFile;
    ArgParam<string>* file;
    ArgParam<string>* key;
    ArgParam<size_t>* offset;

    ProgramParams()
        : logFile(new ArgParam<string>(regex("--l=([^\"]*)"))),
          file(new ArgParam<string>(regex("--f=([^\"]*)"))),
          key(new ArgParam<string>(regex("--k=([^\"]*\\.key)"))),
          offset(new ArgParam<size_t>(regex("--o=([^\"]*)"))) {}

    ~ProgramParams() {
        delete logFile;
        delete file;
        delete key;
        delete offset;
    }
};

#endif