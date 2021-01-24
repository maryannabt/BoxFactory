/* Red-black tree source file.
 Here we mostly implement regular red-black tree operations, managing the nodes of the tree (based on the book's implementation.)
 Our tree holds a single node for each unique key. The user doesn't manage the actual nodes of the tree, but only its keys, and is responsible
 for the keys' memory management. */


#include <stdlib.h>

#include "rb_tree.h"


/* Check whether the node is NIL. */

#define IS_NIL(tree, node) ((node) == &((tree)->nil))			/* Checking whether node points to nil of the tree. */


/* Functions' prototype declarations: */


/* The rotation functions of the tree. Based on the book's implementation. */

static void rb_tree_rotate_left(rb_tree *tree, rb_tree_node *x);

static void rb_tree_rotate_right(rb_tree *tree, rb_tree_node *x);


/* Helper function of rb_tree_insert. Based on the book's implementation. */

static void rb_tree_insert_fixup(rb_tree *tree, rb_tree_node *z);


/* Delete a given node from the tree. Based on the book's implementation. */

static void rb_tree_delete(rb_tree *tree, rb_tree_node *z);


/* Helper function of rb_tree_delete. Based on the book's implementation. */

static void rb_tree_delete_fixup(rb_tree *tree, rb_tree_node *x);


/* Search the tree for a node with an exact given key, starting from the given node. Returns a pointer to the node containing an equal key if found,
 NULL otherwise. We use this function in rb_tree_search_exact. */

static rb_tree_node* rb_tree_search_exact_node(rb_tree *tree, rb_tree_node *node, void *key);


/* Search the tree for a node with the smallest key that is larger than or equal to the given key, starting from the given node.
 Returns a pointer to the node containing the key if found, NULL otherwise (if there's no node in the tree with the key that is larger than or equal
 to the given key.) We use this function in rb_tree_search_smallest_from. */

static rb_tree_node* rb_tree_search_smallest_from_node(rb_tree *tree, rb_tree_node *node, void *key);


/* The implementation: */


rb_tree* rb_tree_create(rb_tree_compare cmp)
{

	rb_tree *red_black_tree = NULL;

    red_black_tree = (rb_tree *)calloc(sizeof(rb_tree), 1);			/* Memory allocation for the tree. (Allocation for the rb_tree structure.) */

    if (red_black_tree == NULL) {

        return red_black_tree;
    }

    red_black_tree->nil.key = NULL;			/* Initializing the NIL node of the tree. */
    red_black_tree->nil.color = BLACK;
    red_black_tree->nil.left = &(red_black_tree->nil);			/* left is now points to nil. And so on. */
    red_black_tree->nil.right = &(red_black_tree->nil);
    red_black_tree->nil.parent = &(red_black_tree->nil);
    red_black_tree->nil.count = 0;

    red_black_tree->root = &(red_black_tree->nil);
    red_black_tree->max = &(red_black_tree->nil);

    red_black_tree->cmp = cmp;			/* Assigning an appropriate (given as a parameter) comparison function for the tree. (Function pointer) */
    red_black_tree->count = 0;

    return red_black_tree;
}


rb_tree_node* rb_tree_successor(rb_tree *tree, rb_tree_node *node)
{

    rb_tree_node *y = NULL;
    y = node->right;

    if (!IS_NIL(tree, y)) {

        while (!IS_NIL(tree, y->left)) {

            y = y->left;
        }

        return IS_NIL(tree, y) ? NULL : y;			/* Find and return a pointer to the leftmost node in the right subtree of the given node. */
    }

    else {

        y = node->parent;

        while (node == y->right) {

            node = y;
            y = y->parent;
        }

        return IS_NIL(tree, y) ? NULL : y;
    }
}


rb_tree_node* rb_tree_max(rb_tree *tree)
{

    rb_tree_node *node = tree->root;

    if (IS_NIL(tree, node)){

        return node;
    }

    while (!IS_NIL(tree, node->right)){

        node = node->right;
    }

    return node;
}


static void rb_tree_rotate_left(rb_tree *tree, rb_tree_node *x)
{

    rb_tree_node *y = NULL;
    y = x->right;
    x->right = y->left;

    if (!IS_NIL(tree, y->left)) {

        y->left->parent = x;
    }

    y->parent = x->parent;

    if (IS_NIL(tree, x->parent)) {

        tree->root = y;
    }

    else {

        if (x == x->parent->left) {

            x->parent->left = y;
        }

        else {

            x->parent->right = y;
        }
    }

    y->left = x;
    x->parent = y;
}


