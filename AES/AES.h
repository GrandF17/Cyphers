/* AES.h  */

#ifndef AES_H
#define AES_H

#include <stdint.h>

namespace AES {

    void encrypt(const uint8_t *plaintext, const uint8_t *e, uint8_t *ciphertext);

    void decrypt(const uint8_t *ciphertext, const uint8_t *e, uint8_t *plaintext);
    
}

#endif