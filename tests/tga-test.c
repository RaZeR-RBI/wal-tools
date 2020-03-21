#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../common/io.h"
#include "../common/quake-colormap.h"
#include "../common/tga.h"
#include "../common/wal.h"
#include "cmocka.h"

void test_export_from_q2(void **state)
{
	(void)state;
	int i;
	sptr_t wal_data = file_read("clip.wal", "r");
	sptr_t palette = (sptr_t){&q2_palette[0], 768};
	assert_false(SPTR_IS_NULL(wal_data));

	struct ll_node *mips_ll = wal_read(wal_data, palette);
	assert_non_null(mips_ll);

	struct image_data mips[MIP_LEVELS_Q2];
	mips[0] = *(struct image_data *)mips_ll->value_ptr;
	mips[1] = *(struct image_data *)mips_ll->next->value_ptr;
	mips[2] = *(struct image_data *)mips_ll->next->next->value_ptr;
	mips[3] = *(struct image_data *)mips_ll->next->next->next->value_ptr;

	char fname_buf[256];
	for (i = 0; i < MIP_LEVELS_Q2; i++) {
		memset(&fname_buf[0], 0, 256);
		sprintf(&fname_buf[0], "out.test_export_from_q2-mip-%d.tga%c", i, '\0');
		size_t size = tga_estimate_size(mips[i]);
		assert_int_not_equal(0, size);
		sptr_t buf = sptr_xmalloc(size);
		tga_write(buf, mips[i]);
		/* we're writing generated files for examination in case of test fail */
		file_write(buf, &fname_buf[0], "w");
	}

	/* compare with reference files */
	for (i = 0; i < MIP_LEVELS_Q2; i++) {
		sprintf(&fname_buf[0], "ref.test_export_from_q2-mip-%d.tga%c", i, '\0');
		sptr_t ref_data = file_read(&fname_buf[0], "r");
		assert_false(SPTR_IS_NULL(ref_data));
		sprintf(&fname_buf[0], "out.test_export_from_q2-mip-%d.tga%c", i, '\0');
		sptr_t test_data = file_read(&fname_buf[0], "r");
		assert_false(SPTR_IS_NULL(test_data));
		assert_int_equal(ref_data.size, test_data.size);
		assert_memory_equal(ref_data.ptr, test_data.ptr, ref_data.size);
	}
}

void test_export_from_dk(void **state)
{
	(void)state;
	int i;
	sptr_t wal_data = file_read("w_glove_hiro.wal", "r");
	assert_false(SPTR_IS_NULL(wal_data));

	struct ll_node *mips_ll = wal_read(wal_data, SPTR_NULL);
	assert_non_null(mips_ll);

	struct image_data mips[MIP_LEVELS_DK];
	struct ll_node *current = mips_ll;
	for (i = 0; i < MIP_LEVELS_DK; i++) {
		mips[i] = *(struct image_data *)current->value_ptr;
		current = current->next;
	}

	char fname_buf[256];
	for (i = 0; i < MIP_LEVELS_DK; i++) {
		memset(&fname_buf[0], 0, 256);
		sprintf(&fname_buf[0], "out.test_export_from_dk-mip-%d.tga%c", i, '\0');
		size_t size = tga_estimate_size(mips[i]);
		assert_int_not_equal(0, size);
		sptr_t buf = sptr_xmalloc(size);
		tga_write(buf, mips[i]);
		/* we're writing generated files for examination in case of test fail */
		file_write(buf, &fname_buf[0], "w");
	}

	/* compare with reference files */
	for (i = 0; i < MIP_LEVELS_DK; i++) {
		sprintf(&fname_buf[0], "ref.test_export_from_dk-mip-%d.tga%c", i, '\0');
		sptr_t ref_data = file_read(&fname_buf[0], "r");
		assert_false(SPTR_IS_NULL(ref_data));
		sprintf(&fname_buf[0], "out.test_export_from_dk-mip-%d.tga%c", i, '\0');
		sptr_t test_data = file_read(&fname_buf[0], "r");
		assert_false(SPTR_IS_NULL(test_data));
		assert_int_equal(ref_data.size, test_data.size);
		assert_memory_equal(ref_data.ptr, test_data.ptr, ref_data.size);
	}
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_export_from_q2),
		cmocka_unit_test(test_export_from_dk),
	};
	return cmocka_run_group_tests(tests, NULL, NULL);
}