#include <WiFiS3.h>
#include <Wire.h>
#include "MMA7660.h"
#include <math.h>

const char* ssid = "Kian";
const char* pass = "10101010";

WiFiServer server(80);
MMA7660 accel;

const int TEMP_PIN = A0;
const int BUZZER_PIN = 2;   // Grove buzzer on D2

// --- Beep timing (quick beep every 1.5 sec) ---
unsigned long lastBeepTime = 0;
unsigned long beepStartTime = 0;
bool beeping = false;

const unsigned long beepInterval = 1500; // ms between beeps
const unsigned long beepDuration = 80;   // ms beep length

// Steinhart-Hart constants (common for 10k thermistor modules)
float readTempC() {
  int raw = analogRead(TEMP_PIN);   // 0..1023
  if (raw <= 0 || raw >= 1023) return NAN; // disconnected / invalid

  float rTherm = 10000.0f * (1023.0f / (float)raw - 1.0f);
  float logR = log(rTherm);

  float tempK = 1.0f / (0.001129148f +
                        0.000234125f * logR +
                        0.0000000876741f * logR * logR * logR);

  return tempK - 273.15f;
}

void setup() {
  Serial.begin(115200);
  delay(2000);

  Wire.begin();
  accel.init();

  pinMode(TEMP_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  Serial.println("Connecting WiFi...");
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // ---- Read accelerometer ----
  int8_t x, y, z;
  accel.getXYZ(&x, &y, &z);

  float total = sqrt((float)x * x + (float)y * y + (float)z * z) - 22.0f;

  // ---- Read temperature probe on A0 ----
  float tempC = readTempC();

  // ---- Buzzer: quick beep every 1.5 sec when temp > 28C ----
  unsigned long now = millis();
  bool hot = (!isnan(tempC) && tempC > 28.0f);

  if (hot) {
    // start a beep every 1.5 seconds
    if (!beeping && (now - lastBeepTime >= beepInterval)) {
      digitalWrite(BUZZER_PIN, HIGH);
      beeping = true;
      beepStartTime = now;
      lastBeepTime = now;
    }
    // stop beep after short duration
    if (beeping && (now - beepStartTime >= beepDuration)) {
      digitalWrite(BUZZER_PIN, LOW);
      beeping = false;
    }
  } else {
    // ensure off when not hot
    digitalWrite(BUZZER_PIN, LOW);
    beeping = false;
  }

  // ---- Permanent Serial Stream ----
  Serial.print("IP:");
  Serial.print(WiFi.localIP());
  Serial.print(" | X:");
  Serial.print(x);
  Serial.print(" Y:");
  Serial.print(y);
  Serial.print(" Z:");
  Serial.print(z);
  Serial.print(" Total:");
  Serial.print(total, 2);
  Serial.print(" TempC:");
  if (isnan(tempC)) Serial.print("ERR");
  else Serial.print(tempC, 2);
  Serial.print(" BEEPING:");
  Serial.println(hot ? 1 : 0);

  // ---- HTTP Response ----
  WiFiClient client = server.available();
  if (client) {
    while (client.connected() && !client.available()) delay(1);
    while (client.available()) client.read();

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/plain");
    client.println("Connection: close");
    client.println();

    client.print("x=");
    client.print(x);
    client.print(",y=");
    client.print(y);
    client.print(",z=");
    client.print(z);
    client.print(",total=");
    client.print(total, 2);
    client.print(",tempC=");
    if (isnan(tempC)) client.print("ERR");
    else client.print(tempC, 2);
    client.print(",hot=");
    client.println(hot ? 1 : 0);

    client.stop();
  }

  delay(100); // 10 updates/sec
}
