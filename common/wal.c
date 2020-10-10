#include "wal.h"
#include "endian-util.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define is_printable(x) (x >= 0x20 && x <= 0x7E)

static int invalid_name(const unsigned char *ptr, int nonnull)
{
	int i;
	for (i = 0; i < NAME_LEN; i++) {
		if (!is_printable(*ptr) && *ptr != 0) {
			return 1;
		}
		if (nonnull && i == 0 && *ptr == 0) {
			return 1;
		}
		ptr++;
	}
	return 0;
}

int32_t wal_get_type(const unsigned char *buffer)
{
	uint8_t version = (uint8_t)(*buffer);
	if (version == 3) {
		goto possibly_daikatana;
	} else if (is_printable(version) || (version == 0)) {
		goto possibly_quake2;
	}
	goto not_a_wal;

possibly_daikatana:
	if (invalid_name(buffer + offsetof(struct wal_dk_header, name), 0) ||
		invalid_name(buffer + offsetof(struct wal_dk_header, animname), 0)) {
		goto not_a_wal;
	}
	return WAL_TYPE_DAIKATANA;
possibly_quake2:
	if (invalid_name(buffer + offsetof(struct wal_q2_header, name), 0) ||
		invalid_name(buffer + offsetof(struct wal_q2_header, animname), 0)) {
		goto not_a_wal;
	}
	return WAL_TYPE_QUAKE2;
not_a_wal:
	return WAL_NOT_A_WAL;
}

unsigned char *wal_q2_read_header(const unsigned char *buffer,
								  struct wal_q2_header *out)
{
	int i;
	unsigned char *ptr = (unsigned char *)buffer;
	memcpy(&(out->name), ptr, NAME_LEN);
	ptr += NAME_LEN;
	out->width = read_le32(&ptr);
	out->height = read_le32(&ptr);
	for (i = 0; i < MIP_LEVELS_Q2; i++) {
		out->offsets[i] = read_le32(&ptr);
	}
	memcpy(&(out->animname), ptr, NAME_LEN);
	ptr += NAME_LEN;
	out->flags = read_le32(&ptr);
	out->contents = read_le32(&ptr);
	out->value = read_le32(&ptr);
	return ptr;
}

unsigned char *wal_dk_read_header(const unsigned char *buffer,
								  struct wal_dk_header *out)
{
	int i;
	unsigned char *ptr = (unsigned char *)(buffer + 4);
	memcpy(&(out->name), ptr, NAME_LEN);
	ptr += NAME_LEN;
	out->width = read_le32(&ptr);
	out->height = read_le32(&ptr);
	for (i = 0; i < MIP_LEVELS_DK; i++) {
		out->offsets[i] = read_le32(&ptr);
	}
	memcpy(&(out->animname), ptr, NAME_LEN);
	ptr += NAME_LEN;
	out->flags = read_le32(&ptr);
	out->contents = read_le32(&ptr);
	memcpy(&(out->palette), ptr, DK_PALETTE_SIZE);
	ptr += DK_PALETTE_SIZE;
	out->value = read_le32(&ptr);
	return ptr;
}

static struct ll_node *wal_q2_read(const sptr_t data, const sptr_t palette)
{
	int i;
	struct image_data mips[MIP_LEVELS_Q2];
	struct wal_q2_header *header = xmalloc(sizeof(struct wal_q2_header));
	wal_q2_read_header(data.ptr, header);
	size_t expected_len = header->width * header->height;
	for (i = 0; i < MIP_LEVELS_Q2; i++) {
		sptr_t p = sptr_slice(data, header->offsets[i], expected_len);
		if (SPTR_IS_NULL(p)) {
			goto err;
		}
		sptr_t copy = {xmalloc(p.size), p.size};
		memcpy(copy.ptr, p.ptr, p.size);
		struct image_palette p_data = {palette, PALETTE_TYPE_RGB_256};
		struct image_data i_data = {copy,
									p_data,
									IMAGE_TYPE_WAL_Q2,
									header->width / (i + 1),
									header->height / (i + 1),
									header,
									NULL};
		mips[i] = i_data;
		expected_len /= 4;
		if (expected_len <= 0) {
			expected_len = 1;
		}
	}
	return ll_from_array(&mips, sizeof(struct image_data), MIP_LEVELS_Q2);
err:
	free(header);
	return NULL;
}

static struct ll_node *wal_dk_read(const sptr_t data, const sptr_t palette)
{
	int i;
	struct image_data mips[MIP_LEVELS_DK];
	struct wal_dk_header *header = xmalloc(sizeof(struct wal_dk_header));
	wal_dk_read_header(data.ptr, header);

	sptr_t pal = palette;
	if (SPTR_IS_NULL(pal)) {
		pal.ptr = &header->palette[0];
		pal.size = DK_PALETTE_SIZE;
	}
	size_t expected_len = header->width * header->height;
	int divisor = 1;
	for (i = 0; i < MIP_LEVELS_DK; i++) {
		sptr_t p = sptr_slice(data, header->offsets[i], expected_len);
		if (SPTR_IS_NULL(p)) {
			goto err;
		}
		sptr_t copy = {xmalloc(p.size), p.size};
		memcpy(copy.ptr, p.ptr, p.size);

		struct image_palette p_data = {pal, PALETTE_TYPE_RGB_256};
		uint32_t width = header->width / divisor;
		if (width == 0) {
			width = 1;
		}
		uint32_t height = header->height / divisor;
		if (height == 0) {
			height = 1;
		}
		struct image_data i_data = {
			copy, p_data, IMAGE_TYPE_WAL_DK, width, height, header, NULL};
		mips[i] = i_data;
		expected_len /= 4;
		if (expected_len <= 0) {
			expected_len = 1;
		}
		divisor *= 2;
	}
	return ll_from_array(&mips, sizeof(struct image_data), MIP_LEVELS_DK);
err:
	free(header);
	return NULL;
}

