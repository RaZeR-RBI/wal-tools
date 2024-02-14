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

size_t tga_estimate_size(struct image_data image, int expand)
{
	if (!is_supported(image)) {
		return 0;
	}
	if (expand) {
		return TGA_HEADER_SIZE + image.pixels.size * ( image.alpha_pixels ? 4 : 3 );
	} else {
		return TGA_HEADER_SIZE + image.pixels.size + 768;
	}
}

static sptr_t tga_write_header(sptr_t buf, struct image_data image, int expand)
{
	(void)image;
	// TODO: replace with endian_util.h/write_8
	unsigned char *p = buf.ptr;
	*p = 0;
	p++;
	*p = expand ? TGA_COLOR_MAP_NONE : TGA_COLOR_MAP_PRESENT;
	p++;
	*p = expand ? TGA_IMAGE_TYPE_UNCOMPRESSED_TRUE_COLOR : TGA_IMAGE_TYPE_UNCOMPRESSED_COLOR_MAPPED;
	p++;

	write_le16(0, &p); // color map origin
	write_le16(expand ? 0 : 256, &p); // color map length
	*p = expand ? 0 : 24; // color map depth
	p++;

	write_le16(0, &p); // x origin
	write_le16(0, &p); // y origin
	write_le16(image.width, &p);
	write_le16(image.height, &p);
	*p = expand ? ( image.alpha_pixels ? 32 : 24 ) : 8; // bits per pixel
	p++;
	*p = TGA_IM_DESCRIPTOR(TGA_IMAGE_ORIGIN_TOP_LEFT, 0);
	p++;
	return (sptr_t){buf.ptr + TGA_HEADER_SIZE, (buf.size - TGA_HEADER_SIZE)};
}

sptr_t tga_write(sptr_t buf, struct image_data image, int expand)
{
	size_t size = tga_estimate_size(image, expand);
	if (size == 0 || buf.size < size) {
		return SPTR_NULL;
	}
	memset(buf.ptr, 0, buf.size);
	sptr_t cm_offset = tga_write_header(buf, image, expand);
	if (!expand)
	{
		/* copy palette data */
		memcpy(cm_offset.ptr, image.palette.data.ptr, image.palette.data.size);
		/* flip from RGB to BGR in place */
		sptr_t cm_palette = sptr_slice(cm_offset, 0, image.palette.data.size);
		flip_bgr(cm_palette);
		/* copy pixel data */
		memcpy(cm_offset.ptr + image.palette.data.size, image.pixels.ptr,
			image.pixels.size);
	}
	else
	{
		unsigned char *buf = cm_offset.ptr;
		int num_pixels = image.pixels.size;
		if (image.alpha_pixels)
		{
			/* write BGRA */
			for (int i = 0; i < num_pixels; i++)
			{
				int j = image.pixels.ptr[i];

				if (j == 255)
				{
					// transparent, so scan around for another color
				    // to avoid alpha fringes
				    // FIXME: do a full flood fill so mips work...
					/*
				    if (i > image.width && image.pixels.ptr[i-image.width] != 255)
					    j = image.pixels.ptr[i-image.width];
				    else if (i < num_pixels-image.width && image.pixels.ptr[i+image.width] != 255)
					    j = image.pixels.ptr[i+image.width];
				    else if (i > 0 && image.pixels.ptr[i-1] != 255)
					    j = image.pixels.ptr[i-1];
				    else if (i < num_pixels-1 && image.pixels.ptr[i+1] != 255)
					    j = image.pixels.ptr[i+1];
				    else
					    j = 0;

					buf[i * 4 + 0] = image.palette.data.ptr[j * 3 + 2];
					buf[i * 4 + 1] = image.palette.data.ptr[j * 3 + 1];
					buf[i * 4 + 2] = image.palette.data.ptr[j * 3 + 0];
					*/
					buf[i * 4 + 0] = 0;
					buf[i * 4 + 1] = 0;
					buf[i * 4 + 2] = 0;
					buf[i * 4 + 3] = 0;
				}
				else
				{
					buf[i * 4 + 0] = image.palette.data.ptr[j * 3 + 2];
					buf[i * 4 + 1] = image.palette.data.ptr[j * 3 + 1];
					buf[i * 4 + 2] = image.palette.data.ptr[j * 3 + 0];
					buf[i * 4 + 3] = 255;
				}
			}
		}
		else
		{
			/* write BGR */
			for (int i = 0; i < num_pixels; i++)
			{
				int j = image.pixels.ptr[i] * 3;

				buf[i * 3 + 0] = image.palette.data.ptr[j + 2];
				buf[i * 3 + 1] = image.palette.data.ptr[j + 1];
				buf[i * 3 + 2] = image.palette.data.ptr[j + 0];
			}
		}
	}
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

static void tga_read_rle_indexed(struct image_data *im, const sptr_t data)
{
	int i;
	unsigned char b;
	unsigned char *p = data.ptr;
	unsigned char *d;
	int pixel_count = 0;
	int run_count = 0;
	im->pixels = sptr_xmalloc(im->width * im->height);
	d = im->pixels.ptr;
	while (pixel_count < im->pixels.size) {
		b = *p;
		run_count = (b & ~128) + 1;
		p++;
		if (b & 128) { // RLE packet
			for (i = 0; i < run_count; i++) {
				*d = *p;
				d++;
			}
			p++;
		} else { // raw packet
			memcpy(d, p, run_count);
			d += run_count;
			p += run_count;
		}
		pixel_count += run_count;
	}
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
	} else if ((header->image_type == TGA_IMAGE_TYPE_RLE_COLOR_MAPPED) &&
			   (header->im_pixel_depth == 8)) {
		tga_read_rle_indexed(im, content);
	}
	return im;
}