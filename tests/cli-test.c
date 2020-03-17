#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmocka.h"

#include "../common/cli.h"

static void test_concat_args(void **state)
{
	(void)state;
	char const *argv[] = {"test.out", "hello", "world"};
	char argc = 3;
	char *actual = concat_args(argc, argv);
	assert_string_equal("hello world", actual);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_concat_args),
	};
	return cmocka_run_group_tests(tests, NULL, NULL);
}