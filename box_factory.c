/*
 Box factory source file.
 Here we implement different operations on our box factory.
 */


#include <stdbool.h>

#include <stdlib.h>

#include "rb_tree.h"

#include "box_factory.h"


/* Functions' prototype declarations: */


/* Comparison function between two keys of the main tree of the box factory (tree_by_side / tree_by_height.)
 Return 0 if two keys are equal, 1 if a > b, and -1 if a < b.
 We compare according to val content of the keys (containing either height or (side * side) value of the box.) */

static int compare_main_tree_keys(void *a, void *b);


/* Comparison function between two keys of the subtree (of the key of a main tree of the box factory.)
 Return 0 if two keys are equal, 1 if a > b, and -1 if a < b.
 We compare according to val content of the keys (containing either height or (side * side) value of the box.) */

static int compare_subtree_keys(void *a, void *b);


/* Create a key of the main tree of the box factory (tree_by_side / tree_by_height.)
 Returns NULL on an allocation error, otherwise returns a pointer to the key.
 The parameter is: either height or (side * side) value of the box, which will be assigned to val field of the created key of the main tree. */

static main_tree_key* create_main_tree_key(unsigned int main_val);


/* Free an allocated given main tree key, assuming that it's subtree is empty. */

static void free_main_tree_key(main_tree_key *main_key);


/* Create a key of the subtree (of the key of a main tree of the box factory.)
 Returns NULL on an allocation error, otherwise returns a pointer to the key.
 The parameter is: either height or (side * side) value of the box, which will be assigned to val field of the created key of the subtree. */

static subtree_key* create_subtree_key(unsigned int sub_val);


/* Insertion function to tree_by_side. Returns FALSE if we fail to insert the keys of the given dimensions, TRUE otherwise.
 The function will be called by box_factory_insert. */

static bool box_factory_insert_tree_by_side(box_factory *factory, unsigned int side, unsigned int height);


/* Insertion function to tree_by_height. Returns FALSE if we fail to insert the keys of the given dimensions, TRUE otherwise.
 The function will be called by box_factory_insert. */

static bool box_factory_insert_tree_by_height(box_factory *factory, unsigned int side, unsigned int height);


/* Removal function from tree_by_side. Returns FALSE if we fail to remove the keys of the given dimensions, TRUE otherwise.
 The function will be called by box_factory_remove. */

static bool box_factory_remove_tree_by_side(box_factory *factory, unsigned int side, unsigned int height);


/* Removal function from tree_by_height. Returns FALSE if we fail to remove the keys of the given dimensions, TRUE otherwise.
 The function will be called by box_factory_remove. */

static bool box_factory_remove_tree_by_height(box_factory *factory, unsigned int side, unsigned int height);


/* A function implementing GETBOX - it is general and can receive as a parameter either one of the box factory's main trees (tree_by_side / tree_by_height.)
 Will be called by box_factory_get_box, passing to it the main tree which is smaller (we compare m and n, which represent the number of unique
 keys in the main trees - tree_by_side and tree_by_height accordingly.) */

static bool box_factory_get_by_input(rb_tree *tree, unsigned int main_val, unsigned int sub_val, unsigned int *found_main_val, unsigned int *found_sub_val);


/* A function implementing CHECKBOX - it is general and can receive as a parameter either one of the box factory's main trees
 (tree_by_side / tree_by_height.) Will be called by box_factory_check_box, passing to it the main tree which is smaller (we compare m and n, which represent
 the number of unique keys in the main trees - tree_by_side and tree_by_height accordingly.) */

static bool box_factory_check_by_input(rb_tree *tree, unsigned int main_val, unsigned int sub_val);


/* Return val of the key of a given main tree node. */

static unsigned int get_main_tree_node_val(rb_tree_node *main_tree_node);


/* Return val of the key of a given subtree node. */

static unsigned int get_subtree_node_val(rb_tree_node *sub_tree_node);


/* Return val of the key of a maximum node in the subtree of the key of a given main tree node. */

