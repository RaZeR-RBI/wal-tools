#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../common/io.h"
#include "../common/quake-colormap.h"
#include "../common/wal.h"
#include "cmocka.h"

void test_q2_read(void **state)
{
	(void)state;
	int i, j;
	sptr_t data = file_read("clip.wal", "r");
	sptr_t palette = {&q2_palette[0], 768};
	assert_false(SPTR_IS_NULL(data));

	struct ll_node *mips_ll = wal_read(data, palette);
	sptr_free(&data);
	assert_non_null(mips_ll);
	assert_int_equal(MIP_LEVELS_Q2, ll_size(mips_ll));

	struct image_data mips[MIP_LEVELS_Q2];
	mips[0] = *(struct image_data *)mips_ll->value_ptr;
	mips[1] = *(struct image_data *)mips_ll->next->value_ptr;
	mips[2] = *(struct image_data *)mips_ll->next->next->value_ptr;
	mips[3] = *(struct image_data *)mips_ll->next->next->next->value_ptr;

	struct wal_q2_header *header = (struct wal_q2_header *)mips[0].header;
	assert_int_equal(32, header->width);
	assert_int_equal(32, header->height);
	assert_int_equal(0x80, header->flags);
	assert_int_equal(0x30000, header->contents);
	assert_int_equal(0, header->value);

	assert_int_equal(32 * 32, mips[0].pixels.size);
	assert_int_equal(16 * 16, mips[1].pixels.size);
	assert_int_equal(8 * 8, mips[2].pixels.size);
	assert_int_equal(4 * 4, mips[3].pixels.size);

	uint8_t first_4_pixels[4][4] = {{0x1d, 0x1d, 0x1b, 0x5d},
									{0x16, 0x10, 0x11, 0x10},
									{0x10, 0x36, 0x37, 0x10},
									{0x36, 0x10, 0x36, 0x85}};
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			uint8_t expected = first_4_pixels[i][j];
			uint8_t actual = *(uint8_t *)(mips[i].pixels.ptr + j);
			assert_int_equal(actual, expected);
		}
	}
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_q2_read),
	};
	return cmocka_run_group_tests(tests, NULL, NULL);
}