#ifndef KEY_MANAGEMENT_H
#define KEY_MANAGEMENT_H

#include <stdint.h>

// Key Storage Structure
typedef struct {
    uint8_t app_key[16];      // Application key
    uint8_t nwk_key[16];      // Network key
    uint8_t app_eui[8];       // Application EUI
    uint8_t dev_eui[8];       // Device EUI
    uint32_t dev_addr;        // Device address
    uint32_t fcnt_up;         // Uplink frame counter
    uint32_t fcnt_down;       // Downlink frame counter
} lora_keys_t;

// Key Storage API
void store_keys(const lora_keys_t *keys);
bool load_keys(lora_keys_t *keys);
void erase_keys();

// Key Derivation Functions
void derive_session_keys(const uint8_t *app_key, const uint8_t *dev_nonce, 
                         uint8_t *nwk_skey, uint8_t *app_skey);

// TTN Integration Keys
const char *get_ttn_api_key();
const char *get_gateway_id();
bool store_ttn_credentials(const char *api_key, const char *gateway_id);

// Security Utilities
bool validate_device_credentials(const uint8_t *app_eui, const uint8_t *dev_eui);
bool verify_packet_integrity(const uint8_t *packet, size_t len);

#endif // KEY_MANAGEMENT_H
