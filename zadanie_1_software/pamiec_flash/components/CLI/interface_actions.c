#include "interface_actions.h"
#include "interface_view.h"
#include "flash_manage.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <math.h>

#include "nvs.h"

//zapis nowej zmiennej w NVS
void save_variable(){
    char var_type[16];
    char name[16];
    char value[32];
    char line[128];

    printf("Insert your variable:\n");
    printf("*type* *name* *value*\n");
    printf("Example:int var 8\n");
    //pobieramy całą linię z terminala
    read_line(line, sizeof(line));
    if(sscanf(line, "%15s %15s %31s", var_type, name, value) != 3){ //odczyt typu, nazwy i wartości oraz 
                                                                    //sprawdzenie liczby podanych danych
        printf("Incorrect input!\n");
        return;
    }

    nvs_type_t existing_type;
    esp_err_t err = get_variable_type(name, &existing_type);

    if(err == ESP_OK){ //nie nadpisujemy istniejącej zmiennej podczas dodawania nowej
        printf("Variable already exists!\n");
        return;
    }

    if(err != ESP_ERR_NVS_NOT_FOUND){//wystąpił błąd inny niż brak zmiennej
        printf("NVS error: %s\n", esp_err_to_name(err));
        return;
    }

    //sprawdzamy typ, konwertujemy wartość i zapisujemy ją w NVS
    if(strcmp("int", var_type) == 0){
        char *end;
        errno = 0;
        long converted = strtol(value, &end, 10);//konwersja tekstu na liczbę całkowitą
        //odrzucamy pustą lub niepoprawną liczbę
        if(end == value || *end != '\0'){
            printf("Incorrect number!\n");
            return;
        }
        //sprawdzamy, czy liczba mieści się w zakresie int32_t
        if(errno == ERANGE || converted < INT32_MIN || converted > INT32_MAX){
            printf("Number out of range!\n");
            return;
        }
        int32_t int_value = (int32_t)converted;//po sprawdzeniu zakresu konwertujemy na int32_t
        err = save_int(name, int_value);//zapis wartości w NVS; err przechowuje kod wyniku operacji
    }else if(strcmp("float", var_type) == 0){
        char *end;
        errno = 0;
        float float_value = strtof(value, &end);
        //odrzucamy pustą lub niepoprawną liczbę
        if(end == value || *end != '\0'){
            printf("Incorrect number!\n");
            return;
        }
        //odrzucamy wartości spoza zakresu oraz NaN i nieskończoność
        if(errno == ERANGE || !isfinite(float_value)){
            printf("Number out of range!\n");
            return;
        }
        err = save_float(name, float_value);
    }else if(strcmp("bool", var_type) == 0){
        bool bool_value;

        //dla bool akceptujemy tylko tekst true lub false
        if(strcmp(value, "true") == 0){
            bool_value = true;
        }else if(strcmp(value, "false") == 0){
            bool_value = false;
        }else{
            printf("Incorrect bool value! Use true or false.\n");
            return;
        }

        err = save_bool(name, bool_value);
    }else if(strcmp("string", var_type) == 0){
        err = save_string(name, value);
    }else{
        printf("Incorrect variable type\n");
        return;
    }

    if(err == ESP_OK){//wyświetlamy wynik operacji zapisu
        printf("Variable saved!\n");
    }else{
        printf("Save error: %s\n", esp_err_to_name(err));
    }
}

