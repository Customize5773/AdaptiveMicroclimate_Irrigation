#ifndef BME280_DRIVER_H
#define BME280_DRIVER_H

#include <stdint.h>

uint8_t bme280_init();
void bme280_read_data(float *temp, float *hum, float *pres);

#endif
