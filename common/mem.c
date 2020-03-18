#include "mem.h"

#include <stdio.h>
#include <stdlib.h>

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
