#include "io.h"

#include <stdio.h>
#include <stdlib.h>

sptr_t file_read(const char *path, const char *mode)
{
	sptr_t result = SPTR_NULL;
	if (path == NULL || mode == NULL) {
		return result;
	}
	FILE *f = fopen(path, mode);
	if (!f) {
		perror("Unable to open file");
		fprintf(stderr, "File: \"%s\", mode \"%s\"\n", path, mode);
		return result;
	}
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	unsigned char *data = xmalloc(fsize);
	if (fread(data, 1, fsize, f) != fsize) {
		fprintf(stderr, "Unable to read all bytes from a file\n");
		fprintf(stderr, "File: \"%s\", mode \"%s\"\n", path, mode);
		free(data);
		goto end;
	}
	result.ptr = data;
	result.size = fsize;
end:
	fclose(f);
	return result;
}
