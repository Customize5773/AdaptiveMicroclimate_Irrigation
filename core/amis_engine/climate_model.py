import math
from datetime import datetime, timedelta

class MicroclimateModel:
    def __init__(self, location=(0, 0), elevation=0):
        """
        Initialize climate model with location parameters
        :param location: (latitude, longitude) in decimal degrees
        :param elevation: Elevation in meters
        """
        self.location = location
        self.elevation = elevation
        self.last_rainfall = None
        self.crop_coefficient = 1.0  # Default for mixed vegetation
        
    def set_crop_type(self, crop_type):
        """Configure crop-specific parameters"""
        crop_params = {
            'tomato': {'k_c': 1.15, 'root_depth': 0.8, 'critical_temp': 5},
            'lettuce': {'k_c': 1.0, 'root_depth': 0.3, 'critical_temp': 2},
            'cactus': {'k_c': 0.3, 'root_depth': 0.5, 'critical_temp': 0},
            'citrus': {'k_c': 0.8, 'root_depth': 1.2, 'critical_temp': -2}
        }
        params = crop_params.get(crop_type.lower(), crop_params['lettuce'])
        self.crop_coefficient = params['k_c']
        self.root_depth = params['root_depth']
        self.critical_temp = params['critical_temp']
    
    def calculate_water_deficit(self, temp, rh, solar_rad, wind_speed=2.0):
        """
        Compute daily evapotranspiration (ET₀) using FAO Penman-Monteith equation
        :param temp: Temperature (°C)
        :param rh: Relative humidity (%)
        :param solar_rad: Solar radiation (W/m²)
        :param wind_speed: Wind speed at 2m height (m/s)
        :return: Water deficit in mm/day
        """
        # Convert solar radiation to MJ/m²/day
        rad_mj = solar_rad * 0.0864
        
        # Calculate saturation vapor pressure (kPa)
        sat_vp = 0.6108 * math.exp((17.27 * temp) / (temp + 237.3))
        
        # Actual vapor pressure (kPa)
        act_vp = sat_vp * (rh / 100.0)
        
        # Slope of vapor pressure curve (kPa/°C)
        delta = 4098 * sat_vp / ((temp + 237.3) ** 2)
        
        # Atmospheric pressure (kPa)
        atm_pressure = 101.3 * ((293 - 0.0065 * self.elevation) / 293) ** 5.26
        
        # Psychrometric constant (kPa/°C)
        gamma = 0.000665 * atm_pressure
        
        # Soil heat flux (negligible for daily)
        g = 0
        
        # FAO Penman-Monteith equation
        numerator = (0.408 * delta * (rad_mj - g) + 
                    gamma * (900 / (temp + 273)) * wind_speed * (sat_vp - act_vp))
        denominator = delta + gamma * (1 + 0.34 * wind_speed)
        
        et0 = numerator / denominator
        
        # Apply crop coefficient
        etc = et0 * self.crop_coefficient
        
        return max(etc, 0)
    
    def frost_risk_adjustment(self, temp, forecast_temp):
        """
        Calculate frost protection watering needs
        :return: Watering duration in minutes for frost protection
        """
        if min(forecast_temp) <= self.critical_temp + 2:
            # Apply 1mm water per hour when temp < critical + 2°C
            return max((self.critical_temp + 2 - min(forecast_temp)) * 10, 0)
        return 0
    
    def rain_absorption_period(self, rainfall_mm):
        """
        Calculate irrigation pause duration after rainfall
        :param rainfall_mm: Measured rainfall in mm
        :return: Hours to pause irrigation
        """
        if rainfall_mm > 5:
            self.last_rainfall = datetime.now()
            return 72  # 3 days for heavy rain
        elif rainfall_mm > 2:
            self.last_rainfall = datetime.now()
            return 48  # 2 days for moderate rain
        return 0
    
    def should_skip_watering(self, forecast):
        """
        Determine if watering should be skipped based on weather forecast
        :param forecast: Dict with 'precip_prob' and 'precip_mm'
        :return: Boolean decision
        """
        # Skip if significant rain probability
        if forecast.get('precip_prob', 0) > 0.4:
            return True
        
        # Skip if within post-rain pause period
        if self.last_rainfall and (datetime.now() - self.last_rainfall) < timedelta(hours=24):
            return True
        
        # Skip if temperature below freezing
        if forecast.get('temp_min', 10) < 1:
            return True
        
        return False

# Example usage:
if __name__ == "__main__":
    model = MicroclimateModel(location=(37.7749, -122.4194), elevation=16)
    model.set_crop_type('tomato')
    
    # Calculate water deficit for current conditions
    deficit = model.calculate_water_deficit(
        temp=25.5, 
        rh=65, 
        solar_rad=850,
        wind_speed=1.5
    )
    print(f"Daily water deficit: {deficit:.2f} mm")
