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