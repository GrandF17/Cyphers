#ifndef KEY_GEN
#define KEY_GEN

#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

#include "../interfaces/constants.h"

using namespace std;

/**
 * @details KeyGen functionally truncated class!!!
 */
class Key {
   public:
    /**
     * consists of two halfs (left and right)
     */
    const unique_ptr<vector<uint8_t>> masterKey;
    const unique_ptr<size_t> keyLen;

    /**
     * @param fileName default value is string "NoFileSelected" to
     */
    Key(string fileName = "./1048576.key", size_t offset = 0, size_t _keyLen = 32)
        : masterKey(make_unique<vector<uint8_t>>(genKey(fileName, offset, _keyLen))),
          keyLen(make_unique<size_t>(_keyLen)) {}

    ~Key() = default;

   private:
    /**
     * reading file, that contains bit sequence, to create key
     */
    vector<uint8_t> genKey(const string& filename, size_t fileShift, size_t _keyLen) {
        ifstream file(filename, ios::binary);
        if (!file.is_open()) {
            cerr << "Error opening file: " << filename << endl;
            exit(1);
        }

        // move the reading pointer to position eq fileShift
        file.seekg(fileShift, ios::beg);

        vector<uint8_t> masteKey(_keyLen);
        for (size_t i = 0; i < _keyLen; i++) {
            uint8_t value = 0;
            for (int j = 0; j < 8; j++) {
                char bit;
                file.get(bit);

                if (file.eof()) {
                    cerr << "Error: End of file reached prematurely." << endl;
                    exit(1);
                }

                /**
                 * convert the character '0' or '1' to the numeric values 0 or 1.
                 */
                if (bit == '0' || bit == '1') {
                    value |= (bit - '0') << (7 - j);  // shift bits left
                } else {
                    cerr << "Error: Invalid bit encountered." << endl;
                    exit(1);
                }
            }

            masteKey[i] = value;
        }

        return masteKey;
    }
};

#endif