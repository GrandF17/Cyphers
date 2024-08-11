
#ifndef KUZNECHIK_LIBS_INCLUDE_INTEGRITY_H_INCLUDED
#define KUZNECHIK_LIBS_INCLUDE_INTEGRITY_H_INCLUDED

#include <atomic>
#include <fstream>
#include <string>
#include <thread>
#include <vector>

using namespace std;

class IntegrityControl {
   public:
    IntegrityControl();
    ~IntegrityControl();

    void startChecksumChecker(char** argv, class Logger* logger);
    void verifyOnStart(char** argv, class Logger* logger);
    void createReferenceFile(char** argv, class Logger* logger);

   private:
    thread checksumThread;
    atomic<bool> closeAllThreads;

    string fileContentToString(const string& filename);

    void checksumChecker(char** argv, class Logger* logger);
    void verification(char** argv, class Logger* logger);
};

#endif