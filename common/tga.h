#ifndef _TGA_H
#define _TGA_H

#include <stdint.h>

#include "image.h"

enum tga_color_map { TGA_COLOR_MAP_NONE = 0x0, TGA_COLOR_MAP_PRESENT = 0x1 };
enum tga_image_type {
	TGA_IMAGE_TYPE_NO_IMAGE = 0x0,
	TGA_IMAGE_TYPE_UNCOMPRESSED_COLOR_MAPPED = 0x1,
	TGA_IMAGE_TYPE_UNCOMPRESSED_TRUE_COLOR = 0x2,
	TGA_IMAGE_TYPE_UNCOMPRESSED_BW = 0x3,
	TGA_IMAGE_TYPE_RLE_COLOR_MAPPED = 0x9,
	TGA_IMAGE_TYPE_RLE_TRUE_COLOR = 0xA,
	TGA_IMAGE_TYPE_RLE_BW = 0xB,
};

enum tga_image_origin {
	TGA_IMAGE_ORIGIN_BOTTOM_LEFT = 0x0,
	TGA_IMAGE_ORIGIN_BOTTOM_RIGHT = 0x1,
	TGA_IMAGE_ORIGIN_TOP_LEFT = 0x2,
	TGA_IMAGE_ORIGIN_TOP_RIGHT = 0x3,
};

#define TGA_IM_DESCRIPTOR(o, a) (((o & 0x3) << 4) + (a & 0xF))
#define TGA_IM_ORIGIN(x) (x & 0x30) >> 4
#define TGA_IM_ALPHA_BITS(x) (x & 0xF)

struct tga_header {
	uint8_t id_length;
	uint8_t color_map_type; /* tga_color_map */
	uint8_t image_type;		/* tga_image_type */
	/* color map spec */
	uint16_t cm_first_entry_index;
	uint16_t cm_entry_count;
	uint8_t cm_entry_size;
	/* image spec */
	uint16_t im_origin_x;
	uint16_t im_origin_y;
	uint16_t im_width;
	uint16_t im_height;
	uint8_t im_pixel_depth;
	uint8_t : 2;
	uint8_t im_origin : 2;
	uint8_t im_alpha_bits : 4;
};

#define TGA_HEADER_SIZE 18

size_t tga_estimate_size(struct image_data image, int expand);
sptr_t tga_write(sptr_t buf, const struct image_data image, int expand);
struct image_data *tga_read(const sptr_t data);

#endif /* _TGA_H */