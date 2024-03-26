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
#include "drv/rgb_ledc_controller.h"
#include "canbus_mngr.h"

static const char *TAG = "app_mngr";

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

#ifdef DEBUG_BUILD
static void print_app_info(void)
{
    const esp_partition_t *running = esp_ota_get_running_partition();
    esp_app_desc_t app_desc;
    ESP_ERROR_CHECK(esp_ota_get_partition_description(running, &app_desc));

    ESP_LOGI(TAG, "%s %s has started from @%" PRIu32 "",
             app_desc.project_name,
             app_desc.version,
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


// Define the GPIO pins for the Red, Green, and Blue LEDs
#define GPIO_LED_RED    (GPIO_NUM_26)
#define GPIO_LED_GREEN  (GPIO_NUM_27)
#define GPIO_LED_BLUE   (GPIO_NUM_25)

// Define colors as RGB values in hexadecimal
#define RED     0xFF0000 // Red color
#define GREEN   0x00FF00 // Green color
#define BLUE    0x0000FF // Blue color
#define YELLOW  0xFFFF00 // Yellow color (combination of Red and Green)
#define VIOLET  0xEE82EE // Violet color

    // Create an instance of the RGB LED structure and initialize it with the defined GPIO pins and LEDC channels
    rgb_led_t led1 = rgb_led_new(GPIO_LED_RED, GPIO_LED_GREEN, GPIO_LED_BLUE, LEDC_CHANNEL_0, LEDC_CHANNEL_1, LEDC_CHANNEL_2);
    //...you can add one more eg: led2.It is limited to 2 leds due to the LEDC module having only 8 channels available.

    // Initialize the RGB LED, check for any errors during the process
    ESP_ERROR_CHECK(rgb_led_init(&led1));

    // Main loop of the program
    while (true) {

        // Start a blinking effect with the Green color. Blink 3 times, each blink lasts 500ms, and the time between blinks is 500ms. The total effect duration is 3000ms.
        rgb_led_start_blink_effect(&led1, RED, 3, 500, 500, 3000);
        vTaskDelay(pdMS_TO_TICKS(5000)); // Delay for 5 seconds


        // Start a blinking effect with the Green color. Blink 3 times, each blink lasts 500ms, and the time between blinks is 500ms. The total effect duration is 3000ms.
        rgb_led_start_blink_effect(&led1, GREEN, 3, 500, 500, 3000);
        vTaskDelay(pdMS_TO_TICKS(5000)); // Delay for 5 seconds

        // Start a blinking effect with the Green color. Blink 3 times, each blink lasts 500ms, and the time between blinks is 500ms. The total effect duration is 3000ms.
        rgb_led_start_blink_effect(&led1, BLUE, 3, 500, 500, 3000);
        vTaskDelay(pdMS_TO_TICKS(5000)); // Delay for 5 seconds


        // Start a blinking effect with the Yellow color. Blink 3 times, each blink lasts 500ms, and the time between blinks is 500ms. The total effect duration is 3000ms.
        rgb_led_start_blink_effect(&led1, YELLOW, 3, 500, 500, 3000);
        vTaskDelay(pdMS_TO_TICKS(5000)); // Delay for 5 seconds

        // Set the LED color Fixed to Violet
        rgb_led_set_color(&led1, YELLOW);

        rgb_led_start_transition_effect(&led1, YELLOW, RED, 10, NULL);
        vTaskDelay(pdMS_TO_TICKS(5000)); // Delay for 5 seconds
    }

#ifdef DEBUG_BUILD
    print_heap_usage("after first init");
#endif

    return ESP_OK;
}