static unsigned int get_subtree_max_node_val(rb_tree_node *main_tree_node);


/* Return a pointer to the subtree of a given main tree node. */

static rb_tree* get_subtree(rb_tree_node *main_tree_node);


/* The implementation: */


static int compare_main_tree_keys(void *a, void *b)
{

	main_tree_key *key_a = a;
	main_tree_key *key_b = b;

    if (key_a->val < key_b->val) {

        return -1;
    }

    if (key_a->val > key_b->val) {

        return 1;
    }

    return 0;			/* If val contents of two keys of the main tree are equal. */
}


static int compare_subtree_keys(void *a, void *b)
{

	subtree_key *key_a = a;
	subtree_key *key_b = b;

    if (key_a->val < key_b->val) {

        return -1;
    }

    if (key_a->val > key_b->val) {

        return 1;
    }

    return 0;			/* If val contents of two keys of the subtree are equal. */
}


box_factory* box_factory_create()
{

    box_factory *factory = NULL;
    rb_tree *rb_tree = NULL;

    factory = calloc(sizeof(box_factory), 1);

    if (factory == NULL) {

        return NULL;
    }

    rb_tree = rb_tree_create((rb_tree_compare) compare_main_tree_keys);

    if (rb_tree == NULL) {

        free(factory);
        return NULL;
    }

    factory->tree_by_side = rb_tree;

    rb_tree = rb_tree_create((rb_tree_compare) compare_main_tree_keys);

    if (rb_tree == NULL) {	/* If we failed to create the second main tree - free all other fields of the box factory structure and the factory itself. */

        free(factory->tree_by_side);
        free(factory);
        return NULL;
    }

    factory->tree_by_height = rb_tree;

    return factory;
}


static main_tree_key* create_main_tree_key(unsigned int main_val)
{

	main_tree_key *main_key = calloc(sizeof(main_tree_key), 1);

    rb_tree *subtree = NULL;

    if (main_key == NULL) {

        return NULL;
    }

    main_key->val = main_val;

    subtree = rb_tree_create((rb_tree_compare) compare_subtree_keys);

    if (subtree == NULL) {

        free(main_key);
        return NULL;
    }

    main_key->subtree = subtree;

    return main_key;
}


static void free_main_tree_key(main_tree_key *main_key)
{

    free(main_key->subtree);
    free(main_key);
}


static subtree_key* create_subtree_key(unsigned int sub_val)
{

	subtree_key *sub_key = calloc(sizeof(subtree_key), 1);

    if (sub_key == NULL) {

        return NULL;
    }

    sub_key->val = sub_val;

    return sub_key;
}