struct ll_node *wal_read(const sptr_t data, const sptr_t palette)
{
	if (SPTR_IS_NULL(data)) {
		return NULL;
	}
	/* Daikatana header is bigger */
	if (data.size < WAL_DK_HEADER_SIZE) {
		return NULL;
	}
	switch (wal_get_type(data.ptr)) {
		case WAL_TYPE_QUAKE2:
			return wal_q2_read(data, palette);
		case WAL_TYPE_DAIKATANA:
			return wal_dk_read(data, palette);
		default:
			return NULL;
	}
}

size_t wal_estimate_size(struct image_data im, int wal_type)
{
	size_t result = 0, mip = 0;
	uint32_t width = im.width, height = im.height, size = 0;
	switch (wal_type) {
		case WAL_TYPE_QUAKE2:
			result += WAL_Q2_HEADER_SIZE;
			for (mip = 0; mip < MIP_LEVELS_Q2; mip++) {
				size = width * height;
				result += size > 0 ? size : 1;
				width /= 2;
				height /= 2;
			}
			break;
		case WAL_TYPE_DAIKATANA:
			result += WAL_DK_HEADER_SIZE;
			for (mip = 0; mip < MIP_LEVELS_DK; mip++) {
				size = width * height;
				result += size > 0 ? size : 1;
				width /= 2;
				height /= 2;
			}
			break;
	}
	return result;
}

static sptr_t wal_write_mips(sptr_t buf, const struct image_data *im,
							 size_t mips, uint32_t *offsets)
{
	int mip_num, x, y;
	size_t bytes_written = 0, width = im->width, height = im->height;
	unsigned char *ptr = NULL;
	unsigned char *out = buf.ptr;
	for (mip_num = 0; mip_num < mips; mip_num++) {
		offsets[mip_num] = bytes_written;
		ptr = im->pixels.ptr;
		for (y = 0; y < height; y += 1 << mip_num) {
			for (x = 0; x < width; x += 1 << mip_num) {
				write_8(ptr[x + width * y], &out);
				bytes_written++;
			}
		}
	}
	return sptr_advance(buf, bytes_written);
}

static sptr_t wal_write_dk(sptr_t buf, const struct image_data *im)
{
	int i;
	unsigned char *ptr = buf.ptr;
	uint32_t *offsets = (uint32_t *)(buf.ptr + 44);
	write_8(3, &ptr);
	ptr += 3;				  // padding
	memset(ptr, 0, NAME_LEN); // name
	ptr += NAME_LEN;
	write_le32(im->width, &ptr);
	write_le32(im->height, &ptr);
	ptr += sizeof(uint32_t) * MIP_LEVELS_DK; // offsets will be filled later
	memset(ptr, 0, NAME_LEN);				 // animname
	ptr += NAME_LEN;
	write_le32(0, &ptr); // flags
	write_le32(0, &ptr); // contents
	memcpy(ptr, im->palette.data.ptr, DK_PALETTE_SIZE);
	ptr += DK_PALETTE_SIZE;
	write_le32(0, &ptr); // value
	wal_write_mips(sptr_advance(buf, WAL_DK_HEADER_SIZE), im, MIP_LEVELS_DK,
				   offsets);
	for (i = 0; i < MIP_LEVELS_DK; i++) {
		offsets[i] += WAL_DK_HEADER_SIZE;
	}
	return buf;
}

static sptr_t wal_write_q2(sptr_t buf, const struct image_data *im)
{
	int i;
	unsigned char *ptr = buf.ptr;
	uint32_t *offsets = (uint32_t *)(buf.ptr + 40);
	memset(ptr, 0, NAME_LEN); // name
	ptr += NAME_LEN;
	write_le32(im->width, &ptr);
	write_le32(im->height, &ptr);
	ptr += sizeof(uint32_t) * MIP_LEVELS_Q2; // offsets will be filled later
	memset(ptr, 0, NAME_LEN);				 // animname
	ptr += NAME_LEN;
	write_le32(0, &ptr); // flags
	write_le32(0, &ptr); // contents
	write_le32(0, &ptr); // value
	wal_write_mips(sptr_advance(buf, WAL_Q2_HEADER_SIZE), im, MIP_LEVELS_Q2,
				   offsets);
	for (i = 0; i < MIP_LEVELS_Q2; i++) {
		offsets[i] += WAL_Q2_HEADER_SIZE;
	}
	return buf;
}

sptr_t wal_write(sptr_t buf, const struct image_data image, int wal_type)
{
	switch (wal_type) {
		case WAL_TYPE_DAIKATANA:
			return wal_write_dk(buf, &image);
		case WAL_TYPE_QUAKE2:
			return wal_write_q2(buf, &image);
		default:
			return SPTR_NULL;
	}
}