#include "endian-util.h"

uint8_t read_8(unsigned char **buffer)
{
	uint8_t result = *(uint8_t*)(*buffer);
	(*buffer)++;
	return result;
}

uint16_t read_le16(unsigned char **buffer)
{
	uint8_t b1, b2;
	b1 = *(uint8_t *)(*buffer);
	b2 = *(uint8_t *)(*buffer + 1);
	uint16_t result = (b1 << 0) | (b2 << 8);
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
	uint32_t result = (b1 << 0) | (b2 << 8) | (b3 << 16) | (b4 << 24);
	(*buffer) += 4;
	return result;
}

void write_8(uint8_t val, unsigned char **buffer)
{
	**(uint8_t **)buffer = val;
	(*buffer)++;
}

void write_le16(uint16_t val, unsigned char **buffer)
{
	**(uint8_t **)buffer = (val & 0xff);
	(*buffer) ++;
	**(uint8_t **)buffer = (val & 0xff00) >> 8;
	(*buffer) ++;
}

void write_le32(uint32_t val, unsigned char **buffer)
{
	**(uint8_t **)buffer = (val & 0xff);
	(*buffer) ++;
	**(uint8_t **)buffer = (val & 0xff00) >> 8;
	(*buffer) ++;
	**(uint8_t **)buffer = (val & 0xff0000) >> 16;
	(*buffer) ++;
	**(uint8_t **)buffer = (val & 0xff000000) >> 24;
	(*buffer) ++;
}