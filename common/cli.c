#include "cli.h"
#include "mem.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void print_item(void **s);

char *cli_concat_args(int argc, const char *argv[])
{
	int i;
	size_t length = 1;
	for (i = 1; i < argc; i++) {
		length += strlen(argv[i]) + 1;
	}
	char *result = (char *)xmalloc(length);

	length = 0;
	for (i = 1; i < argc; i++) {
		length += sprintf(result + length, "%s ", argv[i]);
	}
	result[--length] = '\0';
	return result;
}

static int starts_with(const char *prefix, const char *str)
{
	size_t len_p = strlen(prefix);
	size_t len_s = strlen(str);
	return len_s < len_p ? 0 : memcmp(prefix, str, len_p) == 0;
}

static struct ll_node *parse_arg_range(int from, int count, int *end,
									   const char *argv[], const char *prefix)
{
	int i = from;
	struct cli_option option;
	if (from >= count || from < 0 || argv == NULL || prefix == NULL) {
		return NULL;
	}
	option.name = starts_with(prefix, argv[i]) ? (char *)argv[i] : NULL;
	option.values = NULL;
	if (option.name != NULL) {
		i++;
	}
	from = i;
	while (i < count && !starts_with(prefix, argv[i])) {
		const char *val = argv[i];
		if (option.values == NULL) {
			option.values = ll_create_node(val, strlen(val) + 1);
		} else {
			ll_append(option.values, val, strlen(val) + 1);
		}
		i++;
	}
	*end = i;
	return ll_create_node(&option, sizeof(struct cli_option));
}

struct ll_node *cli_parse_args(int argc, const char *argv[], const char *prefix)
{
	int pos = 1;
	int end = pos;
	struct ll_node *root_node = NULL;
	struct ll_node *prev_node = NULL;
	struct ll_node *cur_node = NULL;

	if (argc <= 0 || argv == NULL) {
		return NULL;
	}
	while (pos < argc) {
		prev_node = cur_node;
		cur_node = parse_arg_range(pos, argc, &end, argv, prefix);
		if (cur_node == NULL) {
			break;
		}
		if (root_node == NULL) {
			root_node = cur_node;
		}
		if (prev_node != NULL) {
			prev_node->next = cur_node;
		}
		pos = end;
	}
	return root_node;
}

static char *g_fmt;
static void print_item(void **s) { printf(g_fmt, (char *)s); }

static void print_options(void **data)
{
	struct cli_option option = *(struct cli_option *)data;
	printf("%s\n", option.name == NULL ? "(args)" : option.name);
	g_fmt = "\t\"%s\"\n";
	ll_foreach(option.values, &print_item);
}

void cli_print_args(struct ll_node *parsed_args)
{
	ll_foreach(parsed_args, &print_options);
}

static char *g_cli_get_option_name = NULL;
static int cli_get_option_predicate(void *data)
{
	struct cli_option option = *(struct cli_option *)data;
	if (option.name != NULL && g_cli_get_option_name != NULL) {
		return strcmp(option.name, g_cli_get_option_name) == 0;
	}
	if (option.name != NULL || g_cli_get_option_name != NULL) {
		return 0;
	}
	return 1;
}
const struct cli_option *cli_get_option(char *name, struct ll_node *parsed)
{
	if (parsed == NULL) {
		return NULL;
	}
	g_cli_get_option_name = name;
	struct ll_node *result = ll_find(parsed, &cli_get_option_predicate);
	if (result == NULL) {
		return NULL;
	}
	return result->value_ptr;
}