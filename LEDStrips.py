#include <Adafruit_NeoPixel.h>

#define PIN        6   // Digital pin connected to the strip
#define NUMPIXELS  4   // Number of LEDs in your strip

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  pixels.begin();
  pixels.clear(); // Initialize all pixels to 'off'
  pixels.show();
  
  Serial.begin(115200);
  Serial.println("Enter command: Front, Middle, Back, or Right");
}

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim(); // Remove whitespace/newlines

    // Turn off all LEDs before lighting the new one
    pixels.clear();

    if (input.equalsIgnoreCase("Front")) {
      pixels.setPixelColor(0, pixels.Color(255, 255, 255)); // Red
    } 
    else if (input.equalsIgnoreCase("Middle")) {
      pixels.setPixelColor(1, pixels.Color(255, 255, 255)); // Green
    } 
    else if (input.equalsIgnoreCase("Back")) {
      pixels.setPixelColor(2, pixels.Color(255, 255, 255)); // Blue
    } 
    else if (input.equalsIgnoreCase("Right")) {
      pixels.setPixelColor(3, pixels.Color(255, 255, 255)); // Yellow
    }

    pixels.show(); // Update the strip
  }
}