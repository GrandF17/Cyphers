#include <cstring>
#include <fstream>
#include <algorithm>
#include <cstdlib>  // rand() srand()
#include <ctime>    // time()
#include <cstdint>  // uint64_t

#include "../classes/key.cpp"
#include "../interfaces/constants.h"

#include "magma.h"
#include "random.h"

using namespace std;

vector<uint8_t> Random::XOR(vector<uint8_t> a, vector<uint8_t> b) {
    for (size_t i = 0; i < a.size(); i++) a[i] ^= b[i];
    return a;
}

vector<uint8_t> Random::u64ToVec(uint64_t a) {
    vector<uint8_t> converted;

    for (int i = 0; i < 8; ++i) {
        converted.push_back(static_cast<uint8_t>(a & 0xFF));
        a >>= 8;
    }

    return converted;
}

uint64_t Random::genPRN() {
	static bool initialized = false;
    if (!initialized) {
        srand(static_cast<unsigned int>(time(0)));
        initialized = true;
    }

	uint64_t randVal = 0;
    for (int i = 0; i < 8; ++i) {
        uint64_t byte = static_cast<uint64_t>(rand() & 0xFF); // Создаем случайный байт
        randVal |= (byte << (i * 8)); // Объединяем байты в 64-битное число
    }

    return randVal;
}

vector<uint8_t> Random::genGamma() {
    srand(static_cast<unsigned int>(time(0)));
    vector<uint8_t> gamma;

    for (int i = 0; i < 32; ++i)
        gamma.push_back(static_cast<uint8_t>(rand() & 0xFF));

    return gamma;
}

Random::Random() : seed(Key("./1048576.key", 0, 32)) {}
Random::Random(const size_t& offset) : seed(Key("./1048576.key", 0, 32)) {}
Random::Random(const string& keyFile, const size_t& offset) : seed(Key(keyFile, offset, 32)) {}

Random::~Random() {
    seed.~Key();
}

uint64_t Random::genBlock(const vector<uint8_t>& key) {
	MagmaLC magma;
	return magma.encrypt(genPRN(), key);
}

vector<uint8_t> Random::genSequence(const size_t len) {
	MagmaLC magma;
	Key gamma = Key(genGamma());

	size_t seqPosition = 0, iterator = 0, keyPosition;
	vector<uint8_t> rand(8);
	
	vector<uint8_t> seq(len);

	while (seqPosition < len) {
		// last 4 iterations of each CHANGE_KEY_BLOCK_SIZE generate gamma for new key
		if ((keyPosition = (iterator + 4) % CHANGE_KEY_BLOCK_SIZE) < 4) {
			rand = u64ToVec(genBlock(seed.masterKey));
			copy(rand.begin(), rand.end(), gamma.masterKey.begin() + keyPosition * 8);

			if (keyPosition == 3) { // all 32 bytes are generated
				seed = Key(XOR(seed.masterKey, gamma.masterKey));
				magma.changeKey(seed.masterKey);
			}
		} 
		else {
			rand = u64ToVec(genBlock(seed.masterKey));
			size_t size = seqPosition + 8 > len ? len-seqPosition : 8;
			copy(rand.begin(), rand.end(), seq.begin() + seqPosition);
			seqPosition += size;
		}
		iterator++;
	}

	return seq;
}

double Random::balance(const vector<uint8_t>& seq) {
	uint64_t weight = 0;
	for (size_t i = 0; i < seq.size(); i++) weight += WEIGHTS[seq[i]];
	return (double) weight / seq.size() / 8;
}

