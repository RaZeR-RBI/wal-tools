#ifndef _ENDIAN_UTIL_H
#define _ENDIAN_UTIL_H

#include <endian.h>
#include <stdint.h>

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

#endif /* _ENDIAN_UTIL_H */