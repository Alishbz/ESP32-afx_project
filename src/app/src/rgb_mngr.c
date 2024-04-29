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
#include "app/rgb_mngr.h"

// Define the GPIO pins for the Red, Green, and Blue LEDs
#define RGB_MNGR_GPIO_LED_RED    (GPIO_NUM_26)
#define RGB_MNGR_GPIO_LED_GREEN  (GPIO_NUM_27)
#define RGB_MNGR_GPIO_LED_BLUE   (GPIO_NUM_25)

// Define colors as RGB values in hexadecimal
#define COLOR_NO        0x000000    // No color
#define COLOR_RED       0xFF0000    // Red color
#define COLOR_GREEN     0x00FF00    // Green color
#define COLOR_BLUE      0x0000FF    // Blue color
#define COLOR_YELLOW    0xFFFF00    // Yellow color (combination of Red and Green)
#define COLOR_VIOLET    0xEE82EE    // Violet color

static const char *TAG = "rgb_mngr";

static rgb_led_t s_rgb_if = {0};

typedef struct {
    uint32_t hex_rgb;
} rgb_color_map_t;

static rgb_color_map_t s_rgb_color_map[RGB_COLOR_MAX] = {
    {.hex_rgb = COLOR_NO},      /* RGB_NO_COLOR */
    {.hex_rgb = COLOR_RED},     /* RGB_COLOR_RED */
    {.hex_rgb = COLOR_GREEN},   /* RGB_COLOR_GREEN */
    {.hex_rgb = COLOR_BLUE},    /* RGB_COLOR_BLUE */
    {.hex_rgb = COLOR_YELLOW},  /* RGB_COLOR_YELLOW */
    {.hex_rgb = COLOR_VIOLET},  /* RGB_COLOR_VIOLET */
};

static rgb_status_t s_rgb_status = 0;

/**
 * @brief The function `rgb_mngr_set_color` sets the color of an RGB LED based on the input color status.
 *
 * @param color The `color` parameter is of type `rgb_status_t`, which is an enumeration representing
 * different RGB colors.
 */
void rgb_mngr_set_color(rgb_status_t color)
{
    ESP_LOGI(TAG, "%s handled!", __func__);
    if (color < RGB_COLOR_MAX) {
        ESP_LOGI(TAG, "Color status is %d. HEX0x%" PRIu32 "", color, s_rgb_color_map[color].hex_rgb);
        rgb_led_set_color(&s_rgb_if, s_rgb_color_map[color].hex_rgb);
        s_rgb_status = color;
    } else {
        ESP_LOGE(TAG, "Unsegmanted color status: %d", color);
    }
}

/**
 * @brief The function `rgb_mngr_get_color` returns the current RGB status.
 *
 * @return The function `rgb_mngr_get_color` is returning a variable of type `rgb_status_t` named
 * `s_rgb_status`.
 */
rgb_status_t rgb_mngr_get_color(void)
{
    return s_rgb_status;
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