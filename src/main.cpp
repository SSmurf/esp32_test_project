#include <Arduino.h>

const int externalLedPin = 2;  // GPIO pin for external LED

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);  // Set built-in LED pin as output
  pinMode(externalLedPin, OUTPUT);  // Set external LED pin as output
  Serial.begin(115200);
  Serial.println("Hello world!");
  // digitalWrite(externalLedPin, HIGH);
}

void loop() {
  Serial.println("Hello world from loop");
  
  // Turn both LEDs on
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(externalLedPin, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(externalLedPin, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(externalLedPin, HIGH);
  delay(500);
  
  // Turn both LEDs off
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(externalLedPin, LOW);
  delay(1000);
}
