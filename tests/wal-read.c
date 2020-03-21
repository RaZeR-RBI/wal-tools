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
	assert_int_equal(32, mips[0].width);
	assert_int_equal(32, mips[0].height);
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

	assert_int_equal(IMAGE_TYPE_WAL_Q2, mips[0].type);
	assert_int_equal(PALETTE_TYPE_RGB_256, mips[0].palette.type);
	assert_ptr_equal(palette.ptr, mips[0].palette.data.ptr);
	assert_int_equal(768, mips[0].palette.data.size);
}

void test_dk_read(void **state)
{
	(void)state;
	int i, j;
	sptr_t data = file_read("w_glove_hiro.wal", "r");
	sptr_t palette = SPTR_NULL;
	assert_false(SPTR_IS_NULL(data));

	struct ll_node *mips_ll = wal_read(data, palette);
	sptr_free(&data);
	assert_non_null(mips_ll);
	assert_int_equal(MIP_LEVELS_DK, ll_size(mips_ll));

	struct image_data mips[MIP_LEVELS_DK];
	struct ll_node *current = mips_ll;
	for (i = 0; i < MIP_LEVELS_DK; i++) {
		mips[i] = *(struct image_data *)current->value_ptr;
		current = current->next;
	}

	struct wal_dk_header *header = (struct wal_dk_header *)mips[0].header;
	assert_int_equal(128, header->width);
	assert_int_equal(256, header->height);
	assert_int_equal(0, header->flags);
	assert_int_equal(0, header->contents);
	assert_int_equal(0, header->value);

	assert_int_equal(128 * 256, mips[0].pixels.size);
	assert_int_equal(64 * 128, mips[1].pixels.size);
	assert_int_equal(32 * 64, mips[2].pixels.size);
	assert_int_equal(16 * 32, mips[3].pixels.size);
	assert_int_equal(8 * 16, mips[4].pixels.size);
	assert_int_equal(4 * 8, mips[5].pixels.size);
	assert_int_equal(2 * 4, mips[6].pixels.size);
	assert_int_equal(1 * 2, mips[7].pixels.size);
	assert_int_equal(1 * 1, mips[8].pixels.size);

	uint8_t first_4_pixels[9][4] = {
		{0x01, 0x01, 0x01, 0x01}, {0x20, 0x01, 0x10, 0x20},
		{0x98, 0x10, 0x21, 0x98}, {0x10, 0x8f, 0x9c, 0x00},
		{0x98, 0x9c, 0x10, 0x98}, {0x20, 0x11, 0x13, 0x22},
		{0x98, 0x13, 0x8f, 0x98}, {0x11, 0x14, 0, 0},
		{0x13, 0, 0, 0},
	};

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			if (j >= mips[i].pixels.size) {
				break;
			}
			uint8_t expected = first_4_pixels[i][j];
			uint8_t actual = *(uint8_t *)(mips[i].pixels.ptr + j);
			assert_int_equal(actual, expected);
		}
	}

	uint8_t first_4_colors[12] = {0x0b, 0x0b, 0x0b, 0x16, 0x16, 0x16,
								  0x21, 0x21, 0x21, 0x2c, 0x2c, 0x2c};

	assert_int_equal(IMAGE_TYPE_WAL_DK, mips[0].type);
	assert_int_equal(PALETTE_TYPE_RGB_256, mips[0].palette.type);
	assert_non_null(mips[0].palette.data.ptr);
	assert_int_equal(768, mips[0].palette.data.size);
	assert_memory_equal(&first_4_colors, mips[0].palette.data.ptr, 12);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_q2_read),
		cmocka_unit_test(test_dk_read),
	};
	return cmocka_run_group_tests(tests, NULL, NULL);
}