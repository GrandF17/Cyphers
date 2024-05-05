#include <cstdint>
#include <iomanip>
#include <iostream>

#include "./classes/key.cpp"
#include "./classes/integrity.cpp"
#include "./libs/streebog.h"
#include "./utils/argvAnalizer.cpp"
#include "./utils/mac.cpp"
#include "./utils/vLog.cpp"
#include "./tests/tests.cpp"

using namespace std;

#define LABEL_LEN 4
#define SEED_LEN 8

vector<uint8_t> kdp(const ProgramParams& params) {
    Key key = Key(params.key->param, params.offset->param, params.keyLen->param);
    vector<uint8_t> message(4 + LABEL_LEN + SEED_LEN, 0x00);

    message[0] = 0x01;

    vector<uint8_t> label = { 0x01, 0x02, 0x03, 0x04 };
    vector<uint8_t> seed = { 0x11, 0x22, 0x44, 0x88, 0xFF, 0x04, 0x08, 0x0F };

    memcpy(message.data() + 1, label.data(), LABEL_LEN);
    message[LABEL_LEN + 1] = 0;

    memcpy(message.data() + LABEL_LEN + 2, seed.data(), SEED_LEN);
    message[2 + LABEL_LEN + SEED_LEN] = 0x01;
    message[3 + LABEL_LEN + SEED_LEN] = 0;

    vector<uint8_t> derivativeKey = NMAC(*key.masterKey, message);
    key.~Key();

    return derivativeKey;
}

int main(int argc, char** argv) {
    ProgramParams params = extractProgramParams(argc, argv);
    Logger logger = Logger(params.logFile->param, false);
    IntegrityControl watcher;

    if (params.countChecksum->param) watcher.createReferenceFile(argv, &logger);
    watcher.startChecksumChecker(argv, &logger);

    if (params.mode->param == 0) {
        logger.log({"Running key diversification  protocol..."});
        logger.log({{"Done! Received vector: "}, {"0x" + logger.bytesToString(kdp(params))}});
    }
    
    if (params.mode->param == 1) {
        logger.log({"Running tests..."});
        bool allPassed = test();
        logger.log({allPassed ? "All tests passed!" : "Some tests failed!"});
    }

    watcher.~IntegrityControl();
    return 0;
}
