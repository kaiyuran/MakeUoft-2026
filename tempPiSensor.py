import math
import time
from gpiozero import MCP3008

# Setup the ADC (MCP3008) 
# Assumes the thermistor is connected to CH0 (Pin 1 of the MCP3008)
adc = MCP3008(channel=0)

calibration_offset = 8

def get_temperature():
    # adc.value returns a float between 0.0 and 1.0
    # We multiply by 1023 to simulate the Arduino's 10-bit resolution
    raw_value = adc.value * 1023
    
    # Avoid division by zero if the sensor is disconnected
    if raw_value == 0:
        return None

    # Steinhart-Hart Equation for 10k Thermistor
    # R2 = R_pullup * (1023 / raw_adc - 1)
    r2 = 10000.0 * (1023.0 / float(raw_value) - 1.0)
    log_r2 = math.log(r2)
    
    # Calculate Kelvin
    temp_k = (1.0 / (0.001129148 + (0.000234125 * log_r2) + 
              (0.0000000876741 * log_r2**3))) + calibration_offset
    
    # Convert to Celsius
    temp_c = temp_k - 273.15
    return temp_c

print("Starting Temperature Readings...")

try:
    while True:
        temperature = get_temperature()
        if temperature is not None:
            print(f"Temperature: {temperature:.2f} C")
        else:
            print("Sensor Error: Check wiring")
            
        time.sleep(1)

except KeyboardInterrupt:
    print("\nStopping...")