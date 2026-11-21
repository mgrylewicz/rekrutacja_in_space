idf_component_register(
    SRCS
        "interface_actions.c"
        "interface_view.c"
    INCLUDE_DIRS "."
    REQUIRES flash_manage nvs_flash
)