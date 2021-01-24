/* Box factory header file.
 Contains macro definitions and functions' prototype declarations for interfaces between source files of the box factory program.
 The functions in this file represent the login operations of the box - a main logic module of the exercise. */


#include <stdbool.h>

#include "rb_tree.h"

#ifndef BOX_FACTORY_H_
#define BOX_FACTORY_H_


typedef struct box_factory_s {		/* Box factory structure. Has two main trees - tree_by_side and tree_by_height. */

    rb_tree *tree_by_side;			/* Tree sorted by (side * side). */
    rb_tree *tree_by_height;		/* Tree sorted by height. */
} box_factory;


typedef struct main_tree_key_s {			/* Main tree key structure. */

    unsigned int val;			/* Holds the value of either height or (side * side) of the box. */
    rb_tree *subtree;			/* A pointer to the subtree, appropriate to the content of val.  */
} main_tree_key;


typedef struct subtree_key_s {			/* Subtree key structure. */

    unsigned int val;			/* Holds the value of either height or (side * side) of the box. */
} subtree_key;


/* Create a box factory instance - allocates and initializes an empty box factory.
 Returns NULL on an allocation error, otherwise returns a pointer to box_factory. */

box_factory* box_factory_create();


/* INSERTBOX of the exercise. Adds a box of the given dimensions to the box factory data structure. Returns FALSE on an allocation error, TRUE otherwise. */

bool box_factory_insert(box_factory *factory, unsigned int side, unsigned int height);


/* REMOVEBOX of the exercise. Removes a box of the given dimensions from the box factory data structure.
 Returns FALSE if there's no box of the given dimensions, TRUE otherwise. */

bool box_factory_remove(box_factory *factory, unsigned int side, unsigned int height);


/* GETBOX of the exercise. Returns FALSE if a box suitable for the given dimensions is not found, TRUE otherwise.
 found_side_square and found_height would contain dimensions ((side * side) and height) of the box, which we found to have the minimal suitable volume
 (minimal volume when the side of the box is at least the given side, and the height of the box is at least the given height.) */

bool box_factory_get_box(box_factory *factory, unsigned int side, unsigned int height, unsigned int *found_side_square, unsigned int *found_height);


/* CHECKBOX of the exercise. Returns TRUE if in our box factory exists a box suitable for the present of the given dimensions, FALSE otherwise. */

bool box_factory_check_box(box_factory *factory, unsigned int side, unsigned int height);


#endif /* BOX_FACTORY_H_ */
