#include <string>
#include <vector>

#include "../utils/kuznechik.cpp"

#ifndef KUZ_MODES
#define KUZ_MODES

using namespace std;

vector<uint8_t> encryptKuznechik(vector<uint8_t> &data, vector<vector<uint8_t>> keys) {
    vector<uint8_t> result;
    for (int i = 0; i < data.size(); i += KUZ_CONST::BLOCK_SIZE) {
        vector<uint8_t> tmp(data.begin() + i, min(data.begin() + i + KUZ_CONST::BLOCK_SIZE, data.end()));
        if (tmp.size() < KUZ_CONST::BLOCK_SIZE) tmp.resize(KUZ_CONST::BLOCK_SIZE, 0);

        vector<uint8_t> blockResult = encrypt(tmp, keys);
        result.insert(result.end(), blockResult.begin(), blockResult.end());
    }
    return result;
}

vector<uint8_t> decryptKuznechik(vector<uint8_t> &data, vector<vector<uint8_t>> keys) {
    vector<uint8_t> result;
    for (int i = 0; i < data.size(); i += KUZ_CONST::BLOCK_SIZE) {
        vector<uint8_t> tmp(data.begin() + i, min(data.begin() + i + KUZ_CONST::BLOCK_SIZE, data.end()));
        if (tmp.size() < KUZ_CONST::BLOCK_SIZE) tmp.resize(KUZ_CONST::BLOCK_SIZE, 0);

        vector<uint8_t> blockResult = decrypt(tmp, keys);
        result.insert(result.end(), blockResult.begin(), blockResult.end());
    }
    return result;
}

#endif