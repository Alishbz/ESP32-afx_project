/**
 * @file rgb_mngr.c
 * @author Hamza Sengul (hamza.sengul@alpelectronix.com)
 * @brief
 * @version 0.1
 * @date 2024-03-26
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

/* Driver Layer Includes */
#include "drv/rgb_ledc_controller.h"

/* Application Layer Includes */
#include "app/app_config.h"
#include "app/app_types.h"

// Define the GPIO pins for the Red, Green, and Blue LEDs
#define RGB_MNGR_GPIO_LED_RED    (GPIO_NUM_26)
#define RGB_MNGR_GPIO_LED_GREEN  (GPIO_NUM_27)
#define RGB_MNGR_GPIO_LED_BLUE   (GPIO_NUM_25)

// Define colors as RGB values in hexadecimal
#define RED     0xFF0000 // Red color
#define GREEN   0x00FF00 // Green color
#define BLUE    0x0000FF // Blue color
#define YELLOW  0xFFFF00 // Yellow color (combination of Red and Green)
#define VIOLET  0xEE82EE // Violet color

static const char *TAG = "rgb_mngr";

static rgb_led_t s_rgb_if = {0};

/**
 * @brief The function `rgb_mngr_set_color` sets the color of an RGB LED and logs the color value.
 *
 * @param color It seems like there is a mistake in the code snippet you provided. The `ESP_LOGI`
 * function expects a format specifier for the second argument, but it is missing in the code. To fix
 * this issue, you should provide the format specifier for the `color` argument.
 */
void rgb_mngr_set_color(uint32_t color)
{
    ESP_LOGI(TAG, "%s handled! Color is 0x%" PRIu32 "", __func__, color);
    rgb_led_set_color(&s_rgb_if, color);
}

/**
 * @brief The function `rgb_mngr_init` initializes an RGB LED structure with defined GPIO pins and LEDC
 * channels, checking for errors during the process.
 *
 * @return The function `rgb_mngr_init` is returning an `esp_err_t` value. If the RGB LED
 * initialization is successful, it will return `ESP_OK`. If there is an error during initialization,
 * it will return `ESP_FAIL`.
 */
esp_err_t rgb_mngr_init(void)
{
    // Create an instance of the RGB LED structure and initialize it with the defined GPIO pins and LEDC channels
    s_rgb_if = rgb_led_new(RGB_MNGR_GPIO_LED_RED, RGB_MNGR_GPIO_LED_GREEN, RGB_MNGR_GPIO_LED_BLUE, LEDC_CHANNEL_0, LEDC_CHANNEL_1, LEDC_CHANNEL_2);

    // Initialize the RGB LED, check for any errors during the process
    if (rgb_led_init(&s_rgb_if) != ESP_OK) {
        return ESP_FAIL;
    }

    return ESP_OK;
}