static bool box_factory_insert_tree_by_side(box_factory *factory, unsigned int side, unsigned int height)
{

	main_tree_key *new_main_key = NULL;
	main_tree_key *tree_by_side_key = NULL;

    subtree_key *new_sub_key = NULL;

    bool exists_in_tree_by_side = false;
    bool exists_in_subtree = false;

    /* Before trying to insert a new key to this main tree (tree_by_side), one of the following cases is true:

     1) There's no box with the given side in the box factory - meaning the key with val = (side * side) wouldn't be found in tree_by_side.
     2) There is a box with the given side, but not with the given height - meaning the key with val = (side * side) would be found in tree_by_side,
        but the key with val = height wouldn't be found in the corresponding subtree.
     3) There is a box with the given side and with the given height - meaning the key with val = (side * side) would be found in tree_by_side and the
        key with val = height would be found in the corresponding subtree. */

    new_main_key = create_main_tree_key(side * side);			/* Create a key of tree_by_side with val = (side * side). */

    if (new_main_key == NULL) {

        return false;
    }

    new_sub_key = create_subtree_key(height);			/* Create a key of the subtree (of the key of tree_by_side) with val = height. */

    if (create_subtree_key == NULL) {

        return false;
    }

    /* First, we would search in the main tree (tree_by_side) in order to check whether the box of the given side already exists in the box factory. */

    tree_by_side_key = rb_tree_search_exact(factory->tree_by_side, new_main_key);

    if (tree_by_side_key == NULL) {			/* Case 1 - there's no box with the given side in the box factory. */

        /* So we insert new_main_key to tree_by_side - this must be a new key in the tree. */

        if (rb_tree_insert(factory->tree_by_side, new_main_key, &exists_in_tree_by_side) == false) {

            free(new_main_key);			/* Free an allocated memory in case of insertion failure. */
            free(new_sub_key);

            return false;
        }

        /* Insert new_sub_key to the subtree of new_main_key - this must be a new key in the tree. */

        if (rb_tree_insert(new_main_key->subtree, new_sub_key, &exists_in_subtree) == false) {

            rb_tree_remove(factory->tree_by_side, new_main_key, (void **) &tree_by_side_key);	/* If failed to insert to subtree - remove new_main_key. */

            free_main_tree_key(new_main_key);
            free(new_sub_key);

            return false;
        }

        return true;
    }

    /* Now, if there is a box with the given side in the box factory (tree_by_side_key != NULL): */

    free_main_tree_key(new_main_key);			/* Free the memory allocated for new_main_key, because we found that it already exists in tree_by_side. */

    /* Now we take care of cases 2 and 3.

    Insert new_sub_key to the subtree of found tree_by_side_key. */

    if (rb_tree_insert(tree_by_side_key->subtree, new_sub_key, &exists_in_subtree) == false) {

        free(new_sub_key);

        return false;
    }

    /* Case 3 - there's a box in the box factory with the given side and with the given height. */

    if (exists_in_subtree) {

        free(new_sub_key);			/* Free the memory allocated for new_sub_key, because we found that it already exists in the corresponding subtree. */
    }

    return true;
}


static bool box_factory_insert_tree_by_height(box_factory *factory, unsigned int side, unsigned int height)
{

	main_tree_key *new_main_key = NULL;
	main_tree_key *tree_by_height_key = NULL;

    subtree_key *new_sub_key = NULL;

    bool exists_in_tree_by_height = false;
    bool exists_in_subtree = false;

    /* Before trying to insert a new key to this main tree (tree_by_height), one of the following cases is true:

     1) There's no box with the given height in the box factory - meaning the key with val = height wouldn't be found in tree_by_height.
     2) There is a box with the given height, but not with the given side - meaning the key with val = height would be found in tree_by_height,
        but the key with val = (side * side) wouldn't be found in the corresponding subtree.
     3) There is a box with the given height and with the given side - meaning the key with val = height would be found in tree_by_height and the
        key with val = (side * side) would be found in the corresponding subtree. */

    new_main_key = create_main_tree_key(height);			/* Create a key of tree_by_height with val = height. */

    if (new_main_key == NULL) {

        return false;
    }

    new_sub_key = create_subtree_key(side * side);			/* Create a key of the subtree (of the key of tree_by_height) with val = (side * side). */

    if (create_subtree_key == NULL) {

        return false;
    }

    /* First, we would search in the main tree (tree_by_height) in order to check whether the box of the given height already exists in the box factory. */

    tree_by_height_key = rb_tree_search_exact(factory->tree_by_height, new_main_key);

    if (tree_by_height_key == NULL) {			/* Case 1 - there's no box with the given height in the box factory. */

        /* So we insert new_main_key to tree_by_height - this must be a new key in the tree. */

        if (rb_tree_insert(factory->tree_by_height, new_main_key, &exists_in_tree_by_height) == false) {

            free(new_main_key);			/* Free an allocated memory in case of insertion failure. */
            free(new_sub_key);

            return false;
        }

        /* Insert new_sub_key to the subtree of new_main_key - this must be a new key in the tree. */

        if (rb_tree_insert(new_main_key->subtree, new_sub_key, &exists_in_subtree) == false) {

            rb_tree_remove(factory->tree_by_side, new_main_key, (void **) &tree_by_height_key);	/* If failed to insert to subtree - remove new_main_key. */

            free_main_tree_key(new_main_key);
            free(new_sub_key);

            return false;
        }

        return true;
    }

    /* Now, if there is a box with the given height in the box factory (tree_by_height_key != NULL): */

    free_main_tree_key(new_main_key);			/* Free the memory allocated for new_main_key, because we found that it already exists in tree_by_height. */

    /* Now we take care of cases 2 and 3.

    Insert new_sub_key to the subtree of found tree_by_height_key. */

    if (rb_tree_insert(tree_by_height_key->subtree, new_sub_key, &exists_in_subtree) == false) {

        free(new_sub_key);

        return false;
    }

    /* Case 3 - there's a box in the box factory with the given height and with the given side. */

    if (exists_in_subtree) {

        free(new_sub_key);			/* Free the memory allocated for new_sub_key, because we found that it already exists in the corresponding subtree. */
    }

    return true;
}


