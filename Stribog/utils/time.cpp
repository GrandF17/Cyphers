#include <string.h>
#include <time.h>

#include <cstdint>
#include <string>

using namespace std;

#ifndef TIME
#define TIME

string dateString() {
    time_t now = time(0);
    tm* localTime = localtime(&now);
    char timeString[80];
    strftime(timeString, 80, "[%Y-%m-%d %X] ", localTime);

    return timeString;
}

#endif