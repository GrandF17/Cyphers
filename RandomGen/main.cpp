#include <cstdint>
#include <iomanip>
#include <iostream>

#include "./classes/key.cpp"
#include "./utils/argvAnalizer.cpp"
#include "./tests/tests.cpp"
#include "./libs/random.h"
#include "./utils/files.cpp"

using namespace std;

void printVectorAsHex(const vector<uint8_t>& vec) {
    for (const auto& byte : vec) {
        cout << hex << setw(2) << setfill('0') << static_cast<int>(byte) << " ";
    }
    cout << dec << endl;
}


int main(int argc, char** argv) {
    ProgramParams params = extractProgramParams(argc, argv);
    Random rand = Random(params.key->param, params.offset->param);

    double balance = -1;
    vector<uint8_t> randomText;
    size_t randomLen  = params.seqLen->param;

    do {
        randomText = rand.genSequence(randomLen);
        balance = rand.balance(randomText);
    } while (abs(balance - 0.5) > BALANCE_THRESHOLD);
        
    printVectorAsHex(randomText);
    save(randomText, "random.dat");
    params.~ProgramParams();
    rand.~Random();

    return 0;
}
