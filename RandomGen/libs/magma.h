#ifndef MAGMA_LC_H
#define MAGMA_LC_H

#include <cstdint>
#include <vector>

using namespace std;

class MagmaLC {
   public:
    MagmaLC();
    uint64_t encrypt(uint64_t in, const vector<uint8_t>& masterKey);
    vector<uint8_t> changeKey(vector<uint8_t> newKey);

   private:
    void G(uint32_t& left, uint32_t& right, const uint32_t& key);
    uint64_t GFin(uint32_t& left, uint32_t& right, const uint32_t& key);
    uint32_t extract4Bytes(const vector<uint8_t>& key, const size_t& from);
};

#endif