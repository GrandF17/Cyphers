#ifndef KUZNECHIK_LIBS_INCLUDE_LOGGER_H_INCLUDED
#define KUZNECHIK_LIBS_INCLUDE_LOGGER_H_INCLUDED

#include <fstream>
#include <vector>

using namespace std;

class Logger {
   public:
    Logger(const string fileName, bool _quietMod);
    ~Logger();

    void log(const vector<string>& logs);

   private:
    ofstream* file;  // file where logs will be written
    bool quietMod;   // true - logs to console, else - not
    
    static inline string bytesToString(const vector<uint8_t>& logs);
};

#endif