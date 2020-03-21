#ifndef _ENDIAN_UTIL_H
#define _ENDIAN_UTIL_H

#include <endian.h>
#include <stdint.h>

uint16_t read_le16(unsigned char **buffer);
uint32_t read_le32(unsigned char **buffer);
void write_le16(uint16_t val, unsigned char **buffer);
void write_le32(uint32_t val, unsigned char **buffer);

#endif /* _ENDIAN_UTIL_H */