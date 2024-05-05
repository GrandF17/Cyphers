#include <atomic>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "../classes/logger.cpp"
#include "../libs/md5.h"

#ifndef INTEGRITY_CONTROL
#define INTEGRITY_CONTROL

using namespace std;

class IntegrityControl {
   public:
    IntegrityControl() : closeAllThreads(false) {}
    ~IntegrityControl() {
        closeAllThreads = true;
        if (checksumThread.joinable()) {
            checksumThread.join();
        }
    }

    void startChecksumChecker(char** argv, class Logger* logger) {
        checksumThread = thread(&IntegrityControl::checksumChecker, this, argv, logger);;
    }

    void createReferenceFile(char** argv, class Logger* logger) {
        string hashtext = md5(fileContentToString(argv[0]));

        ofstream hashfile("./checksum.dat", ios::binary);
        hashfile << hashtext;
        hashfile.close();
        logger->log({"Executable file checksum was written to ./checksum.dat!"});
    }

   private:
    thread checksumThread;
    atomic<bool> closeAllThreads;

    string fileContentToString(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Failed to open file: " << filename << endl;
            return "";
        }

        string content;
        string line;
        while (getline(file, line)) {
            content += line;  // Add newline character if needed
        }

        file.close();
        return content;
    }

    void checksumChecker(char** argv, class Logger* logger) {
        while (!closeAllThreads) {
            inner(argv, logger);
            this_thread::sleep_for(chrono::seconds(3));  // 3 sec delay
        }
    }

    void inner(char** argv, class Logger* logger) {
        string reftext = fileContentToString("./checksum.dat");
        string hashtext = md5(fileContentToString(argv[0]));

        /**
            * 1 - means that second string stands right from first string after sort
            * 0 - both second and first strings appears on equal places after sort
            * -1 - means that second string stands left from first string after sort
            *
            * simplifying:
            * a < b
            * a == a
            * c > b
            */
        if (reftext.compare(hashtext) != 0) {
            logger->log({"Executable integrity is broken, aborting!"});
            exit(1);
        }

        logger->log({"Executable integrity is checked!"});
    }
};

#endif