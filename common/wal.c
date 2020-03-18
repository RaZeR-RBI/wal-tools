#include "wal.h"
#include "endian-util.h"

#include <stddef.h>
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