#ifndef _LLIST_H
#define _LLIST_H
#include <stdlib.h>

typedef void (*ll_closure)(void *);

/* Linked list node */
struct ll_node {
	void *value_ptr;
	size_t value_size;
	struct ll_node *next;
};

/*
Creates a linked list node and copies the pointer value of size isize into it.
*/
struct ll_node* ll_create_node(const void *ptr, size_t isize);

/*
Frees a linked list node along with the pointed value.
*/
void ll_free_node(struct ll_node* node);

/*
Creates a linked list from specified array pointer by copying
n items of size isize into the heap.
*/
struct ll_node *ll_from_array(const void *ptr, size_t isize, size_t n);

/*
Returns the length of a specified linked list (or 0 if it's NULL)
*/
size_t ll_size(const struct ll_node *node);

/*
Frees a linked list along with it's elements starting from the specified node.
Returns the count of the removed elements.
*/
size_t ll_free(struct ll_node *node);

/*
Returns the last element of the specified linked list.
*/
struct ll_node* ll_last(struct ll_node *node);

/*
Copies the specified value located at ptr of size isize into a new node and
appends it to the end of the specified linked list.
*/
struct ll_node* ll_append(struct ll_node *root, const void *ptr, size_t isize);

/*
Appends a node to the end of the specified linked list.
*/
struct ll_node* ll_append_node(struct ll_node* root, struct ll_node *node);

#endif /* _LLIST_H */