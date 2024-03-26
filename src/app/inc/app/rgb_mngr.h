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

/**
 * @brief The function `rgb_mngr_set_color` sets the color of an RGB LED and logs the color value.
 *
 * @param color It seems like there is a mistake in the code snippet you provided. The `ESP_LOGI`
 * function expects a format specifier for the second argument, but it is missing in the code. To fix
 * this issue, you should provide the format specifier for the `color` argument.
 */
void rgb_mngr_set_color(uint32_t color);

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