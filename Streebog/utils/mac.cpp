#ifndef MAC
#define MAC

#include <cstdint>
#include <iomanip>
#include <vector>

#include "../libs/streebog.h"
#include "../utils/bytes.cpp"

using namespace std;

#define C_in 0x36   // 00110110
#define C_out 0x5c  // 01011100

/**
 * NMAC_n(X) = Hash_n[(K XOR C_out) P Hash_n((K XOR C_in) P X)]
 * K - key and it is defined
 * X - text
 * n - key length (KEY_LEN)
 */
vector<uint8_t> HMAC(const vector<uint8_t>& masterKey, const vector<uint8_t>& message) {
    /**
     * define var with key len forced to KEY_LEN enum type
     * for code simplicity / make code better looking
     */
    size_t kLen = KEY_LEN(masterKey.size());
    if (kLen != k32 && kLen != k64)
        throw runtime_error("Key should be 256/512 bits.");

    /**
     * in/out variables
     */
    vector<uint8_t> ipad(kLen, 0x00), opad(kLen, 0x00);
    ipad[kLen - 1] = C_in;
    opad[kLen - 1] = C_out;

    // inner = (key256/512 ^ ipad) | message
    vector<uint8_t> inner(kLen + message.size());
    vector<uint8_t> xoredIpad = XOR(masterKey, ipad);
    copy(xoredIpad.begin(), xoredIpad.end(), inner.begin());
    copy(message.begin(), message.end(), inner.begin() + kLen);

    vector<uint8_t> hashedInner = streebog(inner, KEY_LEN(kLen));  // !hash len variates from 32 to 64 bytes!

    // outter = (key256/512 ^ opad) | hashedInner
    vector<uint8_t> outer(kLen + hashedInner.size());
    vector<uint8_t> xoredOpad = XOR(masterKey, opad);
    copy(xoredOpad.begin(), xoredOpad.end(), outer.begin());
    copy(hashedInner.begin(), hashedInner.end(), outer.begin() + kLen);

    vector<uint8_t> hashedOuter = streebog(outer, KEY_LEN(kLen));  // !hash len variates from 32 to 64 bytes!

    return hashedOuter;
}

/**
 * NMAC_256(X) = Hash_256[(K XOR C_out) P Hash_512((K XOR C_in) P X)]
 * K - key and it is defined
 * X - text
 */
vector<uint8_t> NMAC(const vector<uint8_t>& masterKey, const vector<uint8_t>& message) {
    /**
     * define var with key len forced to KEY_LEN enum type
     * for code simplicity / make code better looking
     */
    size_t kLen = KEY_LEN(masterKey.size());
    if (kLen != k32 && kLen != k64)
        throw runtime_error("Key should be 256/512 bits.");

    /**
     * in/out variables
     */
    vector<uint8_t> ipad(kLen, 0x00), opad(kLen, 0x00);
    ipad[kLen - 1] = C_in;
    opad[kLen - 1] = C_out;

    // inner = (key256/512 ^ ipad) | message
    vector<uint8_t> inner(kLen + message.size());
    vector<uint8_t> xoredIpad = XOR(masterKey, ipad);
    copy(xoredIpad.begin(), xoredIpad.end(), inner.begin());
    copy(message.begin(), message.end(), inner.begin() + kLen);

    vector<uint8_t> hashedInner = streebog(inner, k64);  // !hash len ONLY 512 according to description!

    // outter = (key256/512 ^ opad) | hashedInner
    vector<uint8_t> outer(kLen + hashedInner.size());
    vector<uint8_t> xoredOpad = XOR(masterKey, opad);
    copy(xoredOpad.begin(), xoredOpad.end(), outer.begin());
    copy(hashedInner.begin(), hashedInner.end(), outer.begin() + kLen);

    vector<uint8_t> hashedOuter = streebog(outer, k32);  // !hash len ONLY 256 according to description!

    return hashedOuter;
}

#endif