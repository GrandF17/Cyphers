#include <stdarg.h>
#include <string.h>
#include <time.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

#ifndef LOGGER
#define LOGGER

class Logger {
   private:
    ofstream* file;  // file where logs will be written
    bool quietMod;   // true - logs to console, else - not

    void printBlock(vector<uint8_t> block) {
        cout << hex << setfill('0');
        for (const auto& byte : block) {
            cout << setw(2) << static_cast<int>(byte);
        }
        cout << endl;
    }

   public:
    /**
     * @param fileName default value is string "NoFileSelected" to
     */
    Logger(string fileName = "NoFileSelected", bool _quietMod = true) {
        quietMod = _quietMod;
        file = new ofstream(fileName, ios::out | ios::app);
        if (file->fail())
            throw ios_base::failure(strerror(errno));
        file->exceptions(file->exceptions() | ios::failbit | ifstream::badbit);
    }

    ~Logger() {
        file->close();
        delete file;
    }

    string bytesToString(vector<uint8_t> logs) {
        stringstream ss;
        ss << hex << setfill('0');
        for (const auto& byte : logs) {
            ss << setw(2) << static_cast<int>(byte);
        }
        return ss.str();
    }

    void log(vector<string> logs) {
        if (!file->is_open()) {
            throw ios_base::failure("File is not open for writing.");
        }

        // writing vector of string to file
        for (auto log : logs) {
            time_t now = time(0);
            tm* localTime = localtime(&now);
            char timeString[80];
            strftime(timeString, 80, "[%Y-%m-%d %X] ", localTime);

            if (!quietMod) cout << "Date: " << timeString << " Log: " << log << endl;
            *file << "Date: " << timeString << " Log: " << log << endl;
        }
    }
};

#endif