#include "../../common/cli.h"
#include "../../common/wal.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static void print_info_q2(struct wal_q2_header header)
{
	int i;
	printf("Name: %.*s\n", NAME_LEN, header.name);
	printf("Width: %d\n", header.width);
	printf("Height: %d\n", header.height);
	for (i = 0; i < MIP_LEVELS_Q2; i++) {
		printf("MIP %d offset: %#010x\n", i, header.offsets[i]);
	}
	printf("Next frame name: %.*s\n", NAME_LEN, header.animname);
	printf("Flags: %#010x\n", header.flags);
	printf("Contents: %#010x\n", header.contents);
	printf("Value: %#010x\n", header.value);
}

static void print_info_dk(struct wal_dk_header header)
{
	int i;
	printf("Name: %.*s\n", NAME_LEN, header.name);
	printf("Width: %d\n", header.width);
	printf("Height: %d\n", header.height);
	for (i = 0; i < MIP_LEVELS_DK; i++) {
		printf("MIP %d offset: %#010x\n", i, header.offsets[i]);
	}
	printf("Next frame name: %.*s\n", NAME_LEN, header.animname);
	printf("Flags: %#010x\n", header.flags);
	printf("Contents: %#010x\n", header.contents);
	printf("Value: %#010x\n", header.value);
	printf("Palette:\n");
	for (i = 0; i < DK_PALETTE_ENTRIES; i += 3) {
		printf("#%02x%02x%02x\n", header.palette[i], header.palette[i + 1],
			   header.palette[i + 2]);
	}
}

int main(int argc, const char *argv[])
{
	FILE *fp;
	struct wal_q2_header header_q2;
	struct wal_dk_header header_dk;
	char *path = cli_concat_args(argc, argv);
	char buffer[WAL_DK_HEADER_SIZE]; /* Daikatana's header is bigger */
	const unsigned char *buf_ptr = (const unsigned char *)&buffer;

	if (argc < 2) {
		printf("Usage: wal-info texture.wal\n");
		return -1;
	}
	if (!path) {
		goto on_err;
	}
	fp = fopen(path, "r");
	if (!fp) {
		perror("Could not open file");
		goto on_err;
	}
	size_t total_len = fread(&buffer, 1, WAL_DK_HEADER_SIZE, fp);
	fclose(fp);
	if (total_len < WAL_Q2_HEADER_SIZE) {
		fprintf(stderr, "Unable to read enough bytes to parse the header\n");
		goto on_err;
	}
	int32_t type = wal_get_type(buf_ptr);
	switch (type) {
		case WAL_TYPE_QUAKE2:
			wal_q2_read_header(buf_ptr, &header_q2);
			printf("Type: Quake 2\n");
			print_info_q2(header_q2);
			break;
		case WAL_TYPE_DAIKATANA:
			wal_dk_read_header(buf_ptr, &header_dk);
			printf("Type: Daikatana\n");
			print_info_dk(header_dk);
			break;
		default:
			printf("File type not supported - check if it's really a WAL\n");
			goto on_err;
	}
	return 0;

on_err:
	printf("File path: %s\n", path);
	return 1;
}
