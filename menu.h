#include <stdbool.h>

#ifndef MENU_H_
#define MENU_H_


#define MENU_QUIT_ACTION {menu_quit, "Quit", NULL}

typedef bool (*menu_callback)(void *);


typedef struct menu_item_s {

    menu_callback option;
    char *description;
    void *structure;
} menu_item;


void menu_print(menu_item menu_items[], unsigned int items);

void menu_run(menu_item menu_items[], unsigned int items);

bool menu_quit(void *nothing);


#endif /* MENU_H_ */
