#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmocka.h"

#include "../common/wal.h"

static unsigned char q2_wal_header[WAL_Q2_HEADER_SIZE];
static unsigned char dk_wal_header[WAL_DK_HEADER_SIZE];

static void test_q2_wal_header(void **state)
{
	(void)state; /* unused */
	struct wal_q2_header header;
	// (Quake 2) textures/e1u1/clip.wal
	const unsigned char *buf = (const unsigned char *)&q2_wal_header;
	assert_int_equal(WAL_TYPE_QUAKE2, wal_get_type(buf));
	unsigned char *ptr = wal_q2_read_header(buf, &header);
	assert_ptr_equal(ptr - buf, WAL_Q2_HEADER_SIZE);
	assert_int_equal(WAL_Q2_HEADER_SIZE, 100);

	assert_string_equal(header.name, "e1u1/clip");
	assert_int_equal(header.width, 32);
	assert_int_equal(header.height, 32);
	assert_int_equal(header.offsets[0], 0x00000064);
	assert_int_equal(header.offsets[1], 0x00000464);
	assert_int_equal(header.offsets[2], 0x00000564);
	assert_int_equal(header.offsets[3], 0x000005a4);
	assert_string_equal(header.animname, "");
	assert_int_equal(header.flags, 0x80);
	assert_int_equal(header.contents, 0x30000);
	assert_int_equal(header.value, 0x0);
}

static void test_dk_wal_header(void **state)
{
	(void)state; /* unused */
	struct wal_dk_header header;
	// (Daikatana DM) data/skins/w_glove_hiro.wal
	const unsigned char *buf = (const unsigned char *)&dk_wal_header;
	assert_int_equal(WAL_TYPE_DAIKATANA, wal_get_type(buf));
	unsigned char *ptr = wal_dk_read_header(buf, &header);
	assert_ptr_equal(ptr - buf, WAL_DK_HEADER_SIZE);
	assert_int_equal(WAL_DK_HEADER_SIZE, 892);

	assert_string_equal(header.name, "glove");
	assert_int_equal(header.width, 128);
	assert_int_equal(header.height, 256);
	assert_int_equal(header.offsets[0], 0x0000037c);
	assert_int_equal(header.offsets[1], 0x0000837c);
	assert_int_equal(header.offsets[2], 0x0000a37c);
	assert_int_equal(header.offsets[3], 0x0000ab7c);
	assert_int_equal(header.offsets[4], 0x0000ad7c);
	assert_int_equal(header.offsets[5], 0x0000adfc);
	assert_int_equal(header.offsets[6], 0x0000ae1c);
	assert_int_equal(header.offsets[7], 0x0000ae24);
	assert_int_equal(header.offsets[8], 0x0000ae26);
	assert_string_equal(header.animname, "");
	assert_int_equal(header.flags, 0x0);
	assert_int_equal(header.contents, 0x0);
	assert_int_equal(header.value, 0x0);
}

static void test_malformed_header(void **state)
{
	(void)state; /* unused */
	unsigned char data[WAL_DK_HEADER_SIZE];
	unsigned char *p = (unsigned char *)&data;
	memset(&data, 0, WAL_DK_HEADER_SIZE);
	// check Daikatana logic
	data[0] = 3;
	assert_int_equal(WAL_NOT_A_WAL, wal_get_type(p));
	// check Quake 2 logic
	data[0] = 'a';
	data[offsetof(struct wal_q2_header, animname)] = 4;
	assert_int_equal(WAL_NOT_A_WAL, wal_get_type(p));
	data[0] = 4;
	assert_int_equal(WAL_NOT_A_WAL, wal_get_type(p));
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_q2_wal_header),
		cmocka_unit_test(test_dk_wal_header),
		cmocka_unit_test(test_malformed_header),
	};
	return cmocka_run_group_tests(tests, NULL, NULL);
}

/* Test data ---------------------------------------------------------------- */
static unsigned char q2_wal_header[WAL_Q2_HEADER_SIZE] = {

	0x65, 0x31, 0x75, 0x31, 0x2f, 0x63, 0x6c, 0x69, 0x70, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00,
	0x20, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00, 0x64, 0x04, 0x00, 0x00,
	0x64, 0x05, 0x00, 0x00, 0xa4, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00,
	0x00, 0x00, 0x00, 0x00};

