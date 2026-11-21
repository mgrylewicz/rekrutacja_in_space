#include <stdio.h>
#include "driver/uart.h"
#include "esp_vfs_dev.h"
#include "interface_view.h"
#include "flash_manage.h"

//główna funkcja programu uruchamiana przez ESP-IDF
void app_main(void)
{
    init_memory();
    init_default_variables();

    esp_err_t err = uart_driver_install(UART_NUM_0, 1024, 0, 0, NULL, 0);

    if(err != ESP_OK){
        printf("UART error: %s\n", esp_err_to_name(err));
        return;
    }

    esp_vfs_dev_uart_use_driver(UART_NUM_0);
    setvbuf(stdin, NULL, _IONBF, 0);

    menu_node *menu = init_menu();

    if(menu == NULL){
        printf("Menu initialization failed!\n");
        return;
    }

    show_menu(menu);

    char command[16];

    while (1) {
        printf("\nCommand: ");
        fflush(stdout);

        read_line(command, sizeof(command));

        if (command[0] == '\0') {
            continue;
        }

        action(command);
    }

    free_menu(menu);
}

