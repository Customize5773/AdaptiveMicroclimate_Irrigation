#include <math.h>
#include <time.h>
#include "climate_model.h"

// Sensor calibration parameters
#define SOIL_MOISTURE_MIN 1200   // Dry soil ADC value
#define SOIL_MOISTURE_MAX 2800   // Saturated soil ADC value
#define PUMP_FLOW_RATE 2.5       // ml/sec

// System state structure
typedef struct {
    float soil_moisture;       // Current VWC (%)
    float temperature;          // Ambient temp (°C)
    float humidity;             // Relative humidity (%)
    float solar_radiation;      // Light intensity (W/m²)
    float wind_speed;           // Wind speed (m/s)
} SystemState;

// Weather forecast structure
typedef struct {
    float precip_prob;          // Precipitation probability (0-1)
    float precip_mm;            // Expected precipitation (mm)
    float temp_min;             // Minimum temperature (°C)
    float temp_max;             // Maximum temperature (°C)
} WeatherForecast;

/**
 * Calculate irrigation duration based on environmental conditions
 * 
 * @param state Current system state
 * @param forecast Weather forecast data
 * @param root_depth Plant root depth (m)
 * @return Watering duration in seconds
 */
float calculate_irrigation_duration(SystemState state, WeatherForecast forecast, float root_depth) {
    // 1. Get water deficit from climate model
    float water_deficit = calculate_water_deficit(
        state.temperature,
        state.humidity,
        state.solar_radiation,
        state.wind_speed
    );
    
    // 2. Calculate soil moisture adjustment factor (0-1)
    float soil_factor = 1.0 - (state.soil_moisture / 100.0);
    
    // 3. Calculate root zone volume adjustment
    float root_volume_factor = root_depth * 0.7;  // Assume 70% root density
    
    // 4. Apply forecast adjustments
    float forecast_factor = 1.0;
    if (forecast.precip_prob > 0.3) {
        forecast_factor -= forecast.precip_prob * 0.7;
    }
    
    // 5. Calculate total water requirement (ml)
    float water_ml = water_deficit * soil_factor * root_volume_factor * forecast_factor * 1000;
    
    // 6. Calculate pump runtime (seconds)
    float duration = water_ml / PUMP_FLOW_RATE;
    
    return fmaxf(duration, 0);
}

/**
 * Adaptive irrigation control decision
 * 
 * @param state Current system state
 * @param forecast Weather forecast data
 * @return True if irrigation should be activated
 */
bool should_irrigate(SystemState state, WeatherForecast forecast) {
    // Check climate model skip conditions
    if (should_skip_watering(forecast)) {
        return false;
    }
    
    // Check frost protection mode
    float frost_protection = get_frost_protection_water(forecast.temp_min);
    if (frost_protection > 0) {
        return true;
    }
    
    // Standard moisture-based irrigation logic
    float irrigation_duration = calculate_irrigation_duration(state, forecast, ROOT_DEPTH);
    return (irrigation_duration > 10.0);  // Minimum 10-second runtime
}

/**
 * Frost protection irrigation control
 * 
 * @param min_temp Forecasted minimum temperature
 * @return Watering duration in seconds for frost protection
 */
float get_frost_protection_water(float min_temp) {
    if (min_temp <= FROST_THRESHOLD) {
        // Apply 0.5mm water per degree below threshold
        float temp_difference = FROST_THRESHOLD - min_temp;
        return (0.5 * temp_difference * 1000) / PUMP_FLOW_RATE;  // ml to seconds
    }
    return 0;
}

/**
 * Main irrigation control loop
 */
void irrigation_control_loop() {
    SystemState current_state = read_sensors();
    WeatherForecast forecast = get_weather_forecast();
    
    if (should_irrigate(current_state, forecast)) {
        float duration = calculate_irrigation_duration(
            current_state, 
            forecast, 
            ROOT_DEPTH
        );
        
        // Add frost protection water if needed
        float frost_water = get_frost_protection_water(forecast.temp_min);
        duration += frost_water;
        
        // Execute watering
        activate_irrigation(duration);
        
        // Log irrigation event
        log_irrigation_event(duration, current_state);
    }
}

// Helper Functions (implement in hardware layer)
SystemState read_sensors();
WeatherForecast get_weather_forecast();
void activate_irrigation(float duration_seconds);
void log_irrigation_event(float duration, SystemState state);
