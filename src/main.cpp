#include <Arduino.h>
#include <NimBLEDevice.h>
#include <NimBLEServer.h>
#include <NimBLEUtils.h>

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

const int LED_PINS[3] = {2, 4, 5};  // LED pins

bool deviceConnected = false;
NimBLEServer* pServer = NULL;
NimBLECharacteristic* pCharacteristic = NULL;

const unsigned long TIMEOUT = 5000; // 5 seconds timeout
unsigned long ledTimers[3] = {0, 0, 0};
bool ledActive[3] = {false, false, false};

class MyServerCallbacks: public NimBLEServerCallbacks {
    void onConnect(NimBLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(NimBLEServer* pServer) {
      deviceConnected = false;
    }
};

class MyCallbacks: public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();
      if (value.length() > 0) {
        Serial.print("Received value: ");
        Serial.println(value.c_str());

        int ledIndex = value[0] - '1';
        if (ledIndex >= 0 && ledIndex < 3) {
          if (!ledActive[ledIndex]) {
            digitalWrite(LED_PINS[ledIndex], HIGH);  // Turn LED on
            ledTimers[ledIndex] = millis();  // Start timer
            ledActive[ledIndex] = true;
            Serial.printf("LED %d turned on\n", ledIndex + 1);
          } else {
            digitalWrite(LED_PINS[ledIndex], LOW);  // Turn LED off
            ledActive[ledIndex] = false;
            Serial.printf("LED %d turned off\n", ledIndex + 1);
          }
        } else {
          Serial.println("Invalid command");
        }
      }
    }
};

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < 3; i++) {
    pinMode(LED_PINS[i], OUTPUT);
    digitalWrite(LED_PINS[i], LOW);  // Ensure all LEDs are off at start
  }

  // Create the BLE Device
  NimBLEDevice::init("ESP32 3-LED Control");

  // Create the BLE Server
  pServer = NimBLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  NimBLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      NIMBLE_PROPERTY::WRITE
                    );

  pCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  
  pAdvertising->setMinPreferred(0x12);
  NimBLEDevice::startAdvertising();
  Serial.println("Waiting for a client connection to notify...");
}

void loop() {
  // Check timeouts for each LED
  unsigned long currentTime = millis();
  
  for (int i = 0; i < 3; i++) {
    if (ledActive[i] && currentTime - ledTimers[i] > TIMEOUT) {
      digitalWrite(LED_PINS[i], LOW);
      ledActive[i] = false;
      Serial.printf("LED %d timed out\n", i + 1);
    }
  }
}
