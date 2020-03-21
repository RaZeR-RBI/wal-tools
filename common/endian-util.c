#include "endian-util.h"

uint16_t read_le16(unsigned char **buffer)
{
	uint8_t b1, b2;
	b1 = *(uint8_t *)(*buffer);
	b2 = *(uint8_t *)(*buffer + 1);
	uint16_t result = le16toh((b1) + (b2 << 8));
	(*buffer) += 2;
	return result;
}

uint32_t read_le32(unsigned char **buffer)
{
	uint8_t b1, b2, b3, b4;
	b1 = *(uint8_t *)(*buffer);
	b2 = *(uint8_t *)(*buffer + 1);
	b3 = *(uint8_t *)(*buffer + 2);
	b4 = *(uint8_t *)(*buffer + 3);
	uint32_t result = le32toh((b1) + (b2 << 8) + (b3 << 16) + (b4 << 24));
	(*buffer) += 4;
	return result;
}

void write_le16(uint16_t val, unsigned char **buffer)
{
	**(uint16_t **)buffer = htole16(val);
	(*buffer) += 2;
}

void write_le32(uint32_t val, unsigned char **buffer)
{
	**(uint32_t **)buffer = htole32(val);
	(*buffer) += 4;
}