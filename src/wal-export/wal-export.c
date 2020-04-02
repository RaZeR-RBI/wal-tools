#include "../../common/cli.h"
#include "../../common/image.h"
#include "../../common/io.h"
#include "../../common/quake-colormap.h"
#include "../../common/tga.h"
#include "../../common/wal.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

char *usage =
	"wal-info FILENAME [-mip N] [-o FILENAME_WITHOUT_EXT]\n\n"
	"wal-export - exports WAL textures to the specified format.\n"
	"Supported formats: TGA (default)\n"
	"Options:\n"
	"\t-mip N: export mip N only (Quake 2 - [0..3], Daikatana - [0..8]\n"
	"\t-o FILE: export to specified filename (extension and mip N is "
	"added)\n"
	"Output filename is constructed as follows (for example):\n"
	"source.wal -> source-mip-0.tga\n"
	"If -mip is specified, it's omitted from output filename:\n"
	"source.wal -> source.tga\n";

static int export_tga_single(const image_data_t image, const char *path)
{
	size_t len = tga_estimate_size(image);
	if (!len) {
		return 0;
	}
	sptr_t data = sptr_xmalloc(len);
	if (SPTR_IS_NULL(tga_write(data, image))) {
		return 0;
	}
	if (!file_write(data, path, "w")) {
		return 0;
	}
	return 1;
}

static int export_tga(struct ll_node *mips, int mip_num, const char *out_path)
{
	char buffer[PATH_MAX];
	int i = 0;
	int result = 1;
	if (mip_num == -1) {
		goto export_all;
	}

	printf("Exporting mip %d\n", mip_num);
	const struct ll_node *mip = mips;
	for (i = 0; i < mip_num; i++) {
		mip = mip->next;
	}
	sprintf(&buffer[0], "%s.tga%c", out_path, '\0');
	printf("%s\n", &buffer[0]);
	const image_data_t im = *(const image_data_t *)mip->value_ptr;
	if (!export_tga_single(im, &buffer[0])) {
		fprintf(stderr, "Unable to write to file \"%s\"\n", &buffer[0]);
		result = 0;
	}
	goto end;

export_all:
	printf("Exporting all mips\n");
	const struct ll_node *cur = mips;
	while (cur != NULL) {
		const image_data_t im = *(const image_data_t *)cur->value_ptr;
		sprintf(&buffer[0], "%s-mip-%d.tga%c", out_path, i, '\0');
		printf("%s\n", &buffer[0]);
		if (!export_tga_single(im, &buffer[0])) {
			fprintf(stderr, "Unable to write to file \"%s\"\n", &buffer[0]);
			result = 0;
		}
		cur = cur->next;
		i++;
	}
end:
	return result;
}

static const char *trim_extension(const char *s)
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

int main(int argc, const char *argv[])
{
	if (argc <= 1) {
		goto print_usage;
	}
	struct ll_node *args = cli_parse_args(argc, argv, "-");
	/* Validate args -------------------------------------------------------- */
	/* FILENAME */
	const struct cli_option *filenames = cli_get_option(NULL, args);
	if (ll_size(filenames->values) != 1) {
		goto print_usage;
	}
	const char *src_path = (const char *)filenames->values->value_ptr;
	/* -mip N */
	int mip = -1;
	const struct cli_option *mip_opt = cli_get_option("-mip", args);
	if (mip_opt != NULL) {
		if (ll_size(mip_opt->values) != 1) {
			goto print_usage;
		}
		char *tailptr = NULL;
		unsigned long mip_num =
			strtoul((const char *)mip_opt->values->value_ptr, &tailptr, 10);
		if (mip_num > 8 || (tailptr != NULL && *tailptr != '\0')) {
			goto print_usage;
		}
		mip = mip_num;
	}

	/* -o FILENAME_WITHOUT_EXT */
	const char *dst_path = trim_extension(src_path);
	const struct cli_option *out_opt = cli_get_option("-o", args);
	if (out_opt != NULL) {
		if (ll_size(out_opt->values) != 1) {
			goto print_usage;
		}
		dst_path = (const char *)out_opt->values->value_ptr;
	}

	/* Read the source file ------------------------------------------------- */
	sptr_t src_data = file_read(src_path, "r");
	if (SPTR_IS_NULL(src_data) || src_data.size < WAL_Q2_HEADER_SIZE) {
		exit(2);
	}

	sptr_t palette = SPTR_NULL;
	switch (wal_get_type(src_data.ptr)) {
		case WAL_TYPE_QUAKE2:
			palette = (sptr_t){&q2_palette[0], 768};
			break;
		case WAL_TYPE_DAIKATANA:
			break;
		default:
			printf("Could not parse the supplied file header\n");
			exit(3);
	}
	struct ll_node *mips_ll = wal_read(src_data, palette);
	if (mips_ll == NULL) {
		printf("Could not parse the supplied file contents\n");
		exit(4);
	}
	size_t mip_count = ll_size(mips_ll);
	if (mip >= 0 && mip_count <= mip) {
		printf("MIP number is too high for this file (expected 0 to %ld)\n",
			   mip_count);
		exit(5);
	}

	return export_tga(mips_ll, mip, dst_path);
print_usage:
	printf(usage);
	return 1;
}