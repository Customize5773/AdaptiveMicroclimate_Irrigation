#include "lora_protocol.h"
#include "security/key_management.h"
#include <curl/curl.h>
#include <cJSON.h>

// TTN API Configuration
#define TTN_BASE_URL "https://eu1.cloud.thethings.network/api/v3"
#define TTN_TIMEOUT_MS 5000

// Write callback for CURL
static size_t curl_write_cb(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    char **response_ptr = (char **)userp;
    
    *response_ptr = realloc(*response_ptr, realsize + 1);
    if(*response_ptr == NULL) return 0;
    
    memcpy(*response_ptr, contents, realsize);
    (*response_ptr)[realsize] = '\0';
    return realsize;
}

// Forward packet to TTN
bool forward_to_ttn(const uint8_t *packet, size_t len) {
    CURL *curl = curl_easy_init();
    if(!curl) return false;
    
    char *response = NULL;
    char auth_header[128];
    char url[256];
    
    // Prepare authentication
    snprintf(auth_header, sizeof(auth_header), "Authorization: Bearer %s", get_ttn_api_key());
    
    // Prepare URL
    snprintf(url, sizeof(url), "%s/gs/gateways/%s/packages", 
             TTN_BASE_URL, get_gateway_id());
    
    // Prepare JSON payload
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "gateway_id", get_gateway_id());
    
    // Convert packet to base64
    char *packet_b64 = base64_encode(packet, len);
    cJSON_AddStringToObject(root, "payload", packet_b64);
    free(packet_b64);
    
    char *json_payload = cJSON_PrintUnformatted(root);
    
    // Set CURL options
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, auth_header);
    
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_payload);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, TTN_TIMEOUT_MS);
    
    // Execute request
    CURLcode res = curl_easy_perform(curl);
    
    // Cleanup
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    cJSON_Delete(root);
    free(json_payload);
    
    if(res != CURLE_OK) {
        log_error("TTN request failed: %s", curl_easy_strerror(res));
        free(response);
        return false;
    }
    
    // Parse response
    bool success = false;
    if(response) {
        cJSON *json = cJSON_Parse(response);
        if(json) {
            success = cJSON_HasObjectItem(json, "success");
            cJSON_Delete(json);
        }
        free(response);
    }
    
    return success;
}

// Process Downlinks from TTN
void process_ttn_downlinks() {
    char url[256];
    snprintf(url, sizeof(url), "%s/gs/gateways/%s/packages/down", 
             TTN_BASE_URL, get_gateway_id());
    
    // Similar CURL implementation for GET request
    // ...
    
    // Parse downlink packets and add to queue
}

// Register Gateway with TTN
bool register_gateway_with_ttn() {
    // Implementation for gateway registration API call
    // ...
}
