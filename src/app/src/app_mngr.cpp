/**
 * @file app_mngr.cpp
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
#include "esp_ota_ops.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "nvs_flash.h"

/* Kernel Includes */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* Core Layer Includes */
#include "core_includes.h"

/* Application Layer Includes */
#include "app/app_config.h"
#include "app/app_types.h"
#include "app/tof_mngr.h"
#include "app/rgb_mngr.h"
#include "app/canbus_mngr.h"

static const char *TAG = "app_mngr";
#define APP_MAIN_STATUS_TASK_PERIOD_MS      (500)

#ifdef DEBUG_BUILD
static void print_app_info(void)
{
    const esp_partition_t *running = esp_ota_get_running_partition();
    esp_app_desc_t app_desc;
    ESP_ERROR_CHECK(esp_ota_get_partition_description(running, &app_desc));

    ESP_LOGI(TAG, "%s %s Device ID:%d has started from @%" PRIu32 "",
             app_desc.project_name,
             app_desc.version,
             DEVICE_ID,
             running->address
            );
}

static void print_heap_usage(const char *msg)
{
    ESP_LOGW(TAG, "(%s):free_heap/min_heap size %" PRIu32 "/%" PRIu32 " Bytes",
             msg,
             esp_get_free_heap_size(),
             esp_get_minimum_free_heap_size());
}

#endif

static void app_status_task(void *arg)
{
    ESP_LOGI(TAG, "%s started!", __func__);
    while (pdTRUE) {
        ESP_LOGI(TAG, "%s handled!", __func__);
        print_heap_usage("app status:");
        vTaskDelay(pdMS_TO_TICKS(APP_MAIN_STATUS_TASK_PERIOD_MS));
    }
    vTaskDelete(NULL);
}

static void app_core_init(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}

esp_err_t app_start(void)
{
#ifdef DEBUG_BUILD
    esp_log_level_set("*", ESP_LOG_INFO);           // set all components to INFO level
#else
    esp_log_level_set("*", ESP_LOG_NONE);           // disable logs for all components
#endif

    app_core_init();

#ifdef DEBUG_BUILD
    print_app_info();
#endif

    esp_err_t status = ESP_OK;
    status |= tof_mngr_init();
    status |= canbus_mngr_init();
    status |= rgb_mngr_init();

    BaseType_t task_created =  xTaskCreate(app_status_task,
                                           CORE_APP_STATUS_TASK_NAME,
                                           CORE_APP_STATUS_TASK_STACK,
                                           NULL,
                                           CORE_APP_STATUS_TASK_PRIO,
                                           NULL);
    if (task_created != pdPASS) {
        ESP_LOGE(TAG, "Failed to create %s task.", CORE_APP_STATUS_TASK_NAME);
        status |= ESP_FAIL;
    }

    if (status != ESP_OK) {
        ESP_LOGE(TAG, "first init failed!");
        return ESP_FAIL;
    }


#ifdef DEBUG_BUILD
    print_heap_usage("after first init");
#endif

    return ESP_OK;
}
