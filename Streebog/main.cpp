#include <cstdint>
#include <iomanip>
#include <iostream>

#include "./libs/streebog.h"
#include "./utils/bytes.cpp"

using namespace std;

#define C_in 0x36   // 00110110
#define C_out 0x5c  // 01011100

/**
 * NMAC_n(X) = Hash_n[(K XOR C_out) PHash_n ((K XOR C_in)PX)]
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

    /**
     * check if key got needed length
     * not --> throw ERR
     */
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
    copy(xoredIpad.begin(), xoredIpad.end(), inner.begin());     // copy to inner[0, .., lKey - 1] (K XOR C_in)
    copy(message.begin(), message.end(), inner.begin() + kLen);  // copy to inner[lkey, ...] message

    vector<uint8_t> hashedInner = streebog(inner, KEY_LEN(kLen));

    // outter = (key256/512 ^ opad) | hashedInner
    vector<uint8_t> outer(kLen + hashedInner.size());  //  32/64 + 32/64
    vector<uint8_t> xoredOpad = XOR(masterKey, opad);
    copy(xoredOpad.begin(), xoredOpad.end(), outer.begin());             // copy to inner[0, .., lKey - 1] (K XOR C_out)
    copy(hashedInner.begin(), hashedInner.end(), outer.begin() + kLen);  // copy to inner[lkey, ...] hashedInner

    vector<uint8_t> hashedOuter = streebog(outer, KEY_LEN(kLen));

    return hashedOuter;
}

/**
 * NMAC_256(X) = Hash_256[(K XOR C_out) P Hash_512((K XOR C_in) P X)]
 * K - key and it is defined
 * X - text
 */
vector<uint8_t> NMAC(const vector<uint8_t>& masterKey, const vector<uint8_t>& message) {
}

int main() {
    // by default streebog returns 32 bit hash
    cout << streebog("The quick brown fox jumps over the lazy dog") << endl;
    vector<uint8_t> hmacResult = HMAC({0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                       0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
                                       0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                                       0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f},
                                      {0x01, 0x26, 0xbd, 0xb8, 0x78, 0x00, 0xaf, 0x21, 0x43, 0x41, 0x45, 0x65, 0x63, 0x78, 0x01, 0x00});

    cout << "hmacResult: " << endl;
    cout << hex << setfill('0');
    cout << "0x";
    for (const auto& byte : hmacResult) {
        cout << setw(2) << static_cast<int>(byte);
    }
    cout << endl;

    return 0;
}
