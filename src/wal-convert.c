#include "../common/cli.h"
#include "../common/image.h"
#include "../common/io.h"
#include "../common/quake-colormap.h"
#include "../common/tga.h"
#include "../common/wal.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *usage =
	"wal-convert FILENAME [-o FILENAME_WITHOUT_EXT]\n\n"
	"wal-convert - converts image file to a WAL texture.\n"
	"Supported formats: TGA with 256-color 24bpp RGB palette (default)\n"
	"Options:\n"
	"\t-o FILE: export to specified filename (.wal is added automatically)\n"
	"\t-f FORMAT: 'Q2' or 'DK' (defaults to 'Q2')\n";

int main(int argc, const char *argv[])
{
	char buffer[PATH_MAX];
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

	/* -o FILENAME_WITHOUT_EXT */
	const char *dst_path = trim_extension(src_path);
	const struct cli_option *out_opt = cli_get_option("-o", args);
	if (out_opt != NULL) {
		if (ll_size(out_opt->values) != 1) {
			goto print_usage;
		}
		dst_path = (const char *)out_opt->values->value_ptr;
	}

	/* -f FORMAT */
	const char *format = "Q2";
	const struct cli_option *fmt_opt = cli_get_option("-o", args);
	if (fmt_opt != NULL) {
		if (ll_size(out_opt->values) != 1) {
			goto print_usage;
		}
		dst_path = (const char *)out_opt->values->value_ptr;
	}

	int wal_type = WAL_NOT_A_WAL;
	if (strcmp(format, "Q2") == 0) {
		wal_type = WAL_TYPE_QUAKE2;
	} else if (strcmp(format, "DK")) {
		wal_type = WAL_TYPE_DAIKATANA;
	} else {
		goto print_usage;
	}

	const sptr_t input = file_read(src_path, "r");
	struct image_data *im = tga_read(input);
	int error = 0;
	if ((im->width < 16) || (im->width % 16 != 0) || (im->height < 16) ||
		(im->height % 16 != 0)) {
		printf("Input texture width and height must be a multiple of 16\n");
		error = 1;
	}

	if (im->palette.type != PALETTE_TYPE_RGB_256) {
		printf("Only uncompressed 256-color 24bpp indexed TGAs are supported\n");
		error = 1;
	}

	if (error) {
		return 1;
	}

	const sptr_t output = sptr_xmalloc(wal_estimate_size(*im, wal_type));
	wal_write(output, *im, wal_type);
	sprintf(&buffer[0], "%s.wal%c", dst_path, '\0');
	file_write(output, &buffer[0], "w");
	return 0;

print_usage:
	printf(usage);
	return 1;
}