static unsigned char dk_wal_header[WAL_DK_HEADER_SIZE] = {
	0x03, 0x2f, 0x77, 0x5f, 0x67, 0x6c, 0x6f, 0x76, 0x65, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x80, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x7c, 0x03, 0x00, 0x00,
	0x7c, 0x83, 0x00, 0x00, 0x7c, 0xa3, 0x00, 0x00, 0x7c, 0xab, 0x00, 0x00,
	0x7c, 0xad, 0x00, 0x00, 0xfc, 0xad, 0x00, 0x00, 0x1c, 0xae, 0x00, 0x00,
	0x24, 0xae, 0x00, 0x00, 0x26, 0xae, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0b, 0x0b, 0x0b, 0x16, 0x16, 0x16, 0x21, 0x21, 0x21, 0x2c, 0x2c, 0x2c,
	0x37, 0x37, 0x37, 0x42, 0x42, 0x42, 0x4d, 0x4d, 0x4d, 0x58, 0x58, 0x58,
	0x63, 0x63, 0x63, 0x6e, 0x6e, 0x6e, 0x79, 0x79, 0x79, 0x84, 0x84, 0x84,
	0x8f, 0x8f, 0x8f, 0x9a, 0x9a, 0x9a, 0xa5, 0xa5, 0xa5, 0xb0, 0xb0, 0xb0,
	0x0e, 0x0a, 0x08, 0x1c, 0x15, 0x10, 0x2a, 0x1f, 0x18, 0x37, 0x29, 0x21,
	0x45, 0x33, 0x29, 0x54, 0x3d, 0x30, 0x61, 0x47, 0x39, 0x71, 0x53, 0x41,
	0x7f, 0x5e, 0x49, 0x8d, 0x69, 0x51, 0x9b, 0x73, 0x59, 0xa6, 0x7d, 0x64,
	0xad, 0x87, 0x71, 0xb5, 0x94, 0x7f, 0xbd, 0xa0, 0x8d, 0xc5, 0xab, 0x9b,
	0x0e, 0x0c, 0x0a, 0x1a, 0x15, 0x12, 0x29, 0x21, 0x1b, 0x36, 0x2c, 0x24,
	0x42, 0x35, 0x2c, 0x50, 0x41, 0x36, 0x5c, 0x4b, 0x3e, 0x6a, 0x54, 0x46,
	0x78, 0x60, 0x50, 0x84, 0x6a, 0x58, 0x92, 0x74, 0x60, 0x9e, 0x80, 0x6c,
	0xa7, 0x8c, 0x79, 0xaf, 0x97, 0x85, 0xb8, 0xa3, 0x94, 0xc1, 0xae, 0xa1,
	0x1d, 0x03, 0x06, 0x2f, 0x05, 0x0b, 0x3e, 0x0b, 0x08, 0x4b, 0x12, 0x0d,
	0x58, 0x1b, 0x14, 0x64, 0x25, 0x1c, 0x6e, 0x2d, 0x24, 0x79, 0x38, 0x2d,
	0x85, 0x43, 0x37, 0x8f, 0x4f, 0x41, 0x98, 0x5b, 0x4c, 0xa2, 0x67, 0x58,
	0xac, 0x74, 0x66, 0xb6, 0x81, 0x72, 0xbf, 0x8f, 0x7f, 0xc9, 0x9d, 0x8f,
	0x40, 0x0a, 0x13, 0x50, 0x13, 0x0f, 0x67, 0x1d, 0x07, 0x7b, 0x26, 0x07,
	0x98, 0x22, 0x09, 0xaa, 0x35, 0x12, 0xc7, 0x48, 0x13, 0xd6, 0x63, 0x25,
	0xd7, 0x6a, 0x41, 0xda, 0x81, 0x4e, 0xef, 0xa1, 0x77, 0xfe, 0xbe, 0x74,
	0xfa, 0xd8, 0x87, 0xfe, 0xdf, 0x9a, 0x1c, 0x22, 0x38, 0x27, 0x2f, 0x4b,
	0x2f, 0x37, 0x57, 0x37, 0x43, 0x63, 0x43, 0x4b, 0x73, 0x4b, 0x57, 0x7f,
	0x57, 0x63, 0x8b, 0x63, 0x6f, 0x97, 0x6f, 0x7b, 0xa3, 0x7b, 0x87, 0xb3,
	0x87, 0x8f, 0xbb, 0x93, 0x97, 0xc3, 0x9f, 0x9f, 0xcf, 0xab, 0xab, 0xd7,
	0xbb, 0xb7, 0xe3, 0xcb, 0xc3, 0xeb, 0xdb, 0xd3, 0xf3, 0xeb, 0xe3, 0xff,
	0x91, 0x96, 0x9f, 0x86, 0x89, 0x8e, 0xa2, 0xa7, 0xaf, 0x6e, 0x74, 0x7b,
	0xe2, 0xe8, 0xed, 0xd6, 0xd9, 0xdc, 0xc7, 0xd1, 0xd3, 0xaf, 0xb4, 0xb6,
	0xf9, 0xfa, 0xfa, 0x41, 0x58, 0x5b, 0x28, 0x38, 0x3d, 0x39, 0x38, 0x32,
	0x40, 0x49, 0x40, 0x51, 0x6d, 0x50, 0x61, 0x68, 0x5f, 0xda, 0xc7, 0x47,
	0xe0, 0xe0, 0x4e, 0xf3, 0xf2, 0x8f, 0xdd, 0xda, 0x80, 0xce, 0xcb, 0x86,
	0xd3, 0xdb, 0x98, 0xf6, 0xf3, 0xb8, 0xea, 0xea, 0xd7, 0xa0, 0x9d, 0x71,
	0xd7, 0xd4, 0xa9, 0xe2, 0xe0, 0xc1, 0x9c, 0x9a, 0x8d, 0x58, 0x56, 0x49,
	0xc3, 0xb9, 0x85, 0xb0, 0xa8, 0x7c, 0xec, 0xe0, 0xa1, 0x94, 0x8e, 0x6d,
	0x85, 0x82, 0x76, 0x97, 0x8d, 0x58, 0x75, 0x72, 0x66, 0x22, 0x21, 0x1d,
	0xc4, 0xc1, 0xb8, 0x8b, 0x7f, 0x61, 0xd8, 0xbe, 0x79, 0xec, 0xd1, 0x8b,
	0x93, 0x8e, 0x83, 0xaf, 0x98, 0x61, 0xa4, 0xa2, 0x9c, 0xc3, 0xa8, 0x6b,
	0x86, 0x72, 0x49, 0x32, 0x2d, 0x22, 0x9d, 0x80, 0x4b, 0x29, 0x1e, 0x0c,
	0xb1, 0x89, 0x4a, 0x68, 0x5f, 0x50, 0xb4, 0xad, 0xa1, 0x46, 0x36, 0x1d,
	0xd3, 0x9a, 0x47, 0xd7, 0xa8, 0x66, 0x72, 0x5a, 0x3b, 0x4f, 0x44, 0x37,
	0x19, 0x12, 0x0a, 0x68, 0x4a, 0x2b, 0xac, 0x6e, 0x32, 0x85, 0x59, 0x30,
	0x0c, 0x08, 0x05, 0xc3, 0x7a, 0x3a, 0xa1, 0x6d, 0x41, 0x95, 0x60, 0x37,
	0x78, 0x44, 0x20, 0xd8, 0x81, 0x44, 0xa0, 0x56, 0x24, 0x5a, 0x38, 0x23,
	0x41, 0x1f, 0x0e, 0x8c, 0x3b, 0x1b, 0x7c, 0x29, 0x0f, 0x53, 0x23, 0x15,
	0x36, 0x12, 0x07, 0x83, 0x59, 0x4e, 0xa9, 0x6a, 0x5d, 0x88, 0x6b, 0x67,
	0x6c, 0x25, 0x1c, 0x6f, 0x41, 0x3b, 0x6e, 0x17, 0x0e, 0x84, 0x46, 0x41,
	0x9b, 0x4e, 0x48, 0x7b, 0x34, 0x2f, 0x95, 0x40, 0x3a, 0x77, 0x5e, 0x5c,
	0xb1, 0x56, 0x4f, 0xc3, 0x7d, 0x78, 0xbb, 0x69, 0x64, 0x5b, 0x11, 0x0d,
	0x9b, 0x79, 0x92, 0xbb, 0x98, 0x97, 0x9b, 0x7a, 0x79, 0xab, 0x8a, 0x8a,
	0xd9, 0xbb, 0xb9, 0xde, 0xc8, 0xc8, 0xce, 0xa8, 0xa8, 0xeb, 0xd8, 0xd8,
	0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff,
	0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff,
	0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff,
	0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff,
	0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff,
	0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff,
	0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff,
	0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff,
	0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff,
	0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff,
	0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff,
	0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff,
	0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff,
	0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff,
	0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff,
	0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff,
	0x00, 0x00, 0x00, 0x00};