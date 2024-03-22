/**
 * @file tof_mngr.h
 * @author Hamza Sengul (hamza.sengul@saykal.com)
 * @brief
 * @version 0.1
 * @date 2024-03-22
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "esp_err.h"

/**
 * @brief The function `tof_reader_get_range` returns the range value in millimeters from a time-of-flight
 * sensor.
 *
 * @return The function `tof_reader_get_range` is returning a `uint16_t` value, which is the result of
 * the variable `s_result_mm`.
 */
uint16_t tof_reader_get_range(void);

/**
 * @brief The function `tof_mngr_init` initializes a Time-of-Flight sensor and creates a task for reading
 * sensor data.
 *
 * @return The function `tof_mngr_init` is returning an `esp_err_t` value. It will return `ESP_OK` if
 * the initialization is successful, and `ESP_FAIL` if there is a failure during initialization.
 */
esp_err_t tof_mngr_init(void);
