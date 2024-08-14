#ifndef KUZNECHIK_LIBS_INCLUDE_KUZNECHIK_H_INCLUDED
#define KUZNECHIK_LIBS_INCLUDE_KUZNECHIK_H_INCLUDED

#include <cstring>
#include <iostream>
#include <vector>

using namespace std;

class Kuznechik {
   public:
    Kuznechik();

    /**
     * @param static to call this functiion without class initialization
     */
    static vector<uint8_t> encrypt(const vector<uint8_t>& block, const vector<vector<uint8_t>>& keys);
    static vector<uint8_t> decrypt(const vector<uint8_t>& block, const vector<vector<uint8_t>>& keys);

    static vector<vector<uint8_t>> feistelTransform(
        const vector<uint8_t>& lKey,
        const vector<uint8_t>& rKey,
        const vector<uint8_t>& iterator);

   private:
    vector<vector<vector<uint8_t>>> table;

    // to make var "table" accessible from static inline functions
    static Kuznechik& instance() {
        static Kuznechik instance;
        return instance;
    }
    
    /**
     * using vars' COPY --> func changes their value
     */
    static inline uint8_t gfMul(uint8_t a, uint8_t b);

    static inline vector<uint8_t> xFunc(const vector<uint8_t>& a, const vector<uint8_t>& b);

    static inline vector<uint8_t> sFunc(const vector<uint8_t>& a);
    static inline vector<uint8_t> sFuncInv(const vector<uint8_t>& a);

    static inline vector<uint8_t> rFunc(const vector<uint8_t>& a);
    static inline vector<uint8_t> rFuncInv(const vector<uint8_t>& a);

    static inline vector<uint8_t> lFunc(const vector<uint8_t>& a);
    static inline vector<uint8_t> lFuncInv(const vector<uint8_t>& a);

    // boost for L Func
    static inline vector<vector<vector<uint8_t>>> genLFuncTable();
    static inline vector<uint8_t> SLFunc(const vector<uint8_t>& a);
};

/**
 * When we encrypt file need to define IV,
 * BUT when we decrypt, data contains IV as first 16 bytes
 * so there is no need to pass IV inside function
 */
vector<uint8_t> encryptOFB(
    const vector<uint8_t>& data,
    const vector<vector<uint8_t>>& keys,
    const vector<uint8_t>& IV);
vector<uint8_t> decryptOFB(const vector<uint8_t>& data, const vector<vector<uint8_t>>& keys);

vector<uint8_t> encryptCBC(
    const vector<uint8_t>& data,
    const vector<vector<uint8_t>>& keys,
    const vector<uint8_t>& IV);
vector<uint8_t> decryptCBC(const vector<uint8_t>& data, const vector<vector<uint8_t>>& keys);

#endif