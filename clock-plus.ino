


               // 4-Color support
#include <ePaperDisplay.h>

// Value for serial initialization
// serial_diag_bitrate
#define BAUD 115200

const char HelloWorld[] = "Hello World!";

void setup() {
  Serial.begin(115200);  // Start Serial Monitor
  Serial.println("Starting setup for ESP32-H2");
 
  setupDisplay();
  // upateDisplay();
  // updateTime("23:59");
}



void loop() {
  // put your main code here, to run repeatedly:

  updateTime("23:59");
  delay(60000);
// 
}

void blinkLights(
) {
  // #ifdef RGB_BUILTIN
//   digitalWrite(RGB_BUILTIN, HIGH);  // Turn the RGB LED white
//   delay(1000);
//   digitalWrite(RGB_BUILTIN, LOW);  // Turn the RGB LED off
//   delay(1000);

//   rgbLedWrite(RGB_BUILTIN, RGB_BRIGHTNESS, 0, 0);  // Red
//   delay(1000);
//   rgbLedWrite(RGB_BUILTIN, 0, RGB_BRIGHTNESS, 0);  // Green
//   delay(1000);
//   rgbLedWrite(RGB_BUILTIN, 0, 0, RGB_BRIGHTNESS);  // Blue
//   delay(1000);
//   rgbLedWrite(RGB_BUILTIN, 0, 0, 0);  // Off / black
//   delay(1000);
// #endif
}