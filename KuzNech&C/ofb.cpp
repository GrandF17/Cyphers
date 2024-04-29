#include <algorithm>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <regex>
#include <vector>

#include "./classes/key.cpp"
#include "./classes/logger.cpp"
#include "./interfaces/interfaces.h"
#include "./tests/tests.cpp"
#include "./utils/argvAnalizer.cpp"
#include "./utils/files.cpp"
#include "./utils/integrityControl.cpp"
#include "./utils/kuznechik.cpp"
#include "./utils/kuznechikModes.cpp"
#include "./utils/time.cpp"

using namespace std;

enum KuzMod {
    ENCRIPT,
    DECRIPT,
    TEST,
    TOTAL_MODS
};

int main(int argc, char** argv) {
    /**
     * main class that copntains all arguments
     */
    ProgramParams params = extractProgramParams(argc, argv);
    Logger logger = Logger(params.logFile->param, false);

    /**
     * recount checksum only if user set special parameter in argv
     * and exit program (use only if YOU CHANGED somthing in final program)
     */
    if (params.countChecksum->param) {
        createReferenceFile(argv, &logger);
        exit(0);
    }
    /**
     * importaint check of executable file (main.exe)
     */
    checkExecutableHash(argv, &logger);

    if (params.mode->param == KuzMod::ENCRIPT) {
        Key key(params.key->param, params.offset->param);
        vector<uint8_t> data = read(params.file->param, &logger);

        logger.log({"File " + params.file->param + " encrypting..."});

        ofstream ost(KUZ_CONST::DEFAULT_FILE_SIZE);
        ost << data.size();

        vector<uint8_t> encrypted = encryptKuznechik(data, *key.keys);

        string time = md5(dateString()).append(".ENC");
        string outpuFile = time;
        save(encrypted, outpuFile, 0, &logger);

        logger.log({"Ecrypted to: " + outpuFile});

        /**
         * annihilating Key
         */
        key.~Key();
    }

    if (params.mode->param == KuzMod::DECRIPT) {
        Key key(params.key->param, params.offset->param);
        vector<uint8_t> data = read(params.file->param, &logger);

        logger.log({"File " + params.file->param + " decrypting..."});

        size_t size;
        ifstream size_file(KUZ_CONST::DEFAULT_FILE_SIZE);
        size_file >> size;
        size_file.close();

        vector<uint8_t> decrypted = decryptKuznechik(data, *key.keys);

        string time = md5(dateString()).append(".DEC");
        string outpuFile = time;
        save(decrypted, outpuFile, size, &logger);

        logger.log({"Decrypted to: " + outpuFile});

        /**
         * annihilating Key
         */
        key.~Key();
    }

    if (params.mode->param == KuzMod::TEST) {
        logger.log({"Running tests..."});
        bool allPassed = test();
        logger.log({allPassed ? "All tests passed!" : "Some tests failed!"});
    }

    /**
     * annihilating Logger
     */
    logger.~Logger();

    return 0;
}
