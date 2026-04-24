/* Class of Zigbee clock plus endpoint inherited from common EP class */
#pragma once

#include "soc/soc_caps.h"
#include "sdkconfig.h"
#include <time.h>

#if CONFIG_ZB_ENABLED
#include "ZigbeeEP.h"
#include "ha/esp_zigbee_ha_standard.h"

// The difference between Zigbee Epoch (2000) and Unix Epoch (1970) in seconds
#define ZIGBEE_UNIX_OFFSET 946684800L

class ZigbeeClockPlus : public ZigbeeTempSensor {
public:
  ZigbeeClockPlus(uint8_t endpoint)
    : ZigbeeTempSensor(endpoint) {
    // 1. Set the Device ID FIRST.
    // This tells the base class what kind of "template" to prepare.
    setManufacturerAndModel("Hanawa", "ClockPlus");
    setVersion(1);
    setHardwareVersion(1);
  }

  void initialize() {
    addTimeCluster();
    addHumiditySensor();
  }

  // struct tm timeinfo;
  // struct tm *localTime;
  // int32_t timezone;
  bool is24HourMode = false;

  void formatTimeForDisplay(struct tm *lt) {
    int hour = lt->tm_hour;
    const char *ampm = "";

    if (!is24HourMode) {
      ampm = (hour >= 12) ? " PM" : " AM";
      hour = hour % 12;
      if (hour == 0) hour = 12;  // Handle midnight/noon
    }

    Serial.printf("Display Time: %02d:%02d%s\n", hour, lt->tm_min, ampm);
  }

  void updateTime() {
    if (Zigbee.connected()) {
      struct tm fetched_timeinfo = getTime();
      int32_t fetched_timezone = getTimezone();

      if (fetched_timeinfo.tm_year > 70) {
        // 1. Convert struct tm to time_t (seconds)
        // Note: mktime assumes the input is "utc" time.
        time_t utc_seconds = mktime(&fetched_timeinfo);        

        // 2. Apply the timezone offset and the Zigbee/Unich epoch gap offsets (all in seconds)
        time_t local_seconds = utc_seconds + fetched_timezone + ZIGBEE_UNIX_OFFSET;

        // 3. Convert back to a pointer to a struct tm
        struct tm *local_time = gmtime(&local_seconds);

        // 4. Access your adjusted data
        int currentSecond = local_time->tm_sec;
        int currentHour = local_time->tm_hour;
        int currentMinute = local_time->tm_min;

        int currentDay = local_time->tm_mday;
        int currentMonth = local_time->tm_mon + 1; // Month is 0 based: January 0, February 1 ...
        int currentYear = local_time->tm_year + 1900; // Year is offset by 1900

        Serial.printf("Local Time: %02d:%02d:%02d\n", currentHour, currentMinute, currentSecond);

        Serial.printf("Local Date: %02d.%02d.%02d\n", currentDay, currentMonth, currentYear);

        formatTimeForDisplay(local_time);
      } else {
        Serial.println("Waiting for Time Sync from Hub...");
      }
    } else {
      Serial.println("Oops, disconnected");
    }
  }
};
#endif  // CONFIG_ZB_ENABLED