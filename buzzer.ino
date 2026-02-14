#define BUZZER_PIN 8  // Connect SIG to Pin 8

void setup() {
  pinMode(BUZZER_PIN, OUTPUT); // Tell the Uno Pin 8 is an output
}

void loop() {
  // Turn buzzer ON
  digitalWrite(BUZZER_PIN, HIGH);
  delay(100);  // Buzz for 0.1 seconds
  
  // Turn buzzer OFF
  digitalWrite(BUZZER_PIN, LOW);
  
  delay(2000); // Wait for 2 seconds before beeping again
}