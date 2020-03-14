#include "wal.h"
#include <endian.h>
#include <stddef.h>
#include <string.h>

#define HEADER_OFFSET(addr, X) (addr + offsetof(struct wal_header, X))

static int32_t read_uint32_le(void *addr)
{
	uint8_t b1, b2, b3, b4;
	b1 = *(uint8_t *)(addr);
	b2 = *(uint8_t *)(addr + 1);
	b3 = *(uint8_t *)(addr + 2);
	b4 = *(uint8_t *)(addr + 3);
	return le32toh((b1) + (b2 << 8) + (b3 << 16) + (b4 << 24));
}

void wal_read_header(void *buffer, struct wal_header *out)
{
	memcpy(&(out->name), buffer, NAME_LEN);
	out->width = read_uint32_le(HEADER_OFFSET(buffer, width));
	out->height = read_uint32_le(HEADER_OFFSET(buffer, height));
	for (int i = 0; i < MIP_LEVELS; i++)
	{
		out->offsets[i] = read_uint32_le(HEADER_OFFSET(buffer, offsets[i]));
	}
	memcpy(&(out->animname), HEADER_OFFSET(buffer, animname), NAME_LEN);
	out->flags = read_uint32_le(HEADER_OFFSET(buffer, flags));
	out->contents = read_uint32_le(HEADER_OFFSET(buffer, contents));
	out->value = read_uint32_le(HEADER_OFFSET(buffer, value));
}