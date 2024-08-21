set(sources
    src/app_mngr.cpp
    #src/tof_mngr.cpp
    #src/canbus_mngr.c
    #src/rgb_mngr.c
    src/wifi_mngr.c
    src/esp_rest_main.c
    src/rest_server.c
)

idf_component_register(
    SRCS ${sources}
    INCLUDE_DIRS . inc inc/app
    REQUIRES freertos
            esp_system
            efuse
            lwip
            esp_netif
            esp_event
            esp_wifi
            esp_http_server
            spiffs
            vfs
            mdns
            nvs_flash
            app_update
            core
            drv
            json
)

set(WEB_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../src/front/web-demo")
if(EXISTS ${WEB_SRC_DIR}/dist)
    spiffs_create_partition_image(www ${WEB_SRC_DIR}/dist FLASH_IN_PROJECT)
else()
    message(FATAL_ERROR "${WEB_SRC_DIR}/dist doesn't exit. Please run 'npm run build' in ${WEB_SRC_DIR}")
endif()

target_compile_features(${COMPONENT_LIB} PRIVATE cxx_std_20)
