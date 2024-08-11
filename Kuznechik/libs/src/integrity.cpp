
#ifndef KUZNECHIK_LIBS_SRC_INTEGRITY_CPP_INCLUDED
#define KUZNECHIK_LIBS_SRC_INTEGRITY_CPP_INCLUDED

#include "libs/include/integrity.h"

#include <atomic>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "libs/include/logger.h"
#include "libs/include/md5.h"

using namespace std;

IntegrityControl::IntegrityControl() : closeAllThreads(false) {}
IntegrityControl::~IntegrityControl() {
    closeAllThreads = true;
    if (checksumThread.joinable()) {
        checksumThread.join();
    }
}

/*
 * periodic verification in the stream (thread)
 */
void IntegrityControl::startChecksumChecker(char** argv, class Logger* logger) {
    checksumThread = thread(&IntegrityControl::checksumChecker, this, argv, logger);
}

/*
 * to verify chacksum on start of the program without time delay
 */
void IntegrityControl::verifyOnStart(char** argv, class Logger* logger) {
    verification(argv, logger);
}

/*
 * to recalc checksum in case program logic was changed
 */
void IntegrityControl::createReferenceFile(char** argv, class Logger* logger) {
    string hashtext = md5(fileContentToString(argv[0]));

    ofstream hashfile("./checksum.dat", ios::binary);
    hashfile << hashtext;
    hashfile.close();
    logger->log({"Executable file checksum was written to ./checksum.dat!"});
}

string IntegrityControl::fileContentToString(const string& filename) {
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

void IntegrityControl::checksumChecker(char** argv, class Logger* logger) {
    while (!closeAllThreads) {
        verification(argv, logger);
        this_thread::sleep_for(chrono::seconds(3));  // 3 sec delay
    }
}

void IntegrityControl::verification(char** argv, class Logger* logger) {
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

#endif