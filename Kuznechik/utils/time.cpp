#ifndef KUZNECHIK_UTILS_TIME_CPP_INCLUDED
#define KUZNECHIK_UTILS_TIME_CPP_INCLUDED

#include <string.h>
#include <time.h>

#include <cstdint>
#include <string>

inline std::string dateString() {
    time_t now = time(0);
    tm* localTime = localtime(&now);
    char timeString[80];
    strftime(timeString, 80, "[%Y-%m-%d %X] ", localTime);

    return timeString;
}

#endif