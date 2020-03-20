#ifndef _WAL_H
#define _WAL_H

#include "defs.h"
#include "image.h"
#include "list.h"
#include "mem.h"
#include <stdint.h>

#define NAME_LEN 32

#define WAL_NOT_A_WAL 0
#define WAL_TYPE_QUAKE2 1
#define WAL_TYPE_DAIKATANA 2

/*
	Detects the type of a WAL file by it's header.
	Will return either WAL_NOT_A_WAL, WAL_TYPE_QUAKE2 or WAL_TYPE_DAIKATANA.
	Because both headers doesn't contain enough information to validate
	them, this function may return a false positive, so you should check the
	contents to verify if it's really a WAL texture file.
*/
int32_t wal_get_type(const unsigned char *buffer);

/* Quake 2 .WAL format ------------------------------------------------------ */
#define MIP_LEVELS_Q2 4

/* Defines Quake 2 WAL file header */
struct wal_q2_header {
	char name[NAME_LEN];
	uint32_t width, height;
	uint32_t offsets[MIP_LEVELS_Q2];
	char animname[NAME_LEN];
	uint32_t flags, contents, value;
};

#define WAL_Q2_HEADER_SIZE (sizeof(struct wal_q2_header))

/* Reads a Quake 2 WAL header from the specified buffer buffer into a struct */
unsigned char *wal_q2_read_header(const unsigned char *buffer,
								  struct wal_q2_header *out);

/* Daikatana .WAL format ---------------------------------------------------- */
#define MIP_LEVELS_DK 9
#define DK_PALETTE_ENTRIES 256
#define DK_PALETTE_SIZE (DK_PALETTE_ENTRIES * 3)

/* Defines Daikatana WAL file header */
struct wal_dk_header {
	uint8_t version;	/* should be 3 */
	uint8_t padding[3]; /* unused */
	char name[NAME_LEN];
	uint32_t width, height;
	uint32_t offsets[MIP_LEVELS_DK];
	char animname[NAME_LEN];
	uint32_t flags, contents;
	unsigned char palette[DK_PALETTE_SIZE];
	uint32_t value;
};

#define WAL_DK_HEADER_SIZE (sizeof(struct wal_dk_header))

unsigned char *wal_dk_read_header(const unsigned char *buffer,
								  struct wal_dk_header *out);

/* -------------------------------------------------------------------------- */

/*
Autodetects and tries to read a WAL file.
Returns a linked list of struct image_data which correspond to each mip level or
NULL on failure.
*/
struct ll_node *wal_read(const sptr_t data, const sptr_t palette);

#endif /* _WAL_H */