

#define LED_BRIGHTNESS 16 // make it faint to save energy
#define BLINK_TIME_MS 800

void turn_led_light_off() {
    digitalWrite(RGB_BUILTIN, LOW); // turns off the LED
    delay(BLINK_TIME_MS);
}

void blink_red() {
  rgbLedWriteOrdered(RGB_BUILTIN, LED_COLOR_ORDER_RGB, LED_BRIGHTNESS, 0, 0);  // Red
  delay(BLINK_TIME_MS);
  turn_led_light_off();
}

void blink_green() {
  rgbLedWriteOrdered(RGB_BUILTIN, LED_COLOR_ORDER_RGB, 0, LED_BRIGHTNESS, 0);  // Green
  delay(BLINK_TIME_MS);
  turn_led_light_off();
}

void blink_yellow() {
  rgbLedWriteOrdered(RGB_BUILTIN, LED_COLOR_ORDER_RGB, LED_BRIGHTNESS, LED_BRIGHTNESS, 0);  // Green
  delay(BLINK_TIME_MS);
  turn_led_light_off();
}

void blink_blue() {
  rgbLedWriteOrdered(RGB_BUILTIN, LED_COLOR_ORDER_RGB, 0, 0, LED_BRIGHTNESS);  // Blue
  delay(BLINK_TIME_MS);
  turn_led_light_off();
}

void blink_pink() {
  rgbLedWriteOrdered(RGB_BUILTIN, LED_COLOR_ORDER_RGB, LED_BRIGHTNESS, 0, LED_BRIGHTNESS);  // Blue
  delay(BLINK_TIME_MS);
  turn_led_light_off();
}

void blink_white() {
  rgbLedWriteOrdered(RGB_BUILTIN, LED_COLOR_ORDER_RGB, LED_BRIGHTNESS, LED_BRIGHTNESS, LED_BRIGHTNESS);  // Blue
  delay(BLINK_TIME_MS);
  turn_led_light_off();
}
