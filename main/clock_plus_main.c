#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "bme680.h"
#include "i2cdev.h"
#include "EPD_4in2.h"
#include "GUI_Paint.h"

#define I2C_SDA_IO 11
#define I2C_SCL_IO 10
#define I2C_PORT   0

static const char *TAG = "BME680_SI";
float current_temp = 0, current_hum = 0;

void update_epaper_ui(float temp, float hum) {
    // 1. Create the image buffer in RAM (approx 15KB)
    UBYTE *BlackImage;
    UWORD Imagesize = ((EPD_4in2_WIDTH % 8 == 0)? (EPD_4in2_WIDTH / 8 ): (EPD_4in2_WIDTH / 8 + 1)) * EPD_4in2_HEIGHT;
    BlackImage = (UBYTE *)malloc(Imagesize);

    // 2. Initialize the Paint Tool
    // Parameters: Image Buffer, Width, Height, Rotate (0, 90, 180, 270), Color
    Paint_NewImage(BlackImage, EPD_4in2_WIDTH, EPD_4in2_HEIGHT, 0, WHITE);
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);

    // 3. Draw your Dashboard
    Paint_DrawString_EN(120, 10, "12:45", &Font24, WHITE, BLACK); // Large Clock
    
    char sensor_data[50];
    sprintf(sensor_data, "Temp: %.2f C", temp);
    Paint_DrawString_EN(20, 100, sensor_data, &Font20, WHITE, BLACK);
    
    sprintf(sensor_data, "Humidity: %.1f %%", hum);
    Paint_DrawString_EN(20, 140, sensor_data, &Font20, WHITE, BLACK);

    // 4. Send to the 4.2" screen
    EPD_4in2_Display(BlackImage);
    
    // 5. Clean up memory! (Important on the H2-Mini)
    free(BlackImage);
}

void display_task(void *pvParameters) {
    // Initial hardware setup
    DEV_Module_Init();
    EPD_4in2_Init();
    EPD_4in2_Clear();

    while(1) {
        ESP_LOGI("EPD", "Updating display...");
        update_epaper_ui(current_temp, current_hum);
        
        // E-papers don't like constant refreshing (it can damage them).
        // Update once per minute (60,000ms)
        vTaskDelay(pdMS_TO_TICKS(60000)); 
    }
}

void app_main(void) {
    // Initialize the internal i2cdev library (REQUIRED for esp-idf-lib)
    ESP_ERROR_CHECK(i2cdev_init());

    xTaskCreate(display_task, "display_task", 4096 * 2, NULL, 5, NULL);
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
        uint32_t duration;
    // 1. Calculate how long the sensor needs (based on oversampling settings)
    bme680_get_measurement_duration(&sensor, &duration);

    // 2. Start the measurement
    esp_err_t ret = bme680_force_measurement(&sensor);
    
    if (ret == ESP_OK) {
        // 3. IMPORTANT: Wait for the duration + a small safety buffer (1s)
        // duration is in microseconds, so we divide by 1000
        vTaskDelay(pdMS_TO_TICKS((duration / 1000) + 1000));

        // 4. Check if data is actually ready before reading
        bool busy;
        bme680_is_measuring(&sensor, &busy);
        
        if (!busy) {
            if (bme680_get_results_float(&sensor, &values) == ESP_OK) {
                ESP_LOGI(TAG, "Temp: %.2f °C | Hum: %.2f %% | Pres: %.2f hPa | Gas: %.2f Ohm", 
                         values.temperature, values.humidity, values.pressure, values.gas_resistance);

                current_temp = values.temperature;
                current_hum = values.humidity;
                // update_epaper_ui(values.temperature, values.humidity);
            }
        } else {
            ESP_LOGW(TAG, "Sensor still busy...");
        }
    } else if (ret == ESP_ERR_INVALID_STATE) {
        ESP_LOGE(TAG, "Sensor busy error. Try increasing the delay.");
    }

    // 5. Wait 30 seconds before the next loop
    vTaskDelay(pdMS_TO_TICKS(15000));
    }
}