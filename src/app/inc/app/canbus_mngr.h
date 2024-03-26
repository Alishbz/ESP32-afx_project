/**
 * @file canbus_mngr.h
 * @author Hamza Sengul (hamza.sengul@alpelectronix.com)
 * @brief
 * @version 0.1
 * @date 2024-03-26
 *
 * @copyright Copyright (c) 2024
 *
 */


#ifndef _CANBUS_MNGR_H_
#define _CANBUS_MNGR_H_

#include "esp_err.h"
#ifdef __cplusplus
extern "C" {
#endif

esp_err_t canbus_mngr_init(void);

#ifdef __cplusplus
}
#endif

#endif