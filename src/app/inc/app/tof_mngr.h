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

#ifndef _TOF_MNGR_H_
#define _TOF_MNGR_H_

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief The function `tof_reader_get_range` returns the range value in millimeters from a time-of-flight
 * sensor.
 *
 * @return The function `tof_reader_get_range` is returning a `uint16_t` value, which is the result of
 * the variable `s_result_mm`.
 */
uint16_t tof_reader_get_range(void);

/**
 * @brief The function `tof_reader_get_error` returns the error code stored in the variable
 * `s_read_error`.
 *
 * @return The function `tof_reader_get_error` is returning a `uint16_t` value, which is the
 * error code stored in the variable `s_read_error`.
 */
uint16_t tof_reader_get_error(void);

#ifdef __cplusplus
}
#endif
/**
 * @brief The function `tof_mngr_init` initializes a Time-of-Flight sensor and creates a task for reading
 * sensor data.
 *
 * @return The function `tof_mngr_init` is returning an `esp_err_t` value. It will return `ESP_OK` if
 * the initialization is successful, and `ESP_FAIL` if there is a failure during initialization.
 */
esp_err_t tof_mngr_init(void);

#endif
