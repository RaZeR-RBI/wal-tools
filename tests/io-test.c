
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

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_read),
	};
	return cmocka_run_group_tests(tests, NULL, NULL);
}