#include "flash_manage.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "esp_err.h"
#include "nvs_flash.h"
#include "nvs.h"
 
//inicjalizuje pamięć NVS przed wykonywaniem operacji na zmiennych
void init_memory(){
    esp_err_t err = nvs_flash_init();

    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND){
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }

    ESP_ERROR_CHECK(err);
}
//zapisuje int32_t pod wskazanym kluczem
esp_err_t save_int(const char* name, int32_t value){
    nvs_handle_t handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &handle);
    if(err != ESP_OK){
        return err;
    }

    err = nvs_set_i32(handle, name, value);

    if(err == ESP_OK){
        err = nvs_commit(handle);
    }

    nvs_close(handle);
    return err;
}

//odczytuje int32_t spod wskazanego klucza
esp_err_t get_int(const char* name, int32_t *value){
    nvs_handle_t handle;

    esp_err_t err = nvs_open("storage", NVS_READONLY, &handle);
    if(err != ESP_OK){
        return err;
    }

    err = nvs_get_i32(handle, name, value);

    nvs_close(handle);
    return err;

}

//zapisuje float jako BLOB, ponieważ NVS nie ma osobnego typu float
esp_err_t save_float(const char* name, float value){
    nvs_handle_t handle;
    esp_err_t err = nvs_open("storage",  NVS_READWRITE, &handle);
    if(err != ESP_OK){
        return err;
    }

    err = nvs_set_blob(handle, name, &value, sizeof(value));

    if(err == ESP_OK){
        err = nvs_commit(handle);
    }
    
    nvs_close(handle);
    return err;
}

//odczytuje BLOB i sprawdza, czy ma rozmiar float
esp_err_t get_float(const char *name, float *value){
    nvs_handle_t handle;
    esp_err_t err = nvs_open("storage", NVS_READONLY, &handle);
    if (err != ESP_OK) {
        return err;
    }

    size_t size = sizeof(*value);

    err = nvs_get_blob(handle, name, value, &size);

    if(err == ESP_OK && size != sizeof(*value)){
        err = ESP_ERR_INVALID_SIZE;
    }

    nvs_close(handle);
    return err;
}

//zapisuje bool jako jednobajtową wartość 0 lub 1
esp_err_t save_bool(const char* name, bool value){
    nvs_handle_t handle;
    esp_err_t err = nvs_open("storage",  NVS_READWRITE, &handle);
    if(err != ESP_OK){
        return err;
    }

    err = nvs_set_u8(handle, name, value ? 1 : 0);

    if(err == ESP_OK){
        err = nvs_commit(handle);
    }
    
    nvs_close(handle);
    return err;
}

//odczytuje bool zapisany jako uint8_t
esp_err_t get_bool(const char* name, bool *value){
    nvs_handle_t handle;
    esp_err_t err = nvs_open("storage", NVS_READONLY, &handle);
    if(err != ESP_OK){
        return err;
    }

    uint8_t temp;
    err = nvs_get_u8(handle, name, &temp);

    if(err == ESP_OK){
        *value = (temp != 0);
    }

    nvs_close(handle);
    return err;
}

//zapisuje napis w NVS
esp_err_t save_string(const char *name, const char * value){
    nvs_handle_t handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &handle);
    if(err != ESP_OK){
        return err;
    }

    err = nvs_set_str(handle, name, value);

    if(err == ESP_OK){
        err = nvs_commit(handle);
    }

    nvs_close(handle);
    return err;
}

//odczytuje napis do bufora przekazanego przez wywołującego
esp_err_t get_string(const char *name, char *value, size_t size){
    nvs_handle_t handle;
    esp_err_t err = nvs_open("storage", NVS_READONLY, &handle);
    if(err != ESP_OK){
        return err;
    }

    err = nvs_get_str(handle, name, value, &size);

    nvs_close(handle);
    return err;
}

//przegląda wpisy w przestrzeni storage i szuka podanego klucza
esp_err_t get_variable_type(const char *name, nvs_type_t *type){
    nvs_iterator_t it = NULL;

    esp_err_t err = nvs_entry_find("nvs", "storage", NVS_TYPE_ANY, &it);

    while (err == ESP_OK) {
        nvs_entry_info_t info;
        nvs_entry_info(it, &info);

        if (strcmp(info.key, name) == 0) {
            *type = info.type;
            nvs_release_iterator(it);
            return ESP_OK;
        }

        err = nvs_entry_next(&it);
    }

    nvs_release_iterator(it);

    return err == ESP_ERR_NVS_NOT_FOUND
        ? ESP_ERR_NVS_NOT_FOUND
        : err;
}

//usuwa klucz z NVS i zatwierdza zmianę
esp_err_t delete_key(const char *name){
    nvs_handle_t handle;

    esp_err_t err = nvs_open("storage", NVS_READWRITE, &handle);

    if (err != ESP_OK) {
        return err;
    }

    err = nvs_erase_key(handle, name);

    if (err == ESP_OK) {
        err = nvs_commit(handle);
    }

    nvs_close(handle);

    return err;
}

