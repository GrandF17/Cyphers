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
 * @param key       - file with bit sequence length >= 256 bits
 * @param offset    - offset in file that contains key (only if there are more than 256) default = 0
 */
class ProgramParams {
   public:
    ArgParam<string>* key;
    ArgParam<size_t>* offset;
    ArgParam<size_t>* seqLen;

    ProgramParams():
          key(new ArgParam<string>(regex("--k=([^\"]*\\.key)"))),
          offset(new ArgParam<size_t>(regex("--o=([^\"]*)"))),
          seqLen(new ArgParam<size_t>(regex("--l=([^\"]*)"))) {}

    ~ProgramParams() {
        delete key;
        delete offset;
        delete seqLen;
    }
};

#endif