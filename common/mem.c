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

char *xstrdup(const char *str)
{
	size_t length = strlen(str);
	char *result = xmalloc(length + 1);
	memcpy(result, str, length);
	*(result + length) = '\0';
	return result;
}