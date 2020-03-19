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
	const char *argv[] = {"test.out", "hello", "world"};
	char argc = 3;
	char *actual = cli_concat_args(argc, argv);
	assert_string_equal("hello world", actual);
}

static void test_parse_args_both_types(void **state)
{
	(void)state;
	const char *argv[] = {"test.out", "file1.txt", "file2.txt",
						  "-o",		  "data.csv",  "-v"};
	struct ll_node *parsed = cli_parse_args(6, argv, "-");
	assert_non_null(parsed);

	cli_print_args(parsed);
	/* non-option args */
	struct cli_option args = *cli_get_option(NULL, parsed);
	assert_null(args.name);
	assert_non_null(args.values);
	assert_int_equal(2, ll_size(args.values));
	assert_string_equal(args.values->value_ptr, "file1.txt");
	assert_string_equal(args.values->next->value_ptr, "file2.txt");
	/* options */
	struct cli_option option_o = *cli_get_option("-o", parsed);
	assert_string_equal(option_o.name, "-o");
	assert_non_null(option_o.values);
	assert_int_equal(1, ll_size(option_o.values));
	assert_string_equal(option_o.values->value_ptr, "data.csv");

	struct cli_option option_v = *cli_get_option("-v", parsed);
	assert_string_equal(option_v.name, "-v");
	assert_null(option_v.values);
}

static void test_parse_args_empty(void **state)
{
	(void)state;
	const char *argv[] = {"test.out"};
	struct ll_node *parsed = cli_parse_args(1, argv, "-");
	assert_null(parsed);
}

static void test_parse_args_option_only(void **state)
{
	(void)state;
	const char *argv[] = {"test.out", "-o", "data1.csv", "data2.csv"};
	struct ll_node *parsed = cli_parse_args(4, argv, "-");
	assert_non_null(parsed);

	cli_print_args(parsed);
	/* non-option args */
	const struct cli_option *args = cli_get_option(NULL, parsed);
	assert_null(args);
	/* options */
	struct cli_option option_o = *cli_get_option("-o", parsed);
	assert_string_equal(option_o.name, "-o");
	assert_non_null(option_o.values);
	assert_int_equal(2, ll_size(option_o.values));
	assert_string_equal(option_o.values->value_ptr, "data1.csv");
	assert_string_equal(option_o.values->next->value_ptr, "data2.csv");
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_concat_args),
		cmocka_unit_test(test_parse_args_both_types),
		cmocka_unit_test(test_parse_args_empty),
		cmocka_unit_test(test_parse_args_option_only),
	};
	return cmocka_run_group_tests(tests, NULL, NULL);
}