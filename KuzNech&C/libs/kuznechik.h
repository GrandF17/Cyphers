#ifndef KUZNECHIK_H
#define KUZNECHIK_H

#include <cstring>
#include <iostream>
#include <vector>

using namespace std;

class Kuznechik {
   public:
    Kuznechik();

    vector<uint8_t> Kuznechik::encrypt(const vector<uint8_t>& block, const vector<vector<uint8_t>>& keys);
    vector<uint8_t> Kuznechik::decrypt(const vector<uint8_t>& block, const vector<vector<uint8_t>>& keys);
    vector<vector<uint8_t>> Kuznechik::feistelTransform(
        const vector<uint8_t>& lKey,
        const vector<uint8_t>& rKey,
        const vector<uint8_t>& iterator);

   private:
    /**
     * using only COPY of digits, because the function works with them
     */
    static inline uint8_t Kuznechik::gfMul(uint8_t a, uint8_t b);

    static inline vector<uint8_t> Kuznechik::xFunc(const vector<uint8_t>& a, const vector<uint8_t>& b);

    static inline vector<uint8_t> Kuznechik::sFunc(const vector<uint8_t>& a);
    static inline vector<uint8_t> Kuznechik::sFuncInv(const vector<uint8_t>& a);

    static inline vector<uint8_t> Kuznechik::rFunc(const vector<uint8_t>& a);
    static inline vector<uint8_t> Kuznechik::rFuncInv(const vector<uint8_t>& a);

    static inline vector<uint8_t> Kuznechik::lFunc(const vector<uint8_t>& a);
    static inline vector<uint8_t> Kuznechik::lFuncInv(const vector<uint8_t>& a);
};

vector<uint8_t> encryptOFB(
    const vector<uint8_t>& data,
    const vector<vector<uint8_t>>& keys,
    const vector<uint8_t>& IV);
vector<uint8_t> decryptOFB(
    const vector<uint8_t>& data,
    const vector<vector<uint8_t>>& keys,
    const vector<uint8_t>& IV);
#endif