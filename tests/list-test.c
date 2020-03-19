#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmocka.h"

#include "../common/list.h"
#include "../common/mem.h"

static char *g_string_to_find = NULL;

static int find_string_predicate(void *value_ptr)
{
	return strcmp(*(char **)value_ptr, g_string_to_find) == 0 ? 1 : 0;
}

static void test_ll_from_array(void **state)
{
	(void)state;
	const char *array[] = {"hello", "world", "!"};
	struct ll_node *root = ll_from_array((void *)&array, PTR_SIZE, 3);
	assert_non_null(root);
	assert_int_equal(3, ll_size(root));
	assert_string_equal(*(char **)root->value_ptr, array[0]);
	assert_string_equal(*(char **)root->next->value_ptr, array[1]);
	assert_string_equal(*(char **)root->next->next->value_ptr, array[2]);
	assert_ptr_equal(root->next->next, ll_last(root));
	assert_int_equal(3, ll_free(root));
}

static void test_ll_creation(void **state)
{
	(void)state;
	const char *array[] = {"one", "two", "three"};
	struct ll_node *one = ll_create_node((void *)&array[0], PTR_SIZE);
	assert_non_null(one);
	assert_string_equal(*(char **)one->value_ptr, array[0]);
	assert_int_equal(1, ll_size(one));

	struct ll_node *two = ll_append(one, (void *)&array[1], PTR_SIZE);
	assert_string_equal(*(char **)two->value_ptr, array[1]);
	assert_int_equal(2, ll_size(one));

	struct ll_node *three = ll_append(one, (void *)&array[2], PTR_SIZE);
	assert_string_equal(*(char **)three->value_ptr, array[2]);
	assert_int_equal(3, ll_size(one));
}

static void test_ll_find(void **state)
{
	(void)state;
	const char *array[] = {"one", "two", "three"};
	struct ll_node *root = ll_from_array((void *)&array, PTR_SIZE, 3);
	g_string_to_find = "two";
	assert_ptr_equal(root->next, ll_find(root, &find_string_predicate));
	g_string_to_find = "foo";
	assert_null(ll_find(root, &find_string_predicate));
}

static void test_ll_invalid_calls(void **state)
{
	(void)state;
	assert_null(ll_create_node(NULL, 1));
	ll_free_node(NULL);
	assert_null(ll_from_array(NULL, 1, 1));
	assert_int_equal(0, ll_size(NULL));
	assert_int_equal(0, ll_free(NULL));
	assert_null(ll_append(NULL, NULL, 1));
	assert_null(ll_find(NULL, NULL));
}

static char g_buffer[4096];
static int g_test_count = 0;
static void fn_foreach(void **data)
{
	sprintf(g_buffer + strlen(g_buffer), " %s (%d)", *(char **)data,
			g_test_count);
	g_test_count++;
}

static void test_ll_foreach(void **state)
{
	(void)state;
	const char *array[] = {"one", "two", "three"};
	struct ll_node *root = ll_from_array((void *)&array, PTR_SIZE, 3);
	memset(g_buffer, 0, 4096);
	ll_foreach(root, &fn_foreach);
	assert_string_equal(" one (0) two (1) three (2)", g_buffer);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_ll_from_array),
		cmocka_unit_test(test_ll_creation),
		cmocka_unit_test(test_ll_invalid_calls),
		cmocka_unit_test(test_ll_find),
		cmocka_unit_test(test_ll_foreach),
	};
	return cmocka_run_group_tests(tests, NULL, NULL);
}