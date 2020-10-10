#include "mem.h"

#include <stdio.h>
#include <string.h>

void *xmalloc(size_t size)
{
	if (size == 0) {
		size = 1;
	}
	void *result = malloc(size);
	if (result == NULL) {
		fprintf(stderr, "Out of memory - unable to allocate %lu bytes", size);
		abort();
	}
	return result;
}

char *xstrdup(const char *s)
{
	size_t size = strlen(s);
	char *result = xmalloc(size + 1);
	memcpy(result, s, size);
	*(result + size) = '\0';
	return result;
}

sptr_t sptr_xmalloc(size_t size)
{
	unsigned char *p = xmalloc(size);
	return (sptr_t){p, size};
}

sptr_t sptr_slice(sptr_t ptr, size_t from, size_t n)
{
	if (SPTR_IS_NULL(ptr) || n == 0) {
		return SPTR_NULL;
	}
	if (from + n > ptr.size) {
		return SPTR_NULL;
	}
	return (sptr_t){ptr.ptr + from, n};
}

sptr_t sptr_advance(sptr_t ptr, size_t n)
{
	if (n >= ptr.size) {
		return SPTR_NULL;
	} else if (n == 0) {
		return ptr;
	}
	return (sptr_t){ptr.ptr + n, ptr.size - n};
}

void sptr_free(sptr_t *ptr)
{
	free(ptr->ptr);
	ptr->ptr = NULL;
	ptr->size = 0;
}