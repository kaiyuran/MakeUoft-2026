import math
import time
import grovepi

# Grove analog port A0
sensor_port = 0

calibration_offset = 8

def get_temperature():
    # Read raw analog value (0–1023)
    raw_value = grovepi.analogRead(sensor_port)

    if raw_value == 0:
        return None

    # Thermistor math (same as before)
    r2 = 10000.0 * (1023.0 / float(raw_value) - 1.0)
    log_r2 = math.log(r2)

    temp_k = (1.0 / (0.001129148 + (0.000234125 * log_r2) +
              (0.0000000876741 * log_r2**3))) + calibration_offset

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
