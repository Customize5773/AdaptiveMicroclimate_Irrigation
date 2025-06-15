#ifndef LORA_PROTOCOL_H
#define LORA_PROTOCOL_H

#include <stdint.h>

// LoRaWAN Specification Versions
#define LORAWAN_VERSION_1_0_4 0x04
#define LORAWAN_VERSION_1_1   0x11

// Device Activation Methods
typedef enum {
    OTAA = 0,  // Over-the-air activation
    ABP = 1    // Activation by personalization
} activation_mode_t;

// LoRa Packet Types
typedef enum {
    JOIN_REQUEST = 0,
    JOIN_ACCEPT = 1,
    UNCONFIRMED_UP = 2,
    UNCONFIRMED_DOWN = 3,
    CONFIRMED_UP = 4,
    CONFIRMED_DOWN = 5
} mtype_t;

// Regional Frequency Plans
typedef enum {
    EU868 = 0,
    US915 = 1,
    AS923 = 2,
    AU915 = 3,
    IN865 = 4
} region_t;

// Packet Header Structure
typedef struct __attribute__((packed)) {
    uint8_t major_version : 2;   // LoRaWAN major version
    uint8_t rfu : 3;             // Reserved for future use
    mtype_t mtype : 3;           // Message type
    uint8_t dev_addr[4];          // Device address
    uint8_t fctrl;               // Frame control
    uint16_t fcnt;               // Frame counter
    uint8_t fopts[15];            // Frame options
} lora_header_t;

// Join Request Structure
typedef struct __attribute__((packed)) {
    uint8_t app_eui[8];          // Application EUI
    uint8_t dev_eui[8];          // Device EUI
    uint16_t dev_nonce;          // Device nonce
    uint8_t mic[4];              // Message integrity code
} join_request_t;

// Sensor Data Payload
typedef struct __attribute__((packed)) {
    uint16_t soil_moisture;      // Soil VWC * 100
    int16_t temperature;         // Temperature * 100
    uint16_t humidity;           // Humidity * 100
    uint16_t battery_level;      // mV
    uint8_t sensor_status;       // Status flags
} sensor_data_t;

// Irrigation Command Payload
typedef struct __attribute__((packed)) {
    uint8_t zone;                // Irrigation zone
    uint16_t duration;           // Seconds
    uint8_t priority;            // Command priority
} irrigation_command_t;

// Gateway Configuration
typedef struct {
    activation_mode_t activation;
    region_t region;
    uint8_t datarate;
    uint8_t tx_power;
    uint32_t tx_interval;
    uint8_t retries;
} gateway_config_t;

// Function Prototypes
void lora_configure(gateway_config_t config);
uint8_t calculate_mic(const uint8_t *data, size_t len, const uint8_t *key);

#endif // LORA_PROTOCOL_H