bool box_factory_insert(box_factory *factory, unsigned int side, unsigned int height)
{

	if (box_factory_insert_tree_by_side(factory, side, height) == false) {

        return false;
    }

    if (box_factory_insert_tree_by_height(factory, side, height) == false) {

        box_factory_remove_tree_by_side(factory, side, height);			/* If failed to insert to tree_by_height - remove from tree_by side. */

        return false;
    }

    return true;
}


static bool box_factory_remove_tree_by_side(box_factory *factory, unsigned int side, unsigned int height)
{
	main_tree_key *new_main_key = NULL;
	main_tree_key *tree_by_side_key = NULL;
	main_tree_key *tree_by_side_deleted_key = NULL;

	subtree_key *new_sub_key = NULL;
	subtree_key *sub_key = NULL;

    /* Before trying to remove a key from this main tree (tree_by_side), one of the following cases is true:

     1) There's no box of the given dimensions in the box factory, which means one of the following:
        1.1) The key with val = (side * side) wouldn't be found in tree_by_side.
        1.2) The key with val = (side * side) would be found in tree_by_side, but the key with val = height wouldn't be found in the corresponding subtree.
     2) There is a box of the given dimensions in the box factory - meaning the key with val = (side * side) would be found in tree_by_side and the
        key with val = height would be found in the corresponding subtree. */

	new_main_key = create_main_tree_key(side * side);			/* Create a key of tree_by_side with val = (side * side). */

    if (new_main_key == NULL) {

        return false;
    }

    /* First, we would search in the main tree (tree_by_side) in order to check whether the box of the given side exists in the box factory. */

    tree_by_side_key = rb_tree_search_exact(factory->tree_by_side, new_main_key);

    if (tree_by_side_key == NULL) {			/* Case 1.1 - the box with the given side doesn't exist in the box factory. */

        free_main_tree_key(new_main_key);			/* Free the memory allocated for new_main_key, because there's nothing to remove. */

        return false;
    }

    /* Free the memory allocated for new_main_key, because we found that it exists in tree_by_side. (tree_by_side_key != NULL). */

    free_main_tree_key(new_main_key);

    new_sub_key = create_subtree_key(height);			/* Create a key of the subtree (of the key of tree_by_side) with val = height. */

    if (new_sub_key == NULL) {

        return false;
    }

    /* Search the subtree of found tree_by_side_key in order to check whether the box of the given dimensions exists in the box factory. */

    sub_key = rb_tree_search_exact(tree_by_side_key->subtree, new_sub_key);

    if (sub_key == NULL) {			/* Case 1.2 - there is a box in the box factory with the given side, but not with the given height. */

        free(new_sub_key);			/* Free the memory allocated for new_sub_key, because there's nothing to remove. */

        return false;
    }

    /* Case 2 - there is a box of the given dimensions in the box factory. (sub_key != NULL). */

    /* Remove the key with val = height from the subtree of the found tree_by_side_key. */

    rb_tree_remove(tree_by_side_key->subtree, new_sub_key, (void **) &sub_key);

    if (sub_key) {			/* There are no more keys with val = height in the subtree of tree_by_side_key. */

        free(sub_key);			/* Free the memory allocated for the key with val = height, which was removed from the subtree of tree_by_side_key. */
    }

    /* In case the subtree of tree_by_side_key has been emptied, tree_by_side_key should be removed from tree_by_side. */

    if (tree_by_side_key->subtree->count == 0) {

        rb_tree_remove(factory->tree_by_side, tree_by_side_key, (void **) &tree_by_side_deleted_key);

        /* Free the memory allocated for the key with val = (side * side), which was removed from tree_by_side. */

        free_main_tree_key(tree_by_side_deleted_key);
    }

    free(new_sub_key);			/* Free the memory we allocated for new_sub_key. */

    return true;
}


