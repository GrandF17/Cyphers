
#ifndef KUZNECHIK_LIBS_INCLUDE_KEY_CPP_INCLUDED
#define KUZNECHIK_LIBS_INCLUDE_KEY_CPP_INCLUDED

#include <cstdint>
#include <cstring>
#include <fstream>
#include <iomanip>  // unique_ptr, make_unique
#include <iostream>
#include <vector>

#include "../../constants.h"
#include "../include/kuznechik.h"

using namespace std;

/**
 * @details KeyGen class
 * @public only one function: generateRoundKeys(...)
 */
class Key {
   public:
    /**
     * consists of two halfs (left and right)
     * so here we got 3D array instead of 2D
     */
    const unique_ptr<vector<vector<uint8_t>>> masterKey;
    const unique_ptr<vector<vector<uint8_t>>> keys;

    Key(const string fileName = "./1048576.key", const size_t offset = 0);
    ~Key();

    static vector<vector<uint8_t>> createTestKey(vector<uint8_t> const& lKey, vector<uint8_t> const& rKey);

   private:
    static inline vector<vector<uint8_t>> constants();
    static inline vector<uint8_t> readKey(const string& filename, const size_t& fileShift = 0);
    static inline vector<vector<uint8_t>> expandKey(vector<uint8_t> const& lKey, vector<uint8_t> const& rKey);
    static inline vector<vector<uint8_t>> leftAndRight(const string& filename, const size_t& fileShift);
};

#endif