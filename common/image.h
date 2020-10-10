#ifndef _IMAGE_H
#define _IMAGE_H

#include "mem.h"

enum image_type {
	IMAGE_TYPE_WAL_Q2 = 0x1,
	IMAGE_TYPE_WAL_DK = 0x2,
	IMAGE_TYPE_TGA = 0x3
};

enum palette_type {
	PALETTE_TYPE_NONE = 0x0,
	PALETTE_TYPE_RGB_256 = 0x1,
	PALETTE_TYPE_UNKNOWN = 0xFF
};

struct image_palette {
	sptr_t data;
	uint8_t type;
};

typedef struct image_data {
	sptr_t pixels;
	struct image_palette palette;
	uint8_t type; /* image_type */
	uint32_t width;
	uint32_t height;
	void *header; /* pointer to original header struct */
	void *userdata;
} image_data_t;

void flip_bgr(sptr_t colors);

struct rgb {
	uint8_t r, g, b;
};

#define RGB_BLACK ((struct rgb){0, 0, 0})

#endif /* _IMAGE_H */