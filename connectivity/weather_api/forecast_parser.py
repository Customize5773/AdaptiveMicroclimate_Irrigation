#!/usr/bin/env python3
"""
Advanced Weather Forecast Parser for Irrigation Decisions
Input: JSON forecast from OpenWeather API
Output: Watering decision with environmental context
"""

import json
import sys
import datetime

def parse_forecast(json_data):
    """Parse weather forecast and make irrigation decision"""
    try:
        data = json.loads(json_data)
    except json.JSONDecodeError:
        return {
            'error': 'Invalid JSON format',
            'decision': 'error'
        }

    # Extract critical forecast elements
    current = data.get('current', {})
    hourly = data.get('hourly', [])
    daily = data.get('daily', [])
    
    # Initialize result structure
    result = {
        'decision': 'water',
        'reason': [],
        'forecast_summary': {
            'temp_current': current.get('temp', 0),
            'humidity': current.get('humidity', 0),
            'rain_next_24h': 0,
            'min_temp': float('inf'),
            'max_temp': float('-inf'),
            'rain_probabilities': []
        }
    }
    
    # Process hourly forecast (next 24 hours)
    for hour in hourly[:24]:
        # Track temperature extremes
        temp = hour.get('temp', result['forecast_summary']['temp_current'])
        result['forecast_summary']['min_temp'] = min(result['forecast_summary']['min_temp'], temp)
        result['forecast_summary']['max_temp'] = max(result['forecast_summary']['max_temp'], temp)
        
        # Accumulate precipitation
        rain = hour.get('rain', {}).get('1h', 0)
        result['forecast_summary']['rain_next_24h'] += rain
        
        # Track rain probabilities
        result['forecast_summary']['rain_probabilities'].append(hour.get('pop', 0))
    
    # Calculate significant rain probability
    max_rain_prob = max(result['forecast_summary']['rain_probabilities'], default=0)
    
    # Decision Logic
    if result['forecast_summary']['rain_next_24h'] > 5.0:
        result['decision'] = 'skip'
        result['reason'].append(f"Heavy rain expected: {result['forecast_summary']['rain_next_24h']:.1f}mm")
    elif max_rain_prob > 0.7:
        result['decision'] = 'skip'
        result['reason'].append(f"High rain probability: {max_rain_prob*100:.0f}%")
    elif result['forecast_summary']['min_temp'] < 2.0:
        result['decision'] = 'skip'
        result['reason'].append(f"Freezing temperatures expected: {result['forecast_summary']['min_temp']:.1f}°C")
    
    # Add frost risk warning
    if result['forecast_summary']['min_temp'] < 5.0:
        result['reason'].append(f"Frost risk: {result['forecast_summary']['min_temp']:.1f}°C")
    
    return result

if __name__ == "__main__":
    # Read JSON from stdin
    input_data = sys.stdin.read()
    
    # Parse and make decision
    decision = parse_forecast(input_data)
    
    # Output as JSON
    print(json.dumps(decision, indent=2))
