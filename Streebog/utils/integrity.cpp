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

class ChecksumChecker {
   public:
    ChecksumChecker() : closeAllThreads(false) {}

    void startChecksumChecker() {
        checksumThread = thread(&ChecksumChecker::checksumChecker, this);
    }

    void stopChecksumChecker() {
        closeAllThreads = true;
        if (checksumThread.joinable()) {
            checksumThread.join();
        }
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
        delete &file;

        return content;
    }

    void checksumChecker(char** argv, class Logger* logger) {
        while (!closeAllThreads) {
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
            this_thread::sleep_for(chrono::seconds(3));  // 3 sec delay
        }
    }
};

#endif