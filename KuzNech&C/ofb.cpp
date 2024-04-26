#include <cstdint>
#include <iomanip>
#include <iostream>
#include <regex>
#include <vector>

#include "./classes/keyGen.cpp"
#include "./classes/logger.cpp"
#include "./interfaces/interfaces.h"
#include "./utils/argvAnalizer.cpp"
#include "./utils/integrityControl.cpp"
#include "./utils/kuznechik.cpp"

using namespace std;

static vector<uint8_t> block =
    {0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x00, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11};

int main(int argc, char** argv) {
    /**
     * main class that copntains all arguments
     */
    ProgramParams params = extractProgramParams(argc, argv);
    KeyGen keyGen;
    Logger logger = Logger(params.logFile->param, false);

    createReferenceFile(argv, &logger);
    /**
     * to store logs in main and then write them down to file
     */
    vector<string> logs;

    /**
     * writing logs to vector (1 step)
     */
    {
        logs.push_back("Plain text: ");
        logs.push_back(logger.bytesToString(block));
    }

    vector<vector<uint8_t>> keys = keyGen.generateRoundKeys(params.key->param, params.offset->param);

    /**
     * writing logs to vector (2 step)
     */
    {
        logs.push_back("Master key: ");
        for (auto key : keys) logs.push_back(logger.bytesToString(key));
    }

    vector<uint8_t> encriptedBlock = encript(block, keys);

    /**
     * writing logs to vector (3 step)
     */
    {
        logs.push_back("Ecripted text: ");
        logs.push_back(logger.bytesToString(encriptedBlock));
    }

    vector<uint8_t> decriptedBlock = decript(encriptedBlock, keys);

    /**
     * writing logs to vector (3 step)
     */
    {
        logs.push_back("Decripted text: ");
        logs.push_back(logger.bytesToString(decriptedBlock));
    }

    // output of logs
    logger.log(logs);

    return 0;
}
