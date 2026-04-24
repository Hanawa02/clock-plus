/* Class of Zigbee clock plus endpoint inherited from common EP class */
#pragma once

#include "soc/soc_caps.h"
#include "sdkconfig.h"

#if CONFIG_ZB_ENABLED
#include "ZigbeeEP.h"
#include "ha/esp_zigbee_ha_standard.h"


class ZigbeeClockPlus : public ZigbeeTempSensor {
public:
  ZigbeeClockPlus(uint8_t endpoint) : ZigbeeTempSensor(endpoint) {
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
};
#endif  // CONFIG_ZB_ENABLED