#include "../common/cli.h"
#include "../common/io.h"
#include "../common/wal.h"

#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *usage = "wal-edit FILENAME [-key value]\n\n"
			  "wal-edit - edit the WAL texture header data\n"
			  "Options:\n"
			  "-name SAMPLE - sets the name to 'SAMPLE' (max %d chars)\n"
			  "-animname SAMPLE2 - sets the next animation frame name to "
			  "'SAMPLE2' (max %d chars)\n"
			  "-flags 0x1337 - sets the flags field to 0x1337 (32-bit number)\n"
			  "-contents 100 - sets the contents field to 100 (32-bit number)\n"
			  "-value 100 - sets the value field to 100 (32-bit number)\n";

static int arg_error = 0;

static void print_usage()
{
	printf(usage, NAME_LEN, NAME_LEN);
	exit(1);
}

static void validate_name(const struct cli_option *opt, char **out)
{
	if (opt == NULL) {
		return;
	}
	if (ll_size(opt->values) != 1) {
		fprintf(stderr, "%s: should have one string argument\n", opt->name);
		goto error;
	}
	const char *val = opt->values->value_ptr;
	if (strlen(val) <= NAME_LEN) {
		*out = (char *)val;
		return;
	}
	fprintf(stderr, "%s: must be %d chars max\n", opt->name, NAME_LEN);
error:
	arg_error = 1;
}

static void validate_number(const struct cli_option *opt, uint32_t *out)
{
	if (opt == NULL) {
		return;
	}
	if (ll_size(opt->values) != 1) {
		fprintf(stderr, "%s: should have one number argument\n", opt->name);
		goto error;
	}
	char *val_ptr = opt->values->value_ptr;
	char *tail_ptr;
	uint32_t val = strtoul(val_ptr, &tail_ptr, 0);
	if (val_ptr == tail_ptr || errno != 0) {
		fprintf(stderr, "%s: invalid number value\n", opt->name);
		goto error;
	}
	*out = val;
	return;
error:
	arg_error = 1;
}

void set_name(char *dst, char *val, const struct cli_option *o)
{
	if (o == NULL) {
		return;
	}
	memset(dst, 0, NAME_LEN);
	strncpy(dst, val, NAME_LEN);
}

void set_number(uint32_t *dst, uint32_t val, const struct cli_option *o)
{
	if (o == NULL) {
		return;
	}
	*dst = val;
}

int main(int argc, const char *argv[])
{
	if (argc < 4) {
		print_usage();
	}

	struct ll_node *opts = cli_parse_args(argc, argv, "-");
	const struct cli_option *o_path = cli_get_option(NULL, opts);
	const struct cli_option *o_name = cli_get_option("-name", opts);
	const struct cli_option *o_animname = cli_get_option("-animname", opts);
	const struct cli_option *o_flags = cli_get_option("-flags", opts);
	const struct cli_option *o_contents = cli_get_option("-contents", opts);
	const struct cli_option *o_value = cli_get_option("-value", opts);

	char *name, *animname;
	uint32_t flags, contents, value;
	validate_name(o_name, &name);
	validate_name(o_animname, &animname);
	validate_number(o_flags, &flags);
	validate_number(o_contents, &contents);
	validate_number(o_value, &value);

	if (o_path == NULL || ll_size(o_path->values) != 1) {
		fprintf(stderr, "Single path to texture should be specified");
		return 1;
	}

	if (!!arg_error) {
		return 1;
	}

	sptr_t src = file_read(o_path->values->value_ptr, "r");
	if (SPTR_IS_NULL(src)) {
		return 2;
	}
	struct wal_q2_header *h_q2 = NULL;
	struct wal_dk_header *h_dk = NULL;

	switch (wal_get_type(src.ptr)) {
		case WAL_TYPE_QUAKE2:
			h_q2 = (struct wal_q2_header *)src.ptr;
			set_name((char *)&h_q2->name, name, o_name);
			set_name((char *)&h_q2->animname, animname, o_animname);
			set_number(&h_q2->flags, flags, o_flags);
			set_number(&h_q2->contents, contents, o_contents);
			set_number(&h_q2->value, value, o_value);
			break;
		case WAL_TYPE_DAIKATANA:
			h_dk = (struct wal_dk_header *)src.ptr;
			set_name((char *)&h_dk->name, name, o_name);
			set_name((char *)&h_dk->animname, animname, o_animname);
			set_number(&h_dk->flags, flags, o_flags);
			set_number(&h_dk->contents, contents, o_contents);
			set_number(&h_dk->value, value, o_value);
			break;
		default:
			fprintf(stderr, "Not a WAL file\n");
			return 3;
	}

	file_write(src, o_path->values->value_ptr, "w");
	return 0;
}
