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

void test_invalid_calls(void **state)
{
	(void)state;
	char *sample_data = "Hello, world";
	sptr_t data =
		(sptr_t){(unsigned char *)&sample_data, strlen(sample_data) + 1};
	sptr_t read_non_existing = file_read("zzzzzzzzzzzzzzzzzzz", "r");
	assert_true(SPTR_IS_NULL(read_non_existing));

	int result = file_write(data, "out.xxx", "r");
	assert_false(result);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_read),
		cmocka_unit_test(test_write),
		cmocka_unit_test(test_invalid_calls),
	};
	return cmocka_run_group_tests(tests, NULL, NULL);
}