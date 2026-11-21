#ifndef INTERFACE_VIEW_T
#define INTERFACE_VIEW_T

//struktura menu i deklaracje funkcji obsługujących terminal

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

typedef struct Menu_node{
    char * name;
    uint8_t children_num;
    char * option_table[16];
}menu_node;

menu_node * init_menu(void);//tworzy menu i dodaje domyślne opcje

void show_menu(menu_node * menu);//wyświetla opcje menu
void add_option(menu_node *menu, const char *name);//dodaje nową opcję do menu
void free_menu(menu_node * menu);//zwalnia pamięć przydzieloną na menu
void action(const char * name);//wykonuje komendę wpisaną przez użytkownika


void init_default_variables(void);//zapisuje wartości domyślne tylko dla brakujących kluczy
void read_line(char *buffer, size_t size);//odczytuje linię z terminala i wyświetla wpisywane znaki
#endif