//zmiana wartości istniejącej zmiennej
void change_variable(void){
    char name[16];
    char value[32];
    char line[128];
    nvs_type_t type;

    printf("Insert variable to change:\n");
    printf("*name* *new_value*\n");
    printf("Example: temperature 24.5\n");

    //pobieramy całą linię z terminala
    read_line(line, sizeof(line));
    if(sscanf(line, "%15s %31s", name, value) != 2){
        printf("Incorrect input!\n");
        return;
    }

    //sprawdzamy, czy zmienna istnieje i jaki ma typ w NVS
    esp_err_t err = get_variable_type(name, &type);

    if(err == ESP_ERR_NVS_NOT_FOUND){
        printf("Variable does not exist!\n");
        return;
    }

    if(err != ESP_OK){
        printf("NVS error: %s\n", esp_err_to_name(err));
        return;
    }

    //dla każdego typu odczytujemy starą wartość, sprawdzamy nową i zapisujemy zmianę
    if (type == NVS_TYPE_I32) {
        int32_t old_value;

        err = get_int(name, &old_value);

        if(err != ESP_OK){
            printf("Read error: %s\n", esp_err_to_name(err));
            return;
        }

        char *end;
        errno = 0;

        long converted = strtol(value, &end, 10);

        if(end == value || *end != '\0' || errno == ERANGE || converted < INT32_MIN || converted > INT32_MAX){
            printf("Incorrect integer value!\n");
            return;
        }

        //jeśli wartość się nie zmieniła, pomijamy wywołanie zapisu NVS
        if(old_value == (int32_t)converted){
            printf("Value unchanged. NVS write skipped!\n");
            return;
        }

        err = save_int(name, (int32_t)converted);
        if(err != ESP_OK){
            printf("Save error: %s\n", esp_err_to_name(err));
            return;
        }

        printf("Variable updated!\n");
        printf("Old value: %ld\n", (long)old_value);
        printf("New value: %ld\n", converted); 
    }else if(type == NVS_TYPE_BLOB){
        float old_value;

        err = get_float(name, &old_value);

        if(err != ESP_OK){
            printf("Read error: %s\n", esp_err_to_name(err));
            return;
        }

        char *end;
        errno = 0;

        float converted = strtof(value, &end);

        if(end == value || *end != '\0' || errno == ERANGE || !isfinite(converted)){
            printf("Incorrect float value!\n");
            return;
        }

        //pomijamy zapis, gdy nowa wartość jest taka sama
        if(old_value == converted){
            printf("Value unchanged. NVS write skipped!\n");
            return;
        }

        err = save_float(name, converted);
        if (err != ESP_OK) {
            printf("Save error: %s\n", esp_err_to_name(err));
            return;
        }
    
        printf("Variable updated!\n");
        printf("Old value: %f\n", old_value);
        printf("New value: %f\n", converted);
        
    }else if(type == NVS_TYPE_U8){
        bool old_value;
        bool new_value;

        err = get_bool(name, &old_value);

        if(err != ESP_OK){
            printf("Read error: %s\n", esp_err_to_name(err));
            return;
        }

        //dla bool akceptujemy tylko tekst true lub false
        if(strcmp(value, "true") == 0){
            new_value = true;
        }else if(strcmp(value, "false") == 0){
            new_value = false;
        }else{
            printf("Incorrect bool value! Use true or false.\n");
            return;
        }

        //pomijamy zapis, gdy wartość logiczna się nie zmieniła
        if(old_value == new_value){
            printf("Value unchanged. NVS write skipped!\n");
            return;
        }

        err = save_bool(name, new_value);
        if (err != ESP_OK) {
            printf("Save error: %s\n", esp_err_to_name(err));
            return;
        }
        

        printf("Variable updated!\n");
        printf("Old value: %s\n", old_value ? "true" : "false");
        printf("New value: %s\n", new_value ? "true" : "false");
        
    }else if(type == NVS_TYPE_STR){
        char old_value[128];

        err = get_string(name, old_value, sizeof(old_value));

        if(err != ESP_OK){
            printf("Read error: %s\n", esp_err_to_name(err));
            return;
        }

        //porównujemy zawartość napisów, a nie ich adresy
        if(strcmp(old_value, value) == 0){
            printf("Value unchanged. NVS write skipped!\n");
            return;
        }

        err = save_string(name, value);
        if (err != ESP_OK) {
            printf("Save error: %s\n", esp_err_to_name(err));
            return;
        }
        
        printf("Variable updated!\n");
        printf("Old value: %s\n", old_value);
        printf("New value: %s\n", value);
        
    }else{
        printf("Unsupported variable type!\n");
    }
}
//odczyt i wyświetlenie wybranej zmiennej
void read_variable(){
    char name[16];
    char line[128];
    printf("What variable do you want to read?\n");
    printf("Instert variable's name:\n");
    //pobieramy całą linię z terminala
    read_line(line, sizeof(line));
    if(sscanf(line, "%15s", name) != 1){
        printf("Incorrect input!\n");
        return;
    }
    
    nvs_type_t type;

    //sprawdzamy, czy zmienna istnieje i jaki ma typ w NVS
    esp_err_t err = get_variable_type(name, &type);

    if(err == ESP_OK){
        if(type == NVS_TYPE_I32){
            int32_t int_value;
            err = get_int(name, &int_value);
            if(err == ESP_OK){
                printf("type: int\nname: %15s\nvalue: %ld\n", name, (long)int_value);
            }else{
                printf("Error: %s\n", esp_err_to_name(err));
            }
            
        }else if(type == NVS_TYPE_STR){
            char string_value[64];
            esp_err_t err = get_string(name, string_value, sizeof(string_value));

            if(err == ESP_OK){
                printf("type: string\nname: %s\nvalue: %s\n", name, string_value);
            }else if(err == ESP_ERR_NVS_INVALID_LENGTH){
                printf("Buffer too small!\n");
            }else{
                printf("Error: %s\n", esp_err_to_name(err));
            }
        }else if(type == NVS_TYPE_U8){
            bool bool_value;
            err = get_bool(name, &bool_value);
            if(err == ESP_OK){
                printf("type: bool\nname: %15s\nvalue: %s\n", name, bool_value ? "true" : "false");
            }else{
                printf("Error: %s\n", esp_err_to_name(err));
            }
        }else if(type == NVS_TYPE_BLOB){
            float float_value;
            err = get_float(name, &float_value);
            if(err == ESP_OK){
                printf("type: float\nname: %15s\nvalue: %f\n", name, float_value);
            }else{
                printf("Error: %s\n", esp_err_to_name(err));
            }
        }else {
            printf("Program does not manage this variable type\n");
        }
    }else if (err == ESP_ERR_NVS_NOT_FOUND){
        printf("Variable does not exist!\n");
    }else{
        printf("Error: %s\n", esp_err_to_name(err));
    }


}

