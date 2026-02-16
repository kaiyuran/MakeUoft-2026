#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>
#include <WiFiS3.h>
#include <Wire.h>
#include "MMA7660.h"
#include <math.h>
#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>

// --- Pins ---
#define LED_PIN     6  
#define NUMPIXELS   4  
#define TFT_CS     10
#define TFT_DC      8
#define TFT_RST    -1 
#define BUZZER_PIN  2
#define TEMP_PIN    A0

// --- Objects ---
Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
WiFiServer server(80);
MMA7660 accel;

// --- State Variables ---
struct Item { String name; int amount; String location; };
const int MAX_ITEMS = 20; 
Item pouchItems[MAX_ITEMS];
int actualItemCount = 0; 
int currentStartIndex = 0;
int isMissing = 0;  // Global flag set via WiFi

// --- Timing & Rolling Avg ---
unsigned long ledExpiryTime = 0;
bool isLedActive = false;
const unsigned long LED_HOLD_TIME = 3000; 
unsigned long lastScrollTime = 0;
const unsigned long scrollInterval = 3000;

const int BUFFER_SIZE = 10;
float magBuffer[BUFFER_SIZE];
int bufferIdx = 0;

// --- Beep Settings ---
unsigned long lastBeepTime = 0;
unsigned long beepStartTime = 0;
bool beeping = false;
const unsigned long beepDuration = 80;
const unsigned long SLOW_INTERVAL = 1500; // Temperature
const unsigned long FAST_INTERVAL = 400;  // Movement + Missing

float readTempC() {
  int raw = analogRead(TEMP_PIN);
  if (raw <= 0 || raw >= 1023) return NAN;
  float rTherm = 10000.0f * (1023.0f / (float)raw - 1.0f);
  float logR = log(rTherm);
  float tempK = 1.0f / (0.001129148f + 0.000234125f * logR + 0.0000000876741f * logR * logR * logR);
  return tempK - 273.15f;
}

void triggerItemLed(int ledIndex) {
  if (ledIndex >= 0 && ledIndex < NUMPIXELS) {
    pixels.clear();
    pixels.setPixelColor(ledIndex, pixels.Color(255, 255, 255));
    pixels.show();
    ledExpiryTime = millis() + LED_HOLD_TIME;
    isLedActive = true;
  }
}

void updateInventoryList() {
  tft.fillRect(0, 41, 320, 200, ILI9341_WHITE); 
  if (actualItemCount == 0) return;
  int yPos = 50; 
  for (int i = 0; i < 5; i++) {
    if (i >= actualItemCount) break;
    int idx = (currentStartIndex + i) % actualItemCount;
    tft.setCursor(20, yPos);
    tft.setTextColor(ILI9341_BLACK); tft.setTextSize(2);
    tft.print(pouchItems[idx].name);
    tft.setTextColor(ILI9341_BLUE); tft.print(" x"); tft.print(pouchItems[idx].amount);
    tft.setCursor(220, yPos + 5);
    tft.setTextColor(ILI9341_RED); tft.setTextSize(1);
    tft.print("["); tft.print(pouchItems[idx].location); tft.print("]");
    yPos += 40; 
  }
}

void setup() {
  Serial.begin(115200);
  pixels.begin();
  pixels.show(); 
  tft.begin(2000000); 
  tft.setRotation(1);
  tft.fillScreen(ILI9341_WHITE);
  tft.fillRect(0, 0, 320, 40, ILI9341_NAVY);
  tft.setCursor(10, 10);
  tft.setTextColor(ILI9341_WHITE); tft.setTextSize(2);
  tft.print("SACK");

  Wire.begin();
  accel.init();
  pinMode(BUZZER_PIN, OUTPUT);
  for(int i=0; i<BUFFER_SIZE; i++) magBuffer[i] = 0;

  WiFi.begin("Kaiyuran", "Kai10037");
  while (WiFi.status() != WL_CONNECTED) { delay(500); }
  
  tft.setCursor(160, 15);
  tft.setTextSize(1); tft.setTextColor(ILI9341_CYAN);
  tft.print("IP: "); tft.print(WiFi.localIP().toString());
  server.begin();
}

