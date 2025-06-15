# Usage-Example

```c
#include "bme280_driver.h"
#include "bh1750_driver.h"
#include "soil_moisture.h"

void read_environmental_data() {
    // Initialize sensors
    bme280_init();
    bh1750_init(BH1750_CONT_H_RES);
    
    // Read atmospheric data
    float temp, hum, pres;
    bme280_read_data(&temp, &hum, &pres);
    
    // Read light intensity
    float lux = bh1750_auto_range();
    
    // Read soil moisture (with temperature compensation)
    uint16_t soil_adc = read_analog(SOIL_SENSOR_PIN);
    float soil_temp = read_soil_temperature();
    float vwc = calculate_vwc(soil_adc, soil_temp);
    
    // Output results
    printf("Temp: %.1fC | Hum: %.1f%% | Lux: %.0f | VWC: %.1f%%\n", 
           temp, hum, lux, vwc);
}
```
