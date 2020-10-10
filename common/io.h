#ifndef _IO_H
#define _IO_H

#include "mem.h"

#ifdef _WIN32
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

sptr_t file_read(const char *path, const char *mode);

int file_write(sptr_t data, const char *path, const char *mode);
const char *trim_extension(const char *s);

#endif /* _IO_H */