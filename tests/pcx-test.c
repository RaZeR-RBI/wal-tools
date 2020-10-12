#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../common/io.h"
#include "../common/pcx.h"
#include "../common/quake-colormap.h";
#include "cmocka.h"

void test_read(void **state)
{
	(void)state;
	const sptr_t pcx_data = file_read("colormap.pcx", "r");
	struct image_data *im = pcx_read(pcx_data);
	assert_int_equal(im->width, 256);
	assert_int_equal(im->height, 320);
	assert_memory_equal(im->palette.data.ptr, &q2_palette[0], PCX_PALETTE_SIZE);
}

int main(void)
{
	const struct CMUnitTest tests[] = {test_read};
	return cmocka_run_group_tests(tests, NULL, NULL);
}