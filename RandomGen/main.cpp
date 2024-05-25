#include <math.h>

#include "./classes/key.cpp"
#include "./libs/random.h"
#include "./tests/tests.cpp"
#include "./utils/argvAnalizer.cpp"
#include "./utils/files.cpp"

using namespace std;

int main(int argc, char** argv) {
    ProgramParams params = extractProgramParams(argc, argv);
    Random rand = Random(params.key->param, params.offset->param);

    double balance = -1;
    vector<uint8_t> randomText;
    size_t randomLen = params.seqLen->param;

    do {
        randomText = rand.genSequence(randomLen);
        balance = rand.balance(randomText);
    } while (abs(balance - 0.5) > BALANCE_THRESHOLD);

    // printVectorAsHex(randomText);
    save(randomText, "random.dat");
    params.~ProgramParams();
    rand.~Random();

    // uint64_t num = 345734897;
    // vector<uint8_t> vec = rand.u64ToVec(num);
    // printVectorAsHex(vec);
    // uint64_t newNum = rand.vecToU64(vec);
    // cout << num << " and " << newNum << endl;

    return 0;
}
