#include <stdlib.h>
#include <string.h>

#include "list.h"

struct ll_node *ll_create_node(const void *ptr, size_t isize)
{
	if (ptr == NULL || isize <= 0) {
		return NULL;
	}
	struct ll_node *node = malloc(sizeof(struct ll_node));
	node->next = NULL;
	node->value_ptr = malloc(isize);
	node->value_size = isize;
	memcpy(node->value_ptr, ptr, isize);
	return node;
}

void ll_free_node(struct ll_node *node)
{
	if (node == NULL) {
		return;
	}
	free(node->value_ptr);
	free(node);
}

struct ll_node *ll_from_array(const void *ptr, size_t isize, size_t n)
{
	struct ll_node *root_node = NULL;
	struct ll_node *prev_node = NULL;
	struct ll_node *cur_node = NULL;
	void *item_ptr = (void *)ptr;
	if (n <= 0 || isize <= 0 || ptr == NULL) {
		return NULL;
	}
	for (int i = 0; i < n; i++) {
		if (cur_node == NULL) {
			cur_node = ll_create_node(item_ptr, isize);
			root_node = cur_node;
		} else {
			prev_node = cur_node;
			cur_node = ll_create_node(item_ptr, isize);
			if (prev_node != NULL) {
				prev_node->next = cur_node;
			}
		}
		item_ptr += isize;
	}
	return root_node;
}

size_t ll_size(const struct ll_node *node)
{
	size_t length = 0;
	struct ll_node *cur_node = (struct ll_node *)node;
	while (cur_node != NULL) {
		cur_node = cur_node->next;
		length++;
	}
	return length;
}

size_t ll_free(struct ll_node *node)
{
	struct ll_node *cur_node = node;
	struct ll_node *next_node = NULL;
	size_t count = 0;
	if (node == NULL) {
		return 0;
	}
	do {
		next_node = cur_node->next;
		ll_free_node(cur_node);
		cur_node = next_node;
		count++;
	} while (cur_node != NULL);
	return count;
}

struct ll_node *ll_last(struct ll_node *node)
{
	if (node == NULL) {
		return NULL;
	}
	struct ll_node *cur_node = (struct ll_node *)node;
	while (cur_node->next != NULL) {
		cur_node = cur_node->next;
	}
	return cur_node;
}

struct ll_node *ll_append(struct ll_node *root, const void *ptr, size_t isize)
{
	struct ll_node *new_node = ll_create_node(ptr, isize);
	struct ll_node *result = ll_append_node(root, new_node);
	if (new_node != NULL && result != NULL) {
		return result;
	}
	ll_free_node(new_node);
	return NULL;
}

struct ll_node *ll_append_node(struct ll_node *root, struct ll_node *node)
{
	struct ll_node *last_node = ll_last(root);
	if (last_node == NULL) {
		return NULL;
	}
	last_node->next = node;
	return node;
}