void loop() {
  unsigned long now = millis();

  // 1. Accelerometer Rolling Average
  int8_t x, y, z;
  accel.getXYZ(&x, &y, &z);
  float currentMag = sqrt((float)x * x + (float)y * y + (float)z * z);
  magBuffer[bufferIdx] = currentMag;
  bufferIdx = (bufferIdx + 1) % BUFFER_SIZE;
  float magSum = 0;
  for(int i=0; i<BUFFER_SIZE; i++) magSum += magBuffer[i];
  float avgMag = magSum / (float)BUFFER_SIZE;

  // 2. Temperature
  float tempC = readTempC();

  // 3. Conditional Alert Logic
  bool isMoving = (avgMag > 23.0f);
  bool isHot = (!isnan(tempC) && tempC > 25.5f);
  
  // NEW LOGIC: Fast beep ONLY if moving AND missing is true
  bool missingAndMoving = (isMissing == 1 && isMoving);
  // Fast beep also triggers for general high-speed movement if you want, 
  // but here we focus on the "Missing" requirement:
  bool fastBeepTrigger = missingAndMoving || (isMoving && isMissing == 0); // Keep general movement alarm? 
  // If you ONLY want it to beep for missing items when moving: change to: bool fastBeepTrigger = missingAndMoving;

  if (missingAndMoving || isHot) {
    unsigned long interval = missingAndMoving ? FAST_INTERVAL : SLOW_INTERVAL;
    
    if (!beeping && (now - lastBeepTime >= interval)) {
      digitalWrite(BUZZER_PIN, HIGH);
      beeping = true;
      beepStartTime = now;
      lastBeepTime = now;
      if(missingAndMoving) {
        for(int i=0; i<NUMPIXELS; i++) pixels.setPixelColor(i, pixels.Color(255, 0, 0));
        pixels.show();
      }
    }
    if (beeping && (now - beepStartTime >= beepDuration)) {
      digitalWrite(BUZZER_PIN, LOW);
      beeping = false;
      if(missingAndMoving) { pixels.clear(); pixels.show(); }
    }
  } else {
    digitalWrite(BUZZER_PIN, LOW);
    beeping = false;
  }

  // 4. Maintenance (LEDs/Scrolling)
  if (isLedActive && now >= ledExpiryTime) {
    pixels.clear(); pixels.show();
    isLedActive = false;
  }
  if (actualItemCount > 5 && (now - lastScrollTime >= scrollInterval)) {
    lastScrollTime = now;
    currentStartIndex = (currentStartIndex + 1) % actualItemCount;
    updateInventoryList();
  }

  // 5. WiFi
  WiFiClient client = server.available();
  if (client) {
    String requestBody = "";
    unsigned long timeout = millis();
    while (client.connected() && millis() - timeout < 1000) {
      if (client.available()) {
        char c = client.read();
        requestBody += c;
        if (requestBody.indexOf("]]") != -1) break;
      }
    }

    if (requestBody.indexOf("MISSING=") != -1) {
      isMissing = requestBody.charAt(requestBody.indexOf("MISSING=") + 8) - '0';
    }
    if (requestBody.indexOf("LED=") != -1) {
      triggerItemLed(requestBody.charAt(requestBody.indexOf("LED=") + 4) - '0');
    } 
    if (requestBody.indexOf("/registry") != -1) {
      int jsonStart = requestBody.indexOf("[[");
      if (jsonStart != -1) {
        JsonDocument doc;
        deserializeJson(doc, requestBody.substring(jsonStart));
        JsonArray root = doc.as<JsonArray>();
        int i = 0;
        for (JsonArray row : root) {
          if (i < MAX_ITEMS) {
            pouchItems[i].name = row[0].as<String>();
            pouchItems[i].amount = row[1].as<int>();
            pouchItems[i].location = row[2].as<String>();
            i++;
          }
        }
        actualItemCount = i;
        currentStartIndex = 0;
        updateInventoryList();
      }
    }
    client.println("HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n{\"status\":\"ok\"}");
    client.stop();
  }
  delay(10);
}