static bool box_factory_remove_tree_by_height(box_factory *factory, unsigned int side, unsigned int height)
{
	main_tree_key *new_main_key = NULL;
	main_tree_key *tree_by_height_key = NULL;
	main_tree_key *tree_by_height_deleted_key = NULL;

	subtree_key *new_sub_key = NULL;
	subtree_key *sub_key = NULL;

    /* Before trying to remove a key from this main tree (tree_by_height), one of the following cases is true:

     1) There's no box of the given dimensions in the box factory, which means one of the following:
        1.1) The key with val = height wouldn't be found in tree_by_height.
        1.2) The key with val = height would be found in tree_by_height, but the key with val = (side * side) wouldn't be found in the corresponding subtree.
     2) There is a box of the given dimensions in the box factory - meaning the key with val = height would be found in tree_by_height and the
        key with val = (side * side) would be found in the corresponding subtree. */

	new_main_key = create_main_tree_key(height);			/* Create a key of tree_by_height with val = height. */

    if (new_main_key == NULL) {

        return false;
    }

    /* First, we would search in the main tree (tree_by_height) in order to check whether the box of the given height exists in the box factory. */

    tree_by_height_key = rb_tree_search_exact(factory->tree_by_height, new_main_key);

    if (tree_by_height_key == NULL) {			/* Case 1.1 - the box with the given height doesn't exist in the box factory. */

        free_main_tree_key(new_main_key);			/* Free the memory allocated for new_main_key, because there's nothing to remove. */

        return false;
    }

    /* Free the memory allocated for new_main_key, because we found that it exists in tree_by_height. (tree_by_height_key != NULL). */

    free_main_tree_key(new_main_key);

    new_sub_key = create_subtree_key(side * side);			/* Create a key of the subtree (of the key of tree_by_height) with val = (side * side). */

    if (new_sub_key == NULL) {

        return false;
    }

    /* Search the subtree of found tree_by_height_key in order to check whether the box of the given dimensions exists in the box factory. */

    sub_key = rb_tree_search_exact(tree_by_height_key->subtree, new_sub_key);

    if (sub_key == NULL) {			/* Case 1.2 - there is a box in the box factory with the given height, but not with the given side. */

        free(new_sub_key);			/* Free the memory allocated for new_sub_key, because there's nothing to remove. */

        return false;
    }

    /* Case 2 - there is a box of the given dimensions in the box factory. (sub_key != NULL). */

    /* Remove the key with val = (side * side) from the subtree of the found tree_by_height_key. */

    rb_tree_remove(tree_by_height_key->subtree, new_sub_key, (void **) &sub_key);

    if (sub_key) {			/* There are no more keys with val = (side * side) in the subtree of tree_by_height_key. */

        free(sub_key);		/* Free the memory allocated for the key with val = (side * side), which was removed from the subtree of tree_by_height_key. */
    }

    /* In case the subtree of tree_by_height_key has been emptied, tree_by_height_key should be removed from tree_by_height. */

    if (tree_by_height_key->subtree->count == 0) {

        rb_tree_remove(factory->tree_by_height, tree_by_height_key, (void **) &tree_by_height_deleted_key);

        /* Free the memory allocated for the key with val = height, which was removed from tree_by_height. */

        free_main_tree_key(tree_by_height_deleted_key);
    }

    free(new_sub_key);			/* Free the memory we allocated for new_sub_key. */

    return true;
}


