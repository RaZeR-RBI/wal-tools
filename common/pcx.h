#ifndef _TGA_H
#define _TGA_H

#include <stdint.h>

#include "image.h"

#define PCX_EGA_PALETTE_SIZE 48
#define PCX_HEADER_RESERVED_SIZE 54

enum pcx_version {
	PCX_VERSION_EGA_2_5 = 0x0,
	PCX_VERSION_EGA_2_8 = 0x2,
	PCX_VERSION_NO_PALETTE_2_8 = 0x3,
	PCX_VERSION_WINDOWS = 0x4,
	PCX_VERSION_24BIT = 0x5
};

enum pcx_encoding_type { PCX_ENCODING_NONE = 0x0, PCX_ENCODING_RLE = 0x1 };
enum pcx_palette_type { PCX_PALETTE_COLOR = 0x1, PCX_PALETTE_GRAYSCALE = 0x2 };

struct pcx_header {
	uint8_t magic; /* 0x0a */
	uint8_t version;
	uint8_t encoding; /* pcx_encoding_type */
	uint8_t bpp;
	uint16_t xmin, ymin, xmax, ymax;
	uint16_t hdpi, vdpi;
	uint8_t ega_palette[PCX_EGA_PALETTE_SIZE];
	uint8_t _reserved_1;
	uint8_t planes;
	uint16_t scanline_bytes;
	uint16_t palette_type; /* pcx_palette_type */
	uint16_t hres, vres;
	uint8_t _reserved_2[PCX_HEADER_RESERVED_SIZE];
};

#define PCX_HEADER_SIZE 128
#define PCX_PALETTE_SIZE 768
struct image_data *pcx_read(const sptr_t data);

#endif /* _TGA_H */