#include "mem.h"
#include "cli.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *concat_args(int argc, char const *argv[])
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