#include <stdarg.h>
#include <string.h>

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

#include "../utils/time.cpp"

using namespace std;

#ifndef LOGGER
#define LOGGER

class Logger {
   private:
    ofstream* file;  // file where logs will be written
    bool quietMod;   // true - logs to console, else - not

   public:
    /**
     * @param fileName default value is string "NoFileSelected" to
     */
    Logger(string fileName = "./KuznechikLogs.dat", bool _quietMod = true) {
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

    void log(vector<string> logs) {
        if (!file->is_open()) {
            throw ios_base::failure("File is not open for writing.");
        }

        // writing vector of string to file
        for (auto log : logs) {
            string time = dateString();

            if (!quietMod) cout << "Date: " << time << " Log: " << log << endl;
            *file << "Date: " << time << " Log: " << log << endl;
        }
    }

    /**
     * addititional functionality
     */
    string bytesToString(vector<uint8_t> logs) {
        stringstream ss;
        ss << hex << setfill('0');
        for (const auto& byte : logs) {
            ss << setw(2) << static_cast<int>(byte);
        }
        return ss.str();
    }
};

#endif