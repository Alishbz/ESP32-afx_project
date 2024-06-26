/**
 * @file canbus_mngr.c
 * @author Hamza Sengul (hamza.sengul@alpelectronix.com)
 * @brief
 * @version 0.1
 * @date 2024-03-25
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
#include "driver/twai.h"

/* Application Layer Includes */
#include "app/app_config.h"
#include "app/app_types.h"
#include "app/rgb_mngr.h"
#include "app/tof_mngr.h"

static const char *TAG = "canbus_mngr";

#define CANBUS_MNGR_TX_PERIOD_MS    (100)
#define TX_GPIO_NUM                 (GPIO_NUM_0)
#define RX_GPIO_NUM                 (GPIO_NUM_15)
#define CAN_MSG_BASE_ID             (0x100)
#define CAN_MSG_TX_ID               (CAN_MSG_BASE_ID + DEVICE_ID)       //11 bit standard format ID
#define CAN_MSG_RX_ID               (2 * CAN_MSG_BASE_ID + DEVICE_ID)   //11 bit standard format ID

static const twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
//Filter all other IDs except CAN_MSG_RX_ID
static const twai_filter_config_t f_config = {.acceptance_code = (CAN_MSG_RX_ID << 3),
                                              .acceptance_mask = ~(TWAI_EXTD_ID_MASK << 3),
                                              .single_filter = true
                                             };
//Set to NO_ACK mode due to self testing with single module
static const twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(TX_GPIO_NUM, RX_GPIO_NUM, TWAI_MODE_NORMAL);

typedef union {
    struct {
        uint16_t current_error : 1;
        uint16_t software_error : 1;
        uint16_t first_reset : 1;
        uint16_t distance_out_range_error : 1;
        uint16_t reserved : 12;
    };
    uint16_t word;
} state_union_t;

typedef union {
    struct {
        uint16_t go_to_reset : 1;
        uint16_t reserved : 15;
    };
    uint16_t word;
} command_state_union_t;

typedef struct {
    uint8_t status;
    uint8_t reserved;
} rgb_state_t;

typedef struct {
    uint32_t val;
} distance_tof_t;

typedef struct {
    state_union_t state;
    rgb_state_t rgb;
    distance_tof_t distance;
} __attribute__((packed)) can_comm_frame_t;

static void canbus_tx_task(void *arg)
{
    ESP_LOGI(TAG, "%s started!", __func__);
    twai_message_t tx_msg = {.data_length_code = 8, .identifier = CAN_MSG_TX_ID, .extd = 1};
    uint8_t f_rst_s = 1;
    while (pdTRUE) {
        //twai_start();

        /**
         * @brief Prepare CAN Bus message here.
         *
         */
        can_comm_frame_t can_tx_msg = {0};
        can_tx_msg.state.first_reset = f_rst_s;
        can_tx_msg.rgb.status   = rgb_mngr_get_color();
        can_tx_msg.distance.val = tof_reader_get_range();
        can_tx_msg.state.distance_out_range_error = tof_reader_get_error();
        //Transmit messages using self reception request
        memcpy(tx_msg.data, &can_tx_msg, tx_msg.data_length_code);
        if (twai_transmit(&tx_msg, 100) != ESP_OK) {
            ESP_LOGE(TAG, "CAN BUS TX ERROR!");
            twai_clear_transmit_queue();
        } else {
            ESP_LOGD(TAG, "CAN Bus Message sent - Data: [0x%x] [0x%x] [0x%x] [0x%x] [0x%x] [0x%x] [0x%x] [0x%x] ", tx_msg.data[0], tx_msg.data[1], tx_msg.data[2], tx_msg.data[3], tx_msg.data[4], tx_msg.data[5], tx_msg.data[6], tx_msg.data[7]);
            /* Clear first reset state */
            f_rst_s = 0;
        }
        //twai_stop();
        vTaskDelay(pdMS_TO_TICKS(CANBUS_MNGR_TX_PERIOD_MS));
    }
    vTaskDelete(NULL);
}

static void canbus_rx_process_command(can_comm_frame_t *can_rx_msg)
{
    ESP_LOGW(TAG, "%s handled!", __func__);
    rgb_mngr_set_color(can_rx_msg->rgb.status);
    command_state_union_t state = {0};
    state.word = can_rx_msg->state.word;
    if (state.go_to_reset) {
        esp_restart();
    }
}

static void canbus_rx_task(void *arg)
{
    ESP_LOGI(TAG, "%s started!", __func__);
    twai_message_t rx_message;
    can_comm_frame_t can_rx_msg = {0};
    while (pdTRUE) {
        //Receive message and print message data
        if (twai_receive(&rx_message, portMAX_DELAY) == ESP_OK) {
            ESP_LOGI(TAG, "CAN Bus Message sent - Data: [0x%x] [0x%x] [0x%x] [0x%x] [0x%x] [0x%x] [0x%x] [0x%x] ", rx_message.data[0], rx_message.data[1], rx_message.data[2], rx_message.data[3], rx_message.data[4], rx_message.data[5], rx_message.data[6], rx_message.data[7]);
            if (rx_message.data_length_code == sizeof(can_rx_msg)) {
                memcpy(&can_rx_msg, rx_message.data, rx_message.data_length_code);
                canbus_rx_process_command(&can_rx_msg);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    vTaskDelete(NULL);
}

esp_err_t canbus_mngr_init(void)
{
    //Install TWAI driver
    ESP_ERROR_CHECK(twai_driver_install(&g_config, &t_config, &f_config));
    ESP_LOGI(TAG, "TWAI driver installed");

    ESP_ERROR_CHECK(twai_start());
    ESP_LOGI(TAG, "TWAI driver started");
    //xTaskCreatePinnedToCore(twai_receive_task, "TWAI_rx", 4096, NULL, RX_TASK_PRIO, NULL, tskNO_AFFINITY);
    //xTaskCreatePinnedToCore(twai_transmit_task, "TWAI_tx", 4096, NULL, TX_TASK_PRIO, NULL, tskNO_AFFINITY);


    BaseType_t task_created =  xTaskCreate(canbus_tx_task,
                                           CORE_CANBUS_TX_TASK_NAME,
                                           CORE_CANBUS_TX_TASK_STACK,
                                           NULL,
                                           CORE_CANBUS_TX_TASK_PRIO,
                                           NULL);
    if (task_created != pdPASS) {
        ESP_LOGE(TAG, "Failed to create %s task.", CORE_CANBUS_TX_TASK_NAME);
        return ESP_FAIL;
    }

    task_created =  xTaskCreate(canbus_rx_task,
                                CORE_CANBUS_RX_TASK_NAME,
                                CORE_CANBUS_RX_TASK_STACK,
                                NULL,
                                CORE_CANBUS_RX_TASK_PRIO,
                                NULL);
    if (task_created != pdPASS) {
        ESP_LOGE(TAG, "Failed to create %s task.", CORE_CANBUS_RX_TASK_NAME);
        return ESP_FAIL;
    }

    return ESP_OK;
}
