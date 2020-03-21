#ifndef _MEM_H
#define _MEM_H

#include <stdint.h>
#include <stdlib.h>

#define PTR_SIZE (sizeof(intptr_t))
void *xmalloc(size_t size);

typedef struct sized_ptr {
	unsigned char *ptr;
	size_t size;
} sptr_t;

#define SPTR_NULL ((sptr_t){NULL, 0})
#define SPTR_IS_NULL(x) (x.ptr == NULL)

sptr_t sptr_xmalloc(size_t size);
sptr_t sptr_slice(sptr_t ptr, size_t from, size_t n);
void sptr_free(sptr_t* ptr);

#endif /* _MEM_H */