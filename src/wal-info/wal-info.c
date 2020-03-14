#include "../../common/cli.h"
#include "../../common/wal.h"
#include <stdio.h>
#include <stdlib.h>

static void print_info(struct wal_header header)
{
	printf("Name: %.*s\n", NAME_LEN, header.name);
	printf("Width: %d\n", header.width);
	printf("Height: %d\n", header.height);
	for (int i = 0; i < MIP_LEVELS; i++)
	{
		printf("MIP %d offset: %#010x\n", i, header.offsets[i]);
	}
	printf("Next frame name: %.*s\n", NAME_LEN, header.animname);
	printf("Flags: %#010x\n", header.flags);
	printf("Contents: %#010x\n", header.contents);
	printf("Value: %#010x\n", header.value);
}

int main(int argc, char const *argv[])
{
	FILE *fp;
	struct wal_header header;
	char *path = concat_args(argc, argv);
	char buffer[WAL_HEADER_SIZE];

	if (argc < 2)
	{
		printf("Usage: wal-info texture.wal\n");
		return -1;
	}
	if (!path)
	{
		goto on_err;
	}
	fp = fopen(path, "r");
	if (!fp)
	{
		perror("Could not open file");
		goto on_err;
	}
	size_t total_len = fread(&buffer, 1, WAL_HEADER_SIZE, fp);
	fclose(fp);
	if (total_len < WAL_HEADER_SIZE)
	{
		printf("Unable to read enough bytes to parse the header\n");
		goto on_err;
	}
	wal_read_header(&buffer, &header);
	print_info(header);
	return 0;
on_err:
	printf("File path: %s\n", path);
	return 1;
}
