#ifndef _ENDIAN_UTIL_H
#define _ENDIAN_UTIL_H

#include <stdint.h>

uint8_t read_8(unsigned char **buffer);
uint16_t read_le16(unsigned char **buffer);
uint32_t read_le32(unsigned char **buffer);
void write_8(uint8_t val, unsigned char **buffer);
void write_le16(uint16_t val, unsigned char **buffer);
void write_le32(uint32_t val, unsigned char **buffer);

#endif /* _ENDIAN_UTIL_H */