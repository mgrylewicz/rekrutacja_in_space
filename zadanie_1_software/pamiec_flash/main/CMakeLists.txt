idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
    REQUIRES flash_manage CLI esp_driver_uart esp_vfs_console
)