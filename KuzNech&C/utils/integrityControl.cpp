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
        content += line;  // Add newline character if needed
    }

    file.close();
    return content;
}

void createReferenceFile(char** argv, class Logger* logger) {
    string hashtext = md5(fileContentToString(argv[0]));

    ofstream hashfile("./checksum.dat", ios::binary);
    hashfile << hashtext;
    hashfile.close();
    logger->log({"Executable file checksum was written to ./checksum.dat!"});
}

void checkExecutableHash(char** argv, class Logger* logger) {
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
        throw runtime_error("Executable integrity is broken, aborting!");
        exit(1);
    }

    logger->log({"Executable integrity is checked!"});
}

#endif