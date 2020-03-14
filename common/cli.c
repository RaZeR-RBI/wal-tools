#include "cli.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *concat_args(int argc, char const *argv[])
{
	size_t length = 1;
	for (int i = 1; i < argc; i++)
	{
		length += strlen(argv[i]) + 1;
	}
	char *result = (char *)malloc(length);
	if (!result)
	{
		fprintf(stderr, "concat_args: Could not allocate memory\n");
		goto end;
	}

	length = 0;
	for (int i = 1; i < argc; i++)
	{
		length += sprintf(result + length, "%s ", argv[i]);
	}
	result[--length] = '\0';
end:
	return result;
}