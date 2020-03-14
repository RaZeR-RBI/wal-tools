#include "wal.h"
#include "endian-util.h"

#include <stddef.h>
#include <string.h>

#define HEADER_OFFSET(ptr, X) (ptr + offsetof(struct wal_header, X))

unsigned char *wal_read_header(const unsigned char *buffer, struct wal_header *out)
{
	unsigned char *ptr = (unsigned char *)buffer;
	memcpy(&(out->name), ptr, NAME_LEN);
	ptr += NAME_LEN;
	out->width = read_le32(&ptr);
	out->height = read_le32(&ptr);
	for (int i = 0; i < MIP_LEVELS; i++)
	{
		out->offsets[i] = read_le32(&ptr);
	}
	memcpy(&(out->animname), ptr, NAME_LEN);
	ptr += NAME_LEN;
	out->flags = read_le32(&ptr);
	out->contents = read_le32(&ptr);
	out->value = read_le32(&ptr);
	return ptr;
}