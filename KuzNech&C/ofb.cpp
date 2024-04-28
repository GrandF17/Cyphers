#include <algorithm>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <regex>
#include <vector>

#include "./classes/keyGen.cpp"
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
    KeyGen keyGen;
    Logger logger = Logger(params.logFile->param, false);

    /**
     * recount checksum only if user set parameter
     */
    if (params.countChecksum->param) createReferenceFile(argv, &logger);
    /**
     * importaint check of executable file (main.exe)
     */
    checkExecutableHash(argv, &logger);

    if (params.mode->param == KuzMod::ENCRIPT) {
        vector<string> logs;
        vector<uint8_t> data = read(params.file->param, &logger);
        vector<vector<uint8_t>> keys = keyGen.generateRoundKeys(params.key->param, params.offset->param);

        logs.push_back({"File " + params.file->param + " encrypting..."});

        ofstream ost(KUZ_CONST::DEFAULT_FILE_SIZE);
        ost << data.size();

        vector<uint8_t> encrypted = encryptKuznechik(data, keys);

        string time = md5(dateString()).append(".ENC");
        string outpuFile = time;
        save(encrypted, outpuFile, 0, &logger);

        logs.push_back({"Ecrypted to: " + outpuFile});
        logger.log(logs);
    }

    if (params.mode->param == KuzMod::DECRIPT) {
        vector<string> logs;
        vector<uint8_t> data = read(params.file->param, &logger);
        vector<vector<uint8_t>> keys = keyGen.generateRoundKeys(params.key->param, params.offset->param);

        logs.push_back({"File " + params.file->param + " decrypting..."});

        size_t size;
        ifstream size_file(KUZ_CONST::DEFAULT_FILE_SIZE);
        size_file >> size;
        size_file.close();

        vector<uint8_t> decrypted = decryptKuznechik(data, keys);

        string time = md5(dateString()).append(".DEC");
        string outpuFile = time;
        save(decrypted, outpuFile, size, &logger);

        logs.push_back({"Decrypted to: " + outpuFile});
        logger.log(logs);
    }

    if (params.mode->param == KuzMod::TEST) {
        logger.log({"Running tests..."});
        bool allPassed = test();
        logger.log({allPassed ? "All tests passed!" : "Some tests failed!"});
    }

    return 0;
}
