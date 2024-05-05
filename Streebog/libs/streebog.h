#ifndef STREEBOG_H
#define STREEBOG_H

#include <cstring>
#include <iostream>
#include <vector>

using namespace std;

class Streebog {
   public:
    /**
     * constructor
     */
    Streebog();

    /**
     * @returns hash of byte sequence passed to func  (key len = 256)
     */
    vector<uint8_t> streebog256(const vector<uint8_t>& message);

    /**
     * @overload of streebog256(vector<uint8_t> message)
     */
    vector<uint8_t> streebog256(const string& message);

    /**
     * @returns hash of byte sequence passed to func (key len = 512)
     */
    vector<uint8_t> streebog512(const vector<uint8_t>& message);

    /**
     * @overload of streebog512(vector<uint8_t> message)
     */
    vector<uint8_t> streebog512(const string& message);

    static vector<uint8_t> encode(const string& str);    // str to bytes
    static string decode(const vector<uint8_t>& block);  // bytes to str

    /**
     * don't use link/const to original var cause number is changed in process
     * work with !COPY ONLY!
     */
    static vector<uint8_t> encode(uint64_t number);  // num to bytes

   private:
    // low level byte op with vectors
    static inline vector<uint8_t> XOR(vector<uint8_t> a, vector<uint8_t> b);
    static inline vector<uint8_t> ADD(vector<uint8_t> a, vector<uint8_t> b);

    // basic hash-function logic of Streebog
    static inline vector<uint8_t> S(const vector<uint8_t>& block);
    static inline vector<uint8_t> P(const vector<uint8_t>& block);
    /**
     * don't use link to original var cause of difficulty of realisation
     * work with !COPY ONLY!
     */
    static inline vector<uint8_t> L(vector<uint8_t> block);
    static inline vector<uint8_t> E(vector<uint8_t> K, const vector<uint8_t>& m);
    static inline vector<uint8_t> G(const vector<uint8_t>& N, const vector<uint8_t>& h, const vector<uint8_t>& m);

    static inline vector<uint8_t> keySchedule(vector<uint8_t> K, size_t i);
    static inline vector<uint8_t> countHash(
        const vector<uint8_t>& message,
        vector<uint8_t> h,
        vector<uint8_t> N,
        vector<uint8_t> S,
        vector<uint8_t> m);
};

enum KEY_LEN { k32 = 32,
               k64 = 64 };

string streebog(const string& str, KEY_LEN len = k32);
vector<uint8_t> streebog(const vector<uint8_t>& bytes, KEY_LEN len = k32);

#endif