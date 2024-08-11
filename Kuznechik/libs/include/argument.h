#ifndef KUZNECHIK_LIBS_INCLUDE_ARGUMENT_H_INCLUDED
#define KUZNECHIK_LIBS_INCLUDE_ARGUMENT_H_INCLUDED

#include <regex>

using namespace std;

/**
 * @param regexPattern - regex value for which we search in argv
 * @param param - current parameter extracted out of braces (casually path to a file)
 */
template <typename T>
class Argument {
   public:
    regex regexPattern;
    T param;

    Argument(const regex& _pattern)
        : regexPattern(_pattern) {}

    ~Argument() = default;
};

#endif