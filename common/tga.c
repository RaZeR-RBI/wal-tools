#include "tga.h"
#include "endian-util.h"

#include <stdio.h>
#include <string.h>

static int is_supported(struct image_data image)
{
	// Note: we're supporting only 256-color indexed images
	if ((image.type != IMAGE_TYPE_WAL_Q2 && image.type != IMAGE_TYPE_WAL_DK) ||
		SPTR_IS_NULL(image.palette.data) ||
		image.palette.type != PALETTE_TYPE_RGB_256) {
		return 0;
	}
	return 1;
}

size_t tga_estimate_size(struct image_data image)
{
	if (!is_supported(image)) {
		return 0;
	}
	return TGA_HEADER_SIZE + image.pixels.size + 768;
}

static sptr_t tga_write_header(sptr_t buf, struct image_data image)
{
	(void)image;
	// TODO: replace with endian_util.h/write_8
	unsigned char *p = buf.ptr;
	*p = 0;
	p++;
	*p = TGA_COLOR_MAP_PRESENT;
	p++;
	*p = TGA_IMAGE_TYPE_UNCOMPRESSED_COLOR_MAPPED;
	p++;

	write_le16(0, &p);
	write_le16(256, &p);
	*p = 24;
	p++;

	write_le16(0, &p);
	write_le16(0, &p);
	write_le16(image.width, &p);
	write_le16(image.height, &p);
	*p = 8;
	p++;
	*p = TGA_IM_DESCRIPTOR(TGA_IMAGE_ORIGIN_TOP_LEFT, 0);
	p++;
	return (sptr_t){buf.ptr + TGA_HEADER_SIZE, (buf.size - TGA_HEADER_SIZE)};
}

sptr_t tga_write(sptr_t buf, struct image_data image)
{
	size_t size = tga_estimate_size(image);
	if (size == 0 || buf.size < size) {
		return SPTR_NULL;
	}
	memset(buf.ptr, 0, buf.size);
	sptr_t cm_offset = tga_write_header(buf, image);
	/* copy palette data */
	memcpy(cm_offset.ptr, image.palette.data.ptr, image.palette.data.size);
	/* flip from RGB to BGR in place */
	sptr_t cm_palette = sptr_slice(cm_offset, 0, image.palette.data.size);
	flip_bgr(cm_palette);
	/* copy pixel data */
	memcpy(cm_offset.ptr + image.palette.data.size, image.pixels.ptr,
		   image.pixels.size);
	return buf;
}

static sptr_t tga_read_header(const sptr_t data, struct tga_header *out)
{
	uint8_t im_descriptor;
	unsigned char *ptr = data.ptr;

	out->id_length = read_8(&ptr);
	out->color_map_type = read_8(&ptr);
	out->image_type = read_8(&ptr);
	/* color map spec */
	out->cm_first_entry_index = read_le16(&ptr);
	out->cm_entry_count = read_le16(&ptr);
	out->cm_entry_size = read_8(&ptr);
	/* image spec */
	out->im_origin_x = read_le16(&ptr);
	out->im_origin_y = read_le16(&ptr);
	out->im_width = read_le16(&ptr);
	out->im_height = read_le16(&ptr);
	out->im_pixel_depth = read_8(&ptr);
	im_descriptor = read_8(&ptr);
	out->im_origin = TGA_IM_ORIGIN(im_descriptor);
	out->im_alpha_bits = TGA_IM_ALPHA_BITS(im_descriptor);

	return sptr_advance(data, TGA_HEADER_SIZE);
}

struct image_data *tga_read(const sptr_t data)
{
	struct tga_header *header = xmalloc(sizeof(struct tga_header));
	sptr_t content = tga_read_header(data, header);
	sptr_t palette;
	size_t cm_size = 0;
	struct image_data *im = xmalloc(sizeof(struct image_data));
	if (header->color_map_type > 0) {
		cm_size = (header->cm_entry_count * header->cm_entry_size) / 8;
	}
	im->header = header;
	im->type = IMAGE_TYPE_TGA;
	im->width = header->im_width;
	im->height = header->im_height;
	im->palette = (struct image_palette){SPTR_NULL, PALETTE_TYPE_NONE};
	// skip image ID
	content = sptr_advance(content, header->id_length);
	// color map
	if (header->color_map_type > 0) {
		palette = sptr_xmalloc(cm_size);
		memcpy(palette.ptr, content.ptr, cm_size);
		if (header->cm_entry_size == 24 && cm_size == 3 * 256) {
			flip_bgr(palette);
			im->palette = (struct image_palette){palette, PALETTE_TYPE_RGB_256};
		} else {
			im->palette = (struct image_palette){palette, PALETTE_TYPE_UNKNOWN};
		}
	}
	content = sptr_advance(content, cm_size);
	// pixels
	im->pixels = SPTR_NULL;
	if ((header->image_type == TGA_IMAGE_TYPE_UNCOMPRESSED_COLOR_MAPPED) &&
		(header->im_pixel_depth == 8)) {
		im->pixels = sptr_xmalloc(im->width * im->height);
		memcpy(im->pixels.ptr, content.ptr, im->pixels.size);
	}
	return im;
}