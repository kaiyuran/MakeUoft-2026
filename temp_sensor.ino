#include <math.h>

#define TEMP_PIN A0
float calibration_offset = 8;

void setup() {
  Serial.begin(9600);
}

void loop() {
  int a = analogRead(TEMP_PIN);
  
  // Revised formula for 10k Thermistor modules
  float R2 = 10000.0 * (1023.0 / (float)a - 1.0);
  float logR2 = log(R2);
  float tempK = (1.0 / (0.001129148 + (0.000234125 * logR2) + (0.0000000876741 * logR2 * logR2 * logR2))) + calibration_offset;
  float tempC = tempK - 273.15;

  Serial.print("Temperature: ");
  Serial.print(tempC);
  Serial.println(" C");

  delay(1000);
}