set(sources

)

idf_component_register(
    SRCS ${sources}
    INCLUDE_DIRS . inc inc/core
    REQUIRES freertos
            esp_timer
            esp_lcd
            driver
)
