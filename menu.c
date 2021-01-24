#include <stdbool.h>

#include <stdio.h>

#include "menu.h"


void menu_print(menu_item menu_items[], unsigned int items)
{
    unsigned int i = 0;

    for (i = 0; i < items; ++i) {

        printf("%u. %s\n", i, menu_items[i].description);
    }
}


void menu_run(menu_item menu_items[], unsigned int items)
{
    unsigned int choice;
    bool running = true;

    menu_print(menu_items, items);

    while (running) {

        scanf("%u", &choice);

        if (choice < items) {

            running = menu_items[choice].option(menu_items[choice].structure);
        }

        else {

            printf("Invalid option: %u\n", choice);
        }

        printf("\n");
    }
}


bool menu_quit(void *nothing) {

    return false;
}


