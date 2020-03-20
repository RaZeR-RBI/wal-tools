#ifndef _IMAGE_H
#define _IMAGE_H

#include "mem.h"

enum image_type {
	IMAGE_TYPE_WAL_Q2 = 0x1,
	IMAGE_TYPE_WAL_DK = 0x2,
	IMAGE_TYPE_TGA = 0x3
};

enum palette_type { PALETTE_TYPE_RGB_256 = 0x1 };

struct image_palette {
	sptr_t data;
	uint8_t type;
};

struct image_data {
	sptr_t pixels;
	struct image_palette palette;
	uint8_t type; /* image_type */
	void *header; /* pointer to original header struct */
	void *userdata;
};

#endif /* _IMAGE_H */