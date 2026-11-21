#ifndef FLASH_MANAGE_T
#define FLASH_MANAGE_T

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "nvs.h"
#include "esp_err.h"


void init_memory(void);//inicjalizuje pamięć NVS

esp_err_t save_int(const char* name, int32_t value);//zapisuje liczbę całkowitą pod podanym kluczem
esp_err_t get_int(const char* name, int32_t *value);//odczytuje liczbę całkowitą

esp_err_t save_float(const char* name, float value);//zapisuje float jako dane typu BLOB
esp_err_t get_float(const char* name, float *value);//odczytuje float zapisany jako BLOB

esp_err_t save_bool(const char* name, bool value);//zapisuje bool jako 0 lub 1
esp_err_t get_bool(const char* name, bool *value);//odczytuje bool

esp_err_t save_string(const char *name, const char *value);//zapisuje napis
esp_err_t get_string(const char *name, char *value, size_t size);//odczytuje napis do bufora o podanym rozmiarze

esp_err_t get_variable_type(const char *name, nvs_type_t *type);//wyszukuje klucz i zwraca jego typ NVS
esp_err_t delete_key(const char *name);//usuwa klucz z NVS


#endif