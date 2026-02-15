# Sentinel Sack | MakeUofT 2026 
Sentinel Sack is an intelligent, hardware-integrated backpack designed for the Survival category at MakeUofT 2026. It tracks your gear in real-time, ensuring you never leave an essential item behind.

## How It Works 
The Sentinel Sack operates through a seamless loop between hardware sensing and AI processing:

Detection & Vision: When an object is placed near the bag, the Logitech Webcam captures an image.

AI Identification: This image is sent to a Python script that interfaces with the Gemini API. The AI identifies the object and categorizes its storage location (Front, Middle, Back, or Right).

Real-Time Inventory: The Arduino Uno R4 WiFi receives this data and updates the LCD Screens, providing a live readout of exactly what is inside and where it is located.

Motion Intelligence: The Grove Accelerometer monitors the bag's physical state. If the bag moves (indicating the user is leaving) and the system detects a missing "start-of-trip" item, the LEDs and display trigger a survival alert.

Voice Query: Users can interact with the bag via a microphone to ask "What's in the bag?", receiving an instant status update.

## Tech Stack 
Software
Gemini API: High-level object recognition and data parsing.

Python: The "brain" managing the API logic and hardware communication.

Arduino IDE: Programming the microcontrollers for sensor management.

Hardware
Microcontrollers: Arduino Uno R4 WiFi (Main Hub) & Arduino Uno R3.

Sensors: Grove Accelerometer & Logitech Webcam.

Interface: LCD Screens & LED Indicators.

Connection: Arduino Grove Shield for modular hardware integration.

## Project Context 🏆
Created in 24 hours for MakeUofT 2026. The Sentinel Sack solves the "survival" challenge by ensuring total tool-readiness and spatial awareness through automated inventory management.
