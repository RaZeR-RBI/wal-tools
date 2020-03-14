#ifndef _WAL_H
#define _WAL_H

#include "defs.h"
#include <stdint.h>

#define MIP_LEVELS 4
#define NAME_LEN 32

struct wal_header
{
	char name[NAME_LEN];
	uint32_t width, height;
	uint32_t offsets[MIP_LEVELS];
	char animname[NAME_LEN];
	uint32_t flags, contents, value;
};

#define WAL_HEADER_SIZE (sizeof(struct wal_header))

unsigned char *wal_read_header(const unsigned char *buffer, struct wal_header *out);

#endif /* _WAL_H */