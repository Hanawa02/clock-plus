#include "stdio.h"
#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_master.h"
#include "esp_log.h"

#define I2C_SCL_IO           10      
#define I2C_SDA_IO           11      
#define BME680_ADDR          0x76    

static const char *TAG = "BME680_NG";

// Global handles for the new driver
i2c_master_bus_handle_t bus_handle;
i2c_master_dev_handle_t dev_handle;

void init_bme680_i2c_new() {
    // 1. Configure the I2C Bus
    i2c_master_bus_config_t bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_NUM_0,
        .scl_io_num = I2C_SCL_IO,
        .sda_io_num = I2C_SDA_IO,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &bus_handle));

    // 2. Add the BME680 as a device on that bus
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = BME680_ADDR,
        .scl_speed_hz = 100000,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));
    
    ESP_LOGI(TAG, "I2C New Gen driver initialized.");
}

void check_sensor_new() {
    uint8_t reg_addr = 0xD0; // ID register
    uint8_t sensor_id = 0;

    // The new driver combines write and read into one clean command
    esp_err_t ret = i2c_master_transmit_receive(dev_handle, &reg_addr, 1, &sensor_id, 1, -1);

    if (ret == ESP_OK && sensor_id == 0x61) {
        ESP_LOGI(TAG, "Success! BME680 found with New Driver. ID: 0x%02x", sensor_id);
    } else {
        ESP_LOGE(TAG, "Failed to find BME680. Error: %s, ID: 0x%02x", esp_err_to_name(ret), sensor_id);
    }
}

void read_bme680_data() {
    // 1. Tell the sensor to take a measurement (Set Mode to 0x01)
    uint8_t mode_reg = 0x74; 
    uint8_t mode_val = 0x01; 
    i2c_master_transmit(dev_handle, (uint8_t[]){mode_reg, mode_val}, 2, -1);

    // 2. Wait a moment for the sensor to "cook" the data
    vTaskDelay(pdMS_TO_TICKS(100));

    // 3. Read the Raw Temperature (Registers 0x22 to 0x24)
    uint8_t temp_reg = 0x22;
    uint8_t raw_data[3];
    i2c_master_transmit_receive(dev_handle, &temp_reg, 1, raw_data, 3, -1);

    // 4. Convert Raw Data to a readable format
    // (This is a simplified version of the Bosch math for a quick test)
    int32_t adc_T = (int32_t)(((uint32_t)raw_data[0] << 12) | ((uint32_t)raw_data[1] << 4) | ((uint32_t)raw_data[2] >> 4));
    
    // Note: Without calibration coefficients, this is a "raw" value.
    // Let's print the raw value first to confirm the sensor is changing!
    ESP_LOGI(TAG, "\n\n---Raw Temperature ADC Value: %ld", adc_T);
    
    if (adc_T == 0) {
        ESP_LOGE(TAG, "\n\n---Read failed! Check if sensor is powered.");
    } else {
        ESP_LOGI(TAG, "\n\n---Sensor is reacting! Try touching the sensor to see the number change.");
    }
}

void app_main(void) {
    init_bme680_i2c_new();
    
    while(1) {
        read_bme680_data();
        vTaskDelay(pdMS_TO_TICKS(2000)); // Read every 2 seconds
    }
}