//usunięcie zmiennej o podanej nazwie
void delete_variable(){
    char name[16];
    char line[128];
    printf("What variable do you want to delete?\n");
    printf("Insert variable's name:\n");
    //pobieramy całą linię z terminala
    read_line(line, sizeof(line));
    if(sscanf(line, "%15s", name) != 1){
        printf("Incorrect input!\n");
        return;
    }

    //usuwamy klucz z NVS i sprawdzamy wynik operacji
    esp_err_t err = delete_key(name);
    if(err == ESP_OK){
        printf("Variable deleted!\n");
    }else if(err == ESP_ERR_NVS_NOT_FOUND){
        printf("Variable '%s' does not exist!\n", name);
    }else{
        printf("Failed to delete variable '%s'\nError: %s\n", name, esp_err_to_name(err));
    }
}

//wyświetlanie wszystkich zmiennych zapisanych w NVS
void show_variables(void){
    nvs_iterator_t it = NULL;

    //wyszukujemy pierwszy wpis w przestrzeni nazw storage na partycji nvs
    esp_err_t err = nvs_entry_find("nvs", "storage", NVS_TYPE_ANY, &it);

    if(err == ESP_ERR_NVS_NOT_FOUND){
        printf("No variables saved!\n");
        return;
    }

    if(err != ESP_OK){
        printf("NVS error: %s\n", esp_err_to_name(err));
        return;
    }

    printf("Saved variables:\n");

    //przechodzimy po wszystkich zapisanych wpisach
    while(err == ESP_OK){
        nvs_entry_info_t info;
        nvs_entry_info(it, &info);

        //odczytujemy i wyświetlamy wartość zgodnie z typem wpisu

        if(info.type == NVS_TYPE_I32){
            int32_t value;

            esp_err_t read_err = get_int(info.key, &value);

            if(read_err == ESP_OK){
                printf("int %s %ld\n", info.key, (long)value);
            }else{
                printf("Read error for %s: %s\n", info.key, esp_err_to_name(read_err));
            }
        }else if(info.type == NVS_TYPE_BLOB){
            float value;

            esp_err_t read_err = get_float(info.key, &value);

            if(read_err == ESP_OK){
                printf("float %s %f\n", info.key, value);
            }else{
                printf("Read error for %s: %s\n", info.key, esp_err_to_name(read_err));
            }
        }else if(info.type == NVS_TYPE_U8){
            bool value;

            esp_err_t read_err = get_bool(info.key, &value);

            if(read_err == ESP_OK){
                printf("bool %s %s\n", info.key, value ? "true" : "false");
            }else{
                printf("Read error for %s: %s\n", info.key, esp_err_to_name(read_err));
            }
        }else if(info.type == NVS_TYPE_STR){
            char value[128];

            esp_err_t read_err = get_string(info.key, value, sizeof(value));

            if(read_err == ESP_OK){
                printf("string %s %s\n", info.key, value);
            }else if(read_err == ESP_ERR_NVS_INVALID_LENGTH){
                printf("String %s is too long!\n", info.key);
            }else{
                printf("Read error for %s: %s\n", info.key, esp_err_to_name(read_err));
            }
        }else{
            printf("Unsupported type for %s\n", info.key);
        }

        //przechodzimy do następnego wpisu
        err = nvs_entry_next(&it);
    }

    //zwalniamy iterator, jeśli nadal jest aktywny
    if(it != NULL){
        nvs_release_iterator(it);
    }

    if(err != ESP_ERR_NVS_NOT_FOUND){
        printf("Iteration error: %s\n", esp_err_to_name(err));
    }
}

//wyświetlanie dostępnych poleceń CLI
void command_help(){
    printf("Available commands:\n");
    printf("'save' - save a new variable\n");
    printf("\tInput:*type* *name* *value*\n");
    printf("'change' - change an existing variable\n");
    printf("\tInput:*name* *value*\n");
    printf("'read' - read variable\n");
    printf("\tInput:*name*\n");
    printf("'delete' - delete variable\n");
    printf("\tInput:*name*\n");
    printf("'show' - show all saved variables\n");
    printf("'help' - show available commands\n");
}