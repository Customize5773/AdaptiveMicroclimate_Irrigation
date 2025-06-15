#include "openweather.h"
#include <string.h>
#include <cJSON.h>
#include "esp_http_client.h"
#include "esp_log.h"

static const char *TAG = "OpenWeather";

// OpenWeather API Configuration
#define OPENWEATHER_BASE_URL "https://api.openweathermap.org/data/3.0/onecall"
#define OPENWEATHER_TIMEOUT_MS 5000

// API Key and Location
static char api_key[37] = "";  // 36 chars + null terminator
static float latitude = 0.0;
static float longitude = 0.0;

// HTTP Event Handler
esp_err_t http_event_handler(esp_http_client_event_t *evt) {
    static char *buffer = NULL;
    static int buffer_len = 0;
    
    switch(evt->event_id) {
        case HTTP_EVENT_ON_DATA:
            // Append received data to buffer
            buffer = realloc(buffer, buffer_len + evt->data_len + 1);
            if(!buffer) return ESP_FAIL;
            
            memcpy(buffer + buffer_len, evt->data, evt->data_len);
            buffer_len += evt->data_len;
            buffer[buffer_len] = '\0';
            break;
            
        case HTTP_EVENT_ON_FINISH:
            // Pass buffer to callback
            if(evt->user_data && buffer) {
                weather_callback_t callback = (weather_callback_t)evt->user_data;
                callback(buffer);
            }
            if(buffer) free(buffer);
            buffer = NULL;
            buffer_len = 0;
            break;
            
        case HTTP_EVENT_DISCONNECTED:
            if(buffer) free(buffer);
            buffer = NULL;
            buffer_len = 0;
            break;
            
        default:
            break;
    }
    return ESP_OK;
}

// Initialize OpenWeather API
void openweather_init(const char *key, float lat, float lon) {
    strncpy(api_key, key, sizeof(api_key) - 1);
    latitude = lat;
    longitude = lon;
}

// Fetch Weather Forecast
void openweather_fetch_forecast(weather_callback_t callback) {
    if(strlen(api_key) == 0) {
        ESP_LOGE(TAG, "API key not set");
        return;
    }
    
    // Build request URL
    char url[256];
    snprintf(url, sizeof(url), 
             "%s?lat=%.6f&lon=%.6f&exclude=minutely,alerts&units=metric&appid=%s",
             OPENWEATHER_BASE_URL, latitude, longitude, api_key);
    
    // Configure HTTP client
    esp_http_client_config_t config = {
        .url = url,
        .event_handler = http_event_handler,
        .user_data = callback,
        .timeout_ms = OPENWEATHER_TIMEOUT_MS
    };
    
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_perform(client);
    esp_http_client_cleanup(client);
}

// Parse Forecast Data (Simplified version)
WeatherForecast openweather_parse_forecast(const char *json_str) {
    WeatherForecast forecast = {0};
    cJSON *root = cJSON_Parse(json_str);
    if(!root) return forecast;
    
    // Current conditions
    cJSON *current = cJSON_GetObjectItem(root, "current");
    if(current) {
        forecast.temp = cJSON_GetObjectItem(current, "temp")->valuedouble;
        forecast.humidity = cJSON_GetObjectItem(current, "humidity")->valuedouble;
        
        cJSON *rain = cJSON_GetObjectItem(current, "rain");
        if(rain) {
            forecast.precip_mm = cJSON_GetObjectItem(rain, "1h")->valuedouble;
        }
    }
    
    // Daily forecast (next 24 hours)
    cJSON *hourly = cJSON_GetObjectItem(root, "hourly");
    if(cJSON_GetArraySize(hourly) > 0) {
        cJSON *first_hour = cJSON_GetArrayItem(hourly, 0);
        forecast.precip_prob = cJSON_GetObjectItem(first_hour, "pop")->valuedouble;
    }
    
    // Temperature extremes (next 24 hours)
    forecast.temp_min = forecast.temp;
    forecast.temp_max = forecast.temp;
    for(int i = 0; i < 8 && i < cJSON_GetArraySize(hourly); i++) {
        cJSON *hour = cJSON_GetArrayItem(hourly, i);
        double temp = cJSON_GetObjectItem(hour, "temp")->valuedouble;
        if(temp < forecast.temp_min) forecast.temp_min = temp;
        if(temp > forecast.temp_max) forecast.temp_max = temp;
    }
    
    cJSON_Delete(root);
    return forecast;
}
