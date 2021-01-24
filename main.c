#include <stdio.h>

#include <stdbool.h>

#include "box_factory.h"

#include "box_menu.h"

#include "menu.h"


int main(void)
{
    box_factory *factory = box_factory_create();

    if (factory == NULL) {

            printf("Error: Unable to create a box factory instance\n");
            return -1;
    }

    menu_item menu_items[] = {{box_menu_insert, "Insert a box of the given dimensions", factory},
                              {box_menu_remove, "Remove a box of the given dimensions", factory},
                              {box_menu_get, "Get the dimensions of a suitable box with minimal volume", factory},
                              {box_menu_check, "Check whether there is a suitable box for the present", factory},
                              MENU_QUIT_ACTION};

    menu_run(menu_items, sizeof(menu_items) / sizeof(menu_item));

    return 0;
}



