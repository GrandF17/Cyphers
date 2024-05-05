#include <cstdint>
#include <iomanip>
#include <iostream>

#include "./libs/streebog.h"
#include "./utils/bytes.cpp"

using namespace std;

#define C_in 0x36   // 00110110
#define C_out 0x5c  // 01011100

void printVector(vector<uint8_t> a) {
    cout << hex << setfill('0');
    cout << "0x";
    for (const auto& byte : a) {
        cout << setw(2) << static_cast<int>(byte);
    }
    cout << endl;
}

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

int main() {
    // by default streebog returns 32 bit hash
    cout << "Streebog hash: " << streebog("The quick brown fox jumps over the lazy dog") << endl;
    vector<uint8_t> key256 = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                              0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
                              0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                              0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f};
    vector<uint8_t> key512 = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                              0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
                              0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                              0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
                              0xb1, 0x08, 0x5b, 0xda, 0x1e, 0xca, 0xda, 0xe9,
                              0xeb, 0xcb, 0x2f, 0x81, 0xc0, 0x65, 0x7c, 0x1f,
                              0x2f, 0x6a, 0x76, 0x43, 0x2e, 0x45, 0xd0, 0x16,
                              0x71, 0x4e, 0xb8, 0x8d, 0x75, 0x85, 0xc4, 0xfc};
    vector<uint8_t> text = {0x01, 0x26, 0xbd, 0xb8, 0x78, 0x00, 0xaf, 0x21, 0x43, 0x41, 0x45, 0x65, 0x63, 0x78, 0x01, 0x00};

    cout << "HMAC result (key256): " << endl;
    printVector(HMAC(key256, text));

    cout << "NMAC result (key256): " << endl;
    printVector(NMAC(key256, text));

    cout << "HMAC result (key512): " << endl;
    printVector(HMAC(key512, text));

    cout << "NMAC result (key512): " << endl;
    printVector(NMAC(key512, text));

    return 0;
}
