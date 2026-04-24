#include "ep/ZigbeeBinary.h"
#include "esp_zigbee_cluster.h"
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

class ZigbeeClockPlus {
public:
  ZigbeeTempSensor sensorEP;
  ZigbeeBinary timeDisplayModeEP;

  static ZigbeeClockPlus* instance;
  
  ZigbeeClockPlus()
    : sensorEP(8), timeDisplayModeEP(7) {
      instance = this;
    }

  void initialize() {
    // 1. Set the Device ID FIRST.
    // This tells the base class what kind of "template" to prepare.
    sensorEP.setManufacturerAndModel("Hanawa", "ClockPlus");
    sensorEP.setVersion(1);
    sensorEP.setHardwareVersion(1);
    sensorEP.addHumiditySensor();
    

    sensorEP.addTimeCluster();

    timeDisplayModeEP.setManufacturerAndModel("Hanawa", "ClockPlus");
    timeDisplayModeEP.setVersion(1);
    timeDisplayModeEP.setHardwareVersion(1);
    
    // Adds 12H/24 state selector

    timeDisplayModeEP.addBinaryOutput();
    timeDisplayModeEP.setBinaryOutputApplication(BINARY_OUTPUT_APPLICATION_TYPE_HVAC_OTHER);
    timeDisplayModeEP.setBinaryOutputDescription("Display 24h format");

    timeDisplayModeEP.onBinaryOutputChange(onTimeDisplayModeChangeStatic);    
  }

  void formatTimeForDisplay(struct tm *lt) {
    int hour = lt->tm_hour;
    const char *ampm = "";

    if (!time_display_24_format) {
      ampm = (hour >= 12) ? " PM" : " AM";
      hour = hour % 12;
      if (hour == 0) hour = 12;  // Handle midnight/noon
    }

    Serial.printf("Display Time: %02d:%02d%s\n", hour, lt->tm_min, ampm);
  }

  void updateTime() {
    if (Zigbee.connected()) {
      if (!initial_values_sync_done) {
        Serial.print("Bool? ");
        timeDisplayModeEP.setBinaryOutput(time_display_24_format);
        initial_values_sync_done = true;
        Serial.println(timeDisplayModeEP.getBinaryOutput());
      }


      struct tm fetched_timeinfo = sensorEP.getTime();
      int32_t fetched_timezone = sensorEP.getTimezone();

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
        int currentMonth = local_time->tm_mon + 1;     // Month is 0 based: January 0, February 1 ...
        int currentYear = local_time->tm_year + 1900;  // Year is offset by 1900

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

  void setTemperature(float value) {
    sensorEP.setTemperature(value);
  }

  void setHumidity(float value) {
    sensorEP.setHumidity(value);
  }

  void reportSensorData() {
    sensorEP.report();
  }

static void onTimeDisplayModeChangeStatic(bool use_24_mode) {
    if (instance != nullptr) {
      instance->onTimeDisplayModeChange(use_24_mode);
    }
  }

  void onTimeDisplayModeChange(bool use_24_mode) {
    // print the state
    Serial.printf("Received Time Display format change, use 24 hours format: %u\r\n", use_24_mode);
    time_display_24_format = use_24_mode;
  }

private:
  bool initial_values_sync_done = false;
  bool time_display_24_format = true;
};

// I don't really like this, but seems to be needed because of C++
ZigbeeClockPlus* ZigbeeClockPlus::instance = nullptr;

#endif  // CONFIG_ZB_ENABLED
