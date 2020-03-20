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
	} else if (is_printable(version)) {
		goto possibly_quake2;
	}
	goto not_a_wal;

possibly_daikatana:
	if (invalid_name(buffer + offsetof(struct wal_dk_header, name), 1) ||
		invalid_name(buffer + offsetof(struct wal_dk_header, animname), 0)) {
		goto not_a_wal;
	}
	return WAL_TYPE_DAIKATANA;
possibly_quake2:
	if (invalid_name(buffer + offsetof(struct wal_q2_header, name), 1) ||
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
		struct image_data i_data = {copy, p_data, IMAGE_TYPE_WAL_Q2, header,
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
	for (i = 0; i < MIP_LEVELS_DK; i++) {
		sptr_t p = sptr_slice(data, header->offsets[i], expected_len);
		if (SPTR_IS_NULL(p)) {
			goto err;
		}
		sptr_t copy = {xmalloc(p.size), p.size};
		memcpy(copy.ptr, p.ptr, p.size);
		struct image_palette p_data = {pal, PALETTE_TYPE_RGB_256};
		struct image_data i_data = {copy, p_data, IMAGE_TYPE_WAL_DK, header,
									NULL};
		mips[i] = i_data;
		expected_len /= 4;
		if (expected_len <= 0) {
			expected_len = 1;
		}
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