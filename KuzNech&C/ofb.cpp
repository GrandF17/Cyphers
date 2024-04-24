#include <cstdint>
#include <iomanip>
#include <iostream>
#include <vector>

#include "./classes/keyGen.cpp"
#include "./classes/logger.cpp"
#include "kuznechik.cpp"

using namespace std;

static vector<uint8_t> block =
    {0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x00, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11};

int main() {
    KeyGen keyGen;
    vector<string> logs;
    Logger logger = Logger("logs.txt", false);
    string fileName = "./1048576.key";

    /**
     * writing logs to vector (1 step)
     */
    {
        logs.push_back("Plain text: ");
        logs.push_back(logger.bytesToString(block));
    }

    vector<vector<uint8_t>> keys = keyGen.generateRoundKeys(fileName, 100);

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
