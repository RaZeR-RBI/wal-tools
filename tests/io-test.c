#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../common/io.h"
#include "cmocka.h"

void test_read(void **state)
{
	(void)state;
	sptr_t data = file_read("sample.txt", "r");
	assert_non_null(data.ptr);
	assert_int_equal(13, data.size);
	assert_string_equal("Hello, world!", (const char *)data.ptr);
}

void test_write(void **state)
{
	(void)state;
	char *str = "Hello, world!";
	sptr_t data = (sptr_t){(unsigned char *)str, strlen(str)};
	int result = file_write(data, "out.test_write.txt", "w");
	assert_true(result);

	sptr_t data_read = file_read("out.test_write.txt", "r");
	assert_false(SPTR_IS_NULL(data_read));
	assert_int_equal(data_read.size, data.size);
	assert_memory_equal(data.ptr, data_read.ptr, data.size);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_read),
		cmocka_unit_test(test_write),
	};
	return cmocka_run_group_tests(tests, NULL, NULL);
}