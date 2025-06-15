#include "lora_protocol.h"
#include "ttn_integration.h"
#include "mesh_routing.h"
#include "security/aes_lorawan.h"
#include "security/key_management.h"
#include <string.h>

// LoRa Module Hardware Abstraction
typedef struct {
    void (*init)(region_t region);
    void (*set_datarate)(uint8_t dr);
    void (*set_tx_power)(uint8_t power);
    bool (*send)(const uint8_t *data, size_t len);
    int (*receive)(uint8_t *buffer, size_t size, uint32_t timeout);
} lora_driver_t;

// Global Gateway State
static gateway_config_t current_config;
static lora_driver_t lora_driver;
static uint8_t dev_eui[8] = {0};
static uint32_t uplink_counter = 0;
static uint32_t downlink_counter = 0;

// Initialize LoRa Controller
void lora_controller_init(lora_driver_t driver, gateway_config_t config) {
    memcpy(&lora_driver, &driver, sizeof(lora_driver_t));
    memcpy(&current_config, &config, sizeof(gateway_config_t));
    
    // Initialize hardware
    lora_driver.init(config.region);
    lora_driver.set_datarate(config.datarate);
    lora_driver.set_tx_power(config.tx_power);
    
    // Load security keys
    load_activation_keys();
    
    // Join network
    if(config.activation == OTAA) {
        perform_otaa_join();
    }
}

// Main Control Loop
void lora_control_loop() {
    uint8_t rx_buffer[256];
    uint8_t tx_buffer[256];
    
    while(1) {
        // Receive packets with timeout
        int rx_len = lora_driver.receive(rx_buffer, sizeof(rx_buffer), 1000);
        
        if(rx_len > 0) {
            process_received_packet(rx_buffer, rx_len);
        }
        
        // Periodic uplink transmission
        static uint32_t last_tx = 0;
        if(get_timestamp() - last_tx > current_config.tx_interval) {
            if(prepare_uplink(tx_buffer, sizeof(tx_buffer)) {
                lora_driver.send(tx_buffer, sizeof(tx_buffer));
                last_tx = get_timestamp();
                uplink_counter++;
            }
        }
        
        // Handle downlinks
        process_downlinks();
    }
}

// Process Received Packet
void process_received_packet(uint8_t *packet, size_t len) {
    // Verify MIC first
    if(!verify_packet_integrity(packet, len)) {
        log_error("MIC verification failed");
        return;
    }
    
    // Decrypt payload
    decrypt_payload(packet, len);
    
    // Handle by packet type
    lora_header_t *header = (lora_header_t *)packet;
    
    switch(header->mtype) {
        case JOIN_REQUEST:
            handle_join_request(packet);
            break;
            
        case UNCONFIRMED_UP:
        case CONFIRMED_UP:
            route_mesh_packet(packet, len);
            forward_to_ttn(packet, len);
            break;
            
        case JOIN_ACCEPT:
        case UNCONFIRMED_DOWN:
        case CONFIRMED_DOWN:
            handle_downlink(packet, len);
            break;
            
        default:
            log_warning("Unknown packet type: %d", header->mtype);
    }
}

// Prepare Uplink Data
bool prepare_uplink(uint8_t *buffer, size_t size) {
    sensor_data_t sensor_data;
    if(!read_sensors(&sensor_data)) {
        return false;
    }
    
    lora_header_t header = {
        .major_version = LORAWAN_VERSION_1_0_4,
        .mtype = UNCONFIRMED_UP,
        .fcnt = uplink_counter
    };
    
    memcpy(buffer, &header, sizeof(header));
    size_t offset = sizeof(header);
    
    // Add sensor payload
    memcpy(buffer + offset, &sensor_data, sizeof(sensor_data));
    offset += sizeof(sensor_data);
    
    // Calculate and append MIC
    uint8_t mic[4];
    calculate_mic(buffer, offset, get_app_key(), mic);
    memcpy(buffer + offset, mic, 4);
    offset += 4;
    
    // Encrypt payload
    encrypt_payload(buffer + sizeof(header), offset - sizeof(header) - 4);
    
    return true;
}

// Handle Join Requests
void handle_join_request(uint8_t *packet) {
    join_request_t *request = (join_request_t *)(packet + sizeof(lora_header_t));
    
    // Verify device credentials
    if(!validate_device_credentials(request->app_eui, request->dev_eui)) {
        log_warning("Invalid join credentials");
        return;
    }
    
    // Generate join accept
    uint8_t response[128];
    generate_join_accept(request->dev_nonce, response);
    
    // Send response
    lora_driver.send(response, sizeof(response));
}
