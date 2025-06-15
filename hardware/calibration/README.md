# Sensor Calibration Procedures

## Environmental Sensor (BME680)
### Temperature Calibration
1. Create controlled environment using thermal chamber
2. Record readings at 5°C intervals from -10°C to 60°C
3. Apply 3-point calibration:
   ```python
   # calibration_script.py
   import numpy as np
   from scipy.optimize import curve_fit
   
   def temp_cal_model(x, a, b, c):
       return a*x**2 + b*x + c
   
   measured = [...] # Raw sensor values
   reference = [...] # NIST-traceable thermometer
   params, _ = curve_fit(temp_cal_model, measured, reference)
   ```

### Humidity Calibration
1. Use saturated salt solutions for RH points:
   - LiCl (11% RH), MgCl₂ (33%), NaCl (75%), K₂SO₄ (97%)
2. Soak for 24 hours at 25°C
3. Calculate compensation coefficients:
   ```c
   // In firmware
   float calibrated_rh = raw_rh * 0.98 + 1.2; // Example coefficients
   ```

## Soil Moisture Sensor
### Volumetric Water Content (VWC) Curve
1. Prepare soil samples at known VWC levels:
   | Sample | Water Content | Dry Soil (g) | Water Added (ml) |
   |--------|---------------|--------------|------------------|
   | Dry    | 5%            | 1000         | 50               |
   | Medium | 25%           | 1000         | 250              |
   | Wet    | 45%           | 1000         | 450              |
   
2. Collect sensor readings:
   ```bash
   ./calibration_tool --mode soil --output soil_cal.csv
   ```

3. Generate calibration curve:
   ```python
   # soil_calibration.py
   import pandas as pd
   from sklearn.preprocessing import PolynomialFeatures
   from sklearn.linear_model import LinearRegression
   
   data = pd.read_csv('soil_cal.csv')
   poly = PolynomialFeatures(degree=3)
   X_poly = poly.fit_transform(data[['raw_value']])
   model = LinearRegression().fit(X_poly, data['vwc'])
   ```

### Temperature Compensation
1. Test at multiple temperatures (5°C, 15°C, 25°C, 35°C)
2. Calculate temperature coefficient:
   ```
   Temperature Coefficient = ΔReading / °C
   ```

## Light Sensor (BH1750)
### Lux Calibration
1. Use certified lux meter as reference
2. Test at illumination levels:
   - 0 lx (complete darkness)
   - 100 lx (indoor lighting)
   - 10,000 lx (overcast day)
   - 50,000 lx (direct sun)
   
3. Apply piecewise linear calibration:
   ```c
   // In firmware
   if (lux < 100) {
       calibrated = lux * 1.05;
   } else if (lux < 10000) {
       calibrated = lux * 0.98 + 2.1;
   } else {
       calibrated = lux * 1.02 - 125.4;
   }
   ```
