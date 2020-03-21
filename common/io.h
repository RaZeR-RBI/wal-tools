#ifndef _IO_H
#define _IO_H

#include "mem.h"

sptr_t file_read(const char *path, const char *mode);

int file_write(sptr_t data, const char *path, const char *mode);

#endif /* _IO_H */