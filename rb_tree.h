/* Red-black tree header file.
 Contains macro definitions and functions' prototype declarations for interfaces between source files of the box factory program.
 The functions in this file are for the keys' management. The user doesn't manage the actual nodes of the tree, but only its keys, and is responsible
 for the keys' memory management. */


#include <stdbool.h>

#ifndef RB_TREE_H_
#define RB_TREE_H_


typedef enum rb_tree_color_s {

    BLACK = 0,
    RED = 1,
} rb_tree_color;


/* Create the type rb_tree_compare, for "pointer to function (of two void * arguments) returning int."
 We would use the pointer of this type to point at comparison functions - one between two keys of the main tree of the box factory,
 another one between two keys of the subtree (of the key of a main tree of the box factory.)
 (We would have two different comparison functions because of the difference between types of the keys - main tree keys and subtree keys.)
 We would return 0 if two keys are equal, 1 if a > b, and -1 if a < b. */

typedef int (*rb_tree_compare)(void *a, void *b);


typedef struct rb_tree_node_s rb_tree_node;


struct rb_tree_node_s {			/* Red-black tree node structure. */

    void *key;
    rb_tree_color color;
    rb_tree_node *left;
    rb_tree_node *right;
    rb_tree_node *parent;
    unsigned int count;			/* Number of instances the key of the node has. */
};


typedef struct rb_tree_s {			/* Red-black tree structure. */

    rb_tree_node nil;
	rb_tree_node *root;
    rb_tree_node *max;
    rb_tree_compare cmp;
    unsigned int count;			/* Number of different (unique) keys in the tree. (m / n in the project.) */
} rb_tree;


/* Create a red-black tree instance - allocates and initializes an empty tree.
 Returns NULL on an allocation error, otherwise returns a pointer to rb_tree. */

rb_tree* rb_tree_create(rb_tree_compare cmp);


/* Return a pointer to the successor of the given node (the node with the smallest key that is larger than the key of the given node.)
 Based on the book's implementation. */

rb_tree_node* rb_tree_successor(rb_tree *tree, rb_tree_node *node);


/* Return a pointer to the maximum node in the tree. */

rb_tree_node* rb_tree_max(rb_tree *tree);


/* Insert a given key to the tree. The function works this way:
 In case the key already exists in the tree, we simply increase it's count by 1 and change 'exists' value to TRUE, so we would know whether to free
 the memory allocated for the key.
 In case the key doesn't exist, we allocate a new node for the key and actually insert the node to the three (based on the book's implementation.)
 In this case, since the unique key was added to the tree, we increase the tree's count by 1.
 Returns FALSE on an allocation error, TRUE otherwise. */

bool rb_tree_insert(rb_tree *tree, void *key, bool *exists);


/* Remove the given key from the tree. The function works this way:
 In case the key exists in the tree, we decrease it's count by 1. If key's count is decreased to 0, this means we have to delete the corresponding node
 from the tree (rb_tree_delete - based on the book's implementation.) In this case, since the unique key was removed from the tree, we decrease the
 tree's count by 1, and 'deleted' would contain a pointer to the key that was removed, so we can free the memory allocated for the key.
 Otherwise (if the key's count is not 0 and we don't delete the corresponding node) 'deleted' would contain NULL, and we return TRUE.
 Returns FALSE in case the key doesn't exists in the tree (nothing to remove). */

bool rb_tree_remove(rb_tree *tree, void *key, void **deleted);


/* Search the tree for an exact given key. Returns an equal key if found, NULL otherwise. */

void* rb_tree_search_exact(rb_tree *tree, void *key);


/* Search the tree for a node with the smallest key that is larger than or equal to the given key. Returns a pointer to the node containing the key
 if found, NULL otherwise (if there's no node in the tree with the key that is larger than or equal to the given key.) */

rb_tree_node* rb_tree_search_smallest_from(rb_tree *tree, void *key);


#endif /* RB_TREE_H_ */
