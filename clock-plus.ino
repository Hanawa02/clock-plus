#ifndef ZIGBEE_MODE_ED
#error "Zigbee end device mode is not selected in Tools->Zigbee mode"
#endif

#include "Zigbee.h"

void setup() {
  // put your setup code here, to run once:
  digitalWrite(RGB_BUILTIN, LOW); // turns off the LED

}

void loop() {
  // put your main code here, to run repeatedly:

  // This initiates a connection between the ESP32 and the computer - from Gemini:
  // > The command initializes the UART (Universal Asynchronous Receiver-Transmitter) hardware inside the ESP32. It sets the Baud Rate, which is the speed at which data is transmitted over the serial connection.
  // > The number 115200 represents the speed in bits per second (bps). For the ESP32 to communicate successfully, both the board and the Serial Monitor in your Arduino IDE must be set to the same baud rate.
  Serial.begin(115200);

  
}
