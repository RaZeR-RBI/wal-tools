#ifndef _CLI_H
#define _CLI_H

struct cli_option {
	char *name;
	struct ll_node *values;
};

char *concat_args(int argc, char const *argv[]);



#endif /* _CLI_H */