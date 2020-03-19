#ifndef _CLI_H
#define _CLI_H

#include "list.h"

struct cli_option {
	char *name;				/* Option name (NULL if they don't belong to any) */
	struct ll_node *values; /* Option values (linked list of char*) */
};

char *cli_concat_args(int argc, const char *argv[]);

/*
Returns a linked list of struct cli_option.
*/
struct ll_node *cli_parse_args(int argc, const char *argv[],
							   const char *prefix);

void cli_print_args(struct ll_node *parsed_args);

const struct cli_option* cli_get_option(char *name, struct ll_node *parsed);

#endif /* _CLI_H */