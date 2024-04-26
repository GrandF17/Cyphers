#include "../libraries/md5.h"

#ifndef KUZ_INTEGRITY_CONTROL
#define KUZ_INTEGRITY_CONTROL

using namespace std;

string fileContentToString(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Failed to open file: " << filename << endl;
        return "";
    }

    string content;
    string line;
    while (getline(file, line)) {
        content += line + "\n";  // Add newline character if needed
    }

    file.close();
    return content;
}

void createReferenceFile(char** argv, class Logger* logger) {
    string hashtext = md5(fileContentToString(argv[0]));

    ofstream hashfile("./ref_hash", ios::binary);
    hashfile << hashtext;
    hashfile.close();
    logger->log({"Executable file checksum was written to ./ref_hash!"});
}

void checkExecutableHash(char** argv, class Logger* logger) {
    string reftext = md5(fileContentToString("./ref_hash"));
    string hashtext = md5(fileContentToString(argv[0]));

    for (size_t i = 0; i < reftext.size(); i++) {
        if (reftext[i] != hashtext[i]) {
            logger->log({"Executable integrity is broken, aborting!"});
            throw std::runtime_error("Executable integrity is broken, aborting!");
            exit(1);
        }
    }

    logger->log({"Executable integrity is checked!"});
}

#endif