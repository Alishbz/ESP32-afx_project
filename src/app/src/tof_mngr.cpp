/**
 * @file tof_mngr.c
 * @author Hamza Sengul (hamza.sengul@saykal.com)
 * @brief
 * @version 0.1
 * @date 2024-03-22
 *
 * @copyright Copyright (c) 2024
 *
 */

/* Standart Library Includes*/
#include <stdlib.h>
#include <string.h>

/* Platform Includes */
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_event.h"

/* Kernel Includes */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* Core Layer Includes */
#include "core_includes.h"

/* Driver Layer Includes */
#include "drv/VL53L0X.h"

/* Application Layer Includes */
#include "app/app_config.h"
#include "app/app_types.h"

static const char *TAG = "tof_mngr";

/* Definations */
#define I2C_PORT I2C_NUM_0
#define PIN_SDA GPIO_NUM_33
#define PIN_SCL GPIO_NUM_32

#define TOF_MNGR_READ_POLLING_PERIOD_MS (10)

static uint16_t s_result_mm = 0;
static VL53L0X vl(I2C_PORT);

void tof_reader_task(void *args)
{
    ESP_LOGI(TAG, "%s started!", __func__);
    while (pdTRUE) {
        uint16_t s_result_mm_tmp = 0;
        TickType_t tick_start = xTaskGetTickCount();
        bool res = vl.read(&s_result_mm_tmp);
        TickType_t tick_end = xTaskGetTickCount();
        int took_ms = ((int)tick_end - tick_start) / portTICK_PERIOD_MS;
        if (res) {
            s_result_mm = s_result_mm_tmp;
            ESP_LOGI(TAG, "Range: %d [mm] took %d [ms]", (int)s_result_mm, took_ms);
        } else {
            ESP_LOGE(TAG, "Failed to measure.");
        }
        vTaskDelay(pdMS_TO_TICKS(TOF_MNGR_READ_POLLING_PERIOD_MS));
    }
}

/**
 * @brief The function `tof_reader_get_range` returns the range value in millimeters from a time-of-flight
 * sensor.
 *
 * @return The function `tof_reader_get_range` is returning a `uint16_t` value, which is the result of
 * the variable `s_result_mm`.
 */
uint16_t tof_reader_get_range(void)
{
    return s_result_mm;
}

/**
 * @brief The function `tof_mngr_init` initializes a Time-of-Flight sensor and creates a task for reading
 * sensor data.
 *
 * @return The function `tof_mngr_init` is returning an `esp_err_t` value. It will return `ESP_OK` if
 * the initialization is successful, and `ESP_FAIL` if there is a failure during initialization.
 */
esp_err_t tof_mngr_init(void)
{
    /* initialization */
    vl.i2cMasterInit(PIN_SDA, PIN_SCL);
    if (!vl.init()) {
        ESP_LOGE(TAG, "Failed to initialize VL53L0X.");
        return ESP_FAIL;
    }
    BaseType_t task_created =  xTaskCreate(tof_reader_task,
                                           CORE_TOF_READER_TASK_NAME,
                                           CORE_TOF_READER_TASK_STACK,
                                           NULL,
                                           CORE_TOF_READER_TASK_PRIO,
                                           NULL);
    if (task_created != pdPASS) {
        ESP_LOGE(TAG, "Failed to create %s task.", CORE_TOF_READER_TASK_NAME);
        return ESP_FAIL;
    }
    return ESP_OK;
}
