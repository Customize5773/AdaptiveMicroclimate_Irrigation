#ifndef BH1750_DRIVER_H
#define BH1750_DRIVER_H

#include <stdint.h>

void bh1750_init(uint8_t mode);
float bh1750_read_lux();
void bh1750_set_measurement_time(uint8_t mt_reg);
float bh1750_auto_range();

#endif
