#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "../common/wal.h"

static void test_q2_wal_header(void **state)
{
	(void)state; /* unused */
	struct wal_header header;
	// (Quake 2) textures/e1u1/clip.wal
	unsigned char data[] = {
		0x65, 0x31, 0x75, 0x31, 0x2f, 0x63, 0x6c, 0x69, 0x70, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00,
		0x20, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00, 0x64, 0x04, 0x00, 0x00,
		0x64, 0x05, 0x00, 0x00, 0xa4, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00,
		0x00, 0x00, 0x00, 0x00};
	const unsigned char *buf = (const unsigned char *)&data;
	unsigned char *ptr = wal_read_header(buf, &header);
	assert_ptr_equal(ptr - buf, WAL_HEADER_SIZE);
	assert_int_equal(WAL_HEADER_SIZE, 100);

	assert_string_equal(header.name, "e1u1/clip");
	assert_int_equal(header.width, 32);
	assert_int_equal(header.height, 32);
	assert_int_equal(header.offsets[0], 0x00000064);
	assert_int_equal(header.offsets[1], 0x00000464);
	assert_int_equal(header.offsets[2], 0x00000564);
	assert_int_equal(header.offsets[3], 0x000005a4);
	assert_string_equal(header.animname, "");
	assert_int_equal(header.flags, 0x00000080);
	assert_int_equal(header.contents, 0x00030000);
	assert_int_equal(header.value, 0000000000);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_q2_wal_header),
	};
	return cmocka_run_group_tests(tests, NULL, NULL);
}