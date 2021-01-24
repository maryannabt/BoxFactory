#include <stdbool.h>

#include <stdio.h>

#include <math.h>

#include "box_factory.h"

#include "box_menu.h"


static void get_dimensions(unsigned int *side, unsigned int *height)
{

    printf("Enter the side of the box: ");
    scanf("%u", side);

    printf("Enter the height of the box: ");
    scanf("%u", height);
}


bool box_menu_insert(void *the_box_factory)
{

    box_factory *factory = the_box_factory;

    unsigned int side = 0;
    unsigned int height = 0;

    get_dimensions(&side, &height);

    printf("Requesting to insert a box with side=%d and height=%d\n", side, height);

    if (!box_factory_insert(factory, side, height)) {

        printf("Error: Insertion failed\n");

        return false;
    }

    else {

        printf("Inserted a box with side=%d and height=%d\n", side, height);
    }

    return true;
}


bool box_menu_remove(void *the_box_factory)
{

    box_factory *factory = the_box_factory;

    unsigned int side = 0;
    unsigned int height = 0;

    get_dimensions(&side, &height);

    printf("Requesting to remove a box with side=%d and height=%d\n", side, height);

    if (!box_factory_remove(factory, side, height)) {

        printf("Error: Box of the given dimensions is not found\n");
    }

    else {

        printf("Removed a box with side=%d and height=%d\n", side, height);
    }

    return true;
}


bool box_menu_get(void *the_box_factory)
{

    box_factory *factory = the_box_factory;

    unsigned int side = 0;
    unsigned int height = 0;
    unsigned int found_side_square = 0;
    unsigned int found_height = 0;

    bool found = false;

    get_dimensions(&side, &height);

    printf("Searching for a box of minimal volume with minimum side=%d and height=%d\n", side, height);

    found = box_factory_get_box(factory, side, height, &found_side_square, &found_height);

    if (found) {

        printf("Found a box with side=%d and height=%d\n", (unsigned int) sqrt((double) found_side_square), found_height);
    }

    else {

        printf("Error: The suitable box is not found\n");
    }

    return true;
}


bool box_menu_check(void *the_box_factory)
{

    box_factory *factory = the_box_factory;

    unsigned int side = 0;
    unsigned int height = 0;

    get_dimensions(&side, &height);

    printf("Checking whether a box with minimum side=%d and height=%d exists\n", side, height);

    if (!box_factory_check_box(factory, side, height)) {

        printf("The suitable box does not exist\n");
    }

    else {

        printf("There is a suitable box\n");
    }

    return true;
}

