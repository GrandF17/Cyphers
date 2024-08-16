#ifndef KUZNECHIK_LIBS_INCLUDE_KUZNECHIK_H_INCLUDED
#define KUZNECHIK_LIBS_INCLUDE_KUZNECHIK_H_INCLUDED

#include <cstring>
#include <iostream>
#include <vector>

using namespace std;

class Kuznechik {
   public:
    Kuznechik();
    ~Kuznechik();

    enum Table {LFunc, LFuncInv};

    /**
     * @param static to call this functiion without class initialization.
     * Constructor creates transition tables that increase speed of Kuznechik encrypt/decrypt functions.
     */
    static vector<uint8_t> encrypt(vector<uint8_t> const& block, vector<vector<uint8_t>> const& keys);
    static vector<uint8_t> decrypt(vector<uint8_t> const& block, vector<vector<uint8_t>> const& keys);
    static vector<vector<uint8_t>> feistelTransform(
        vector<uint8_t> const& lKey,
        vector<uint8_t> const& rKey,
        vector<uint8_t> const& iterator);

   private:
    vector<vector<vector<uint8_t>>> tableL;
    vector<vector<vector<uint8_t>>> tableLInv;

    // to make var "tableL" and "tableLInv" accessible from static inline functions
    static Kuznechik& instance() {
        static Kuznechik instance;
        return instance;
    }

    /**
     * using vars' COPY --> func changes their value
     */
    static inline uint8_t gfMul(uint8_t a, uint8_t b);

    static inline vector<uint8_t> xFunc(vector<uint8_t> const& a, vector<uint8_t> const& b);

    static inline vector<uint8_t> sFunc(vector<uint8_t> const& a);
    static inline vector<uint8_t> sFuncInv(vector<uint8_t> const& a);

    static inline vector<uint8_t> rFunc(vector<uint8_t> const& a);
    static inline vector<uint8_t> rFuncInv(vector<uint8_t> const& a);

    static inline vector<uint8_t> lFunc(vector<uint8_t> const& a);
    static inline vector<uint8_t> lFuncInv(vector<uint8_t> const& a);

    // boost for L and S Func-s
    static inline vector<vector<vector<uint8_t>>> genTableFor(Table const& currTable);
    static inline vector<uint8_t> SLFunc(vector<uint8_t> const& a);
    static inline vector<uint8_t> LSFunc(vector<uint8_t> const& a);
    static inline vector<uint8_t> mapping(
        vector<uint8_t> const& a,
        vector<vector<vector<uint8_t>>> const& tableRef);
};

/**
 * When we encrypt file need to define IV,
 * BUT when we decrypt, data contains IV as first 16 bytes
 * so there is no need to pass IV inside function
 */
vector<uint8_t> encryptOFB(
    vector<uint8_t> const& data,
    vector<vector<uint8_t>> const& keys,
    vector<uint8_t> const& IV);
vector<uint8_t> decryptOFB(vector<uint8_t> const& data, vector<vector<uint8_t>> const& keys);

vector<uint8_t> encryptCBC(
    vector<uint8_t> const& data,
    vector<vector<uint8_t>> const& keys,
    vector<uint8_t> const& IV);
vector<uint8_t> decryptCBC(vector<uint8_t> const& data, vector<vector<uint8_t>> const& keys);

#endif