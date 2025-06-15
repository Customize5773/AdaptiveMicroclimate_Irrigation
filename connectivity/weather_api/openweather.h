#ifndef OPENWEATHER_H
#define OPENWEATHER_H

#include <stdbool.h>

// Weather Forecast Structure
typedef struct {
    float temp;          // Current temperature (°C)
    float humidity;       // Relative humidity (%)
    float precip_prob;    // Precipitation probability (0-1)
    float precip_mm;      // Precipitation amount (mm)
    float temp_min;       // Minimum temperature (°C)
    float temp_max;       // Maximum temperature (°C)
} WeatherForecast;

// Callback Function Type
typedef void (*weather_callback_t)(const char *json);

// API Functions
void openweather_init(const char *key, float lat, float lon);
void openweather_fetch_forecast(weather_callback_t callback);
WeatherForecast openweather_parse_forecast(const char *json_str);

#endif // OPENWEATHER_H
