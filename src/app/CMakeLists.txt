set(sources
    src/app_mngr.c
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
            nvs_flash
            app_update
            core
            json
)

target_compile_features(${COMPONENT_LIB} PRIVATE cxx_std_20)
