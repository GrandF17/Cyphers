#include <algorithm>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <random>
#include <regex>
#include <vector>

#include "./libs/include/integrity.h"
#include "./libs/include/key.h"
#include "./libs/include/kuznechik.h"
#include "./libs/include/logger.h"
#include "./libs/include/md5.h"
#include "./libs/include/params.h"
#include "./tests/tests.cpp"
#include "./utils/files.cpp"
#include "./utils/time.cpp"

using namespace std;

enum KuzMod {
    ENCRIPT,
    DECRIPT,
    TEST,
    TOTAL_MODS
};

/**
 * function to generate IV based on the results
 * of uint8_t based on randomness
 */
vector<uint8_t> generateIV() {
    random_device rd;
    mt19937 gen(rd());

    vector<uint8_t> iv(16);  // IV with (len ~ 16 bytes ~ 128 bits)

    for (int i = 0; i < 16; ++i)
        iv[i] = static_cast<uint8_t>(gen() % 256);

    return iv;
}

int main(int argc, char** argv) {
    /**
     * main class that copntains all arguments
     */
    ProgramParams params(argc, argv);
    Logger logger = Logger(params.logFile->param, false);
    IntegrityControl watcher;

    /**
     * recount checksum only if user set special parameter in argv
     * and exit program (use only if YOU CHANGED somthing in final program)
     */
    if (params.recountCheksum->param) {
        watcher.createReferenceFile(argv, &logger);
        exit(0);
    }

    /**
     * importaint check of executable file (main.exe)
     */
    watcher.verifyOnStart(argv, &logger);
    watcher.startChecksumWatcher(argv, &logger);

    if (params.mode->param == KuzMod::ENCRIPT) {
        logger.log({"File " + params.file->param + " encrypting..."});
        vector<uint8_t> data = read(params.file->param, &logger);

        Key key(params.key->param, params.offset->param); // create key
        vector<uint8_t> encrypted = encryptOFB(data, *key.keys, generateIV());
        key.~Key(); // instantly annihilating Key

        string outpuFile = md5(dateString()).append(".ENC");
        save(encrypted, outpuFile, &logger);

        logger.log({"Ecrypted to: " + outpuFile});
    }

    if (params.mode->param == KuzMod::DECRIPT) {
        logger.log({"File " + params.file->param + " decrypting..."});
        vector<uint8_t> data = read(params.file->param, &logger);

        Key key(params.key->param, params.offset->param); // create key
        vector<uint8_t> decrypted = decryptOFB(data, *key.keys);
        key.~Key(); // instantly annihilating Key

        string outpuFile = md5(dateString()).append(".DEC");
        save(decrypted, outpuFile, &logger);

        logger.log({"Decrypted to: " + outpuFile});
    }

    if (params.mode->param == KuzMod::TEST) {
        logger.log({"Running tests..."});
        bool allPassed = test();
        logger.log({allPassed ? "All tests passed!" : "Some tests failed!"});
    }

    watcher.~IntegrityControl();
    logger.~Logger();
    return 0;
}
