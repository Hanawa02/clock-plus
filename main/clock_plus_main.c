#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "bme680.h"
#include "i2cdev.h"

#define I2C_SDA_IO 11
#define I2C_SCL_IO 10
#define I2C_PORT   0

static const char *TAG = "BME680_SI";

void app_main(void) {
    // Initialize the internal i2cdev library (REQUIRED for esp-idf-lib)
    ESP_ERROR_CHECK(i2cdev_init());

    // Define the Descriptor (The "ID Card" for the sensor)
    bme680_t sensor;
    memset(&sensor, 0, sizeof(bme680_t));

    // Initialize the descriptor with your pins and address
    ESP_ERROR_CHECK(bme680_init_desc(&sensor, BME680_I2C_ADDR_0, I2C_PORT, I2C_SDA_IO, I2C_SCL_IO));

    // MANUALLY LOWER THE SPEED (Safety first for breadboards!)
    // The descriptor has a nested i2c_dev_t. We set it to 100kHz.
    sensor.i2c_dev.cfg.master.clk_speed = 400000;

    // Initialize the physical sensor
    ESP_ERROR_CHECK(bme680_init_sensor(&sensor));

    // Configure the oversampling (Standard for Bosch sensors)
    bme680_values_float_t values;
    bme680_set_oversampling_rates(&sensor, BME680_OSR_2X, BME680_OSR_4X, BME680_OSR_8X);
    bme680_set_filter_size(&sensor, BME680_IIR_SIZE_3);

    while (1) {
        // Trigger measurement and wait for the duration
        uint32_t duration;
        bme680_get_measurement_duration(&sensor, &duration);

        if (bme680_force_measurement(&sensor) == ESP_OK) {
            // Wait for the sensor to finish its "cook time"
            vTaskDelay(pdMS_TO_TICKS(duration / 1000));

            // Get the results in SI Units (Celsius, %, hPa)
            if (bme680_get_results_float(&sensor, &values) == ESP_OK) {
                ESP_LOGI(TAG, "\n\nTemperature: %.2f °C", values.temperature);
                ESP_LOGI(TAG, "Humidity:    %.2f %%", values.humidity);
                ESP_LOGI(TAG, "Pressure:    %.2f hPa", values.pressure);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(5000)); // Read every 5 seconds
    }
}