#include "io.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

sptr_t file_read(const char *path, const char *mode)
{
	sptr_t result = SPTR_NULL;
	if (path == NULL || mode == NULL) {
		return result;
	}
	FILE *f = fopen(path, mode);
	if (!f) {
		perror("Unable to open file for reading\n");
		fprintf(stderr, "File: \"%s\", mode \"%s\"\n", path, mode);
		return result;
	}
	fseek(f, 0, SEEK_END);
	size_t fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	unsigned char *data = xmalloc(fsize);
	size_t total = fread(data, 1, fsize, f);
	if (total) {
		result.ptr = data;
		result.size = total;
		if (fsize != total) {
			fprintf(stderr, "Warning: only %lld of %lld bytes read from \"%s\"\n", total, fsize, path);
		}
	} else {
		perror("Unable to read file\n");
		fprintf(stderr, "File: \"%s\", mode \"%s\"\n", path, mode);
		if (ferror(f)) {
			fprintf(stderr, "IO error %d\n", ferror(f));
		} else if (feof(f)) {
			fprintf(stderr, "Unexpected EOF\n");
		}
		free(data);
		total = 0;
		data = NULL;
		goto end;
	}
end:
	fclose(f);
	return result;
}

int file_write(sptr_t data, const char *path, const char *mode)
{
	int result = 0;
	if (SPTR_IS_NULL(data) || path == NULL || mode == NULL) {
		return result;
	}
	FILE *f = fopen(path, mode);
	if (!f) {
		perror("Unable to open file for writing\n");
		fprintf(stderr, "File: \"%s\", mode \"%s\"\n", path, mode);
		return result;
	}
	size_t bytes = fwrite(data.ptr, 1, data.size, f);
	if (bytes < data.size) {
		fprintf(stderr, "Unable to write all bytes to a file\n");
		fprintf(stderr, "File: \"%s\", mode \"%s\"\n", path, mode);
		goto end;
	}
	result = 1;
end:
	fclose(f);
	return result;
}

const char *trim_extension(const char *s)
{
	char buffer[PATH_MAX];
	sprintf(&buffer[0], "%s%c", s, '\0');
	char *delim_pos = strrchr(buffer, '.');
	if (delim_pos == &buffer[0] || *(delim_pos - 1) == PATH_SEPARATOR) {
		return xstrdup(&buffer[0]);
	}
	*delim_pos = '\0';
	return xstrdup(&buffer[0]);
}
