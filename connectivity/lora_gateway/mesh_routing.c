#include "lora_protocol.h"
#include <string.h>

#define MAX_HOPS 5
#define MESH_TIMEOUT_MS 2000

typedef struct {
    uint8_t source_addr[4];
    uint8_t dest_addr[4];
    uint8_t next_hop[4];
    uint32_t last_seen;
    uint8_t hop_count;
} routing_entry_t;

static routing_entry_t routing_table[20];
static int routing_count = 0;

// Add packet to routing table
void add_to_routing_table(const uint8_t *packet, size_t len) {
    lora_header_t *header = (lora_header_t *)packet;
    
    // Find existing entry or create new
    routing_entry_t *entry = find_routing_entry(header->dev_addr);
    if(!entry) {
        if(routing_count >= 20) return;
        entry = &routing_table[routing_count++];
        memcpy(entry->source_addr, header->dev_addr, 4);
    }
    
    // Update routing info
    memcpy(entry->dest_addr, get_packet_destination(packet), 4);
    entry->last_seen = get_timestamp();
    entry->hop_count = get_hop_count(packet);
}

// Route mesh packet
void route_mesh_packet(uint8_t *packet, size_t len) {
    lora_header_t *header = (lora_header_t *)packet;
    
    // Check hop count
    uint8_t hops = get_hop_count(packet);
    if(hops >= MAX_HOPS) {
        log_warning("Max hop count reached");
        return;
    }
    
    // Update hop count
    set_hop_count(packet, hops + 1);
    
    // Find next hop
    uint8_t next_hop[4];
    if(!find_next_hop(header->dev_addr, next_hop)) {
        log_warning("No route to destination");
        return;
    }
    
    // Recalculate MIC before forwarding
    recalculate_mic(packet, len);
    
    // Forward packet
    lora_send_packet(packet, len);
}

// Find next hop in routing table
bool find_next_hop(const uint8_t *dest_addr, uint8_t *next_hop) {
    for(int i = 0; i < routing_count; i++) {
        if(memcmp(routing_table[i].dest_addr, dest_addr, 4) == 0) {
            memcpy(next_hop, routing_table[i].next_hop, 4);
            return true;
        }
    }
    return false;
}

// Periodic routing table maintenance
void routing_table_maintenance() {
    uint32_t current_time = get_timestamp();
    
    for(int i = 0; i < routing_count; i++) {
        if(current_time - routing_table[i].last_seen > MESH_TIMEOUT_MS) {
            // Remove stale entries
            memmove(&routing_table[i], &routing_table[i+1], 
                   (routing_count - i - 1) * sizeof(routing_entry_t));
            routing_count--;
            i--;
        }
    }
}
