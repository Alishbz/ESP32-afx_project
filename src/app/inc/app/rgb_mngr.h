/**
 * @file rgb_mngr.h
 * @author Hamza Sengul (hamza.sengul@alpelectronix.com)
 * @brief
 * @version 0.1
 * @date 2024-03-26
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef _RGB_MNGR_H_
#define _RGB_MNGR_H_

#include "esp_err.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    RGB_NO_COLOR,
    RGB_COLOR_RED,
    RGB_COLOR_GREEN,
    RGB_COLOR_BLUE,
    RGB_COLOR_YELLOW,
    RGB_COLOR_VIOLET,
    RGB_COLOR_MAX,
} rgb_status_t;

/**
 * @brief The function `rgb_mngr_set_color` sets the color of an RGB LED based on the input color status.
 *
 * @param color The `color` parameter is of type `rgb_status_t`, which is an enumeration representing
 * different RGB colors.
 */
void rgb_mngr_set_color(rgb_status_t color);

/**
 * @brief The function `rgb_mngr_get_color` returns the current RGB status.
 *
 * @return The function `rgb_mngr_get_color` is returning a variable of type `rgb_status_t` named
 * `s_rgb_status`.
 */
rgb_status_t rgb_mngr_get_color(void);

/**
 * @brief The function `rgb_mngr_init` initializes an RGB LED structure with defined GPIO pins and LEDC
 * channels, checking for errors during the process.
 *
 * @return The function `rgb_mngr_init` is returning an `esp_err_t` value. If the RGB LED
 * initialization is successful, it will return `ESP_OK`. If there is an error during initialization,
 * it will return `ESP_FAIL`.
 */
esp_err_t rgb_mngr_init(void);

#ifdef __cplusplus
}
#endif

#endif