static void rb_tree_rotate_right(rb_tree *tree, rb_tree_node *x)
{

    rb_tree_node *y = NULL;
    y = x->left;
    x->left = y->right;

    if (!IS_NIL(tree, y->right)) {

        y->right->parent = x;
    }

    y->parent = x->parent;

    if (IS_NIL(tree, x->parent)) {

        tree->root = y;
    }

    else {

        if (x == x->parent->right) {

            x->parent->right = y;
        }

        else {

            x->parent->left = y;
        }
    }

    y->right = x;
    x->parent = y;
}


bool rb_tree_insert(rb_tree *tree, void *key, bool *exists)
{

    rb_tree_node *x = NULL;
    rb_tree_node *y = NULL;
    rb_tree_node *z = NULL;

    *exists = false;

    /* First, search the tree for an exact given key. In case the key exists in the tree, we simply increase it's count by 1 and change 'exists'
     value to TRUE, so the user, who manages the keys, would know whether to free the memory allocated for the key. */

    x = rb_tree_search_exact_node(tree, tree->root, key);

    if (x != NULL) {

        *exists = true;
        x->count += 1;
        return true;
    }

    /* In case the key doesn't exist, we allocate a new node for the key and actually insert the node to the three.
     Based on the book's implementation. */

    z = (rb_tree_node *)calloc(sizeof(rb_tree_node), 1);

    if (z == NULL) {

        return false;
    }

    z->key = key;
    z->count = 1;			/* One new key was inserted. */

    y = &(tree->nil);
    x = tree->root;

    while (!IS_NIL(tree, x)) {

        y = x;

        if (tree->cmp(z->key, x->key) < 0) {			/* If (z->key) < (x->key) */

            x = x->left;
        }

        else {

            x = x->right;
        }
    }

    z->parent = y;

    if (IS_NIL(tree, y)) {

        tree->root = z;
    }

    else {

        if (tree->cmp(z->key, y->key) < 0) {			/* If (z->key) < (y->key) */

        	y->left = z;
        }

        else {

            y->right = z;
        }
    }

    z->left = &(tree->nil);
    z->right = &(tree->nil);
    z->color = RED;

    rb_tree_insert_fixup(tree, z);

    /* In this case, since the unique key was added to the tree, we increase the tree's count by 1. */

    tree->count++;
    tree->max = rb_tree_max(tree);			/* Find the new maximum node in the tree. */

    return true;
}


static void rb_tree_insert_fixup(rb_tree *tree, rb_tree_node *z)
{

    rb_tree_node *y = NULL;

    while (z->parent->color == RED) {

        if (z->parent == z->parent->parent->left) {

            y = z->parent->parent->right;

            /* Case 1 */

            if (y->color == RED) {

                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }

            else {

                /* Case 2 */

                if (z == z->parent->right) {

                    z = z->parent;

                    rb_tree_rotate_left(tree, z);
                }

                /* Case 3 */

                z->parent->color = BLACK;
                z->parent->parent->color = RED;

                rb_tree_rotate_right(tree, z->parent->parent);
            }
        }

        else {

            y = z->parent->parent->left;

            /* Case 1 */

            if (y->color == RED) {

                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }

            else {

                /* Case 2 */

                if (z == z->parent->left) {

                    z = z->parent;

                    rb_tree_rotate_right(tree, z);
                }

                /* Case 3 */

                z->parent->color = BLACK;
                z->parent->parent->color = RED;

                rb_tree_rotate_left(tree, z->parent->parent);
            }
        }
    }

    tree->root->color = BLACK;
}


static void rb_tree_delete(rb_tree *tree, rb_tree_node *z)
{

    rb_tree_node *y = NULL;
    rb_tree_node *x = NULL;

    if (IS_NIL(tree, z->left) || IS_NIL(tree, z->right)) {

        y = z;
    }

    else {

        y = rb_tree_successor(tree, z);
    }

    if (IS_NIL(tree, y->left)) {

        x = y->right;
    }

    else {

        x = y->left;
    }

    x->parent = y->parent;

    if (IS_NIL(tree, y->parent)) {

        tree->root = x;
    }

    else {

        if (y == y->parent->left) {

            y->parent->left = x;
        }

        else {

            y->parent->right = x;
        }
    }

    if (y != z) {

        z->key = y->key;
        z->count = y->count;			/* Copy y's satellite data into z. */
    }

    if (y->color == BLACK) {

        rb_tree_delete_fixup(tree, x);
    }

    free(y);
}