bool box_factory_remove(box_factory *factory, unsigned int side, unsigned int height)
{
    if (box_factory_remove_tree_by_side(factory, side, height) == false) {

        return false;
    }

    /* If we were able to remove from tree_by_side, this means the box of the given dimensions exists in the box factory, so we should be able to remove
     from tree_by_height. */

    box_factory_remove_tree_by_height(factory, side, height);

    return true;
}


static bool box_factory_get_by_input(rb_tree *tree, unsigned int main_val, unsigned int sub_val, unsigned int *found_main_val, unsigned int *found_sub_val)
{
    rb_tree_node *main_node = NULL;
    rb_tree_node *sub_node = NULL;

    rb_tree_node *min_main_node = NULL;
    rb_tree_node *min_sub_node = NULL;

    main_tree_key target_main_key = {.val = main_val, .subtree = NULL};
    subtree_key target_sub_key = {.val = sub_val};

    unsigned int volume = 0;
    unsigned int min_volume = 0;

    /* Search the given main tree for a node with the smallest key that is larger than or equal to the given key (target_main_key). */

    main_node = rb_tree_search_smallest_from(tree, &target_main_key);

    /* If val of the key of a maximum node in the subtree of the key of the found main tree node (main_node), is smaller than the given sub_val - then
     main_node can't be the node we're looking for, therefore we proceed to check the next node of the main tree (successor of the current main_node.) */

    while (main_node && (get_subtree_max_node_val(main_node) < sub_val)) {

    	main_node = rb_tree_successor(tree, main_node);
    }

    if (main_node == NULL) {

        return false;
    }

    /* Search the subtree of the key of the found suitable main_node, for a node with the smallest key that is larger than or equal to the given key
     (target_sub_key). */

    sub_node = rb_tree_search_smallest_from(get_subtree(main_node), &target_sub_key);

    /* Now, when the suitable (according to the given dimensions) main_node and sub_node are found - calculate the minimal volume to start with. */

    min_volume = get_main_tree_node_val(main_node) * get_subtree_node_val(sub_node);

    min_main_node = main_node;
    min_sub_node = sub_node;

    /* We divide the current minimal volume by val of the key of the current main_node.
     We do this in order to check whether we need to continue looking for the minimal possible volume by checking the next node in the tree (successor
     of the current main_node.)
     If we find that the result of described division is less than or equal to the given sub_val - this means there's no point to continue, because
     there's no way we will receive a suitable volume which is less than the minimal volume that we've already found, by checking the successors. */

    while (main_node && (min_volume / get_main_tree_node_val(main_node) > sub_val)) {

    	main_node = rb_tree_successor(tree, main_node);

    	/* Check whether the subtree of the key of the currently checked main_node meets the requirements of containing the suitable dimensions.
    	 If it doesn't - go back to the while condition. */

        if ((main_node == NULL) || (get_subtree_max_node_val(main_node) < sub_val)) {

            continue;
        }

        /* Search the subtree of the key of the found suitable main_node, for a node with the smallest key that is larger than or equal to the given key
         (target_sub_key). */

        sub_node = rb_tree_search_smallest_from(get_subtree(main_node), &target_sub_key);

        volume = get_main_tree_node_val(main_node) * get_subtree_node_val(sub_node);

        if (min_volume > volume) {			/* Check whether we have found a new minimal volume. */

            min_volume = volume;
            min_main_node = main_node;
            min_sub_node = sub_node;
        }
    }

    /* At the end, found_main_val and found_sub_val would contain val of the key of the main tree node and val of the key of the corresponding subtree node,
     which we found to give the minimal suitable volume. */

    *found_main_val = get_main_tree_node_val(min_main_node);

    *found_sub_val = get_subtree_node_val(min_sub_node);

    return true;
}


