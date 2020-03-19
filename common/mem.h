#ifndef _MEM_H
#define _MEM_H

#include <stdint.h>
#include <stdlib.h>

#define PTR_SIZE (sizeof(intptr_t))
void *xmalloc(size_t size);

char* xstrdup(const char* str);

#endif /* _MEM_H */