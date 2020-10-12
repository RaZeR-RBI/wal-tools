#include <string.h>

#include "endian-util.h"
#include "mem.h"
#include "pcx.h"

static struct pcx_header *read_header(const sptr_t data)
{
	unsigned char *ptr = data.ptr;
	struct pcx_header *header = xmalloc(PCX_HEADER_SIZE);
	header->magic = read_8(&ptr);
	header->version = read_8(&ptr);
	header->encoding = read_8(&ptr);
	header->bpp = read_8(&ptr);
	header->xmin = read_le16(&ptr);
	header->ymin = read_le16(&ptr);
	header->xmax = read_le16(&ptr);
	header->ymax = read_le16(&ptr);
	header->hdpi = read_le16(&ptr);
	header->vdpi = read_le16(&ptr);
	memcpy(&header->ega_palette[0], ptr, PCX_EGA_PALETTE_SIZE);
	ptr += PCX_EGA_PALETTE_SIZE;
	header->_reserved_1 = read_8(&ptr);
	header->planes = read_8(&ptr);
	header->scanline_bytes = read_le16(&ptr);
	header->palette_type = read_le16(&ptr);
	header->hres = read_le16(&ptr);
	header->vres = read_le16(&ptr);
	memcpy(&header->_reserved_2, ptr, PCX_HEADER_RESERVED_SIZE);
	return header;
}

struct image_data *pcx_read(const sptr_t data)
{
	unsigned char has_palette = 0;
	struct image_data *im = xmalloc(sizeof(struct image_data));
	struct pcx_header *header = read_header(data);
	sptr_t content = sptr_advance(data, PCX_HEADER_SIZE);
	im->type = IMAGE_TYPE_PCX;
	im->header = header;
	if (content.size > 768) {
		has_palette = content.ptr[content.size - PCX_PALETTE_SIZE - 1] == 0x0C;
	}
	im->width = header->xmax - header->xmin;
	im->height = header->ymax - header->ymin;
	im->pixels = has_palette ? sptr_xmalloc(content.size - PCX_PALETTE_SIZE)
							 : sptr_xmalloc(content.size);
	memcpy(im->pixels.ptr, content.ptr, im->pixels.size);
	content = sptr_advance(content, im->pixels.size);
	if (has_palette) {
		im->palette = (struct image_palette){sptr_xmalloc(PCX_PALETTE_SIZE),
											 PALETTE_TYPE_RGB_256};
		memcpy(im->palette.data.ptr, content.ptr, PCX_PALETTE_SIZE);
	} else {
		im->palette = (struct image_palette){SPTR_NULL, PALETTE_TYPE_NONE};
	}
	return im;
}