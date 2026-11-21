#include "interface_view.h"
#include "interface_actions.h"
#include "flash_manage.h"

#include <stdio.h>
#include <string.h>

//odczytuje całą linię i wyświetla wpisywane znaki (echo)
//zastępuje scanf, z którym użytkownik nie widział co wpisuje do terminala
void read_line(char *buffer, size_t size){
    size_t i = 0;
    int c;

    while(1){
        c = getchar();

        if(c == EOF){
            clearerr(stdin);
            continue;
        }

        if(c == '\r' || c == '\n'){
            putchar('\n');
            break;
        }

        if(c == '\b' || c == 127){
            if(i > 0){
                i--;
                printf("\b \b");
            }
            continue;
        }

        if(i < size - 1){
            buffer[i++] = (char)c;
            putchar(c);  // Echo wpisanego znaku
            fflush(stdout);
        }
    }

    buffer[i] = '\0';
}

//dodaje opcję do menu, o ile jest jeszcze wolne miejsce
void add_option(menu_node * menu,const char * name){
    if(menu == NULL || name == NULL){
        return;
    }

    if(menu->children_num >= 16){
        printf("Menu is full!\n");
        return;
    }

    char *copy = strdup(name);
    if(copy == NULL){
        printf("Memory allocation failed!\n");
        return;
    }

    menu->option_table[menu->children_num] = copy;
    menu->children_num++;
}

//tworzy menu i dodaje wszystkie dostępne opcje
menu_node * init_menu(){
    menu_node *menu = calloc(1, sizeof(menu_node));

    if(menu == NULL){
        return NULL;
    }

    menu->name = strdup("Main");

    if(menu->name == NULL){
        free(menu);
        return NULL;
    }

    add_option(menu, "Save variable");
    add_option(menu, "Change variable");
    add_option(menu, "Read variable");
    add_option(menu, "Delete variable");
    add_option(menu, "Show all variables");
    add_option(menu, "Help");

    return menu;
}



//wyświetla nazwę menu i wszystkie dodane opcje
void show_menu(menu_node * menu){
    if(menu == NULL){
        return;
    }

    printf("%s\n", menu->name);

    for(int i = 0; i < menu->children_num; ++i){
        printf("\t%s\n", menu->option_table[i]);
    }
}

//rozpoznaje przekazaną komendę i wywołuje odpowiednią funkcję
void action(const char * name){
    if (name == NULL) {
        return;
    }
    
    if(strcmp("save", name) == 0){
        save_variable();
    }else if(strcmp("change", name) == 0){
        change_variable();
    }else if(strcmp("read", name) == 0){
        read_variable();
    }else if(strcmp("delete", name) == 0){
        delete_variable();
    }else if(strcmp("show", name) == 0){
        show_variables();
    }else if(strcmp("help", name) == 0){
        command_help();
    }else{
        printf("No such command: %s\n", name);
        command_help();
    }

}

//inicjalizuje domyślne zmienne o ile jeszcze nie są w pamięci flash
void init_default_variables(void){
    nvs_type_t type;

    if(get_variable_type("engine_power", &type) == ESP_ERR_NVS_NOT_FOUND){
        save_int("engine_power", 50);
    }

    if(get_variable_type("fuel_limit", &type) == ESP_ERR_NVS_NOT_FOUND){
        save_float("fuel_limit", 75.5f);
    }

    if(get_variable_type("safety_mode", &type) == ESP_ERR_NVS_NOT_FOUND) {
        save_bool("safety_mode", true);
    }

    if(get_variable_type("project_name", &type) == ESP_ERR_NVS_NOT_FOUND) {
        save_string("project_name", "Flash");
    }
}

//zwalnia pamięć nazw opcji, nazwy menu i samej struktury
void free_menu(menu_node * menu){
    if(menu == NULL){
        return;
    }

    for(int i = 0; i < menu->children_num; ++i){
        free(menu->option_table[i]);
    }

    free(menu->name);
    free(menu);
}