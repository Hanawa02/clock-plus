// #ifndef ZIGBEE_MODE_ED
// #error "Zigbee end device mode is not selected in Tools->Zigbee mode"
// #endif

#include "Zigbee.h"
#include "BlinkLight.h"
#include "ClockPlus.h"
#include <Preferences.h>


/* Zigbee contact sensor configuration */
#define CLOCK_PLUS_ENDPOINT_NUMBER 2


ZigbeeClockPlus zbClockPlus = ZigbeeClockPlus(CLOCK_PLUS_ENDPOINT_NUMBER);

uint8_t button = BOOT_PIN;

/* Preferences for storing ENROLLED flag to persist across reboots */
Preferences preferences;

void setup() {

  // This initiates a connection between the ESP32 and the computer - from Gemini:
  // > The command initializes the UART (Universal Asynchronous Receiver-Transmitter) hardware inside the ESP32. It sets the Baud Rate, which is the speed at which data is transmitted over the serial connection.
  // > The number 115200 represents the speed in bits per second (bps). For the ESP32 to communicate successfully, both the board and the Serial Monitor in your Arduino IDE must be set to the same baud rate.
  Serial.begin(115200);  

  while (!Serial) {
    delay(10); // Wait for the serial port to connect
  }

  blink_white();

  // Setting up enrollment
  Serial.println("Setup 1. Setting Zigbee enrollment.");
  preferences.begin("Zigbee", false);               // Save ENROLLED flag in flash so it persists across reboots
  bool enrolled = preferences.getBool("ENROLLED");  // Get ENROLLED flag from preferences
  preferences.end();

  // Init boot button 
  pinMode(button, INPUT_PULLUP);

  Serial.println("Setup 2. Initializing zbClockPlus");
  zbClockPlus.initialize();

  // Add endpoint to Zigbee Core
  Zigbee.addEndpoint(&zbClockPlus);

  Serial.println("Setup 3. Starting Zigbee");

  // When all EPs are registered, start Zigbee in End Device mode
  if (!Zigbee.begin()) {
    blink_red();
    Serial.println("Setup 3a. Zigbee failed to start! Rebooting...");
    ESP.restart();
  } else {
    blink_green();
    Serial.println("Setup 3b. Zigbee started successfully!");
    delay(500);
  }

  Serial.println("Setup 4. Connecting to Zigbee network");

  if (Zigbee.connected()) {
    blink_blue();
    Serial.println("Setup 4a. Connected to Zigbee successfully!");

  }

  while (!Zigbee.connected()) {
    blink_yellow();
    Serial.println("Setup 4b. Waiting for connection to Zigbee network");
    delay(500);    
  }

  delay(3000);

}

void loop() {
  // Checking button for factory reset
  if (digitalRead(button) == LOW) {  // Push button pressed

    Serial.println("Pressed?");

    // Key debounce handling
    delay(100);
    int startTime = millis();
    while (digitalRead(button) == LOW) {
      delay(100);
      if ((millis() - startTime) > 3000) {
        // If key pressed for more than 3secs, factory reset Zigbee and reboot
        blink_pink();
        Serial.println("Loop 1. Resetting Zigbee to factory and rebooting in 1s.");
        delay(1000);
        Zigbee.factoryReset();
        blink_blue();
      }
    }
  }

  blink_green();
  delay(300);

  zbClockPlus.updateTime();

  esp_zb_lock_acquire(portMAX_DELAY);
  zbClockPlus.setTemperature(28.4);
  zbClockPlus.setHumidity(44.0);
  zbClockPlus.report();

  Serial.println("Loop - Data Sent");

  esp_zb_lock_release();

  delay(30000);  // Wait 30 seconds before next update
}