bool box_factory_get_box(box_factory *factory, unsigned int side, unsigned int height, unsigned int *found_side_square, unsigned int *found_height)
{
    if (factory->tree_by_height->count == 0) {			/* If one of the main trees is empty - there're no boxes in the factory. */

        return false;
    }

    /* Check the main tree which is smaller (we compare m and n, which represent the number of unique keys in the main trees - tree_by_side and
     tree_by_height accordingly.) */

    if (factory->tree_by_height->count > factory->tree_by_side->count){

        return box_factory_get_by_input(factory->tree_by_side, side * side, height, found_side_square, found_height);
    }

    return box_factory_get_by_input(factory->tree_by_height, height, side * side, found_height, found_side_square);

    /* At the end, found_side_square and found_height would contain dimensions ((side * side) and height) of the box, which we found to have the minimal
     suitable volume (minimal volume when the side of the box is at least the given side, and the height of the box is at least the given height.) */
}


static bool box_factory_check_by_input(rb_tree *tree, unsigned int main_val, unsigned int sub_val)
{
    rb_tree_node *main_node = NULL;
    main_tree_key *main_key = NULL;

    main_tree_key target_main_key;
    subtree_key target_sub_key;

    target_main_key.val = main_val;
    target_sub_key.val = sub_val;

    /* Search the given main tree for a node with the smallest key that is larger than or equal to the given key (target_main_key). */

    main_node = rb_tree_search_smallest_from(tree, &target_main_key);

    /* If we didn't find such a node - it means there's no box in the box factory suitable for the present of the given dimensions. */

    if (main_node == NULL){

        return false;
    }

    main_key = (main_tree_key*) main_node->key;

    /* We compare between two keys:
     1. The key of the maximum node in the subtree of the key (main_key) of the found suitable main tree node (main_node).
     2. The given key (target_sub_key).
     In case the key of the maximum node is smaller than the given key - then main_node can't be the node we're looking for, therefore we proceed to check
     the next node of the main tree (successor of the current main_node.) */

    while ((main_node != NULL) && (main_key->subtree->cmp(main_key->subtree->max->key, &target_sub_key) == -1)){

        main_node = rb_tree_successor(tree, main_node);

        if (main_node != NULL){

            main_key = (main_tree_key*) main_node->key;
        }
    }

    if (main_node == NULL){

        return false;
    }

    /* If in the end main_node != NULL - it means there is a box in the box factory suitable for the present of the given dimensions  */

    return true;
}


bool box_factory_check_box(box_factory *factory, unsigned int side, unsigned int height)
{

    /* Check the main tree which is smaller (we compare m and n, which represent the number of unique keys in the main trees - tree_by_side and
     tree_by_height accordingly.) */

    if (factory->tree_by_height->count > factory->tree_by_side->count){

        return box_factory_check_by_input(factory->tree_by_side, side * side, height);
    }

    return box_factory_check_by_input(factory->tree_by_height, height, side * side);
}


static unsigned int get_main_tree_node_val(rb_tree_node *main_tree_node)
{
	main_tree_key *main_key = (main_tree_key*)main_tree_node->key;

    return main_key->val;
}


static unsigned int get_subtree_node_val(rb_tree_node *sub_tree_node)
{
	subtree_key *sub_tree_key = (subtree_key*) sub_tree_node->key;

    return sub_tree_key->val;
}


static unsigned int get_subtree_max_node_val(rb_tree_node *main_tree_node)
{
	main_tree_key *main_key = NULL;
	rb_tree *sub_tree = NULL;
    rb_tree_node *max_sub_node = NULL;
    subtree_key *max_sub_key = NULL;

    main_key = (main_tree_key*) main_tree_node->key;

    sub_tree = main_key->subtree;

    max_sub_node = sub_tree->max;

    max_sub_key = (subtree_key*) max_sub_node->key;

    return max_sub_key->val;
}


static rb_tree* get_subtree(rb_tree_node *main_tree_node)
{
	main_tree_key *main_key = NULL;

	main_key = (main_tree_key*) main_tree_node->key;

    return main_key->subtree;
}

