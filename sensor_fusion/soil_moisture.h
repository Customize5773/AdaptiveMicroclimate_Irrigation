#ifndef SOIL_MOISTURE_H
#define SOIL_MOISTURE_H

#include <stdint.h>

// Sensor status codes
#define SENSOR_OK 0
#define SENSOR_OPEN_CIRCUIT 1
#define SENSOR_SHORT_CIRCUIT 2

float calculate_vwc(uint16_t adc_value, float temp);
float read_soil_temperature();
float compensate_conductivity(float vwc, float ec);
int sensor_diagnostics(uint16_t adc_value);

// Calibration API
void set_dry_calibration(float value);
void set_wet_calibration(float value);
void set_temp_coefficient(float coeff);

#endif