static void rb_tree_delete_fixup(rb_tree *tree, rb_tree_node *x)
{

    rb_tree_node *root = tree->root;
    rb_tree_node *w = NULL;

    while ((x->color == BLACK) && (x != root)) {

        if (x == x->parent->left) {

            w = x->parent->right;

            if (w->color == RED) {

                w->color = BLACK;
                x->parent->color = RED;

                rb_tree_rotate_left(tree, x->parent);

                w = x->parent->right;
            }

            if ((w->right->color == BLACK) && (w->left->color == BLACK)) {

                w->color = RED;
                x = x->parent;
            }

            else {

                if (w->right->color == BLACK) {

                    w->left->color = BLACK;
                    w->color = RED;

                    rb_tree_rotate_right(tree, w);

                    w = x->parent->right;
                }

                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->right->color = BLACK;

                rb_tree_rotate_left(tree, x->parent);

                x = root;
            }
        }

        else {

            w = x->parent->left;

            if (w->color == RED) {

                w->color = BLACK;
                x->parent->color = RED;

                rb_tree_rotate_right(tree, x->parent);

                w = x->parent->left;
            }

            if ((w->right->color == BLACK) && (w->left->color == BLACK)) {

                w->color = RED;
                x = x->parent;
            }

            else {

                if (w->left->color == BLACK) {

                    w->right->color = BLACK;
                    w->color = RED;

                    rb_tree_rotate_left(tree, w);

                    w = x->parent->left;
                }

                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->left->color = BLACK;

                rb_tree_rotate_right(tree, x->parent);

                x = root;
            }
        }
    }

    x->color = BLACK;
}


bool rb_tree_remove(rb_tree *tree, void *key, void **deleted)
{

	*deleted = NULL;

	/* First, search the tree for an exact given key. In case the key exists in the tree, we decrease it's count by 1. */

    rb_tree_node *node = rb_tree_search_exact_node(tree, tree->root, key);

    if (node == NULL) {

        return false;			/* Returns FALSE in case the key doesn't exists in the tree (nothing to remove). */
    }

    node->count -= 1;

    /* If key's count is decreased to 0 (no more instances of the key left), this means we have to delete the corresponding node from the tree.
     Based on the book's implementation. */

    if (node->count == 0) {

        *deleted = node->key;	/* 'deleted' would contain a pointer to the key that was removed, so we can free the memory allocated for the key. */
        rb_tree_delete(tree, node);
        tree->count--;			/* In this case, since the unique key was removed from the tree, we decrease the tree's count by 1 */
    }

    tree->max = rb_tree_max(tree);			/* Find the new maximum node in the tree. */

    return true;
}


static rb_tree_node* rb_tree_search_exact_node(rb_tree *tree, rb_tree_node *node, void *key)
{

    int compare = 0;

    if (IS_NIL(tree, node)) {

        return NULL;
    }

    compare = tree->cmp(key, node->key);

    if (compare == 0) {			/* If (key) = (node->key) */

        return node;
    }

    if (compare < 0) {	/* If (key) < (node->key). This means we may find suitable node (a node with an exact given key) in the left subtree. */

        return rb_tree_search_exact_node(tree, node->left, key);
    }

    else {			/* If (key) > (node->key). Search the right subtree for a node with bigger key. */

        return rb_tree_search_exact_node(tree, node->right, key);
    }
}


void* rb_tree_search_exact(rb_tree *tree, void *key)
{

	/* Search the tree for a node with an exact given key, starting from the root of the tree. */

    rb_tree_node *found_node = rb_tree_search_exact_node(tree, tree->root, key);

    if (found_node == NULL) {

        return NULL;
    }

    return found_node->key;			/* Returns an equal key if found, NULL otherwise. */
}


static rb_tree_node* rb_tree_search_smallest_from_node(rb_tree *tree, rb_tree_node *node, void *key)
{

    rb_tree_node *found = NULL;
    int compare = 0;

    if (IS_NIL(tree, node)) {

        return NULL;
    }

    compare = tree->cmp(key, node->key);

    if (compare == 0) {			/* If (key) = (node->key) */

        return node;
    }

    if (compare < 0) {			/* If (key) < (node->key). This means we may find suitable node (a node with the smallest key that is larger than
                                 or equal to the given key) in the left subtree.  */

        if (IS_NIL(tree, node->left)) {			/* If there's no left subtree - then the current node is the one we're looking for. */

            return node;
        }

        found = rb_tree_search_smallest_from_node(tree, node->left, key);

        if (found == NULL) { /* If we didn't find the node with the smaller suitable key than the key of the current node - return current node.*/

            return node;
        }

        else {

            return found;
        }
    }

    else {			/* If (key) > (node->key). Search the right subtree for a node with bigger key. */

        if (IS_NIL(tree, node->right)) {			/* The given key is too big and there're no more nodes to check. */

            return NULL;
        }

        return rb_tree_search_smallest_from_node(tree, node->right, key);
    }
}


rb_tree_node* rb_tree_search_smallest_from(rb_tree *tree, void *key)
{

	/* Search the tree for a node with the smallest key that is larger than or equal to the given key, starting from the root of the tree. */

    rb_tree_node *node = rb_tree_search_smallest_from_node(tree, tree->root, key);

    if (NULL == node) {

        return NULL;
    }

    return node;			/* Returns a pointer to the node containing the key if found, NULL otherwise. */
}

