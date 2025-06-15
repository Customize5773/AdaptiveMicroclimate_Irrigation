#include "aes_lorawan.h"
#include <string.h>

// AES-128 Implementation for LoRaWAN
void lorawan_aes_encrypt(uint8_t *buffer, size_t len, const uint8_t *key) {
    // AES-128 ECB encryption implementation
    // (Using hardware-accelerated crypto if available)
    
    // Simplified implementation using software crypto
    for(size_t i = 0; i < len; i += 16) {
        aes128_ecb_encrypt(buffer + i, key, buffer + i);
    }
}

// Encrypt payload (LoRaWAN specific)
void encrypt_payload(uint8_t *payload, size_t len, const uint8_t *key, 
                     uint32_t dev_addr, uint32_t counter, uint8_t direction) {
    uint8_t a_block[16] = {
        0x01, 0x00, 0x00, 0x00, 0x00,
        direction,
        dev_addr & 0xFF, (dev_addr >> 8) & 0xFF, 
        (dev_addr >> 16) & 0xFF, (dev_addr >> 24) & 0xFF,
        counter & 0xFF, (counter >> 8) & 0xFF, 
        (counter >> 16) & 0xFF, (counter >> 24) & 0xFF,
        0x00, (uint8_t)len
    };
    
    for(size_t i = 0; i < len; i++) {
        if(i % 16 == 0) {
            // Increment counter
            a_block[15] = i / 16 + 1;
            lorawan_aes_encrypt(a_block, 16, key);
        }
        payload[i] ^= a_block[i % 16];
    }
}

// Calculate Message Integrity Code (MIC)
void calculate_mic(const uint8_t *data, size_t len, const uint8_t *key, uint8_t *mic) {
    uint8_t b0[16] = {
        0x49, 0x00, 0x00, 0x00, 0x00,
        0x00, // Direction (0=uplink, 1=downlink)
        data[1], data[2], data[3], data[4], // DevAddr
        data[6], data[7], data[8], data[9], // FCnt
        0x00, (uint8_t)(len + 4)
    };
    
    // Compute B0
    lorawan_aes_encrypt(b0, 16, key);
    
    // Compute MIC
    uint8_t mic_block[16];
    memset(mic_block, 0, 16);
    
    // XOR with B0
    for(int i = 0; i < 16; i++) {
        mic_block[i] ^= b0[i];
    }
    
    // Process payload
    // ... (full implementation per LoRaWAN spec)
    
    // Final encryption
    lorawan_aes_encrypt(mic_block, 16, key);
    
    // Copy first 4 bytes as MIC
    memcpy(mic, mic_block, 4);
}
