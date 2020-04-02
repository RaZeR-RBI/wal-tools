#include "../common/cli.h"
#include "../common/image.h"
#include "../common/io.h"
#include "../common/quake-colormap.h"
#include "../common/wal.h"

#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *usage =
	"wal-term FILENAME\n"
	"Displays a WAL texture in a terminal using 24-bit color escape codes.\n";

wchar_t ub_char = 0x2580; /* upper half-block character */

int main(int argc, const char *argv[])
{
	if (argc <= 1) {
		goto print_usage;
	}
	setlocale(LC_ALL, "C.UTF-8");
	const char *path = cli_concat_args(argc, argv);
	sptr_t data = file_read(path, "r");
	if (SPTR_IS_NULL(data)) {
		exit(2);
	}

	sptr_t palette = SPTR_NULL;
	switch (wal_get_type(data.ptr)) {
		case WAL_TYPE_QUAKE2:
			palette = (sptr_t){&q2_palette[0], 768};
			break;
		case WAL_TYPE_DAIKATANA:
			break;
		default:
			printf("Could not parse the supplied file header\n");
			exit(3);
	}
	struct ll_node *mips_ll = wal_read(data, palette);
	const image_data_t im = *(const image_data_t *)mips_ll->value_ptr;

	uint32_t screen_width = im.width + 1;
	const char *columns = getenv("COLUMNS");
	if (columns) {
		screen_width = atoi(columns);
	}
	int add_newline = screen_width == im.width ? 0 : 1;
	long step = im.width > screen_width ? (im.width / screen_width + 1) : 1;
	if (step <= 0) {
		step = 1;
	}
	int x, y;
	long index_upper, index_lower;
	uint8_t color_index;
	struct rgb color;
	for (y = 0; y < im.height; y += 2 * step) {
		for (x = 0; x < im.width; x += step) {
			index_upper = x + y * im.width;
			index_lower = x + (y + step) * im.width;
			/* reset colors */
			printf("%c[0m", 0x1B);
			if (index_lower < im.pixels.size) {
				color_index = *(uint8_t *)(im.pixels.ptr + index_lower);
				color = *(struct rgb *)(im.palette.data.ptr + color_index * 3);
				/* set background */
				printf("%c[48;2;%d;%d;%dm", 0x1B, color.r, color.g, color.b);
			}
			if (index_upper < im.pixels.size) {
				color_index = *(uint8_t *)(im.pixels.ptr + index_upper);
				color = *(struct rgb *)(im.palette.data.ptr + color_index * 3);
				/* set foreground */
				printf("%c[38;2;%d;%d;%dm", 0x1B, color.r, color.g, color.b);
			}
			printf("%lc", ub_char);
		}
		if (add_newline) {
			printf("%c[0m\n", 0x1B);
		}
	}

	printf("%c[0m\n", 0x1B);
	return 0;
print_usage:
	printf(usage);
	return 1;
}