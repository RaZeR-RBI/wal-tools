#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../common/io.h"
#include "cmocka.h"

void test_sptr_slice(void **state)
{
	(void)state;
	unsigned char buffer[] = "abcdefg 1234567\0";
	buffer[7] = 0;

	sptr_t p = {(unsigned char *)&buffer, 16};
	sptr_t first_half = sptr_slice(p, 0, 8);
	assert_false(SPTR_IS_NULL(first_half));
	assert_int_equal(first_half.size, 8);
	assert_string_equal(first_half.ptr, "abcdefg");

	sptr_t second_half = sptr_slice(p, 8, 8);
	assert_false(SPTR_IS_NULL(second_half));
	assert_int_equal(second_half.size, 8);
	assert_string_equal(second_half.ptr, "1234567");

	assert_true(SPTR_IS_NULL(sptr_slice(p, 0, 17)));
	assert_true(SPTR_IS_NULL(sptr_slice(p, 8, 9)));
	assert_true(SPTR_IS_NULL(sptr_slice(p, 42, 1337)));
}

void test_xstrdup(void **state)
{
	(void)state;
	char *s1 = "Hello, world!";
	char *s2 = xstrdup(s1);
	assert_non_null(s2);
	assert_string_equal(s1, s2);
	assert_ptr_not_equal(s1, s2);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_sptr_slice),
		cmocka_unit_test(test_xstrdup),
	};
	return cmocka_run_group_tests(tests, NULL, NULL);
}