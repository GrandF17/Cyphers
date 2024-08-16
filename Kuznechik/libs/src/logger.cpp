#ifndef KUZNECHIK_LIBS_SRC_LOGGER_CPP_INCLUDED
#define KUZNECHIK_LIBS_SRC_LOGGER_CPP_INCLUDED

#include "../include/logger.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

#include "../../utils/time.cpp"

using namespace std;

void Logger::log(const vector<string>& logs) {
    if (!file->is_open())
        throw ios_base::failure("File is not open for writing.");

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
inline string Logger::bytesToString(vector<uint8_t> const& logs) {
    stringstream ss;
    ss << hex << setfill('0');
    for (const auto& byte : logs) {
        ss << setw(2) << static_cast<int>(byte);
    }
    return ss.str();
}

//////////////////////////////

/**
 * @param fileName default value is string "NoFileSelected" to
 */
Logger::Logger(const string fileName = "./KuznechikLogs.dat", bool _quietMod = true) {
    quietMod = _quietMod;
    file = new ofstream(fileName, ios::out | ios::app);
    if (file->fail())
        throw ios_base::failure(strerror(errno));
    file->exceptions(file->exceptions() | ios::failbit | ifstream::badbit);
}

Logger::~Logger() {
    if (file) {
        file->close();
        delete file;
        file = nullptr;
    }
}

#endif